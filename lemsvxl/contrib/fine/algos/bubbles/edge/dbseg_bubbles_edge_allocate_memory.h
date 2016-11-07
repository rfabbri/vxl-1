#ifndef _ALLOCATE_MEMORY_H_
#define _ALLOCATE_MEMORY_H_

#include "bubbles_edge.h"

void allocate_memory_for_deform(Bubble_Deform *BubDeform, int height, int width);
void free_memory_for_deform(Bubble_Deform *BubDeform); 
void allocate_cedt_data(Cedt *cedt, int height,int width);
void free_cedt_data(Cedt *cedt);


#endif /* ALLOCATE_MEMORY */


