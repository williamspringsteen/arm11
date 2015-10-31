#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "emulateEnums.h"
#include "machineStruct.h"
#include "binMethods.h"
#include "executeFunction.h"
#include "decodedStruct.h"
#include "fetchedStruct.h"

//EXECUTE

void executeSDT(struct single_data_transfer_decoded single_data_transfer_instr){

   //PREINDEXING
   if(single_data_transfer_instr.pre_or_post_indexing_bit){
      
     //LOADED FROM MEMORY Rn := [address], LDR PREINDEXING
     if(single_data_transfer_instr.load_or_store_bit){ 
       
       //Offset is added to base register
       if(single_data_transfer_instr.up_bit){
	
	 //Memory address is definitely in memory
         if(machine.registers[single_data_transfer_instr.base_reg]
                               + single_data_transfer_instr.offset < MEMSIZE) {
	
	  machine.registers[single_data_transfer_instr.src_or_dst_reg] =
                 getWord(machine.registers[single_data_transfer_instr.base_reg]
                  + single_data_transfer_instr.offset);
		 
         //NOTE: isGPIOPin prints 'Pin accessed..'
	 } else if(
	   isGPIOPin(machine.registers[single_data_transfer_instr.base_reg] 
	          + single_data_transfer_instr.offset)) { 
	   
	    machine.registers[single_data_transfer_instr.src_or_dst_reg] =
	          machine.registers[single_data_transfer_instr.base_reg] 
	             + single_data_transfer_instr.offset;
	    
	  //No print needed here for ldr
	  } else if(
        isGPIOSetOrClear(machine.registers[single_data_transfer_instr.base_reg]
	                             + single_data_transfer_instr.offset)) {  
	    
	    //This bit of memory isn't actually in the emulator
	    
	  } else {
            printf("Error: Out of bounds memory access at address 0x%08x\n", 
                    machine.registers[single_data_transfer_instr.base_reg]
                        + single_data_transfer_instr.offset);
	  }

        } else {
	  
	  if(machine.registers[single_data_transfer_instr.base_reg]
                       - single_data_transfer_instr.offset < MEMSIZE) {
	
	    machine.registers[single_data_transfer_instr.src_or_dst_reg] =
              getWord(machine.registers[single_data_transfer_instr.base_reg]
                      - single_data_transfer_instr.offset);
	      
	  //NOTE: isGPIOPin prints 'Pin accessed..'
	  } else if(
	    isGPIOPin(machine.registers[single_data_transfer_instr.base_reg]
	                              - single_data_transfer_instr.offset)) { 
	   
	    machine.registers[single_data_transfer_instr.src_or_dst_reg] = 
	       machine.registers[single_data_transfer_instr.base_reg] 
	          - single_data_transfer_instr.offset;
	    
	  //No print needed here for ldr
	  } else if(
	isGPIOSetOrClear(machine.registers[single_data_transfer_instr.base_reg]
	                 - single_data_transfer_instr.offset)) { 

	    //This bit of memory isn't actually in the emulator
	    
	  } else {
            printf("Error: Out of bounds memory access at address 0x%08x\n",
		   machine.registers[single_data_transfer_instr.base_reg]
                   - single_data_transfer_instr.offset);
	  }
         
        }
        
    //STORING INTO MEMORY [address] := Rn, STR PREINDEXING
    } else {
       
       if(single_data_transfer_instr.up_bit){
        
	 if(machine.registers[single_data_transfer_instr.base_reg]
                 + single_data_transfer_instr.offset < MEMSIZE) {
	
	  putWord(machine.registers[single_data_transfer_instr.base_reg] +
             single_data_transfer_instr.offset,
                 machine.registers[single_data_transfer_instr.src_or_dst_reg]);
	  
	  //NOTE: isGPIOPin prints 'Pin accessed..'
	  } else if(
	    isGPIOPin(machine.registers[single_data_transfer_instr.base_reg]
	                               + single_data_transfer_instr.offset)) {
	   
            //This bit of memory isn't actually in emulator
	    
	  } else if(
	isGPIOSetOrClear(machine.registers[single_data_transfer_instr.base_reg]
	                 + single_data_transfer_instr.offset)) {
	    
	    if (machine.registers[single_data_transfer_instr.src_or_dst_reg] != 0) {
	    
	      //If output set register, PIN ON, otherwise it is clearing
              if(machine.registers[single_data_transfer_instr.base_reg] 
		      + single_data_transfer_instr.offset == GPIO_OUTPUT_SET_0) {
	        printf("PIN ON\n");
	      } else {
	        printf("PIN OFF\n");
	      }
	      
	    }
	    
	    //This bit of memory isn't actually in emulator
	    
	  } else {
          printf("Error: Out of bounds memory access at address 0x%08x\n",
		 machine.registers[single_data_transfer_instr.base_reg]
                 + single_data_transfer_instr.offset);
	  } 
	 
	 
       } else {
	 
	 if(machine.registers[single_data_transfer_instr.base_reg]
                    - single_data_transfer_instr.offset < MEMSIZE) {
	
	  putWord(machine.registers[single_data_transfer_instr.base_reg] -
             single_data_transfer_instr.offset,
                 machine.registers[single_data_transfer_instr.src_or_dst_reg]);
	  
	  //NOTE: isGPIOPin prints 'Pin accessed..'
	  } else if(
	    isGPIOPin(machine.registers[single_data_transfer_instr.base_reg]
	              - single_data_transfer_instr.offset)) { 
	   
            //This bit of memory isn't actually in emulator
	    
	  } else if(
       isGPIOSetOrClear(machine.registers[single_data_transfer_instr.base_reg] 
	            - single_data_transfer_instr.offset)) {
	    
	    if (machine.registers[single_data_transfer_instr.src_or_dst_reg]
	                                                               != 0) {
	    
              if(machine.registers[single_data_transfer_instr.base_reg]
		    - single_data_transfer_instr.offset == GPIO_OUTPUT_SET_0) {
	        printf("PIN ON\n");
	      } else { 
	        printf("PIN OFF\n");
	      }
	      
	    }
	    
	    //This bit of memory isn't actually in emulator
	    
	  } else {
	    
            printf("Error: Out of bounds memory access at address 0x%08x\n", 
		   machine.registers[single_data_transfer_instr.base_reg]
                     - single_data_transfer_instr.offset);
	    
	  } 
	
       }
    }
    
  //POSTINDEXING
  } else {

    //LOADED FROM MEMORY, LDR POSTINDEXING (No offset)
    if(single_data_transfer_instr.load_or_store_bit) {   
      
       if(machine.registers[single_data_transfer_instr.base_reg] < MEMSIZE){
	
	  machine.registers[single_data_transfer_instr.src_or_dst_reg] =
             getWord(machine.registers[single_data_transfer_instr.base_reg]);
	     
	  //NOTE: isGPIOPin prints 'Pin accessed..'   
	} else if(
	   isGPIOPin(machine.registers[single_data_transfer_instr.base_reg])) { 
	   
	    machine.registers[single_data_transfer_instr.src_or_dst_reg] =
	               machine.registers[single_data_transfer_instr.base_reg];
		       
	//No print needed here for ldr  
	} else if(isGPIOSetOrClear(
	             machine.registers[single_data_transfer_instr.base_reg])) {
	    
	  //This bit of memory isn't actually in the emulator
	    
	} else {
	    
          printf("Error: Out of bounds memory access at address 0x%08x\n", 
	         machine.registers[single_data_transfer_instr.base_reg]);
	    
	}
	  
      //Adding offset
      if(single_data_transfer_instr.up_bit) {

        machine.registers[single_data_transfer_instr.base_reg] +=
           single_data_transfer_instr.offset;

      //Subtracting offset   
      } else {

       machine.registers[single_data_transfer_instr.base_reg] -=
           single_data_transfer_instr.offset;
	   
      }

    //STR POSTINDEXING
    } else {

      if(machine.registers[single_data_transfer_instr.base_reg] < MEMSIZE){
	
	  putWord(machine.registers[single_data_transfer_instr.base_reg],
                 machine.registers[single_data_transfer_instr.src_or_dst_reg]);
	  
	  //NOTE: isGPIOPin prints 'Pin accessed..'
	  } else if(isGPIOPin(
	            machine.registers[single_data_transfer_instr.base_reg])) { 
	   
            //This bit of memory isn't actually in emulator
	    
	  } else if(isGPIOSetOrClear(
	             machine.registers[single_data_transfer_instr.base_reg])) {
	    
	    if (machine.registers[single_data_transfer_instr.src_or_dst_reg]
	                                                                != 0) {
	    
              if(machine.registers[single_data_transfer_instr.base_reg]
		                                               == GPIO_OUTPUT_SET_0) {
	        printf("PIN ON\n");
	      } else { 
	        printf("PIN OFF\n");
	      }
	      
	    }
	    
	    //This bit of memory isn't actually in emulator
	    
	  } else {
	    
          printf("Error: Out of bounds memory access at address 0x%08x\n", 
		  machine.registers[single_data_transfer_instr.base_reg]);
	  
	  }    
	  
      if(single_data_transfer_instr.up_bit){

        machine.registers[single_data_transfer_instr.base_reg] +=
            single_data_transfer_instr.offset;
	  
      } else {
      
        machine.registers[single_data_transfer_instr.base_reg] -=
            single_data_transfer_instr.offset;
	  
      }
    
    }

  }

}

