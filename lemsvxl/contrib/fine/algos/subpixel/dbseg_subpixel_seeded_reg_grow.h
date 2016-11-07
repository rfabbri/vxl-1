#ifdef __cplusplus
extern "C" {
#endif

/* srg_types.h    -- Thomas
 *
 * Time-stamp: <96/12/23 16:48:45 tbs>
 * $Header: /vision/projects/cvsroot2svn/cvsroot/lemsvxlsrc/contrib/fine/algos/subpixel/dbseg_subpixel_seeded_reg_grow.h,v 1.1 2009-06-09 22:29:18 fine Exp $
 * $Log: not supported by cvs2svn $
 */

#ifndef _SRG_TYPES_H_
#define _SRG_TYPES_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>

#define UNALLOCATED -1000
/* #define NEIGHBOR 1 */
#define ALLOCATED 1000
#define MAX_NEW_NBRS 4

#define NEW_NEIGHBOR 1001
#define PSEUDO_NEIGHBOR 1002
#define NOT_NEW_NEIGHBOR 1003

#define BACKGROUND 255
#define DAM 254

typedef struct{
  unsigned char *image_array; /* Image */
  int *seed_array; /* Seed array. Whenever a pixel is added to a region, seed_array changes.
              Can either be a region label or BACKGROUND (DAM). Doesn't change when 
              the pixel is added to the neighbor list.*/
  int *alloc_flag; /* Keeps track of the status of pixels, UNALLOCATED, ALLOCATED, or any of
              the region labels (indicating NEIGHBOR) are the options. */
  int *contour_array; /* Keeps track of the initial neighbors. I don't think it is used
             after the initialization step. */
  float *means; /* Stores the mean of each region. Size of array is no_of_regions */
  int *no_of_pixels; /* Stores the pixel count of each region. Size of array is no_of_regions */
  int unalloc_count; /* Number of pixels not yet added to the neighbot list (this is 
            the first step for a pixel to be allocated to a region) */
  int alloc_count; /* Number of pixels that have been added to regions. These pixels won't 
              be touched again. */
  int nbr_count; /* Number of pixels in the neighbor list. */
} SRG_Params;

typedef struct SRG_List_node *SRG_List_node_ptr;

/* Linked list (sorted based on distance) of all the pixels flagged as
   neighbors. */
typedef struct SRG_List_node{
  int x,y,z; /*Location*/
  int nbr_region; /*label of adjoining region*/
  float distance; /* distance (intensity-mean or smth. like that)*/ 
  SRG_List_node_ptr next;
} SRG_List;
  

/* This structure keeps the parameters of the new neighbor pixels at each 
   step. */
typedef struct{
  int x,y,z; /*Location*/
  int nbr_region; /*label of adjoining region*/
  int nbr_flag; /* label indicating what kind of neighbor the pixel is*/
}Nbr_Params;


/*********************
  FUNCTION HEADERS
  *******************/

void find_means(unsigned char *image_array, int *seed_array, 
        float *means, int *no_of_pixels,int *alloc_array,
        int no_of_regions,int height, int width);

void find_initial_nbrs(int *seed_array, int *nbr_array, int *alloc_array,
               int height, int width, int *unalloc_ct, int *nbr_ct);

void find_initial_distances(unsigned char* image_array, int *alloc_array,
                int *nbr_array, int height,int width,
                SRG_List *srg_list, float *means);

void insert_SRG_list(SRG_List *srg_list, float distance, int x, int y, int nbr);

void free_SRG_list(SRG_List *srg_list_head);

SRG_List * new_SRG_element(int x, int y, int nbr, float distance);

SRG_List *smallest_distance(SRG_List *srg_list, int *seed_array, int *alloc_array,
               unsigned char *image_array, float *means);

int find_new_nbrs(int *seed_array,int *contour_array,
        int *alloc_flag, SRG_List *alloc_ptr, 
        int width, int height, Nbr_Params *new_nbrs);

Nbr_Params new_nbr_element(int x, int y, int nbr);


void seeded_region_growing_2d(unsigned char * image_array,
                  int * seed_array,
                  int no_of_regions,
                  int height,
                  int width,
                  double distance_threshold);
#endif

#ifdef __cplusplus
}
#endif

