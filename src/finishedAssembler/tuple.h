#ifndef TUPLE
#define TUPLE

#define LINE_LENGTH 512
//Struct to store a label and an associated memory address/line number
//Used for the lookup table
struct stringIntTuple {

  //This label variable is the label string without the colon.
  int memoryAddress;
  char label[LINE_LENGTH];
  
};

//Function to help get something from lookUpTable.
int lookUp(char labelToFind[], struct stringIntTuple* lookUpTable);

#endif
