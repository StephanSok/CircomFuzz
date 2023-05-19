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


void writeBinWitness(Circom_CalcWit *ctx, std::string wtnsFileName) {
    FILE *write_ptr;

    write_ptr = fopen(wtnsFileName.c_str(),"wb");

    fwrite("wtns", 4, 1, write_ptr);

    u32 version = 2;
    fwrite(&version, 4, 1, write_ptr);

    u32 nSections = 2;
    fwrite(&nSections, 4, 1, write_ptr);

    // Header
    u32 idSection1 = 1;
    fwrite(&idSection1, 4, 1, write_ptr);

    u32 n8 = Fr_N64*8;

    u64 idSection1length = 8 + n8;
    fwrite(&idSection1length, 8, 1, write_ptr);

    fwrite(&n8, 4, 1, write_ptr);

    fwrite(Fr_q.longVal, Fr_N64*8, 1, write_ptr);

    uint Nwtns = get_size_of_witness();
    
    u32 nVars = (u32)Nwtns;
    fwrite(&nVars, 4, 1, write_ptr);

    // Data
    u32 idSection2 = 2;
    fwrite(&idSection2, 4, 1, write_ptr);
    
    u64 idSection2length = (u64)n8*(u64)Nwtns;
    fwrite(&idSection2length, 8, 1, write_ptr);

    FrElement v;

    for (int i=0;i<Nwtns;i++) {
        ctx->getWitness(i, &v);
        Fr_toLongNormal(&v, &v);
        fwrite(v.longVal, Fr_N64*8, 1, write_ptr);
    }
    fclose(write_ptr);
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


std::vector<size_t> decToBinary(unsigned char n)
{
    std::vector<size_t> binaryNum(8);
    int i = 0;
    while (n > 0) {
 
        binaryNum[i] = n % 2;
        n = n / 2;
        i++;
    }
    return binaryNum;
}
extern "C" int LLVMFuzzerTestOneInput(const unsigned char *Data, size_t Size) {
  static bool init = get_dat_file("sha.dat");
  u8* bdata = global_bdata;
 
  if ((Size != 8)){
      return 0;
  }


  std::string datfile = "sha.dat";
  json j;
  unsigned char message[8];
  for (int i = 0; i < 8; i++) {
    message[i] = Data[i];
  }

  unsigned int len = 8;
  unsigned char hash [SHA256_DIGEST_LENGTH];
  // printf("|%zu|\n\n", strlen((char*)Data));  
  // printf("-%s-\n\n", message);
  SHA256(message, len, hash);

  // for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
  //   printf("%d ", int(hash[i]));
  // }
  // printf("\n");
  std::vector<size_t> sha_array;
  for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
    std::vector<size_t> binaryNum = decToBinary(hash[i]);
    std::reverse(binaryNum.begin(), binaryNum.end());
    sha_array.insert(std::end(sha_array), std::begin(binaryNum), std::end(binaryNum));
    // for (int j = 0; j < binaryNum.size(); j++) {
    //   printf("%lu", binaryNum[j]);
    // }
  }
  // for (int i=0; i < sha_array.size(); i++){
  //   printf("%lu", sha_array[i]);
  // }
  // printf("\n");

  std::vector<size_t> input_data;
  for(int i = 0; i < 8; i++) {
    std::vector<size_t> binaryNum = decToBinary(message[i]);
    std::reverse(binaryNum.begin(), binaryNum.end());
    input_data.insert(std::end(input_data), std::begin(binaryNum), std::end(binaryNum));
  }

  // for (int i=0; i < 64; i++){
  //   printf("%lu", input_data[i]);
  // }
  // printf("\n");

  j["in"] = input_data;
  IODef* defs[64];
  Circom_Circuit *circuit = loadCircuit(datfile, defs, bdata);

  


  Circom_CalcWit *ctx = new Circom_CalcWit(circuit);
  
  loadJson(ctx, j);
  if (ctx->getRemaingInputsToBeSet()!=0) {
    std::cerr << "Not all inputs have been set. Only " << get_main_input_signal_no()-ctx->getRemaingInputsToBeSet() << " out of " << get_main_input_signal_no() << std::endl;
    assert(false);
  }
   
  bool flag = false;
  std::string result;
  char* elem;
  size_t bn = 0;
  for (uint i = 1; i<257; i++){
     FrElement x;
     ctx->getWitness(i, &x);
     elem = Fr_element2str(&x);
    //  printf("%s", elem);
    //  std::cout << i << ": " << elem << std::endl;
    if ((sha_array[i-1] == 0 && elem[0] == '1') || (sha_array[i-1] == 1 && elem[0] == '0')){
      for (int i=0; i < sha_array.size(); i++){
        printf("%lu ", sha_array[i]);
      }
      for (int i=0; i < 64; i++){
        printf("%lu", input_data[i]);
      }
      printf("\n");
      printf("%c|\n", Data[0]);
      throw std::runtime_error("Bad result");
      return 1;
    }
    free(elem);
    
    }
    
  for (int i = 0; i < 64; i++){
    delete[] defs[i][0].lengths;
    free(defs[i]);
  }

  // free(*defs);
  // free(defs);
  // delete[] ctx->templateInsId2IOSignalInfo;
  delete[] ctx->componentMemory[0].subcomponents;
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


