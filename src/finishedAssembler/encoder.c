#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "encoder.h"
#include "assemblerUtils.h"
#include "assemblerEnums.h"

#define FIRST_COND_BIT 28
#define MAX_MOV 0xFF
#define PCREG 15 
#define WORD_SIZE 32


//helper functions for branch encode
uint32_t encodeB(char instruction[], struct stringIntTuple* lookUpTable,
		 int memorySize, uint32_t memory[], int currentInstrAddress) { 
  
  struct B branchstruct;
  uint32_t output;
  
  //Token is the first part of the instruction (For "beq foo", token is "beq")
  char *token = strtok(instruction, " "); 
  
  if(strcmp(token, "beq") == 0) {
    
    branchstruct.cond = EQ; //Cond part is 0000 for beq
    
  } else if(strcmp(token, "bne") == 0) {
    
    branchstruct.cond = NE << FIRST_COND_BIT;  //Cond part is 0001 for bne
    
  } else if(strcmp(token, "bge") == 0) {
    
    branchstruct.cond = GE << FIRST_COND_BIT; //Cond part is 1010 for bge
  
  } else if(strcmp(token, "blt") == 0) {
   
    branchstruct.cond = LT << FIRST_COND_BIT;  //Cond part is 1011 for blt
    
  } else if(strcmp(token, "bgt") == 0) {
    
    branchstruct.cond = GT << FIRST_COND_BIT; //Cond part is 1100 for bgt
    
  } else if(strcmp(token, "ble") == 0) {
    
    branchstruct.cond = LE << FIRST_COND_BIT;  //Cond part is 1101 for ble
  
  } else {
    
    branchstruct.cond = AL << FIRST_COND_BIT;  //Cond part is 1110 for b (Always)
  
  }
  
  char* label = strtok(NULL, "\n");       //Label is now rest of the string
  removeCharFromString(' ',label);
  
  //Look up label address (Returns 4 for the second line)
  int labelAddress = lookUp(label, lookUpTable); 

  if (labelAddress == -1) {
    printf("The label at line %d could not be found\n", (currentInstrAddress/4) + 1);
    exit(EXIT_FAILURE);
  }
  
  //Offset accounts for PC being 8 ahead in pipeline
  int32_t offset = labelAddress - currentInstrAddress - 8;  
  
  //Make 26th bit a 1 if negative (Gives effect of being a 26-bit signed int)
  if (!((offset & ((1 << (WORD_SIZE-1)))) == 0)) {
     offset |= (1 << 25); 
  }
  
  offset >>= 2; //Shift right by two bits
  
  uint32_t mask = 0xFFFFFF;
  
  branchstruct.offset = offset & mask; //Offset is final 24 bits of this offset
  
  output = branchstruct.cond + branchstruct.offset + (10 << 24);
               
  return output;
  
}

