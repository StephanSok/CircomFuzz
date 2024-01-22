#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <nlohmann/json.hpp>
#include <vector>
#include <chrono>

using json = nlohmann::json;

#include "calcwit.hpp"
#include "circom.hpp"
extern "C" {
  #include "../../openssl-3.1.0/include/openssl/evp.h"
  #include "../../openssl-3.1.0/include/openssl/sha.h"
  #include "../../openssl-3.1.0/include/openssl/bn.h"
}

#define handle_error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)

u8* global_bdata = nullptr; 
struct stat global_sb;
bool flag = false;


bool get_dat_file(std::string const &datFileName){
  int fd;
  struct stat sb;

  fd = open(datFileName.c_str(), O_RDONLY); 

  if (fd == -1) {
      std::cout << ".dat file not found: " << datFileName << "\n";
      throw std::system_error(errno, std::generic_category(), "open");
  }
  
  if (fstat(fd, &sb) == -1) {          /* To obtain file size */
      throw std::system_error(errno, std::generic_category(), "fstat");
  }

  u8* bdata = (u8*)mmap(NULL, sb.st_size, PROT_READ , MAP_PRIVATE, fd, 0);
  flag = true;
  printf("%s", "GET .dat file");
  close(fd);
  global_bdata = bdata;
  global_sb = sb;
  return true;
}

Circom_Circuit* loadCircuit(std::string const &datFileName, IODef** defss, u8* bdata) {
    Circom_Circuit *circuit = new Circom_Circuit;

    int fd;
    struct stat sb = global_sb;    

    circuit->InputHashMap = new HashSignalInfo[get_size_of_input_hashmap()];
    uint dsize = get_size_of_input_hashmap()*sizeof(HashSignalInfo);
    memcpy((void *)(circuit->InputHashMap), (void *)bdata, dsize);

  

    circuit->witness2SignalList = new u64[get_size_of_witness()];
    uint inisize = dsize;    
    dsize = get_size_of_witness()*sizeof(u64);
    memcpy((void *)(circuit->witness2SignalList), (void *)(bdata+inisize), dsize);

    circuit->circuitConstants = new FrElement[get_size_of_constants()];
    if (get_size_of_constants()>0) {
      inisize += dsize;
      dsize = get_size_of_constants()*sizeof(FrElement);
      memcpy((void *)(circuit->circuitConstants), (void *)(bdata+inisize), dsize);
    }

    std::map<u32,IODefPair> templateInsId2IOSignalInfo1;
    if (get_size_of_io_map()>0) {
      u32 index[get_size_of_io_map()];
      inisize += dsize;
      dsize = get_size_of_io_map()*sizeof(u32);
      memcpy((void *)index, (void *)(bdata+inisize), dsize);
      inisize += dsize;
      assert(inisize % sizeof(u32) == 0);    
      assert(sb.st_size % sizeof(u32) == 0);
      u32 dataiomap[(sb.st_size-inisize)/sizeof(u32)];
      // printf("%ld && %d\n", sb.st_size, inisize);
      memcpy((void *)dataiomap, (void *)(bdata+inisize), sb.st_size-inisize);
      
      u32* pu32 = dataiomap;

      for (int i = 0; i < get_size_of_io_map(); i++) {
        u32 n = *pu32;
        IODefPair p;
        p.len = n;
        IODef defs[n];
        pu32 += 1;
        for (u32 j = 0; j <n; j++){
          // printf("%d\n--", n);
          defs[j].offset=*pu32;
          u32 len = *(pu32+1);
          defs[j].len = len;
          defs[j].lengths = new u32[len];
          memcpy((void *)defs[j].lengths,(void *)(pu32+2),len*sizeof(u32));
          pu32 += len + 2;
        }
        p.defs = (IODef*)calloc(10, sizeof(IODef));
        for (u32 j = 0; j < p.len; j++){
          p.defs[j] = defs[j];
        }
        templateInsId2IOSignalInfo1[index[i]] = p;
        // printf("%d\n", get_size_of_io_map());
        defss[i] = p.defs;
        // free(p.defs);
      }
    }
    circuit->templateInsId2IOSignalInfo = move(templateInsId2IOSignalInfo1);
    
    // munmap(bdata, sb.st_size);
    
    return circuit;
}

void json2FrElements (json val, std::vector<FrElement> & vval){
  if (!val.is_array()) {
    FrElement v;
    std::string s;
    if (val.is_string()) {
        s = val.get<std::string>();
    } else if (val.is_number()) {
        double vd = val.get<double>();
        std::stringstream stream;
        stream << std::fixed << std::setprecision(0) << vd;
        s = stream.str();
    } else {
        throw new std::runtime_error("Invalid JSON type");
    }
    Fr_str2element (&v, s.c_str());
    vval.push_back(v);
  } else {
    for (uint i = 0; i < val.size(); i++) {
      json2FrElements (val[i], vval);
    }
  }
}

