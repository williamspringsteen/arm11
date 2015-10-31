#include <stdbool.h>
#include <stdint.h>
#include "emulateEnums.h"
#include "machineStruct.h"
#include "binMethods.h"
#include "fetchedStruct.h"

//The little endianness of the binary file should be taken into account 
//when being put into pipeline
uint32_t fetch(void) {		          
  
  int PC = machine.registers[PCREG];
	
  return getWord(PC);                     // Return instruction created
  
}
// END FETCH
