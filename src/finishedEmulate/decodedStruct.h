#ifndef DECODED_STRUCT_H
#define DECODED_STRUCT_H

//STRUCTS FOR EACH OF THE FOUR INSTRUCTION TYPES

struct data_processing_decoded {

  //Execute only if this condition is met
  enum Cond cond;                           
  //1 (or true) means operand2 is an immediate constant,
  //otherwise it is a shifted register
  bool immediate_operand_bit;               
  enum Opcode opcode;
  //1 (or true) means CSPR flags should be updated during execution
  bool set_condition_bit;                   
  //Register number, NOT what is stored inside register
  uint32_t operand_1_reg;                   
  uint32_t dest_reg;
  //Can be register or immediate operand, both are uint32_t
  uint32_t operand_2;                       
  //Can be 1 or 0
  uint32_t carry;

};

struct multiply_decoded {

  enum Cond cond;
  //1 (or true) means Rd = (Rm * Rs) + Rn, otherwise just Rd = (Rm * Rs)
  bool accumulate_bit;                      
  bool set_condition_bit;
  uint32_t dest_reg;
  uint32_t multiply_operand_1_reg;
  uint32_t multiply_operand_2_reg;
  uint32_t accumulating_operand_reg;

};

struct single_data_transfer_decoded {

  enum Cond cond;
  //1 (or true) means offset is a shifted reg, 
  //otherwise it is unsigned 12 bit immediate offset
  bool immediate_bit;                       
  //1 (or true) means pre-indexing (offset added/subtracted to base reg before   
  //transfer), otherwise it means post-indexing (offset added/subtracted after)
  //In this task, pre-indexing won't change contents of base reg
  bool pre_or_post_indexing_bit;            
  //1 (or true) means offset added to base reg, otherwise subtracted
  bool up_bit;                              
  //1 (or true) means word is loaded from memory, otherwise stored in memory
  bool load_or_store_bit;                   
  uint32_t base_reg;
  uint32_t src_or_dst_reg;
  uint32_t offset;                          
  uint32_t carry;                         

};

struct branch_decoded {

  enum Cond cond;
  //SIGNED (two's complement) 26 bit offset
  int32_t offset;                   

};

//A decoded instruction can only be one of the four structs above
union decoded_instruction {                 

  struct data_processing_decoded data_processing;
  struct multiply_decoded multiply;
  struct single_data_transfer_decoded single_data_transfer;
  struct branch_decoded branch;

};

//A DECODED INSTRUCTION NEEDS TO BE CARRIED AROUND WITH ITS INSTRUCTION TYPE 
//(As otherwise we can't find out what is in the union at any given moment)
struct decoded_final {

  union decoded_instruction decoded_instr;
  enum Instruction_Type type;

}; //

void decode(uint32_t instruction);

//Will be used to store the next instruction be executed, which was
//the last to be decoded, in the pipeline
extern struct decoded_final Decoded;     

#endif
