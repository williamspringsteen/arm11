#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include "emulateEnums.h"
#include "binMethods.h"
#include "machineStruct.h"
#include "shiftStruct.h"

//Returns the bit number 'location' from the input (Where bit 0 is rightmost)
int getSpecificBit(uint32_t input, int location) {

  int mask = 1 << location;
  
  mask &= input;
  
  return mask >> location;
  
}

//Returns the new number after setting the bit number 'location' from the input
//to 'carry'
uint32_t setSpecificBit(uint32_t instruction, uint32_t carry, int location){
  
  carry = carry << location; 

  uint32_t maskleft = (instruction >> (location + 1)) << (location + 1);
  uint32_t maskright = (instruction << (BITSINWORD -location)) 
                                 >> (BITSINWORD-location);
     
  return (carry | maskleft | maskright);

}

//Returns true if address is 0x20200000, 0x20200004, or 0x20200008, as these
//are the memory addresses of the first 29 GPIO pin control bits
bool isGPIOPin(uint32_t address) {
  
  switch (address) {
   
    case(GPIO_OUTPUT_PINS_0): 
                        printf("One GPIO pin from 0 to 9 has been accessed\n");
                        break;
      
    case(GPIO_OUTPUT_PINS_1): 
                        printf("One GPIO pin from 10 to 19 has been accessed\n");
                        break;
      
    case(GPIO_OUTPUT_PINS_2): 
                        printf("One GPIO pin from 20 to 29 has been accessed\n");
                        break;
      
    default              :
                        return false;
    
  }
  
  return true;
  
}

//Returns true if address is 0x2020001C (Output set) or 0x20200028 (Output 
//clear), doesn't print anything like isGPIOPin does
bool isGPIOSetOrClear(uint32_t address) {
 
  return (address == GPIO_OUTPUT_SET_0 || address == GPIO_OUTPUT_CLEAR_0);
  
}

//Prints the bits of a supplied number to stdout
void printBits(uint32_t x) {              

  uint32_t mask = 1 << (BITSINWORD - 1);

  for(int i = 0; i< BITSINWORD; ++i) {

    if((x & mask) == 0) {
      printf("0");
    } else {
      printf("1");
    }

    x = x << 1;

  }

  printf("\n");

}

//Returns the integer of a certain range of a number 'instruction' (Range of
//width 'width', 'maskShift' bits from the right of the binary number)
//Inclusive on both ends of the range.
//binaryToIntRange(3, 1, 0b10011010) = 4, as 0b101 = 4.
uint32_t binaryToIntRange(int width, int maskShift, uint32_t instruction) { 
  
  uint32_t output;                          
  
  // makes mask and shifts to right position
  int mask = ((1 << width) - 1) << maskShift;          
  
  // ands mask and intstruction then shifts back to right 
  output = (instruction & mask) >> maskShift;     

  return output;
  
}

//Same as above but treats the range as a two's complement signed integer
//signedBinaryToIntRange(3, 1, 0b10011010) = -3, as 0b101 = -3 in
//two's complement representation.
int signedBinaryToIntRange(int width, int maskShift, uint32_t instruction) { 
                                                                                
  int output;                                  
                         
  // makes mask and shifts to right position
  int mask = ((1 << width) - 1) << maskShift;     
  
   // makes mask for signBit;
  int signMask = 1 << (width - 1);                
  
  //finds number in range and shifts back
  output = (instruction & mask) >> maskShift;       
  
  //finds signBit;
  int signBit = (output & signMask) >> (width - 1); 
  
  if (signBit == 1) {
    mask >>= 1;                         //if signBit = 1 shift mask one right; 
    output &= mask;                     //take away sign bit
    output = -output;                   //make number negative;
  }
  
  return output;
  
}

//Shifts register according to the shiftCode and shiftNumber 
//ShiftCode tells us what type of shift to do, shiftNumber tells 
//us how much to shift by
struct shift_struct* shift(enum ShiftCode shiftCode, int shiftNumber, int regNum) {    
                                                           
  uint32_t registerValue = machine.registers[regNum];
  int carry = 0;
  int bit31;

  switch(shiftCode) {

    case LSL :    //Logical shift left
      
      for (int i = 0; i < shiftNumber; i++) {
        carry = getSpecificBit(registerValue, BITSINWORD - 1);
        registerValue <<= 1;
      }
      
      break;

    case LSR :    //Logical shift right

      for (int i = 0; i < shiftNumber; i++) {
        carry = getSpecificBit(registerValue,0);
        registerValue >>= 1;
      }
      
      break;

    case ASR :    //Arithmetic shift right

      bit31 = getSpecificBit(registerValue, BITSINWORD - 1);
      
      for (int i = 0; i < shiftNumber; i++) {
        carry = getSpecificBit(registerValue,0);
        registerValue >>= 1;
        registerValue = setSpecificBit(registerValue, BITSINWORD - 1, bit31);
      }
      
      break;

    case ROR :    //Rotate right

      for (int i = 0; i < shiftNumber; i++) {
        carry = getSpecificBit(registerValue,0);
        registerValue >>= 1;
        registerValue = setSpecificBit(registerValue, BITSINWORD - 1, carry);
      }
      
      break;

  }

