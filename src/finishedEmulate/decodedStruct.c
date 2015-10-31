//HOLDS THE STUCTURE LAYOUT FOR DECODE AND DECODE FUNCTION
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "machineStruct.h"
#include "emulateEnums.h"
#include "decodedStruct.h"
#include "binMethods.h"
#include "shiftStruct.h"

//DECODE FUNCTION
//CHANGES THE DECODED_FINAL STRUCT AT THE TOP
void decode(uint32_t instruction) {  

  union decoded_instruction decoded;
  enum Instruction_Type type;
  
  if (instruction == 0) {         // DECODING HALT
    
    //Decoding an all 0 intruction gives a HALT (Termination when executed)
    type = H;                     

  } else if (getSpecificBit(instruction, BRANCHIDBIT) == 1) { //DECODING BRANCH

    struct branch_decoded branch_instr;
    type = B;
    
    enum Cond condition;
    condition = binaryToIntRange(CONDBITSIZE, FIRSTCONDBIT, instruction);
    branch_instr.cond = condition;

    branch_instr.offset = branchOffsetGetter(instruction);

    decoded.branch = branch_instr;

  } else if (getSpecificBit(instruction, SDT_SET_BIT) == 1) {       //DECODING SDT

    struct single_data_transfer_decoded single_data_transfer_instr;
    type = SDT;
    
    enum Cond condition;
    condition = binaryToIntRange(CONDBITSIZE, FIRSTCONDBIT, instruction);
    single_data_transfer_instr.cond = condition;

    single_data_transfer_instr.immediate_bit = 
                                          getSpecificBit(instruction, ISHIFT);

    single_data_transfer_instr.pre_or_post_indexing_bit = 
                                     getSpecificBit(instruction, PSHIFT) == 1;

    single_data_transfer_instr.up_bit = 
                                     getSpecificBit(instruction, USHIFT) == 1;

    single_data_transfer_instr.load_or_store_bit = 
                                     getSpecificBit(instruction, SET_LOAD_SHIFT) == 1;

    single_data_transfer_instr.base_reg = 
               binaryToIntRange(BITSINREGNUM, SDT_RN, instruction);

    single_data_transfer_instr.src_or_dst_reg = 
             binaryToIntRange(BITSINREGNUM, SDT_RD, instruction);
    
    if (!single_data_transfer_instr.immediate_bit) {      //Offset is immediate

      single_data_transfer_instr.offset = 
                               binaryToIntRange(SDTOFFSETSIZE, 0, instruction);
     
    } else {     //Offset is shifted register

       int shiftValue;
       
       if((getSpecificBit(instruction, SHIFTEDREGID1) == 1) && 
	                 (getSpecificBit(instruction, SHIFTEDREGID0) == 0)) {
 
         //registernum is rs
         int registernum = 
                 binaryToIntRange(BITSINREGNUM, FIRSTSHIFTREGBIT, instruction);
         uint32_t mask = 0xFF;
         //shiftValue equals first 8 bits of registernum
         shiftValue = machine.registers[registernum] & mask;  
 
       } else {
 
	 //Number to shift by
         shiftValue = 
             binaryToIntRange(SHIFTVALUESIZE, FIRSTSHIFTVALUEBIT, instruction);

       }

      //Rm from diagram
      int regNumber = binaryToIntRange(BITSINREGNUM, 0, instruction);    
      
      //Offset is shifted register
      enum ShiftCode shiftCode = binaryToIntRange(SHIFTCODESIZE, FIRSTSHIFTCODEBIT, instruction);    

      struct shift_struct* shifted = shift(shiftCode, shiftValue, regNumber);

      single_data_transfer_instr.offset = shifted->offset;

      single_data_transfer_instr.carry = shifted->carry;
      
    }
    
    decoded.single_data_transfer = single_data_transfer_instr;

  } else if (getSpecificBit(instruction, ISHIFT) || //DECODING DATA PROCESSING 
                getSpecificBit(instruction, SHIFTEDREGID1) == 0 || 
                ((getSpecificBit(instruction, SHIFTEDREGID1) == 1) && 
                         (getSpecificBit(instruction, SHIFTEDREGID0) == 0))) {      
    
    struct data_processing_decoded data_processing_instr;
    type = DP;

    enum Cond condition = binaryToIntRange(CONDBITSIZE, FIRSTCONDBIT, instruction);
    
    data_processing_instr.cond = condition;

    data_processing_instr.immediate_operand_bit = 
                                     getSpecificBit(instruction, ISHIFT) == 1;
    
    enum Opcode opcode = binaryToIntRange(DPOPCODESIZE, DP_OP_SHIFT, instruction);

    data_processing_instr.opcode = opcode;

    data_processing_instr.set_condition_bit = 
                                      getSpecificBit(instruction, SETCONDBIT) == 1;

    data_processing_instr.operand_1_reg = binaryToIntRange(BITSINREGNUM, FIRSTDPOPERAND1BIT, instruction);

    data_processing_instr.dest_reg = signedBinaryToIntRange(BITSINREGNUM, FIRSTDPDSTREGBIT, instruction);
    
    if (data_processing_instr.immediate_operand_bit) {
      
      int immediate = binaryToIntRange(DPIMMEDIATESIZE, 0, instruction);  

      int rotateValue = 2 * binaryToIntRange(DPROTATEVALUESIZE, FIRSTDPROTATEVALUEVBIT, instruction);

      immediate = rotateRight(immediate, rotateValue);
      
      data_processing_instr.operand_2 = immediate;
      
      data_processing_instr.carry = getSpecificBit(immediate, BITSINWORD - 1);
      
    } else {
      
      int shiftValue;
      if((getSpecificBit(instruction, SHIFTEDREGID1) == 1) && 
                           (getSpecificBit(instruction, SHIFTEDREGID0) == 0)) {
            
        int registernum = binaryToIntRange(BITSINREGNUM, FIRSTSHIFTREGBIT, instruction);
        uint32_t mask = 0xFF;
        shiftValue = machine.registers[registernum] & mask;
             
      } else {

        shiftValue = binaryToIntRange(SHIFTVALUESIZE, FIRSTSHIFTVALUEBIT, instruction);  //Number to shift by
   
      }

      int regNumber = binaryToIntRange(BITSINREGNUM, 0, instruction);      //Rm

      //Offset is shifted register
      enum ShiftCode shiftCode = binaryToIntRange(SHIFTCODESIZE, FIRSTSHIFTCODEBIT, instruction);            

      struct shift_struct* shifted = shift(shiftCode, shiftValue, regNumber);

      data_processing_instr.operand_2 = shifted->offset;

      data_processing_instr.carry = shifted->carry;
      
    }

    decoded.data_processing = data_processing_instr;
    
  } else {                                               //DECODING MULTIPLY
    
    struct multiply_decoded multiply_instr;
    type = M;

    enum Cond condition = binaryToIntRange(CONDBITSIZE, FIRSTCONDBIT, instruction);
    multiply_instr.cond = condition;

    multiply_instr.accumulate_bit = getSpecificBit(instruction, MULTACCUMULATEBIT) == 1;

    multiply_instr.set_condition_bit = getSpecificBit(instruction, SETCONDBIT) == 1;

    multiply_instr.dest_reg = binaryToIntRange(BITSINREGNUM, M_RD, instruction);
    
    multiply_instr.multiply_operand_1_reg = binaryToIntRange(BITSINREGNUM, M_RS, instruction);

    multiply_instr.multiply_operand_2_reg = binaryToIntRange(BITSINREGNUM, 0, instruction);

    multiply_instr.accumulating_operand_reg = binaryToIntRange(BITSINREGNUM, M_RN, instruction);
    
    decoded.multiply = multiply_instr;

  }

  //Set the global variable Decoded to hold this decoded instruction
  Decoded.decoded_instr = decoded;
  Decoded.type = type;

}
//END OF DECODE