//helper function for Multiply encode
uint32_t encodeM(char instruction[], int currentInstrAddress){
  
  struct M mstruct; //mstruct has form cond 000000 A S _rd_ _rn_ _rs_ 1001 _rm_
  uint32_t output;
  char *rd = "";       //mstruct has form cond 000000 A S _rd_ _rn_ _rs_ 1001 _rm_
  char *rm = "";
  char *rs = "";
  char *rn = "";
  
  char *token = strtok(instruction, " ");
  char *restOfInstr = strtok(NULL,"\n");
  
  removeCharFromString(' ', restOfInstr);
  
  if(strcmp(token, "mul") == 0) {
      
    rd = strtok(restOfInstr, ",r");     //Gets everything in between r and a comma
  
    rm = strtok(NULL, ",r");
  
    rs = strtok(NULL, ",r");
    
    if (strtok(NULL, "\n") == NULL) {
    mstruct.rn = 0;
    mstruct.a = 0;
    } else {
      printf("Arguments for mul on line %d are not correct.\n", (currentInstrAddress/4) + 1);
      exit(EXIT_FAILURE);
    }
    
     
  //Same as mul but has an extra register at the end and a = 1
  } else if(strcmp(token, "mla") == 0) {  
                     
    rd = strtok(restOfInstr, ",r");
      
    rm = strtok(NULL, ",r");
      
    rs = strtok(NULL, ",r");
      
    rn = strtok(NULL, ",r");    
 
    if (strtok(NULL, "\n") == NULL) {
      mstruct.rn = atoi(rn) << M_RN;
      mstruct.a = 1 << AShift;
    } else {
      printf("Arguments for mla on line %d are not correct.\n", (currentInstrAddress/4) + 1);
      exit(EXIT_FAILURE);
    }
    
  }

  mstruct.rm = atoi(rm);  //change (char register num) into (int register num) 
  mstruct.rd = atoi(rd) << M_RD;  
  mstruct.rs = atoi(rs) << M_RS;
  mstruct.s = 0;
  mstruct.cond = AL << FIRST_COND_BIT;
      
  output = mstruct.cond +
                mstruct.a +
                mstruct.s +
                mstruct.rd +
                mstruct.rn +
                mstruct.rs +
                mstruct.rm +
                (9 << SDT_SET_BITS);

  return output;
 
}