  //NEEDS TO BE FREED LATER ON
  struct shift_struct* shiftedPtr = malloc(sizeof(struct shift_struct));  

  //This is the struct to be returned that will hold all information required
  struct shift_struct shifted;                              

  shiftedPtr[0] = shifted;       

  shiftedPtr[0].carry = carry;
  shiftedPtr[0].offset = registerValue;    

  return shiftedPtr;                       //Returns pointer to the struct

}

//Rotates the input right 'rotateValue' times
int rotateRight(uint32_t input, int rotateValue) {   

  int movingBit;

  for (int i = 0; i < rotateValue; i++) {   
    movingBit = getSpecificBit(input, 0);
    input >>= 1;
    input = setSpecificBit(input, movingBit, BITSINWORD - 1);
  }

  return input;
  
}

//Calculates the offset for use when decoding a branch instruction.
//Final 24 bits are taken, shifted left by 2, and it is then
//sign extended to 32 bits.
//Final offset can be negative (signed 32-bit int returned)
//Doesn't have 8 subtracted from it, that happens in execute
int32_t branchOffsetGetter(uint32_t instruction) { 

  int32_t mask = 0xFFFFFF;
  int32_t offset = mask & instruction;
  offset <<= 2;
  
  int32_t extendingBit = getSpecificBit(offset, BRANCHOFFSETSIGNBIT);

  for (int i = BRANCHOFFSETSIGNBIT + 1; i < BITSINWORD; i++) {
    offset = setSpecificBit(offset, extendingBit, i);
  }
  
  return offset;

}


// gets a word and puts it in the right order from right to left 
uint32_t getWord(int memoryAddress) {
  
  uint32_t instr = 0;
  
  for (int i = BYTESINWORD - 1; i >= 0; i--) {
    instr <<= BITSINBYTE;
    uint32_t currentByte = machine.memory[memoryAddress + i];
    instr += currentByte;
  }
  
  return instr;

} 

//Only used when printing the non-zero memory upon termination of the program.
//Like getWord above, but returns it in little endian form.
uint32_t getWordLittleEndian(int memoryAddress) {

  uint32_t instr = 0;

  for (int i = 0; i <= BYTESINWORD - 1; i++) {
    instr <<= BITSINBYTE;
    uint32_t currentByte = machine.memory[memoryAddress + i];
    instr += currentByte;
  }
  
  return instr;

}

//Puts the 32-bit word 'data' into memory at address 'memoryAddress'
void putWord(int memoryAddress, uint32_t data) {
  
  uint32_t mask = 0xFF;    //In binary this is 11111111 (8 1's) 
 
  for (int i = 0; i <= BYTESINWORD - 1; i++) {
    uint32_t currentByte = data & mask;
    machine.memory[i + memoryAddress] = currentByte;
    data = data >> BITSINBYTE;
  }
  
}

//Prints all non-zero memory
void printMemory() {
  
  printf("%s\n", "Non-zero memory:");
  
  int memory_address = 0;
  
  while(memory_address <= MEMSIZE - BYTESINWORD) {    // loop through VIRTUAL MACHINE memory
    
    if(getWord(memory_address) != 0) {
      printf("0x%08x: ", memory_address);
      printf("0x%08x\n", getWordLittleEndian(memory_address)); 
    }
    
    memory_address += BYTESINWORD;
     
  } 

} 

//Print values in all registers (r13 and r14 ignored)
void printRegisters() {

  printf("%s\n", "Registers:");

  // loop through virtual machine registers (r0 - r12)
  for(int i = 0; i < NUMGPREG; i++){
    printf("$");
    printf("%-3i", i);
    printf(":");
    printf(" %10d", machine.registers[i]);
    printf(" (0x%08x)\n", machine.registers[i]);
  }  
  
  //Need to print PC and CPSR registers separately 
  printf("%s  : ", "PC");
  printf("%10d", machine.registers[PCREG]);
  printf(" (0x%08x)\n", machine.registers[PCREG]);
  printf("%s: ", "CPSR");
  printf("%10d", machine.registers[CPSR]);
  printf(" (0x%08x)\n", machine.registers[CPSR]);

}

//Check that the given condition is met, as shown by the status of the CPSR
//register flag bits
bool checkCondition(enum Cond condition){
 
  uint32_t cpsrBits = machine.registers[CPSR];	

  switch(condition){
  
    case(EQ) : return getSpecificBit(cpsrBits,Z) == 1; 
    
    case(NE) : return getSpecificBit(cpsrBits,Z) == 0;
      
    case(GE) : return getSpecificBit(cpsrBits,N) == getSpecificBit(cpsrBits,V); 
      
    case(LT) : return getSpecificBit(cpsrBits,N) != getSpecificBit(cpsrBits,V);
      
    case(GT) : return (getSpecificBit(cpsrBits,Z) == 0) && (getSpecificBit(cpsrBits,N) == getSpecificBit(cpsrBits,V));
      
    case(LE) : return (getSpecificBit(cpsrBits,Z) == 1) || (getSpecificBit(cpsrBits,N) != getSpecificBit(cpsrBits,V));
      
    case(AL) : return true;
    
    default  : return false;  
    
  } 
    
}
