#include <stdio.h>
#include <iostream>
#include <assert.h>
#include "circom.hpp"
#include "calcwit.hpp"
void BabyAdd_0_create(uint soffset,uint coffset,Circom_CalcWit* ctx,std::string componentName,uint componentFather);
void BabyAdd_0_run(uint ctx_index,Circom_CalcWit* ctx);
void Main_1_create(uint soffset,uint coffset,Circom_CalcWit* ctx,std::string componentName,uint componentFather);
void Main_1_run(uint ctx_index,Circom_CalcWit* ctx);
Circom_TemplateFunction _functionTable[2] = { 
BabyAdd_0_run,
Main_1_run };
Circom_TemplateFunction _functionTableParallel[2] = { 
NULL,
NULL };
uint get_main_input_signal_start() {return 3;}

uint get_main_input_signal_no() {return 4;}

uint get_total_signal_no() {return 17;}

uint get_number_of_components() {return 2;}

uint get_size_of_input_hashmap() {return 256;}

uint get_size_of_witness() {return 11;}

uint get_size_of_constants() {return 5;}

uint get_size_of_io_map() {return 0;}

void release_memory_component(Circom_CalcWit* ctx, uint pos) {{

delete[] ctx->componentMemory[pos].subcomponents;

// delete ctx->componentMemory[pos].subcomponentsParallel;

// delete ctx->componentMemory[pos].outputIsSet;

// delete ctx->componentMemory[pos].mutexes;

// delete ctx->componentMemory[pos].cvs;

// delete ctx->componentMemory[pos].sbct;

}}


// function declarations

// generate extra counters start
#include <cstring>
#include <gmpxx.h>
static const size_t ELEMENTS_COUNT = 14;
static const size_t CHECKS_COUNT = 3;

#ifdef __linux__
__attribute__((section("__libfuzzer_extra_counters")))
#endif
static uint8_t extra_counters[ELEMENTS_COUNT + ELEMENTS_COUNT * 253 + 1];

static uint8_t is_not_bool_elements[ELEMENTS_COUNT];
static size_t proccess_count = 0;

void proccess_bool_element(PFrElement signal_element, size_t index){
    char* str_element = Fr_element2str(signal_element);
    if (strlen(str_element) != 1 || (str_element[0] != '0' && str_element[0] != '1')){
        is_not_bool_elements[index] = 1;
    }
    if (index == ELEMENTS_COUNT - 1) ++proccess_count;
    if (proccess_count < CHECKS_COUNT || is_not_bool_elements[index]){
        free(str_element);
        return;
    }
    extra_counters[index] = (str_element[0] == '1');
    free(str_element);
}

void proccess_overflow_element(PFrElement signal_element, size_t index){
    char* str_element = Fr_element2str(signal_element);
    mpz_class element;
    element = str_element;
    int highest_bit = mpz_sizeinbase(element.get_mpz_t(), 2) - 1;
    extra_counters[ELEMENTS_COUNT + index*253 + highest_bit] = 1;
    free(str_element);
}
// generate extra counters end

// template declarations
void BabyAdd_0_create(uint soffset,uint coffset,Circom_CalcWit* ctx,std::string componentName,uint componentFather){
ctx->componentMemory[coffset].templateId = 0;
ctx->componentMemory[coffset].templateName = "BabyAdd";
ctx->componentMemory[coffset].signalStart = soffset;
ctx->componentMemory[coffset].inputCounter = 4;
ctx->componentMemory[coffset].componentName = componentName;
ctx->componentMemory[coffset].idFather = componentFather;
ctx->componentMemory[coffset].subcomponents = new uint[0];
}