void loadJson(Circom_CalcWit *ctx, json j) {
  // std::ifstream inStream(filename);
  // json j;
  // inStream >> j;
  
  u64 nItems = j.size();
  // printf("Items : %llu\n",nItems);
  if (nItems == 0){
    ctx->tryRunCircuit();
  }
  for (json::iterator it = j.begin(); it != j.end(); ++it) {
    // std::cout << it.key() << " => " << it.value() << '\n';
    u64 h = fnv1a(it.key());
    std::vector<FrElement> v;
    json2FrElements(it.value(),v);
    uint signalSize = ctx->getInputSignalSize(h);
    if (v.size() < signalSize) {
	std::ostringstream errStrStream;
	errStrStream << "Error loading signal " << it.key() << ": Not enough values\n";
	throw std::runtime_error(errStrStream.str() );
    }
    if (v.size() > signalSize) {
	std::ostringstream errStrStream;
	errStrStream << "Error loading signal " << it.key() << ": Too many values\n";
	throw std::runtime_error(errStrStream.str() );
    }
    for (uint i = 0; i<v.size(); i++){
      try {
	// std::cout << it.key() << "," << i << " => " << Fr_element2str(&(v[i])) << '\n';
  json::iterator k = it;
  if (++k == j.end()) {
    ctx->setInputSignal(h,i,v[i]);
  }else{
    ctx->setInputSignal(h,i,v[i]);
  }
	
  // std::cout << "set signal \n";
      } catch (std::runtime_error e) {
	std::ostringstream errStrStream;
	errStrStream << "Error setting signal: " << it.key() << "\n" << e.what();
	throw std::runtime_error(errStrStream.str() );
      }
    }
  }
}

struct Point {
    BIGNUM* x;
    BIGNUM* y;
};

Point MontgomeryAdd(const Point& in1, const Point& in2, const BIGNUM* A, const BIGNUM* B, BN_CTX* ctx) {
    Point result {BN_new(), BN_new()};
    BIGNUM* lambda = BN_new();
    BIGNUM* bn_circom = NULL;
    const char* PrimeCircom = "21888242871839275222246405745257275088548364400416034343698204186575808495617";
    BN_dec2bn(&bn_circom, PrimeCircom);
    //lambda = (y2 - y1) / (x2 - x1)
    BN_sub(lambda, in2.y, in1.y);
    BIGNUM* temp = BN_new();
    BN_sub(temp, in2.x, in1.x);
    BIGNUM* bn_t = BN_mod_inverse(temp, temp, bn_circom, ctx);
    BN_mul(lambda, lambda, temp, ctx);
    BN_mod(lambda, lambda, bn_circom, ctx);

    //result.x = B * lambda^2 - A - x1 - x2
    BN_sqr(temp, lambda, ctx);
    BN_mul(temp, temp, B, ctx);
    BN_mod(temp, temp, bn_circom, ctx);
    BN_sub(result.x, temp, A);
    BN_sub(result.x, result.x, in1.x);
    BN_sub(result.x, result.x, in2.x);
    BN_mod(result.x, result.x, bn_circom, ctx);

    //result.y = lambda * (x1 - result.x) - y1
    BN_sub(temp, in1.x, result.x);
    BN_mul(result.y, lambda, temp, ctx);
    BN_sub(result.y, result.y, in1.y);
    BN_mod(result.y, result.y, bn_circom, ctx);

    BN_free(lambda);
    BN_free(temp);
    BN_free(bn_circom);
    // BN_free(bn_t);
    return result;
}

std::pair<Point, Point> GetPointsFromData(const unsigned char* data, size_t size) {
    size_t quarterSize = size / 4;

    Point points[2];
    for (int i = 0; i < 2; ++i) {
        points[i].x = BN_bin2bn(data + quarterSize * (2 * i), quarterSize, NULL);
        points[i].y = BN_bin2bn(data + quarterSize * (2 * i + 1), quarterSize, NULL);
    }

    return {points[0], points[1]};
}


