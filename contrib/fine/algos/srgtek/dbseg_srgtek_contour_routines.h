#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONTOUR_ROUTINES_H
#define CONTOUR_ROUTINES_H


/*****************************************************************************
*
*    Include files
*
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
/***#include <malloc.h> ***/       

/*****************************************************************************
*
*    Structure definitions
*
*****************************************************************************/

/***    a Vector is one link in a simple chain that follows
    the edge of a contour from sample point to sample point    ***/

typedef struct vector
{
    short        dir;
    struct vector *    next;
} Vector;

/***    these are the 'dir' values in a Vector:

    0    right
    1    right and up
    2    up
    3    left and up
    4    left
    5    left and down
    6    down
    7    right and down        ***/


struct boundsrect
{
    int x_min;
    int y_min;
    int x_max;
    int y_max;
};

typedef struct boundsrect Boundsrect;

/*** This lets you treat a one-d array as a 2-day array.  x_dim is the largest x
  index.  Thus, an 8X8 array would have x_dim as 7.    ***/

/*#define Get2D(the_array,x_dim,y,x) (the_array[y*(x_dim+1)+x])*/
#define CHAINEND -1
#define ENDOFCHAINLIST -2


typedef struct pre_chainmap
{
    int h_size;
    int v_size;
    int *map;
} chainmap;

/**********************
* The FUNCTIONS
**********************/
short in_cont(int x, 
          int y, 
          int elev, 
          const int *sample, 
          const Boundsrect *boundary);

short probe(int x, 
        int y, 
        int elev, 
        int dir, 
        int *new_x, 
        int *new_y, 
        const int *sample, 
        const Boundsrect *boundary);

int neighbor(int x, 
         int y, 
         int elev, 
         int last_dir, 
         int *new_x, 
         int *new_y, 
         const int *sample, 
         const Boundsrect *boundary);

void find_contour(int elev,
          int *chain,
          int chainlength,
          const int *sample,
          int width,
          int height,
          int start_x, 
          int start_y,
          int *gnew,   
          chainmap *contour_map);

void report(int elev, 
        const int *chain);

void find_start (int elevation,
         int *const chain,
         int chainlength,
         const int *sample,
         int width,
         int height);

int dir2point (int x, 
           int y, 
           int dir, 
           int *new_x, 
           int *new_y);

int Get2D(const int* the_array,
      int x_dim,
      int y,
      int x);

int power (int base, int n);

int make_bitmap(int h_size,
        int v_size,
        chainmap **returned_chainmap);

void setpos (int h_pos,
         int v_pos,
         chainmap *change_map);

int getpos (int h_pos,
        int v_pos,
        chainmap *change_map);

void save_contour_points (char*    filename,
              const int *chain);
              
              
#endif

#ifdef __cplusplus
}
#endif

