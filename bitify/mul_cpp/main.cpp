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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <algorithm>
#include <string>

using json = nlohmann::json;

#include "calcwit.hpp"
#include "circom.hpp"
#include "bitify.cpp"

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

std::vector<size_t> decToBinary(size_t n)
{
    std::vector<size_t> binaryNum(70);
    int i = 0;
    while (n > 0) {
 
        binaryNum[i] = n % 2;
        n = n / 2;
        i++;
    }
    return binaryNum;
}


extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  static bool init = get_dat_file("mul.dat");
  u8* bdata = global_bdata;
 
  std::string datfile = "mul.dat";
  json j;
  std::stringstream strValue;
  for (int i = 0; i < std::min(Size, size_t(8)); i++) {
        strValue << Data[i];
  }
  size_t intValue;
  strValue >> intValue;
  std::string value = std::to_string(intValue);
  // intValue = 18374686479671623935;
  std::vector<size_t> binaryNum = decToBinary(intValue);

  j["in"] = value;
  // std::cout << intValue << std::endl;
  // std::cout << "\n======= " << intValue << " ---------------------------- " << Size << "\n" << j["in"] << "\n";
  Circom_Circuit *circuit = loadCircuit(datfile, bdata);

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
  for (uint i = 0; i<get_size_of_witness(); i++){
    bn = 0;
    FrElement x;
    ctx->getWitness(i, &x);
    if (i == 0) continue;
    elem = Fr_element2str(&x);
    result += elem;
    result += " ";
    if (elem[0] == '1') bn = 1;
    if (i < 71 && binaryNum[i - 1] != bn){
      flag = true;
      // std::cout << "\n" << i << " (((( " << elem << " ))))" << binaryNum[i] << std::endl;
    }
    free(elem);
    //  std::cout << i << ": " << Fr_element2str(&x) << std::endl;
  }
  if (flag){
    std::cout << "\n->>> " << intValue << "\n" << result << std::endl;
    for (int i = 0; i < 70; i++){
      std::cout << binaryNum[i] << "-";
    }
    std::cout << std::endl;
    delete[] circuit->InputHashMap;
    delete[] circuit -> witness2SignalList;
    delete[] circuit->circuitConstants;
    delete circuit;
    delete ctx;
    throw std::runtime_error("Bad result");
  }
  delete[] circuit->InputHashMap;
  delete[] circuit -> witness2SignalList;
  delete[] circuit->circuitConstants;
  delete circuit;
  delete ctx;
  return 0;
   
  
   //auto t_mid = std::chrono::high_resolution_clock::now();
   //std::cout << std::chrono::duration<double, std::milli>(t_mid-t_start).count()<<std::endl;

  //  writeBinWitness(ctx,wtnsfile);
  
   //auto t_end = std::chrono::high_resolution_clock::now();
   //std::cout << std::chrono::duration<double, std::milli>(t_end-t_mid).count()<<std::endl;

}  

// extern int LLVMFuzzerTestOneInput(const unsigned char *data, size_t size);
// __attribute__((weak)) extern int LLVMFuzzerInitialize(int *argc, char ***argv);
// int main(int argc, char **argv) {
//   fprintf(stderr, "StandaloneFuzzTargetMain: running %d inputs\n", argc - 1);
//   if (LLVMFuzzerInitialize)
//     LLVMFuzzerInitialize(&argc, &argv);
//   for (int i = 2; i < argc; i++) {
//     fprintf(stderr, "Running: %s\n", argv[i]);
//     FILE *f = fopen(argv[i], "r");
//     assert(f);
//     fseek(f, 0, SEEK_END);
//     size_t len = ftell(f);
//     fseek(f, 0, SEEK_SET);
//     unsigned char *buf = (unsigned char*)malloc(len);
//     size_t n_read = fread(buf, 1, len, f);
//     fclose(f);
//     assert(n_read == len);
//     LLVMFuzzerTestOneInput(buf, len);
//     free(buf);
//     fprintf(stderr, "Done:    %s: (%zd bytes)\n", argv[i], n_read);
//   }
// }


// int main(){
//   std::string datfile = "mul.dat";
//   json j;
//   j["in"] = 0;
//   // std::cout << Data[0] << "------------------------------------------------------------------------------------------------------------------- " << Size;
//   Circom_Circuit *circuit = loadCircuit(datfile);

//   Circom_CalcWit *ctx = new Circom_CalcWit(circuit);
  
//   loadJson(ctx, j);
//   if (ctx->getRemaingInputsToBeSet()!=0) {
//     std::cerr << "Not all inputs have been set. Only " << get_main_input_signal_no()-ctx->getRemaingInputsToBeSet() << " out of " << get_main_input_signal_no() << std::endl;
//     assert(false);
//   }
   
//      for (uint i = 0; i<get_size_of_witness(); i++){
//      FrElement x;
//      ctx->getWitness(i, &x);
//      std::cout << i << ": " << Fr_element2str(&x) << std::endl;
//      }

//   return 0;
// }

// int main (int argc, char *argv[]) {
//   std::string cl(argv[0]);
//   if (argc!=3) {
//         std::cout << "Usage: " << cl << " <input.json> <output.wtns>\n";
//   } else {
//     std::string datfile = "mul.dat";
//     std::string jsonfile(argv[1]);
//     std::string wtnsfile(argv[2]);
  
//     // auto t_start = std::chrono::high_resolution_clock::now();

//    Circom_Circuit *circuit = loadCircuit(datfile);

//    Circom_CalcWit *ctx = new Circom_CalcWit(circuit);
//    json j;
//    j["in"] = 256;
//    loadJson(ctx, j);
//    if (ctx->getRemaingInputsToBeSet()!=0) {
//      std::cerr << "Not all inputs have been set. Only " << get_main_input_signal_no()-ctx->getRemaingInputsToBeSet() << " out of " << get_main_input_signal_no() << std::endl;
//      assert(false);
//    }
   
//      for (uint i = 0; i<get_size_of_witness(); i++){
//      FrElement x;
//      ctx->getWitness(i, &x);
//      std::cout << i << ": " << Fr_element2str(&x) << std::endl;
//      }
   
  
//    //auto t_mid = std::chrono::high_resolution_clock::now();
//    //std::cout << std::chrono::duration<double, std::milli>(t_mid-t_start).count()<<std::endl;

//   //  writeBinWitness(ctx,wtnsfile);
  
//    //auto t_end = std::chrono::high_resolution_clock::now();
//    //std::cout << std::chrono::duration<double, std::milli>(t_end-t_mid).count()<<std::endl;

//   }  
// }




	