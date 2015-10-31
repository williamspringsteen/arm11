#ifndef UTILS_H
#define UTILS_H
#include <stdint.h>

#define MAX_LINE_SIZE 512          //Line cannot have more than 512 characters

bool isLineALabel(char* string);
bool isLineEmpty(char* string);
int getNextFreeAddress(int memorySize, uint32_t memory[]);
void removeCharFromString(char charToRemove, char* string);
uint32_t convertIntString(char *intString);
bool isNegativeIntString(char *intString);
int power(int base, int exponent);
uint32_t shift(char *operand2);
int findlastone(uint32_t binarynum);
int findfirstone(uint32_t binarynum);
uint32_t rotation(char *operand2);

#endif
