//This holds the stucture for the machine and global constants
#ifndef MACHINE
#define MACHINE
 
#define NUMREG 17
#define NUMGPREG 13
#define BITSINREGNUM 4
#define PCREG 15                
#define CPSR 16
#define MEMSIZE 65536           //2^16 bytes = 2^14 words (32 bit)
#define SINGLEREGSIZE 4         //4 bytes = 32-bit
#define BITSINWORD 32
#define BYTESINWORD 4
#define BITSINBYTE 8
#define BRANCHOFFSETSIGNBIT 25
#define BRANCHIDBIT 27
#define FIRSTCONDBIT 28
#define CONDBITSIZE 4
#define SDTOFFSETSIZE 12
#define SHIFTEDREGID1 4
#define SHIFTEDREGID0 7
#define SHIFTVALUESIZE 5
#define FIRSTSHIFTVALUEBIT 7
#define FIRSTSHIFTREGBIT 8
#define SHIFTCODESIZE 2
#define FIRSTSHIFTCODEBIT 5
#define DPOPCODESIZE 4
#define SETCONDBIT 20
#define FIRSTDPOPERAND1BIT 16
#define FIRSTDPDSTREGBIT 12
#define DPIMMEDIATESIZE 8
#define DPROTATEVALUESIZE 4
#define FIRSTDPROTATEVALUEVBIT 8
#define MULTACCUMULATEBIT 21

//OUR VIRTUAL MACHINE
struct Machine{     		

  uint8_t *memory; 		//array for memory (array of unsigned ints)
  uint32_t *registers;		//array for registers (array of unsigned ints)

};//

//Global variable for our virtual machine
//Defined in main
extern struct Machine machine;    

#endif
 