//helper function for data processing
uint32_t encodeDP(char instruction[]) {
   
  struct DP dpstruct;
  uint32_t output;

  char *rd = NULL; 
  char *rn = NULL;
  char *operand2 = NULL;
  
  bool operand2NeedsFreeing = false;

  char *token = strtok(instruction, " ");
  char *restOfInstr = strtok(NULL, "\n");
  
  removeCharFromString(' ', restOfInstr);
  
  if(strcmp(token, "and") == 0 || strcmp(token, "andeq") == 0) {
    
    rd = strtok (restOfInstr, ",");
    rn = strtok (NULL, ",");
    operand2 = strtok (NULL, "\n");
     
    dpstruct.opcode = 0;
    dpstruct.s = 0;
     
    if(strcmp(token, "and") == 0) {
      dpstruct.cond = AL << FIRST_COND_BIT;  
    } else {
      // if it is andeq
      dpstruct.cond = 0;
    }
  
  } else if(strcmp(token, "eor") == 0) {
    
    rd = strtok (restOfInstr, ",");
    rn = strtok (NULL, ",");
    operand2 = strtok (NULL, "\n");
     
    dpstruct.opcode = EOR << OP_SHIFT;
    dpstruct.s = 0;
    dpstruct.cond = AL << FIRST_COND_BIT;
    
  }else if(strcmp(token, "sub") == 0) {
    
    rd = strtok (restOfInstr, ",");
    rn = strtok (NULL, ",");
    operand2 = strtok (NULL, "\n");

    dpstruct.opcode = SUB << OP_SHIFT;
    dpstruct.s = 0;
    dpstruct.cond = AL << FIRST_COND_BIT;
    
  } else if(strcmp(token, "rsb") == 0) {
    
    rd = strtok (restOfInstr, ",");
    rn = strtok (NULL, ",");
    operand2 = strtok (NULL, "\n");

    dpstruct.opcode = RSB << OP_SHIFT;
    dpstruct.s = 0;
    dpstruct.cond = AL << FIRST_COND_BIT;
  
  } else if(strcmp(token, "add") == 0) {
     
    rd = strtok (restOfInstr, ",");
    rn = strtok (NULL, ",");
    operand2 = strtok (NULL, "\n");

    dpstruct.opcode = ADD << OP_SHIFT;
    dpstruct.s = 0;
    dpstruct.cond = AL << FIRST_COND_BIT;
    
  } else if(strcmp(token, "orr") == 0) {
    
    rd = strtok (restOfInstr, ",");
    rn = strtok (NULL, ",");
    operand2 = strtok (NULL, "\n");

    dpstruct.opcode = ORR << OP_SHIFT; 
    dpstruct.s = 0;
    dpstruct.cond = AL << FIRST_COND_BIT;
    
  } else if(strcmp(token, "mov") == 0 || strcmp(token, "lsl") == 0) {
    
    rd = strtok (restOfInstr, ",");
    operand2 = strtok (NULL, "\n");
    
    dpstruct.opcode = MOV << OP_SHIFT;
    dpstruct.s = 0;
    dpstruct.cond = AL << FIRST_COND_BIT;
     
    if(strcmp(token, "lsl") == 0) {
       
      //reconstruct as lsl instr
	char newoperand2[strlen(rd) + strlen(operand2) + 5];
	strcpy(newoperand2, rd);
	strcat(newoperand2, ",lsl");
	strcat(newoperand2, operand2);
	operand2 = (char*)calloc(strlen(rd) + strlen(operand2) + 5, sizeof(char));
	strcpy(operand2, newoperand2);
	operand2NeedsFreeing = true;  
                              
    }     
    
  } else if(strcmp(token, "tst") == 0) {
    
    rn = strtok (restOfInstr, ",");
    operand2 = strtok (NULL, "\n");
    
    dpstruct.opcode = TST << OP_SHIFT;
    dpstruct.s = 1 << SET_LOAD_SHIFT;
    dpstruct.cond = AL << FIRST_COND_BIT;
    
  } else if(strcmp(token, "teq") == 0) {
    
    rn = strtok (restOfInstr, ",");
    operand2 = strtok (NULL, "\n");
         
    dpstruct.opcode = TEQ << OP_SHIFT;
    dpstruct.s = 1 << SET_LOAD_SHIFT;
    dpstruct.cond = AL << FIRST_COND_BIT;
     
  } else if(strcmp(token, "cmp") == 0) {
    
    rn = strtok (restOfInstr, ",");
    operand2 = strtok (NULL, "\n");
    
    dpstruct.opcode = CMP << OP_SHIFT;
    dpstruct.s = 1 << SET_LOAD_SHIFT;
    dpstruct.cond = AL << FIRST_COND_BIT;
    
  }
    
  char temp[strlen(operand2)+1];
  strcpy(temp, operand2);
    
  char *hashtest = strtok(temp, "r");
   
  if(strlen(hashtest) == strlen(operand2)) {
          
    dpstruct.i = 1 << IShift; 

      operand2 = strtok(operand2, "#");

    dpstruct.operand2 = rotation(operand2);
       
  } else {
  // shifted
    
    dpstruct.i = 0;
       
      dpstruct.operand2 = shift(operand2);
    
  }
  
  if(!rd) {
    // IF rd is NULL
     
    dpstruct.rd = 0;
    
  } else {
   
    char* value = strtok(rd, "r"); 
   
    dpstruct.rd = atoi(value) << DP_RD;
  
  }
  
  if(!rn) {
   // IF rn is NULL 

    dpstruct.rn = 0;
    
  } else {
          
    char* value = strtok(rn, "r"); 
 
    dpstruct.rn = atoi(value) << DP_RN; 
    
  }
  
  if (operand2NeedsFreeing) {
    free(operand2);
  }
  
  output = dpstruct.cond +
           dpstruct.i +
           dpstruct.opcode +
           dpstruct.s +
           dpstruct.rn +
           dpstruct.rd +
           dpstruct.operand2;

  return output;
  
}

