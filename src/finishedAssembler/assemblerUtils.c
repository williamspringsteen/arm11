#include <stdint.h> 
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "assemblerUtils.h"

#define WORD_SIZE 32
//Returns true if string is a label
bool isLineALabel(char* string) {           
  
  for (int i = 0; i < MAX_LINE_SIZE; i++) {
    if (string[i] == ':') {                //If it finds a colon, it is a label
      return true;
    }
    
    if (string[i] == '\n') {              //If it finds a '\n', we reached end
      return false;                       // of string without finding a ':'
    }
  }
  
  return false;                       //Need this to stop compiler complaining
  
}

//Returns true if string is just a '\n'
//line is also emty if it's all a comment
bool isLineEmpty(char* string) {        
  
  return (string[0] == '\n' || string[0] == ';');
  
}

//Should call when there is a free space in the array
int getNextFreeAddress(int memorySize, uint32_t memory[]) {               
  
  int i = memorySize/2;       //Checks bottom half of array
  
  while (i < memorySize) {
    if (memory[i] == 0) { 
      return i;
    }
    i++;
  }
  
  return -1;         //Returns -1 if there wasn't a free space
  
}

// a helper method needed for encodeSDT
void removeCharFromString(char charToRemove, char* string) {

  char *src, *dest;

  src = dest = string; // all pointing to the same place
  
  while (*src != '\0') {
    
    if (*src != charToRemove) {
      *dest = *src;
      dest++;
    } 
//  printf("%s\n" , string);
  src++;
  } 
  
  *dest = '\0';
  
}

//checks whether the input string is hex or int.
//Pre: Unsigned interger string.
uint32_t convertIntString(char *intString) {

  uint32_t output;

  if(intString[1] == 'x') {                    
    
    output = (uint32_t)strtol(intString, NULL, 0);     //(0 passed because hex value has 0x at beginning)
     
  } else {
    
    output = atoi(intString);                   
  }
  
  return output;
  
} 

//returns whether intstring is negative
//(If first character is '-')
bool isNegativeIntString(char *intString) {

  bool isNegative = false;
  
  if (intString[0] == '-') {
    isNegative = true;
  }

  return isNegative;

}

int power(int base, int exponent) {
  
  int total = 1;
  
  for(int i = 0; i < exponent; i++) {
    total = total * base;  
  }
  
  return total;
  
}
 
uint32_t shift(char *operand2){
  
  removeCharFromString(' ', operand2);

  char *rm = strtok(operand2, ","); 
  char *restOfOperand2 = strtok(NULL, "\n");
    
  uint32_t registervalue =  atoi(strtok(rm,"r")); // register number
  
  if(restOfOperand2 == NULL) {
    
    return registervalue;
  
  } else {
    
    // this will have a space at the front
   
    uint32_t mask;
    
    if(restOfOperand2[0] == 'l') {
      
      if(restOfOperand2[2] == 'l') {
        mask = 0;
      } else {
        mask = 1 << 5;
      }
    
    } else if(restOfOperand2[0] == 'a') {
      
      mask = 2 << 5;
      
    } else {
      
      mask = 3 << 5;
      
    }
   
    restOfOperand2 = restOfOperand2 + 3;  //shift pointer by 3
   
    if(restOfOperand2[0] == '#') {
     
      char *removehash = strtok(restOfOperand2,"#");
     
      uint32_t value;

      //change the address string to an int

      value = convertIntString(removehash);

      value = value << 7;
      return value | mask | registervalue;
     
    } else {
     
      uint32_t indicator = 1 << 4;

      uint32_t registervalue2 = atoi((strtok(restOfOperand2," r")));
      registervalue2 <<= 8;
     
      return indicator | registervalue2 | mask | registervalue;
     
    }
  
  }
  
}

uint32_t rotation(char *operand2){
// HERE WE ASSUME THAT THE OPERAND2 ENDS WITH AN \0
  
  uint32_t total = 0;

  if((operand2[0] == '0') && (operand2[1] == 'x')) {
    total = (int)strtol(operand2, NULL, 0);
  } else {
    total = atoi(operand2);
  }  
  
  uint32_t mask = 0xFF;
  
  if (total <= mask) {
    return total;
  }
  
  if((findfirstone(total) % 2) == 0) {
     
    if(findlastone(total) - findfirstone(total) < 8){
      
      if( findlastone(total) - findfirstone(total) < 0){
        printf("error as this number cannot be represented in rotation");
	exit(EXIT_FAILURE);
      }
      
      int rotation = ((WORD_SIZE - findfirstone(total)) % WORD_SIZE)/2;
      total = total >> (WORD_SIZE - rotation*2);
     
      rotation <<= 8;
      return (total | rotation);
      
    } else {
      printf("error as this number cannot be represented in rotation");
      exit(EXIT_FAILURE);
    }
    
  } else {
    
    if(findlastone(total) - findfirstone(total) < 7 ){
      
      if(findlastone(total) - findfirstone(total) < 0){
        printf("error as this number cannot be represented in rotation");
	exit(EXIT_FAILURE);
      }
      
      int rotation = (((WORD_SIZE+1) - findfirstone(total)) % WORD_SIZE) /2;
  
      total >>= WORD_SIZE - (rotation*2);
     
      rotation <<= 8;
    
      return (total | rotation);
      
    } else {
      printf("error as this number cannot be represented in rotation");
      exit(EXIT_FAILURE);
    }
    
  }
  
}

int findlastone(uint32_t binarynum){

  uint32_t mask = 1 << (WORD_SIZE-1); 
  int i  = (WORD_SIZE-1);
  
  while(((binarynum & mask) >> (WORD_SIZE-1)) != 1 && i > 0){
    binarynum = binarynum << 1; 
    i--;
  }

  return i;

}

int findfirstone(uint32_t binarynum){

  uint32_t mask = 1;
  int i  = 0;
  
  while((binarynum & mask) != 1 && i < (WORD_SIZE-1)){
    binarynum = binarynum >> 1; 
    i++;
  }

  return i; 
  
}
