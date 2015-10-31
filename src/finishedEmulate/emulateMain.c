#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>
#include <stdbool.h>
#include "emulateEnums.h"
#include "machineStruct.h"
#include "binMethods.h"
#include "executeFunction.h"
#include "decodedStruct.h"
#include "fetchedStruct.h"
#include "binaryFileReader.h"

//intitialising the global structs from struct.Header
struct Machine machine;
struct decoded_final Decoded;
struct fetched_final Fetched;

//MAIN
int main(int argc, char *argv[]) {

  //Set to cleared initially
  Fetched.isFetchedCleared = true;        
  //Doesn't really matter what this initially is as it is cleared
  Fetched.fetched_instr = 0;              

  //Set to cleared initially
  Decoded.type = CL; 
  
  FILE *binaryFile;
  
  //allocate space for ARM memory, all initally set to zero
  machine.memory = calloc(MEMSIZE, 1);                                       

  //allocate space for ARM registers, all set to zero
  machine.registers = calloc(NUMREG, SINGLEREGSIZE);                         

  //exit if number of arguments < 2
  if (argc < 2) {                                        
    printf("Please pass binary file as an argument");
    exit(EXIT_FAILURE);
  }
  
  //open the file reader
  binaryFile = fopen(argv[1], "rb");                                   
  
  if (binaryFile == NULL) {
    fprintf(stderr, "File error"); 
    exit(EXIT_FAILURE);
  }
  
  if (sizeOfBinFile(binaryFile) > MEMSIZE) {
    printf("Too many commands for memory to hold.\n");
  }
  
  //put binary file in memory one byte at a time
  binaryFileLoader(binaryFile, machine.memory);                             
  
  //close file
  fclose(binaryFile);                                     

  // will eventually halt (Hit an all-zero command)
  while (!(Decoded.type == H)) {
   
    //Program cannot fetch instruction outside of it's memory
    if (machine.registers[PCREG] >= MEMSIZE) {
      printf("Trying to access instruction with address greater than 65536. ");
      printf("You either didn't include a halt command, ");
      printf("or you used branch to go to an invalid address.\n");
      exit(EXIT_FAILURE);
    }

    //only execute if not clear
    if (!(Decoded.type == CL)) {           
      execute();
    }

    //only set decode if fetch isnt clear 
    if (!Fetched.isFetchedCleared) {
      decode(Fetched.fetched_instr);
    }
    
    //Fetch an instruction, and then set isFetchedCleared to false
    Fetched.fetched_instr = fetch(); 
    
    Fetched.isFetchedCleared = false;     
   
    //Increment PC by 4
    machine.registers[PCREG] += BYTESINWORD; 
    
  } 
  
  //Print values in registers and the non-zero memory upon termination
  printRegisters();
  printMemory();

  return EXIT_SUCCESS;
  
}