void BabyAdd_0_run(uint ctx_index,Circom_CalcWit* ctx){
FrElement* signalValues = ctx->signalValues;
u64 mySignalStart = ctx->componentMemory[ctx_index].signalStart;
std::string myTemplateName = ctx->componentMemory[ctx_index].templateName;
std::string myComponentName = ctx->componentMemory[ctx_index].componentName;
u64 myFather = ctx->componentMemory[ctx_index].idFather;
u64 myId = ctx_index;
u32* mySubcomponents = ctx->componentMemory[ctx_index].subcomponents;
bool* mySubcomponentsParallel = ctx->componentMemory[ctx_index].subcomponentsParallel;
FrElement* circuitConstants = ctx->circuitConstants;
std::string* listOfTemplateMessages = ctx->listOfTemplateMessages;
FrElement expaux[8];
FrElement lvar[2];
uint sub_component_aux;
{
PFrElement aux_dest = &lvar[0];
// load src
// end load src
Fr_copy(aux_dest,&circuitConstants[0]);

// generate extra counters start
proccess_overflow_element(aux_dest, 0);
// generate extra counters end

// generate extra counters start
proccess_bool_element(aux_dest, 0);
// generate extra counters end

}
{
PFrElement aux_dest = &lvar[1];
// load src
// end load src
Fr_copy(aux_dest,&circuitConstants[1]);

// generate extra counters start
proccess_overflow_element(aux_dest, 1);
// generate extra counters end

// generate extra counters start
proccess_bool_element(aux_dest, 1);
// generate extra counters end

}
{
PFrElement aux_dest = &signalValues[mySignalStart + 6];
// load src
Fr_mul(&expaux[0],&signalValues[mySignalStart + 2],&signalValues[mySignalStart + 5]); // line circom 40
// end load src
Fr_copy(aux_dest,&expaux[0]);

// generate extra counters start
proccess_overflow_element(aux_dest, 2);
// generate extra counters end

// generate extra counters start
proccess_bool_element(aux_dest, 2);
// generate extra counters end

}
{
PFrElement aux_dest = &signalValues[mySignalStart + 7];
// load src
Fr_mul(&expaux[0],&signalValues[mySignalStart + 3],&signalValues[mySignalStart + 4]); // line circom 41
// end load src
Fr_copy(aux_dest,&expaux[0]);

// generate extra counters start
proccess_overflow_element(aux_dest, 3);
// generate extra counters end

// generate extra counters start
proccess_bool_element(aux_dest, 3);
// generate extra counters end

}
{
PFrElement aux_dest = &signalValues[mySignalStart + 8];
// load src
Fr_mul(&expaux[2],&circuitConstants[2],&signalValues[mySignalStart + 2]); // line circom 42
Fr_add(&expaux[1],&expaux[2],&signalValues[mySignalStart + 3]); // line circom 42
Fr_add(&expaux[2],&signalValues[mySignalStart + 4],&signalValues[mySignalStart + 5]); // line circom 42
Fr_mul(&expaux[0],&expaux[1],&expaux[2]); // line circom 42
// end load src
Fr_copy(aux_dest,&expaux[0]);

// generate extra counters start
proccess_overflow_element(aux_dest, 4);
// generate extra counters end

// generate extra counters start
proccess_bool_element(aux_dest, 4);
// generate extra counters end

}
{
PFrElement aux_dest = &signalValues[mySignalStart + 9];
// load src
Fr_mul(&expaux[0],&signalValues[mySignalStart + 6],&signalValues[mySignalStart + 7]); // line circom 43
// end load src
Fr_copy(aux_dest,&expaux[0]);

// generate extra counters start
proccess_overflow_element(aux_dest, 5);
// generate extra counters end

// generate extra counters start
proccess_bool_element(aux_dest, 5);
// generate extra counters end

}
{
PFrElement aux_dest = &signalValues[mySignalStart + 0];
// load src
Fr_add(&expaux[1],&signalValues[mySignalStart + 6],&signalValues[mySignalStart + 7]); // line circom 45
Fr_mul(&expaux[4],&circuitConstants[1],&signalValues[mySignalStart + 9]); // line circom 45
Fr_add(&expaux[2],&circuitConstants[3],&expaux[4]); // line circom 45
Fr_div(&expaux[0],&expaux[1],&expaux[2]); // line circom 45
// end load src
Fr_copy(aux_dest,&expaux[0]);

// generate extra counters start
proccess_overflow_element(aux_dest, 6);
// generate extra counters end

// generate extra counters start
proccess_bool_element(aux_dest, 6);
// generate extra counters end

}
Fr_mul(&expaux[4],&circuitConstants[1],&signalValues[mySignalStart + 9]); // line circom 46
Fr_add(&expaux[2],&circuitConstants[3],&expaux[4]); // line circom 46
Fr_mul(&expaux[1],&expaux[2],&signalValues[mySignalStart + 0]); // line circom 46
Fr_add(&expaux[2],&signalValues[mySignalStart + 6],&signalValues[mySignalStart + 7]); // line circom 46
Fr_eq(&expaux[0],&expaux[1],&expaux[2]); // line circom 46
if (!Fr_isTrue(&expaux[0])) std::cout << "Failed assert in template/function " << myTemplateName << " line 46. " <<  "Followed trace of components: " << ctx->getTrace(myId) << std::endl;
assert(Fr_isTrue(&expaux[0]));
{
PFrElement aux_dest = &signalValues[mySignalStart + 1];
// load src
Fr_mul(&expaux[4],&circuitConstants[0],&signalValues[mySignalStart + 6]); // line circom 48
Fr_add(&expaux[2],&signalValues[mySignalStart + 8],&expaux[4]); // line circom 48
Fr_sub(&expaux[1],&expaux[2],&signalValues[mySignalStart + 7]); // line circom 48
Fr_mul(&expaux[4],&circuitConstants[1],&signalValues[mySignalStart + 9]); // line circom 48
Fr_sub(&expaux[2],&circuitConstants[3],&expaux[4]); // line circom 48
Fr_div(&expaux[0],&expaux[1],&expaux[2]); // line circom 48
// end load src
Fr_copy(aux_dest,&expaux[0]);

// generate extra counters start
proccess_overflow_element(aux_dest, 7);
// generate extra counters end

// generate extra counters start
proccess_bool_element(aux_dest, 7);
// generate extra counters end

}
Fr_mul(&expaux[4],&circuitConstants[1],&signalValues[mySignalStart + 9]); // line circom 49
Fr_sub(&expaux[2],&circuitConstants[3],&expaux[4]); // line circom 49
Fr_mul(&expaux[1],&expaux[2],&signalValues[mySignalStart + 1]); // line circom 49
Fr_mul(&expaux[5],&circuitConstants[0],&signalValues[mySignalStart + 6]); // line circom 49
Fr_add(&expaux[3],&signalValues[mySignalStart + 8],&expaux[5]); // line circom 49
Fr_sub(&expaux[2],&expaux[3],&signalValues[mySignalStart + 7]); // line circom 49
Fr_eq(&expaux[0],&expaux[1],&expaux[2]); // line circom 49
if (!Fr_isTrue(&expaux[0])) std::cout << "Failed assert in template/function " << myTemplateName << " line 49. " <<  "Followed trace of components: " << ctx->getTrace(myId) << std::endl;
assert(Fr_isTrue(&expaux[0]));
for (uint i = 0; i < 0; i++){
uint index_subc = ctx->componentMemory[ctx_index].subcomponents[i];
release_memory_component(ctx,index_subc);
}
}

