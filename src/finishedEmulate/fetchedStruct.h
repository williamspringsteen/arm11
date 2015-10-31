#ifndef FETCH_STRUCT
#define FETCH_STRUCT
#include <stdbool.h>

//FETCHED INSTRUCTION NEEDS TO CARRY ROUND THE INSTRUCTION ITSELF 
//AS A uint32_t AND WHETHER IT IS CLEARED OR NOT
struct fetched_final {

  uint32_t fetched_instr;
  bool isFetchedCleared;

}; //

//Fetched is global and will hold the most recently fetched instruction in the
//pipeline, which will be the next instruction to be decoded.
//Will be initialised to cleared.
//Defined in main.
extern struct fetched_final Fetched;    

uint32_t fetch(void);

#endif