extern "C" int LLVMFuzzerTestOneInput(const unsigned char *Data, size_t Size) {
  static bool init = get_dat_file("mont.dat");
  u8* bdata = global_bdata;
  
  if ((Size < 4) || (Size % 4 != 0)) {
      return 0;
  }
  
  std::string datfile = "mont.dat";
  json j;
  auto points = GetPointsFromData(Data, Size);
  auto Data_bn = BN_bin2bn(Data, Size, NULL);
  auto p1_x = BN_bn2dec(points.first.x);
  auto p1_y = BN_bn2dec(points.first.y);
  auto p2_x = BN_bn2dec(points.second.x);
  auto p2_y = BN_bn2dec(points.second.y);
  
  j["in1"] = {p1_x, p1_y};
  j["in2"] = {p2_x, p2_y};
  auto hex_data = BN_bn2hex(Data_bn);
  // std::cout << "Data: " << hex_data << " Size: " << Size << std::endl;
  // std::cout << j << std::endl;
  BN_CTX* ctxx = BN_CTX_new();
  BIGNUM* a = BN_new();
  BIGNUM* d = BN_new();
  BIGNUM* A = BN_new();
  BIGNUM* B = BN_new();
  BN_set_word(a, 168700);
  BN_set_word(d, 168696);

  BN_add(A, a, d);
  BN_mul_word(A, 2);
  BN_sub(B, a, d);
  BN_div(A, NULL, A, B, ctxx);
  BN_set_word(B, 4);
  BN_div(B, NULL, B, B, ctxx);
  Point result = MontgomeryAdd(points.first, points.second, A, B, ctxx);

  BIGNUM* bn_circom = NULL;
  const char* PrimeCircom = "21888242871839275222246405745257275088548364400416034343698204186575808495617";
  BN_dec2bn(&bn_circom, PrimeCircom);
  if (BN_is_negative(result.x)){
    // auto res_x = BN_bn2dec(result.x);
    // std::cout << " xxxxxxxxxx " << res_x << std::endl;
    // OPENSSL_free(res_x);
    BN_add(result.x, result.x, bn_circom);
    // BN_mod_inverse(result.x, result.x, bn_circom, ctxx);
  }
  if (BN_is_negative(result.y)){
    // auto res_x = BN_bn2dec(result.x);
    // std::cout << " yyyyyyyyy " << res_x << std::endl;
    // OPENSSL_free(res_x);
    // BN_mod_inverse(result.y, result.y, bn_circom, ctxx);
    BN_add(result.y, result.y, bn_circom);
  }
  BN_free(bn_circom);
  auto res_x = BN_bn2dec(result.x);
  auto res_y = BN_bn2dec(result.y);
  // std::cout << res_x << " || " << res_y << std::endl;
  

  
  IODef* defs[64];
  Circom_Circuit *circuit = loadCircuit(datfile, defs, bdata);

  Circom_CalcWit *ctx = new Circom_CalcWit(circuit);
  
  loadJson(ctx, j);
  if (ctx->getRemaingInputsToBeSet()!=0) {
    std::cerr << "Not all inputs have been set. Only " << get_main_input_signal_no()-ctx->getRemaingInputsToBeSet() << " out of " << get_main_input_signal_no() << std::endl;
    assert(false);
  }
   
  // bool flag = false;
  // std::string result;
  char* elem;
  size_t bn = 0;
  // printf("%c|\n", Data[0]);
  for (uint i = 0; i<get_size_of_witness(); i++){
    bn = 0;
    FrElement x;
    ctx->getWitness(i, &x);
    if (i == 0) continue;
    elem = Fr_element2str(&x);
    // std::cout << j << std::endl;
    // std::cout << "\n" << i << " (((( " << elem << " ))))" << std::endl;
    if ((i == 1) && (strcmp(res_x, elem) != 0)){
      std::cout << "\n" << i << " (x(x(x( " << elem << " )x)x)x)" << std::endl;
      FrElement x;
      ctx->getWitness(2, &x);
      if (i == 0) continue;
      elem = Fr_element2str(&x);
      std::cout << "\n" << 2 << " (y(y(y( " << elem << " )y)y)y)" << std::endl;
      std::cout << res_x << " || " << res_y << std::endl;
      std::cout << "Data: " << hex_data << " Size: " << Size << std::endl;
      throw std::runtime_error("Bad result");
    }

    if ((i == 2) && (strcmp(res_y, elem) != 0)){
      std::cout << "\n" << i << " (y(y(y( " << elem << " )y)y)y)" << std::endl;
      FrElement x;
      ctx->getWitness(1, &x);
      if (i == 0) continue;
      elem = Fr_element2str(&x);
      std::cout << "\n" << 1 << " (x(x(x( " << elem << " )x)x)x)" << std::endl;
      std::cout << res_x << " || " << res_y << std::endl;
      std::cout << "Data: " << hex_data << " Size: " << Size << std::endl;
      throw std::runtime_error("Bad result");
    }
    free(elem);
    //  std::cout << i << ": " << Fr_element2str(&x) << std::endl;
  }

  // for (int i = 0; i < 64; i++){
  //   delete[] defs[i][0].lengths;
  //   free(defs[i]);
  // }

  // free(*defs);
  // free(defs);
  OPENSSL_free(hex_data);
  OPENSSL_free(p1_x);
  OPENSSL_free(p1_y);
  OPENSSL_free(p2_x);
  OPENSSL_free(p2_y);
  OPENSSL_free(res_x);
  OPENSSL_free(res_y);
  BN_free(points.first.x);
  BN_free(points.first.y);
  BN_free(points.second.x);
  BN_free(points.second.y);
  BN_free(result.x);
  BN_free(result.y);
  BN_free(Data_bn);
  BN_free(a);
  BN_free(d);
  BN_free(A);
  BN_free(B);
  BN_CTX_free(ctxx);
  delete[] ctx->signalValues;
  delete[] ctx->inputSignalAssigned;
  delete[] ctx->componentMemory;
  delete[] circuit->InputHashMap;
  delete[] circuit -> witness2SignalList;
  delete[] circuit->circuitConstants;
  delete circuit;
  delete ctx;
  return 0;
}