void Main_1_create(uint soffset,uint coffset,Circom_CalcWit* ctx,std::string componentName,uint componentFather){
ctx->componentMemory[coffset].templateId = 1;
ctx->componentMemory[coffset].templateName = "Main";
ctx->componentMemory[coffset].signalStart = soffset;
ctx->componentMemory[coffset].inputCounter = 4;
ctx->componentMemory[coffset].componentName = componentName;
ctx->componentMemory[coffset].idFather = componentFather;
ctx->componentMemory[coffset].subcomponents = new uint[1];
}

void Main_1_run(uint ctx_index,Circom_CalcWit* ctx){
FrElement* signalValues = ctx->signalValues;
u64 mySignalStart = ctx->componentMemory[ctx_index].signalStart;
std::string myTemplateName = ctx->componentMemory[ctx_index].templateName;
std::string myComponentName = ctx->componentMemory[ctx_index].componentName;
u64 myFather = ctx->componentMemory[ctx_index].idFather;
u64 myId = ctx_index;
u32* mySubcomponents = ctx->componentMemory[ctx_index].subcomponents;
bool* mySubcomponentsParallel = ctx->componentMemory[ctx_index].subcomponentsParallel;
FrElement* circuitConstants = ctx->circuitConstants;
std::string* listOfTemplateMessages = ctx->listOfTemplateMessages;
FrElement expaux[2];
FrElement lvar[0];
uint sub_component_aux;
{
uint aux_create = 0;
int aux_cmp_num = 0+ctx_index+1;
uint csoffset = mySignalStart+6;
for (uint i = 0; i < 1; i++) {
std::string new_cmp_name = "baby";
BabyAdd_0_create(csoffset,aux_cmp_num,ctx,new_cmp_name,myId);
mySubcomponents[aux_create+i] = aux_cmp_num;
csoffset += 10 ;
aux_cmp_num += 1;
}
}
{
uint cmp_index_ref = 0;
{
PFrElement aux_dest = &ctx->signalValues[ctx->componentMemory[mySubcomponents[cmp_index_ref]].signalStart + 2];
// load src
// end load src
Fr_copy(aux_dest,&signalValues[mySignalStart + 2]);

// generate extra counters start
proccess_overflow_element(aux_dest, 8);
// generate extra counters end

// generate extra counters start
proccess_bool_element(aux_dest, 8);
// generate extra counters end

}
// no need to run sub component
assert(--ctx->componentMemory[mySubcomponents[cmp_index_ref]].inputCounter);
}
{
uint cmp_index_ref = 0;
{
PFrElement aux_dest = &ctx->signalValues[ctx->componentMemory[mySubcomponents[cmp_index_ref]].signalStart + 3];
// load src
// end load src
Fr_copy(aux_dest,&signalValues[mySignalStart + 3]);

// generate extra counters start
proccess_overflow_element(aux_dest, 9);
// generate extra counters end

// generate extra counters start
proccess_bool_element(aux_dest, 9);
// generate extra counters end

}
// no need to run sub component
assert(--ctx->componentMemory[mySubcomponents[cmp_index_ref]].inputCounter);
}
{
uint cmp_index_ref = 0;
{
PFrElement aux_dest = &ctx->signalValues[ctx->componentMemory[mySubcomponents[cmp_index_ref]].signalStart + 4];
// load src
// end load src
Fr_copy(aux_dest,&signalValues[mySignalStart + 4]);

// generate extra counters start
proccess_overflow_element(aux_dest, 10);
// generate extra counters end

// generate extra counters start
proccess_bool_element(aux_dest, 10);
// generate extra counters end

}
// no need to run sub component
assert(--ctx->componentMemory[mySubcomponents[cmp_index_ref]].inputCounter);
}
{
uint cmp_index_ref = 0;
{
PFrElement aux_dest = &ctx->signalValues[ctx->componentMemory[mySubcomponents[cmp_index_ref]].signalStart + 5];
// load src
// end load src
Fr_copy(aux_dest,&signalValues[mySignalStart + 5]);

// generate extra counters start
proccess_overflow_element(aux_dest, 11);
// generate extra counters end

// generate extra counters start
proccess_bool_element(aux_dest, 11);
// generate extra counters end

}
// need to run sub component
assert(!(--ctx->componentMemory[mySubcomponents[cmp_index_ref]].inputCounter));
BabyAdd_0_run(mySubcomponents[cmp_index_ref],ctx);
}
{
PFrElement aux_dest = &signalValues[mySignalStart + 0];
// load src
// end load src
Fr_copy(aux_dest,&ctx->signalValues[ctx->componentMemory[mySubcomponents[0]].signalStart + 0]);

// generate extra counters start
proccess_overflow_element(aux_dest, 12);
// generate extra counters end

// generate extra counters start
proccess_bool_element(aux_dest, 12);
// generate extra counters end

}
{
PFrElement aux_dest = &signalValues[mySignalStart + 1];
// load src
// end load src
Fr_copy(aux_dest,&ctx->signalValues[ctx->componentMemory[mySubcomponents[0]].signalStart + 1]);

// generate extra counters start
proccess_overflow_element(aux_dest, 13);
// generate extra counters end

// generate extra counters start
proccess_bool_element(aux_dest, 13);
// generate extra counters end

}
for (uint i = 0; i < 1; i++){
uint index_subc = ctx->componentMemory[ctx_index].subcomponents[i];
release_memory_component(ctx,index_subc);
}
delete[] mySubcomponents;
}

void run(Circom_CalcWit* ctx){
Main_1_create(1,0,ctx,"main",0);
Main_1_run(0,ctx);
}

