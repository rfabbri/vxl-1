#ifndef _HEAP_H_
#define _HEAP_H_

/***************** heap.h from H.Tek *************************/
#include "cedt3d.h"

#define LARGE_NUMBER 99999.0
#define SMALL_NUMBER -99999.0

typedef struct {
  int    N;
  int   *index, *loc;
  int    size;
  dist_sq_t *data;
} Heap;

void up_minheap(Heap *heap, int k);
void insert_to_minheap(Heap *heap, const int& location, const int& data, const int& x, const int& y, const int& z);
void down_minheap(Heap *heap, int k); 
void remove_from_minheap(Heap *heap);
#endif /* _HEAP_H_ */
