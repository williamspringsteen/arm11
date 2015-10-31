#ifndef BIN_METHODS
#define BIN_METHODS

void printBits(uint32_t x);
int getSpecificBit(uint32_t input, int location);
uint32_t binaryToIntRange(int width, int maskShift, uint32_t instruction);
int signedBinaryToIntRange(int width, int maskShift, uint32_t instruction);
struct shift_struct* shift(enum ShiftCode shiftCode, int shiftNumber, int regNum);
int rotateRight(uint32_t input, int rotateValue);
int32_t branchOffsetGetter(uint32_t instruction);
uint32_t getWord(int memoryAddress);
void putWord(int memoryAddress, uint32_t data);
void printMemory();
void printRegisters();
uint32_t setSpecificBit(uint32_t instruction, uint32_t carry, int location);
bool checkCondition(enum Cond condition);
uint32_t getWordLittleEndian(int memoryAddress);
bool isGPIOPin(uint32_t address);
bool isGPIOSetOrClear(uint32_t address);

#endif