//helper function for Single Data Transfer encode
uint32_t encodeSDT(char instruction[], uint32_t memory[], 
		   int memorySize, int currentInstrAddress) {
 
  struct SDT sdtstructRet;  //struct to return 
  uint32_t output;
  
  sdtstructRet.cond = AL << FIRST_COND_BIT; //cond field always 1110
  sdtstructRet.p = 1 << PShift;     //set P bit to 1 for pre indexing always
  sdtstructRet.i = 1 << IShift;     //set to zero if address is expression
  char *address;                //address (in same place for both str ldr)

  char instructionCopy[strlen(instruction)];  // copy instruction to edit
  strcpy(instructionCopy, instruction);
  char *token = strtok(instructionCopy, " "); //(instruction && token) == ldr 
  
  char *rd = strtok(NULL, ",");                    //saves rd
  
  //address could hold a constant or some register combination
  address = strtok(NULL, "\n");                         
  
  removeCharFromString(' ', address);              // remove spaces from address string
  
  //rd = number register in int form
  sdtstructRet.rd = atoi(strtok (rd, "r")) << SDT_RD;        
  //set up bit for offset for now as assumed to be up if nothing there
  sdtstructRet.u = 1 << UShift;                              
  // set i bit to zero                                    
  sdtstructRet.i = 0; 
  
  //check if it's a load instruction.
  if(strcmp(token, "ldr") == 0) {
     
    sdtstructRet.l = 1 << SET_LOAD_SHIFT; //set L bit for load always
     
    //if address is an expression const this is true (only for load)
    if (address[0] == '=') { 
 
      //change the address string to an int 
      //(0 passed because hex value has 0x at beginnig)

      char *intString = strtok(address, "=");           //get hex string  
     
      uint32_t newInt;

      //change the address string to an int
      if (isNegativeIntString(intString)) {
        intString++;
        sdtstructRet.u = 0;
      }

      newInt = convertIntString(intString);
     
      //swap load for mov and encode as DP if expr <= 0xFF.
      if (newInt <= MAX_MOV) { 
       
        //replace ldr for mov in instruction.
        instruction = strncpy(instruction, "mov",3);                  
        //replace '=' sign infront of address for '#'
        address[0] = '#';                                             
        //token1 = part of instruction before '=' sign ('mov rd, ')
        char *token1 = strtok(instruction, "=");                      
        //make new array with size of both strings plus 1 for '\0'
        char newMovInstr[strlen(address) + strlen(token1) + 1];       
        //put first part in newMovInstr
        strcpy(newMovInstr,token1);                                   
        //concat new address with '# onto the end of newMovInstr
        strcat(newMovInstr,address);                            
       
        return encodeDP(newMovInstr);  
       
      }

      //otherwise save hexInt found in memory.
    
      //finds next free address from half way in the memory
      int nextFreeAddress = getNextFreeAddress(memorySize, memory);   

      //set next free address to address constant
      memory[nextFreeAddress] = newInt;                               
      //find offset from PC taking into account pipeline
      int offset = (nextFreeAddress*4) - (currentInstrAddress + 8);  
      //define offset variable in sdtsruct
      sdtstructRet.offset = offset;                                  
      //define rn register as PC
      sdtstructRet.rn  = PCREG << SDT_RN;                                
      //return struct formed with constant address
      goto endSDT;                                                
     
    }

  //else its a str instruction
  } else {
    
     sdtstructRet.l = 0;  //L bit = 0 always for store
     
  } 
  
  //By this point we know its not a constant address and token is either str or
  //ldr, it dosen't matter as the post and pre indexing happens the same way.
  //The following, before the first if statement, has to happen with both 
  //pre and post.
  //address now holds '[rn,'.
  
  //rn = integer value of register num in all cases.
  sdtstructRet.rn = atoi(strtok(address, "[r],")) << SDT_RN; 
  //now address points to anything after [rn,
  char *restOfAddress = strtok(NULL,"\n");                          

  if(restOfAddress == NULL){
  //Rn has already been set
        sdtstructRet.offset = 0;            //set offset to 0
        goto endSDT;
  }
  
  //restOfAddress isn't null by this point.
   
  //if SDT is pre-index (when the last characterof leftover string is a ']'.
  
  if (restOfAddress[strlen(restOfAddress) - 1] == ']') {
  
      //rest of address will be one of the following cases
      
      //case address = [Rn, #expression]  
    if(restOfAddress[0]=='#') {
	
      //removes '#' from '#expression]
      char *expression1 = strtok(restOfAddress, "#]");  

      int offset1;
     
      //change the address string to an int
      if (isNegativeIntString(expression1)) {
        expression1++;
        sdtstructRet.u = 0;
      }

      offset1 = convertIntString(expression1);

      sdtstructRet.offset = offset1;                //set offtset
  
    //case address = [Rn,{+/-}Rm{,<shift>}]
    } else {

      sdtstructRet.i = 1 << IShift; //shifted register
      char *shiftString;
      char *removeSign;
  
      if(restOfAddress[0]=='-') { 
	    
	//set up bit to zero
        sdtstructRet.u = 0;                           
	//get rid of '-' at front
        removeSign = strtok(restOfAddress, "-");   
	//store string to pass to shift
        shiftString = strtok(removeSign, "]");     
	    
      } else { 
	    
	//get rid of '+' at front
        removeSign = strtok(restOfAddress, "+");    
	//store string to pass to shift
        shiftString  = strtok(removeSign, "]");     
	    
      }
        
      sdtstructRet.offset = shift(shiftString);
	  
    } 
  
  //Else it's post indexing (rest of address is not null pointer here).
  //Also it can do the same as pre indexing without looking at end bracket.
  } else { 
    
    //P bit is 0 for post indexing
    sdtstructRet.p = 0;                         
    restOfAddress++; // get rid of comma at front of string
    //case address = [Rn], #expression  
    //don't have to look at end bracket in this case.
    if(restOfAddress[0]=='#') {

      //removes '#' from '#expression'
      char *expression2 = strtok(restOfAddress, "#");  
      
      int offset2;

      //change the address string to an int
      if (isNegativeIntString(expression2)) {
        expression2++;
        sdtstructRet.u = 0;
      }

      offset2 = convertIntString(expression2);

      sdtstructRet.offset = offset2;                      
  
      //case address = [Rn,{+/-}Rm{,<shift>}]
    } else {

      sdtstructRet.i = 1 << IShift; //shifted register
      char *shiftString;
  
      if(restOfAddress[0]=='-') {
         
	//set up bit to zero.          
        sdtstructRet.u = 0;                           
	//get rid of sign at front 
        shiftString = strtok(restOfAddress, "-");     
        
      } else {
	  
	//get rid of '+' at front.  
        shiftString = strtok(restOfAddress, "+");      
	  
      }
        
      //set shift from shift function
      sdtstructRet.offset = shift(shiftString);       
  
    }   
    
  }

  endSDT:  output = sdtstructRet.cond +
               sdtstructRet.u +
               sdtstructRet.i +
               sdtstructRet.p +
               sdtstructRet.l +
               sdtstructRet.rn +
               sdtstructRet.rd +
               sdtstructRet.offset +
               (1 << SDT_SET_BIT);

  return output;
  
}

//Main encoding fuction
uint32_t encode(char instruction[], uint32_t memory[], int memorySize, 
		struct stringIntTuple* lookUpTable, int currentInstrAddress) {

  uint32_t finalOutput;  
    
  if(instruction[0] == 'b') {                         //If branch instruction
    finalOutput = encodeB(instruction,lookUpTable, memorySize, 
			  memory, currentInstrAddress);   
  } else if((instruction[0] == 's' && instruction[1] == 't'   //If str or ldr
            && instruction[2] == 'r') || (instruction[0] == 'l' 
	    && instruction[1] == 'd' && instruction[2] == 'r')) {
    finalOutput = encodeSDT(instruction, memory, memorySize,
			    currentInstrAddress);  
  } else if((instruction[0] == 'm' && instruction[1] == 'u'   //If mul or mla
            && instruction[2] == 'l') || (instruction[0] == 'm' 
	    && instruction[1] == 'l' && instruction[2] == 'a')){
    finalOutput = encodeM(instruction, currentInstrAddress);  
  } else {                                               //If data processing
    finalOutput = encodeDP(instruction);  
  }
    
  return finalOutput;
  
}