// set type of Decoded instr to Clear in here and Fetched clear bool to true
void executeB(){
  
 Decoded.type = CL;
 Fetched.isFetchedCleared = true;

 machine.registers[PCREG] = machine.registers[PCREG] +
     (Decoded.decoded_instr.branch.offset);
     
}

//Helper function to execute a branch command
void executeDP(struct data_processing_decoded data_processing_instr) {
  
    uint32_t NZCV = machine.registers[CPSR];
    uint32_t result;
    
    //Can't declare these variables (below) in the
    // switch statement in which they are needed
    
    uint32_t top;
    uint32_t bottom;
    uint32_t orresult;
    int zeroposition;
    uint32_t andresult;
    int oneposition;
    
    bool setcondition = data_processing_instr.set_condition_bit;
    
    switch(data_processing_instr.opcode){
	
	case(AND): 
		   machine.registers[data_processing_instr.dest_reg] =
                       (machine.registers[data_processing_instr.operand_1_reg])
                           & (data_processing_instr.operand_2);
		   result = machine.registers[data_processing_instr.dest_reg];
		   
		   if(setcondition) {
		     
		     machine.registers[CPSR] =
		         setSpecificBit(machine.registers[CPSR],
                                        data_processing_instr.carry, C); 
			 
		   }
		   
		   break;
	case(EOR): 
		   machine.registers[data_processing_instr.dest_reg] =
                       (machine.registers[data_processing_instr.operand_1_reg])
                           ^ (data_processing_instr.operand_2);
		   result = machine.registers[data_processing_instr.dest_reg];
		   
		   if(setcondition) {
		     
		     machine.registers[CPSR] = 
		         setSpecificBit(machine.registers[CPSR],
                                        data_processing_instr.carry, C);   
		   
		   }
		   
		   break;
	case(SUB): 
                   top = machine.registers[data_processing_instr.operand_1_reg];
		   bottom = (data_processing_instr.operand_2); //two's complement
		   
		   if(setcondition) {
		   
	             if(top >= bottom){
		       machine.registers[CPSR] = 
		           setSpecificBit(machine.registers[CPSR], 1, C);
		     } else {
		       machine.registers[CPSR] = 
		           setSpecificBit(machine.registers[CPSR], 0, C);
		     }
		     
		   }
		   
		   machine.registers[data_processing_instr.dest_reg] =
                       (machine.registers[data_processing_instr.operand_1_reg])
                           - (data_processing_instr.operand_2);
		   result = machine.registers[data_processing_instr.dest_reg];
		   
		   break;
	case(RSB): 
	           bottom = machine.registers[data_processing_instr.operand_1_reg];
		   top = data_processing_instr.operand_2;
		   
		   if(setcondition) {
		   
		     if(((top ^ bottom) & bottom) == 0){
                     // no borrow
 		       machine.registers[CPSR] = 
 		           setSpecificBit(machine.registers[CPSR], 1, C);
		     } else {
		     // produces borrow
		       machine.registers[CPSR] = 
		           setSpecificBit(machine.registers[CPSR], 0, C);
		     }
		     
		   }
		   
		   machine.registers[data_processing_instr.dest_reg] =
                      (data_processing_instr.operand_2) - 
                      (machine.registers[data_processing_instr.operand_1_reg]);
		   result = machine.registers[data_processing_instr.dest_reg];
		   
		   break;
	case(ADD): 
		   top = 
		      machine.registers[data_processing_instr.operand_1_reg];
		   bottom = data_processing_instr.operand_2;
		   
		   if(setcondition) {
		     orresult = top | bottom;
		     zeroposition = BITSINWORD - 1;
		   
		     while(zeroposition >= 0){
		       
		       if(((orresult << (BITSINWORD - 1 - zeroposition)) >> (BITSINWORD - 1))  == 0) {
		         break; 
		       }
		       
		       zeroposition--;

		     }

		     // zeroposition = where the first zero occurs 

		     andresult = top & bottom;
		     oneposition = BITSINWORD - 1; 
		     
		     while(oneposition >= 0) {
		       
		       if(((andresult << (BITSINWORD - 1 - oneposition)) >> (BITSINWORD - 1)) == 1) {
		         break;
		       }
		       
		     oneposition--;

		     }
		     
	             if(oneposition > zeroposition) {
		       machine.registers[CPSR] = setSpecificBit(NZCV, 1, C);
		     } else {
		       machine.registers[CPSR] = 
		           setSpecificBit(machine.registers[CPSR], 0, C);
		     }
		    
		   }  

		   machine.registers[data_processing_instr.dest_reg] =
                       (machine.registers[data_processing_instr.operand_1_reg])
                           + (data_processing_instr.operand_2);
	           result = machine.registers[data_processing_instr.dest_reg];
		   break;
	case(TST): 
	           result =
	               (machine.registers[data_processing_instr.operand_1_reg])
                          & (data_processing_instr.operand_2); 
			  
		   if(setcondition) {
		     
		     machine.registers[CPSR] =
		         setSpecificBit(machine.registers[CPSR],
                                        data_processing_instr.carry, C); 
		       
		   }
		   
		   break;
	case(TEQ): 
	           result = 
	               (machine.registers[data_processing_instr.operand_1_reg])
                               ^ (data_processing_instr.operand_2);  //^ is XOR
			       
		   if(setcondition) {
		     machine.registers[CPSR] = setSpecificBit(machine.registers[CPSR],
                         data_processing_instr.carry, C);
		   }
		   
		   break;
	case(CMP): 
	           result = 
	               (machine.registers[data_processing_instr.operand_1_reg])
                           - (data_processing_instr.operand_2);
                   top = machine.registers[data_processing_instr.operand_1_reg];
		   bottom = data_processing_instr.operand_2; //two's complement
		   
		   //setting CPSR bits if set condition holds
		   if(setcondition) {
		  
	             if(top >= bottom) {

      	               machine.registers[CPSR] = 
      	                   setSpecificBit(machine.registers[CPSR], 1, C);
                   
		     } else {
		       
		       machine.registers[CPSR] = 
		           setSpecificBit(machine.registers[CPSR], 0, C);
		       
		     }	
		    
		   }
		   
		   break;
	case(ORR): 
	           machine.registers[data_processing_instr.dest_reg] =
                       (machine.registers[data_processing_instr.operand_1_reg])
                           | (data_processing_instr.operand_2);
		   result = machine.registers[data_processing_instr.dest_reg];
		   
		   if(setcondition) { 
		     
		     machine.registers[CPSR] = 
		         setSpecificBit(machine.registers[CPSR], 
			                data_processing_instr.carry, C);
		     
		   }
		   
		   break;
	case(MOV): 
		   machine.registers[data_processing_instr.dest_reg] = 
                       (data_processing_instr.operand_2);
		   result = machine.registers[data_processing_instr.dest_reg];
		   
		   if(setcondition) {
		     
		     machine.registers[CPSR] = 
		         setSpecificBit(machine.registers[CPSR],
                                        data_processing_instr.carry, C);
			 
		   }
		   
		   break;

    }
	
    if(setcondition) {
	
      if(result == 0) {
	    
        machine.registers[CPSR] = setSpecificBit(machine.registers[CPSR], 1, Z);

      } else {
	    
        machine.registers[CPSR] = setSpecificBit(machine.registers[CPSR], 0, Z);
	
      }

      uint32_t bit31ofresult = result >> (BITSINWORD - 1);
      machine.registers[CPSR] = setSpecificBit(machine.registers[CPSR], bit31ofresult, N);
	
    }
  
}

