#ifdef __cplusplus
extern "C" {
#endif

/* from MAP_ROUTINES.C */

#include "dbseg_srgtek_contour_routines.h"
/***  
*   For some reason, the pow function declared in math.h doesn't seem to work.  
*   So I'm going to stick in the example one from Kernighan and Ritchie.  
*
***/

int power (int base, int n)
{
  int i, p;

  p = 1;
  for (i=1;i<=n;++i)
    p = p * base;
  return p;
};


int make_bitmap(int h_size,
        int v_size,
        chainmap **returned_chainmap)
{
   
   int index;
   
   if ((*returned_chainmap = (chainmap *)malloc(sizeof(chainmap))) == NULL)
   {
      printf("Insufficient memory.\n");
      return (0);
   }
         
   if (((*returned_chainmap)->map = (int *)malloc(h_size * v_size * sizeof(int))) == NULL)
   {
      printf("Insufficient memory.\n");
      return (0);
   }
   
   (*returned_chainmap)->h_size = h_size;
   (*returned_chainmap)->v_size = v_size;
   
   for (index = 0;index < h_size*v_size;index++)    
   {
      (*returned_chainmap)->map[index] = 0;
   }
   
   return(1);
   
};

void setpos (int h_pos,
         int v_pos,
         chainmap *change_map)
{
   int bytepos;
   
   bytepos = (change_map->h_size)*v_pos + h_pos;
   
   change_map->map[bytepos] = 1;
};

int getpos (int h_pos,
        int v_pos,
        chainmap *change_map)

{
   int bytepos;
   
   bytepos = (change_map->h_size)*v_pos + h_pos;
   
   return (change_map->map[bytepos]);
   
   
};
    
/*************************
 from SAVE_CONTOUR_POINTS
/*************************

/**************************************************************
*
*   save_contour_points(filename,start_x,start_y,chain)
*
*   Given a Freeman chain of vectors and a starting point, 
*   writes a list of the points traversed to the file specified
*   in 'filename'.
*
***************************************************************/

void save_contour_points (char*    filename,
              const int *chain)

{

  int current_element = 0;   /* Used to step through the chain of vectors. */
  
/*** Create the write-only file.    ***/

FILE* point_file = NULL;
if ((point_file = fopen(filename,"w")) == NULL)
    {
        printf ("Error opening file.\n");
    }
    
/*** Write the points to the file until the vector is exhausted. ***/

while (chain[current_element] != ENDOFCHAINLIST)
    {
/***    figure out coordinates of neighbor    ***/
/***
    if ( (chain[current_element] < 2) || (chain[current_element] > 6) )
        ++x;

    if ( (chain[current_element] > 2) && (chain[current_element] < 6) )
        --x;

    if ( (chain[current_element] > 0) && (chain[current_element] < 4) )
        ++y;

    if (chain[current_element] > 4)
        --y;
****/
    fprintf (point_file,"%d\t%d\n",chain[current_element],chain[current_element+1]);
    
    current_element += 2;
    }
    
};

/***********************
 from FIND_CONTOUR.CC
//***********************

/*****************************************************************************
*
*    contour.c
*
*    Author:    Tim Feldman
*        Island Graphics Corporation
*
*    Vectorizes the outline of an elevation contour in a set of sampled
*    data.  Uses Freeman chain encoding.
*
*****************************************************************************/

FILE *debug_file;

/*****************************************************************************
*
*    in_cont(x, y)
*
*    Determines whether the sample point at 'x, y' is within the contour
*    being outlined.  Points outside of the array of samples are not
*    in the contour.
*
*    Returns 0 if the point is not in the contour.
*    Returns 1 if the point is     in the contour.
*
*****************************************************************************/

short in_cont(int x, 
          int y, 
          int elev, 
          const int *sample, 
          const Boundsrect *boundary)

{
   if ( (x < boundary->x_min) || (x > boundary->x_max) || (y < boundary->y_min) || (y > boundary->y_max) )
   return(0);
   
   if (Get2D(sample,boundary->x_max,y,x) == elev)
   return(1);
   
   return(0);
};

/*****************************************************************************
*
*    probe(x, y, dir, new_x, new_y)
*
*    Checks a sample neighboring 'x, y' to see if it is in the contour
*    being outlined.  'dir' specifies which neighboring sample to check.
*    'new_x, new_y' always get the coordinates of the neighbor.
*
*    Returns 0 if the neighbor is not in the contour.
*    Returns 1 if the neighbor is     in the contour.
*
*****************************************************************************/

short probe(int x, 
        int y, 
        int elev, 
        int dir, 
        int *new_x, 
        int *new_y, 
        const int *sample, 
        const Boundsrect *boundary)

{
   /***    always return the new coordinates    ***/
   
   *new_x = x;
   *new_y = y;

   /***    figure out coordinates of neighbor    ***/
   
   if ( (dir < 2) || (dir > 6) )
   *new_x += 1;
   
   if ( (dir > 2) && (dir < 6) )
   *new_x -= 1;
   
   if ( (dir > 0) && (dir < 4) )
   *new_y += 1;
   
   if (dir > 4)
   *new_y -= 1;
   
   /***    determine if the new sample point is in the contour    ***/
   
   return(in_cont(*new_x, *new_y, elev, sample, boundary));
};

/*****************************************************************************
*
*    neighbor(x, y, last_dir, new_x, new_y)
*
*    Finds a neighbor of the sample at 'x, y' that is in the same
*    contour.  Always follows the contour in a clockwise direction.
*    'last_dir' is the direction that was used to get to 'x, y'
*    when it was found.  'new_x, new_y' always get the coordinates
*    of the neighbor.
*
*    This procedure should only be called for a sample that has at
*    least one neighbor in the same contour.
*
*    Returns the direction to the neighbor.
*
*****************************************************************************/

int neighbor(int x, 
         int y, 
         int elev, 
         int last_dir, 
         int *new_x, 
         int *new_y, 
         const int *sample, 
         const Boundsrect *boundary)

{
   int    n;
   int    new_dir;
   
   /***    figure out where to start looking for a neighbor --
     always look ahead and to the left of the last direction
     
     if the last vector was 0
     then start looking at  1
     
     if the last vector was 1
     then start looking at  3
     
     if the last vector was 2
     then start looking at  3
     
     if the last vector was 3
     then start looking at  5
     
     if the last vector was 4
     then start looking at  5
     
     if the last vector was 5
     then start looking at  7
     
     if the last vector was 6
     then start looking at  7
     
     if the last vector was 7
     then start looking at  1    ***/
   
   if (last_dir & 0x01)
   {
      /***    last dir is odd --
    add 2 to it        ***/
      
      new_dir = last_dir + 2;
   }
   else
   {
      /***    last dir is even --
    add 1 to it        ***/
      
      new_dir = last_dir + 1;
   }
   
   /***    keep new_dir in the range 0 through 7    ***/
   
   if (new_dir > 7)
   new_dir -= 8;
   
   /***    probe the neighbors, looking for one on the edge    ***/
   
   for (n = 0; n < 8; n++)
   {
      if (probe(x, y, elev, new_dir, new_x, new_y, sample, boundary))
      {
     /***    found the next clockwise edge neighbor --
       its coordinates have already been
       stuffed into new_x, new_y        ***/
     
     return(new_dir);
      }
      else
      {
     /***    check the next clockwise neighbor    ***/
     
     if (--new_dir < 0)
     new_dir += 8;
      }
   }
   /***    This means that we've encountered a one-point contour!    ***/
   printf ("SQUAWWK!  Something's wrong in NEIGHBORville!\n");
   return -1;
};

/*****************************************************************************
*
*    build()
*
*    Builds a Freeman chain of vectors describing the edge of the
*    contour with elevation 'elev'.  Always follows the contour
*    in a clockwise direction.  Uses 'start_x, start_y' as tentative
*    starting point; modifies them to hold coordinates of first point
*    in chain.
*
*    Returns 0 if unsuccessful.
*    Returns 1 if   successful.
*
*****************************************************************************/

void find_contour(int elev,
          int *chain,
          int chainlength,
          const int *sample,
          int width,
          int height,
          int start_x, /* Maybe just replace this and the one after with x and y? */
          int start_y,
          int *gnew,   
          chainmap *contour_map)

{
   int        x;
   int        y;
   int        new_x;
   int        new_y;
   int        dir;
   int        last_dir;
   int          corner_x;
   int          corner_y;    
   int          corner;
   Boundsrect   *boundary;
   int          scratch_x;
   int          scratch_y;

   boundary = (Boundsrect *)malloc(sizeof(Boundsrect));
   
   boundary->x_min = 0;
   boundary->y_min = 0;
   boundary->x_max = width;
   boundary->y_max = height;
   
   /***    check if the starting point
     has no neighbors in the contour --
     the starting direction to check is arbitrary    ***/
   
   x = start_x;
   y = start_y;
   
   dir = 0;
   
   for ( ; ; )  /* This whole loop should be in a different function. */
   {
      if (probe(x, y, elev, dir, &new_x, &new_y, sample, boundary))
      {
     /***    found a neighbor in that direction
       (its coordinates are in new_x, new_y
       but we don't use them here)        ***/
     
     break;
      }
      
      /***    try next direction    ***/
      
      if (++dir == 8)
      {
     /***    starting point has no neighbors --
       make the chain one vector long    ***/
     
     /*** Make sure there's room for the information    ***/
     
     if (chainlength < 1)
     {
        printf("Output array has no length.\n");
        free(boundary);
        return;
     }
     
     /***    fill in the vector --
       the direction is arbitrary,
       since the point is isolated    ***/
     
     *gnew += 2;
     
/* This line is for debugging purposes only and should be removed. */
/*     printf ("(%d,%d)\n",new_x,new_y);  */

     chain[*gnew]=new_x;
     chain[(*gnew)+1]=new_y;
     chain[(*gnew)+2] = (int)CHAINEND;
     chain[(*gnew)+3] = (int)CHAINEND;
     
     setpos (new_x,new_y,contour_map);
     free(boundary);
     return;
      }
   }
   
   /***    get ready to follow the edge --
     This direction is down and to the right.      
     Figure out why it should be so.   ***/

   
   last_dir = 1;
   
   /***    follow the edge (counter?) clockwise, building a Freeman chain    ***/
   
   for ( ; ; )
   {
      /***    get the next point on the edge
    and the vector to it        ***/
    
      dir = neighbor(x, y, elev,last_dir, &new_x, &new_y, sample, boundary);
      
      /***    allocate storage for the new vector    ***/
      
      *gnew += 2;

      if ((*gnew + 3) > (chainlength - 1))   /*** 3 because you need space for two integers in
                          order to mark the end of a chain.  ***/
      
      {
     printf("Output array is of insufficient length.  Error 1.\n");
     free(boundary);
     return;
      }
      
      /*** fill in the new vector   ***/
/* #ifdef D_SEARCH_FLAG
      if ((probe(x,y,elev,dir - 1,&corner_x,&corner_y,sample,boundary)) && (dir & 0x01))
      {  */
     /* dir is odd, so the piece it cuts off must be added to the list, as well.  */
/*
     corner = *new;
     *new += 2;
     
     if ((*new + 3) > chainlength - 1)
     {
        printf("Output array is of insufficient length.  Error 1.\n");
        free(boundary);
        return;
     }
     dir2point(x,y,dir-1,&scratch_x,&scratch_y);
     chain[corner] = scratch_x;
     chain[corner+1] = scratch_y;
     chain[*new]    = new_x;
     chain[(*new)+1] = new_y;
     chain[(*new)+2]  = (int)CHAINEND;
     chain[(*new)+3]  = (int)CHAINEND;
     
     setpos (scratch_x,scratch_y,contour_map);
     setpos (new_x,new_y,contour_map);
     
      }
      else
      {
#endif     
*/
/* This line is for debugging purposes only and should be removed. */
/*     printf ("(%d,%d)\n",new_x,new_y);  */

     chain[*gnew] = new_x;
     chain[(*gnew)+1] = new_y;
     chain[(*gnew)+2] = (int)CHAINEND;
     chain[(*gnew)+3] = (int)CHAINEND;
     
     setpos (new_x,new_y,contour_map);
/*
#ifdef D_SEARCH_FLAG
      }
#endif*/       
      /***    maybe done with contour    ***/
      
      
      if ( (new_x == start_x) && (new_y == start_y) )
      {
     free(boundary);
     return;
      }
      
      
      /***    else get ready to continue following the edge    ***/
      
      x = new_x;
      y = new_y;
      last_dir = dir;
   }
};

/*****************************************************************************
*
*    report()
*
*    Follows the Freeman chain of vectors describing the edge of the
*    contour with elevation 'elev'.  Reports the elevation, start point,
*    direction vectors, and the number of vectors in the chain.
*
*****************************************************************************/

void report(int elev, 
        const int *chain)

{
int  p;
char filename[32];

    printf("Choose a file in which to save contour points: ");
    scanf ("%s",filename);
    
    printf("Elevation = %d\n", elev);


    p = 0;

    while (chain[p] != (int)ENDOFCHAINLIST)
    {
        p++;
    }
    if (p > 1)
        printf("%d elements in the chain.\n", p);
    else if (p > 0)
        printf("1 element in the chain.\n");
        else 
                printf("No contour points found.\n");
    
    save_contour_points(filename,chain);  
};

/****************************************************************************
*
*       find_start (x,y,image, boundary, elev)
*
*       Passed a pointer to an image, as well as size information, this 
*       function scans the image looking for a point with value 'elev'.
*       If it finds it, it returns its coordinates in 'x' and 'y'.
*
*****************************************************************************/

void find_start (int elevation,
         int *const chain,
         int chainlength,
         const int *sample,
         int width,
         int height)

{
   int index;
   int x;
   int y;
   int gnew;
   chainmap *contour_map;
   int prev_value;
   
   /* This line is for debugging purposes only, and should be removed. */
/*   debug_file = fopen ("debug","w"); */
   

   gnew = -2;
   
   if (make_bitmap(width+1,height+1,&contour_map) == 0)
   {
      printf ("Insufficient memory!\n");
      return;
   }
   
   prev_value = -1;  /*** This is to take care of the case when the first point
               is on a contour.  ***/
   
   for (index = 0;index < (((width) + 1)*((height) +1));index++){
     x = (index %((width)+1));
     y = index / (width+1);
     /*((sample[index] == elevation) && (sample[index+1] != elevation) && (x != width))*/
     /*((sample[index] == elevation) && (prev_value != elevation && (x != 0))) ||*/
     if (((sample[index] == elevation) && (prev_value != elevation) && (x != 0))
     && (getpos (x,y,contour_map) == 0)){
     
       find_contour(elevation, chain, chainlength, sample, width, 
            height, x, y, &gnew, contour_map); 
       /*     printf("The starting index is: %d\n",index); */
       gnew+=2;
     }
     prev_value = sample[index];
     
   }
   gnew+=2;
   
   /* This line is for debugging purposes only and should be removed. */
   /*     printf ("(%d,%d)\n",(int)ENDOFCHAINLIST,(int)ENDOFCHAINLIST); */
   
   chain[gnew]=(int)ENDOFCHAINLIST; /*  ENDOFCHAINLIST  = -2*/
   chain[gnew+1]=(int)ENDOFCHAINLIST;
   free(contour_map->map);
   free(contour_map);
   
   /*   fclose (debug_file); */
   return;
};

/* DIR2POINT
   Converts a direction and a starting point into the coordinates of a new point.
   The directions are
                       5  6  7
                \ | /
               4 - - 0
                / | \
               3  2  1
*/

int dir2point (int x, 
           int y, 
           int dir, 
           int *new_x, 
           int *new_y)
{
   *new_x = x;
   *new_y = y;

    if ( (dir < 2) || (dir > 6) )
        ++(*new_x);

    if ( (dir > 2) && (dir < 6) )
        --(*new_x);

    if ( (dir > 0) && (dir < 4) )
        ++(*new_y);

    if (dir > 4)
        --(*new_y);
   return 1;
};

int Get2D(const int* the_array,
      int x_dim,
      int y,
      int x)
{
   return the_array[y*(x_dim+1)+x];
};




#ifdef __cplusplus
}
#endif





