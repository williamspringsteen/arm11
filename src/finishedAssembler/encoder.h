#ifndef ENCODER
#define ENCODER
#include<stdint.h>
#include "tuple.h"

//Structs to hold all appropriate bits for each type of instruction.
//The bits are all shifted so that they are in the correct place, 
//i.e. the cond part is shifted to be bits 31 - 28.

//Data processing stuct
struct DP {
  
  uint32_t cond; 
  uint32_t i;
  uint32_t opcode;
  uint32_t s;
  uint32_t rn;
  uint32_t rd;
  uint32_t operand2;
  
};

//Multiply struct
struct M {
  
  uint32_t cond; 
  uint32_t a;
  uint32_t s;
  uint32_t rd;
  uint32_t rn;
  uint32_t rs;
  uint32_t rm;
  
};

//Single data transfer struct
struct SDT {
  
  uint32_t cond; 
  uint32_t i;
  uint32_t p;
  uint32_t u;
  uint32_t l;
  uint32_t rn;
  uint32_t rd;
  uint32_t offset;
  
};

//Branch struct
struct B {
  
  uint32_t cond; 
  uint32_t offset;
  
};

uint32_t encode(char instruction[], uint32_t memory[], int memorySize, 
		struct stringIntTuple* lookUpTable, int currentInstrAddress);

#endif
