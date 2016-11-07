/*------------------------------------------------------------*\
 * File: min_heap.c
 *
 * Functions:
 *    insert_to_minheap
 *    up_minheap
 *    remove_from_minheap
 *    down_minheap
 *
 * Ref.:    http://cfatab.harvard.edu/nr/bookc.html
 *
 * History:
 *    #0: by H.Tek
 *    #1: March '98, updated and commented by F.Leymarie
\*------------------------------------------------------------*/

#include <stdio.h>
#include <vcl_cmath.h> 
#include <stdlib.h> 

#include "heap.h" 


/*------------------------------------------------------------*\
 * Function: insert_to_minheap
 *
 * Usage: Hiring a new member into the Heap
\*------------------------------------------------------------*/
void insert_to_minheap(Heap *pTheHeap, const int& loc, const int& data, const int& x, const int& y, const int& z)
{
  pTheHeap->N++;
  pTheHeap->data[loc] = data;
  pTheHeap->index[pTheHeap->N] = loc;
  pTheHeap->loc[loc] = pTheHeap->N;
  up_minheap(pTheHeap, pTheHeap->N); /* Go find the proper promotion level */
  return;
}

/*------------------------------------------------------------*\
 * Function: up_minheap
 *
 * Usage: Finds the proper level of promotion in the Heap stack for the
 *     New Data (or "recruit"), by moving up the stack.
\*------------------------------------------------------------*/

void
up_minheap(Heap *pTheHeap, int iLocHeap) 
{ 
  int    iLocNewData;  
  int    iLocOldData;

  iLocNewData = pTheHeap->index[iLocHeap];
  iLocOldData = pTheHeap->index[(int) (iLocHeap/2.0)];


 while (iLocHeap > 0 && pTheHeap->data[iLocOldData] >= pTheHeap->data[iLocNewData])
    { //  Then shift down the lesser "competent worker" in  the stack
      pTheHeap->index[iLocHeap] = iLocOldData;
      pTheHeap->loc[iLocOldData] = iLocHeap;
      iLocHeap = (int) (iLocHeap/2.0);
      iLocOldData = pTheHeap->index[(int) (iLocHeap/2.0)];
    }

  // Found the right place in the Heap stack for our new "recruit" 
  pTheHeap->index[iLocHeap] = iLocNewData;
  pTheHeap->loc[iLocNewData] = iLocHeap;
  return;
}
/*------------------------------------------------------------*\
 * Function: remove_from_minheap
 *
 * Usage: Removes the top element from the heap and replaces it by the
 *     last one, which is then moved down the Heap to its appropriate
 *     level.
\*------------------------------------------------------------*/

void remove_from_minheap(Heap *pTheHeap)
{
  pTheHeap->index[0] = pTheHeap->index[pTheHeap->N];
//  pTheHeap->loc[pTheHeap->index[pTheHeap->N]] = 1;
  pTheHeap->loc[pTheHeap->index[pTheHeap->N]] = 0 ;
  pTheHeap->N--;
  down_minheap(pTheHeap, 0);  /* Go find right level in Heap for promoted element */
}

/*------------------------------------------------------------*\
 * Function: down_minheap
 *
 * Usage: Finds the proper level of promotion in the Heap stack for the
 *     promoted element, by moving down the stack.
\*------------------------------------------------------------*/
void
down_minheap(Heap *pTheHeap, int iLocHeap) 
{ 
  int    iLocData, iLocDownHeap, iLocHalfHeap;

  iLocData = pTheHeap->index[iLocHeap];

  iLocHalfHeap = (int) (pTheHeap->N / 2.0);
  while (iLocHeap <= iLocHalfHeap)
    {
      iLocDownHeap = iLocHeap + iLocHeap;
      if (iLocDownHeap < pTheHeap->N && 
          pTheHeap->data[pTheHeap->index[iLocDownHeap]] > pTheHeap->data[pTheHeap->index[iLocDownHeap+1]]){
              iLocDownHeap++;
      }
      if (pTheHeap->data[iLocData] <= pTheHeap->data[pTheHeap->index[iLocDownHeap]]) {
              break;
      }
      pTheHeap->index[iLocHeap] = pTheHeap->index[iLocDownHeap]; 
      pTheHeap->loc[pTheHeap->index[iLocHeap]] = iLocHeap;
      iLocHeap = iLocDownHeap;
    }

  // Found the right place in the Heap for our "promoted element"
  pTheHeap->index[iLocHeap] = iLocData; 
  pTheHeap->loc[iLocData] = iLocHeap;
}

/* =======================================================*/
