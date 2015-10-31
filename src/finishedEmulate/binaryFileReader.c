#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "binaryFileReader.h"
#include "machineStruct.h"

//BINARY FILE READER

// finds size of binary file 
int sizeOfBinFile(FILE *readFile) {

  int fSize = 0;
  
  // get file size
  fseek(readFile,0,SEEK_END);
  fSize = ftell(readFile);
  rewind(readFile);

  return fSize;

}

// puts the binary file in memory (Little endian)
void binaryFileLoader(FILE *readFile, uint8_t *memArr) {

  // set size of the binary file to variable
  int fSize = sizeOfBinFile(readFile);

  // put binary file in memory byte by byte
  
  int result = 0;
  result = fread(machine.memory, 1, fSize, readFile);          

  // tells you if all bytes are put in memory.
  if (result != fSize) {
    fputs("Reading Error", stderr); exit(EXIT_FAILURE);
  }

}
