//shift_struct used when return both the offset and the carry from the shift method
#ifndef SHIFT_STRUCT
#define SHIFT_STRUCT

//Used to return the offset itself, as well as the carry, 
//from the shift method in binMethods.c
struct shift_struct {

  uint32_t offset;
  uint32_t carry;

};

#endif
