//------------------------------------------------------
//Biliana Kaneva, Smith College
//structs.cpp
//
// Overriding the new and delete operators in order to 
// gather information about the amount of dynamically
// allocated memory.
//
//------------------------------------------------------

/*--------------------------------------------------------------------
This code is Copyright 2000 by Biliana Kaneva & Joseph O'Rourke.  
It may be freely redistributed in its entirety provided that this 
copyright notice is not removed.  Question to orourke@cs.smith.edu.
--------------------------------------------------------------------*/


#include "structs.h"
#include <vcl_cstddef.h>
#include <vcl_cstdlib.h>

#ifdef MEM_COUNT

void *operator new(size_t k)
{
  void *p = malloc(k);
  maxmemory+=k;
  if (maxm < maxmemory)
    maxm = maxmemory;
  return p;
}

void *operator new [](size_t k)
{
  void *p = malloc(k);
  maxmemory+=k;
  if (maxm < maxmemory)
    maxm = maxmemory;
  return p;
}

void operator delete(void *p)
{
  free(p);
}

void operator delete [](void *p)
{
   free(p);
}

#endif
