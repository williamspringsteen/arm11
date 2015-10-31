//Values are the code in an instruction for each condition
enum Cond {
                
  EQ = 0x0,                //Equal: Z set
  NE = 0x1,                //Not Equal: Z clear
  GE = 0xA,                //Greater than or equal to: N equals V
  LT = 0xB,                //Less than: N not equal to V
  GT = 0xC,                //Greater than: Z clear AND (N equals V)
  LE = 0xD,                //Less than or equal to: Z set OR (N not equal to V)
  AL = 0xE                 //Always: Ignore CSPR flag bits, always succeeds

};

//Values are position of the flag bit in CPSR
enum CPSRFlagBit {
                                           
  N = 31,                  //Last result was NEGATIVE
  Z = 30,                  //Last result was ZERO
  C = 29,                  //Last result caused a bit to be CARRIED out
  V = 28                   //Last result OVERFLOWED
  
};

//Values are the code in an instruction for each opcode
enum Opcode {
                                           
  AND = 0x0,               //Rn AND operand2 (Writes result)
  EOR = 0x1,               //Rn XOR operand2 (Writes result)
  SUB = 0x2,               //Rn - operand2 (Writes result)
  RSB = 0x3,               //operand2 - Rn
  ADD = 0x4,               //Rn + operand2
  TST = 0x8,               //Same as AND but doesn't write result
  TEQ = 0x9,               //Same as EOR but doesn't write result
  CMP = 0xA,               //Same as SUB but doesn't write result
  ORR = 0xC,               //Rn OR operand2
  MOV = 0xD                //operand2 (Ignores Rn)

};

//Extra Bits for specific instructions
enum InstructionBits {
  
  IShift = 25,
  PShift = 24,
  UShift = 23,
  AShift = 21,
  SET_LOAD_SHIFT = 20
  
};

enum DPShifts {
 
  OP_SHIFT = 21,
  DP_RD = 12,
  DP_RN = 16
  
};

enum SDTShifts {
 
  SDT_RD = 12,
  SDT_RN = 16,
  SDT_SET_BIT = 26
  
};

enum MShifts {

  M_RS = 8,
  M_RN = 12,
  M_RD = 16,
  SDT_SET_BITS = 4
  
};