void executeM(struct multiply_decoded multiply_instr) {
 
  machine.registers[multiply_instr.dest_reg] = 
      machine.registers[multiply_instr.multiply_operand_1_reg]
        * machine.registers[multiply_instr.multiply_operand_2_reg];
  
  if (multiply_instr.accumulate_bit) {
    machine.registers[multiply_instr.dest_reg] += 
        machine.registers[multiply_instr.accumulating_operand_reg];
  }

  if (multiply_instr.set_condition_bit) {

    uint32_t nBit = (machine.registers[multiply_instr.dest_reg] >> N) << N;
    machine.registers[CPSR] = (machine.registers[CPSR] << 1) >> 1;
    
    machine.registers[CPSR] += nBit;

    if (machine.registers[multiply_instr.dest_reg] == 0) {
      uint32_t mask = 1 << Z;
      machine.registers[CPSR]  = machine.registers[CPSR] | mask;
    }
    
  }
  
}

void execute() {
  
  switch(Decoded.type) {

    case(DP):
              if (checkCondition(Decoded.decoded_instr.data_processing.cond)){
                executeDP(Decoded.decoded_instr.data_processing);
	      }
              break; 
    case(SDT): 
	      if (checkCondition(Decoded.decoded_instr.single_data_transfer.cond)){
                executeSDT(Decoded.decoded_instr.single_data_transfer);
	      }
              break;
    case(M) : 
	      if (checkCondition(Decoded.decoded_instr.multiply.cond)){
                executeM(Decoded.decoded_instr.multiply);
	      }
              break;
    case(B):
	      if (checkCondition(Decoded.decoded_instr.branch.cond)){ 
                executeB(); 
	      }
              break;
    default: ;      
      
  }

}
 
// END OF EXECUTE
