#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "tuple.h"
#include "encoder.h"
#include "assemblerUtils.h"

#define MAX_ARGS 3

//1st argument will be source .s assembly file, 
//2nd argument is binary file to write to
int main (int argc, char* argv[]) {   
                                      
  //If number of args isn't 3 please enter correct number of args.
  if (argc != MAX_ARGS) {
    perror("Please enter 2 arguments (the input file and outut file)");
    exit(EXIT_FAILURE);
  }

  //Create commands file and open it for reading.
  //Create output binary file and open it for writing.
  FILE* assemblyCommands;
  FILE* binaryCommands;

  assemblyCommands = fopen(argv[1], "r");    //Source file
  binaryCommands = fopen(argv[2], "wb");     //Binary file to write to

  //Handle null file errors.
  if(assemblyCommands == NULL) {
    perror("Error in opening assembly file");
    exit(EXIT_FAILURE);
  }

  if(binaryCommands == NULL) {
    perror("Error in opening binary file");
    exit(EXIT_FAILURE);
  }

  //creates enough space for the lookUpTable.
  //fseek with SEEK_END makes pointer point to the end of the file
  //ftell gets the offset in bytes between the current position pointed to
  //and the start of the file
  //fseek with SEEK_SET makes pointer point to start of the file.
  //declare the lookup table.
  fseek(assemblyCommands, 0, SEEK_END);
  int size = ftell(assemblyCommands);
  fseek(assemblyCommands,0,SEEK_SET);
  //This lookup table bigger than it needs to be
  struct stringIntTuple* lookUpTable = 
        calloc(size, sizeof(struct stringIntTuple));

  //Sets all elements of lookUpTable to be empty
  struct stringIntTuple empty = {0, ""};
  for (int i = 0; i < size; i++) {
    lookUpTable[i] = empty;
  }
  
  //use fgetc to traverse through file and creates lookup table of labels.
  //lookup table will only have to be use when encoding branch.
  int lines = -1; //As lines is being incremented/decremented, it is used to
                 //store the line number of the label in the lookup table.
                 //Afterwards, lines will be total no. of lines.
  int actual_Lines = 0;               
  int sizeOfLookUpTable = 0;
  int i = 0;
  char currentChar = '\0';
  char *temp = calloc(MAX_LINE_SIZE, sizeof(char)); // You could have a label take up whole line
  char *tempBeginning = temp;
  
  do { 
    actual_Lines++;
    lines++;       //Increase lines whenever we find a '\n'

    do { // get next character after spaces in currentChar
        currentChar = fgetc(assemblyCommands);
    } while (currentChar == ' ');
      
    if (currentChar == '\n') {  
        lines--;         //Accounts for any line being empty
    }
    
    int closeBracketCount = 0;
    int charsPassed = 0;

    while(currentChar != '\n' && currentChar != EOF) {

      if (currentChar == ')' && (closeBracketCount == 0) && (charsPassed == 0)) {
        printf("looks like your winky face comment has no eyes at line %d!\n", actual_Lines); 
        exit(EXIT_FAILURE);
      }

      if (currentChar == ';' && (charsPassed == 0)) {
        if ((currentChar = fgetc(assemblyCommands)) != ')') {
        printf("looks like your winky face comment has no mouth at line %d!\n",actual_Lines); 
        exit(EXIT_FAILURE);
        }
        closeBracketCount++;
      }
      
      if(currentChar == ':') {

        //Copy temp into label in struct with no spaces in front;
	while (temp[0] == ' ') {
          temp++;
        }
        strcpy(lookUpTable[sizeOfLookUpTable].label, temp); 
        //Memory address is the BYTE-ADDRESSABLE address of instruction
        lookUpTable[sizeOfLookUpTable].memoryAddress = 4*lines;
        sizeOfLookUpTable++;
        actual_Lines--;
        lines--;  //If line is a label, it is part of the next line
        break;
       
      }

      temp[i] = currentChar;    //Builds up temp string (label)
      currentChar = fgetc(assemblyCommands); // set current char to next char.
      i++;     //This next char will be the next character in temp
      charsPassed++;  
    }
    

    memset(temp,'\0',i); //reset temporary string every new line.
    i = 0;
  
  } while(currentChar != EOF);   //EOF = end of file
  
  //set assemblyCommands pointer back to the begininng of the file;
  rewind(assemblyCommands);

  //use fgets to pull an instruction from assembelyCode, encode it,
  //then put in an array of instructions for fwrite.
  //in the beginning the array holds all zero instructions, all halt.
  uint32_t *instrArray = (uint32_t*)calloc(lines*2, sizeof(uint32_t));
  int j = 0;  //Counter for index of instrArray

  while (fgets(temp, MAX_LINE_SIZE, assemblyCommands) && j < lines) {

    while (temp[0] == ' ') {
      temp++;
    }
      
    if (!isLineEmpty(temp)) {
      char *instruction = strtok(temp, ";)");

      if (!isLineALabel(instruction)) {
        //encoded binary representation of the instruction.
        //pass j*4 so branch and sdt can see current address 	
        uint32_t encoded = encode(instruction, instrArray, lines*2, lookUpTable, 4*j); 
        instrArray[j] = encoded;
        j++;
      }
    
    }
    
      
  }

  //traverse through the array of instructiuons and add to the binary file
  //until we reach a halt.
  //lines is now the total number of lines
  int k = 0; //Choosing element of instrArray to write - will be incremented
  
  //After all actual instructions have been written, stop when we 
  //find an all 0 number in the instrArray, which will be after
  //all ldr numerical constants > 0xFF.
  while (instrArray[k] != 0 || k < lines) { 
    //Write one 32-bit encoded instruction at a time
    fwrite(&instrArray[k], sizeof(uint32_t),1, binaryCommands);
    k++; 
  }

  //Close all files opened using fopen now we are finished with them
  fclose(assemblyCommands);
  fclose(binaryCommands);

  //free the space on the heap allocated using calloc earlier
  temp = tempBeginning;
  free(temp);
  free(lookUpTable);
  free(instrArray);

  return EXIT_SUCCESS;

}
