#include "tuple.h"
#include <string.h>

//Function to help get something from lookUpTable.
//Pre: label is always in the table.
//if returns -1 label hasn't benn put in table correctly.
int lookUp(char labelToFind[], struct stringIntTuple* lookUpTable) {
  
  struct stringIntTuple empty = {0, ""};

  int address = -1; 
  
  int i = 0;
  
  while ((lookUpTable[i].memoryAddress != empty.memoryAddress) 
          && (strcmp(lookUpTable[i].label, empty.label) != 0)) {
    
    if (strcmp(lookUpTable[i].label,labelToFind) == 0) {
      address = lookUpTable[i].memoryAddress;
      break;
    }
    
    i++;
    
  }
  
  return address;
  
}
