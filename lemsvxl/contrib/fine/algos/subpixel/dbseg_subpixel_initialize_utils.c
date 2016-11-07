#ifdef __cplusplus
extern "C" {
#endif

#include "dbseg_subpixel_subpixel_bubbles.h"


/**************************************************************************
  Description: Function that allocates memory for Window_Stats
  Inputs :  height,width - image dimensions
  Output :  win - filled  Window_Stats structure.

**************************************************************************/
void allocate_window_stats(Window_Stats *win, int height, int width)
{

  win->mean = (double *)calloc(height*width,sizeof(double));
  win->stdev = (double *)calloc(height*width,sizeof(double));
  win->no_of_pixels = (int *)calloc(height*width,sizeof(int));
}/* End of function allocate_window_stats */


/**************************************************************************
  Description: Function that allocates memory for CEDT
  Inputs :  height,width - image dimensions
  Output :  cedt - filled CEDT structure.

**************************************************************************/
void allocate_grid_cedt_data(Cedt *cedt, int height, int width)
{
  cedt->dx = (float*)calloc(height*width,sizeof(float));
  cedt->dy = (float*)calloc(height*width,sizeof(float));
  cedt->surface = (float*)calloc(height*width,sizeof(float));
  cedt->region = (int*)calloc(height*width,sizeof(int));
  cedt->dir = (int*)calloc(height*width,sizeof(int));
}

/**************************************************************************
  Description: Function that frees memory for CEDT
  Input :  cedt - filled CEDT structure.

**************************************************************************/
void free_grid_cedt_data(Cedt *cedt)
{
  free(cedt->dx);
  free(cedt->dy);
  free(cedt->surface);
  free(cedt->region);
  free(cedt->dir);
}

/**************************************************************************
  Description: Function that allocates memory for SP_Bubbles_Force
  Inputs :  height,width - image dimensions
  Output :  force - filled SP_Bubble_Force structure.

**************************************************************************/

void allocate_sp_bubbles_force(SP_Bubbles_Force *force, int height, int width)
{
  force->edge_force = (double *)calloc(height*width,sizeof(double));
  force->line_force = (double *)calloc(height*width,sizeof(double));
  force->statistics_force = (double *)calloc(height*width,sizeof(double));
  force->stop = (double *)calloc(height*width,sizeof(double));
  force->force_direction = (int *)calloc(height*width,sizeof(int));

  force->snake.snake_force = (double *)calloc(height*width,sizeof(double));
  force->snake.Sx = (double *)calloc(height*width,sizeof(double));
  force->snake.Sy = (double *)calloc(height*width,sizeof(double));

  alloc_mem_for_heap_SPCedt(&force->exp_heap,height*width);
  /* allocate_heap_data(&force->exp_heap,height*width); */
  initialize_heap_SPCedt(&force->exp_heap,height*width);
  /*force->exp_heap.end = 0;*/
  force->exp_heap.N = 0;  
  allocate_grid_cedt_data(&force->exp_cedt,height,width);
  /* initialize_grid_cedt(force->exp_cedt,height,width); */
  

}/* End of function allocate_sp_bubbles_force */


void allocate_data_SP_Cedt(SPCedt *cedt, int height, int width)     
{
  cedt->dx = (double *)  calloc(height*width,sizeof(double));
  cedt->dy = (double *)  calloc(height*width,sizeof(double));
  cedt->dir = (unsigned char *)  calloc(height*width,sizeof(unsigned char));
  cedt->surface = (double *) calloc(height*width,sizeof(double));
  cedt->label = (int *)  calloc(height*width,sizeof(int));
  cedt->tag = (unsigned char *) calloc(height*width,sizeof(unsigned char));
  cedt->type = (unsigned char *) calloc(height*width,sizeof(unsigned char));
}

void initialize_data_SP_Cedt(SPCedt *cedt, int height, int width) 
{
  int y,x,pos;

  for(y=0; y<height; y++) {
    for(x=0; x<width; x++) {
      pos = y*width+x;
      /* initialize CEDT */
      cedt->surface[pos] = 99999.0;
      cedt->dx[pos] = 0.0;
      cedt->dy[pos] = 0.0;
      cedt->dir[pos] = 17.0;
      cedt->tag[pos] = 3.0;
      cedt->type[pos] = 0;
      cedt->label[pos] = BACKGROUND;
    }
  }
}


void free_data_SP_Cedt(SPCedt *cedt)
{
  free(cedt->dx);
  free(cedt->dy);  
  free(cedt->dir);
  free(cedt->surface);
  free(cedt->label); 
  free(cedt->tag); 
  free(cedt->type);
}


/**************************************************************************
  Description: Function that allocates memory for SP_Bubbles
  Inputs :  height,width - image dimensions
            no_of_regions - No. of regions present in the image
  Output :  bubbles - filled SP_Bubble structure.

**************************************************************************/
void allocate_sp_bubbles(SP_Bubbles *bubbles, int height, int width, 
              int no_of_regions)
{
  /*bubbles->height = height;
    bubbles->width = width;*/
  bubbles->surface = (double *)calloc(height*width,sizeof(double));
  bubbles->region_label_array = (int *)calloc(height*width,sizeof(int));
  bubbles->zero_level_set = (double *)calloc(height*width,sizeof(double));
  if (no_of_regions > 0){
    bubbles->region = (Region_Stats *)calloc(no_of_regions,sizeof(Region_Stats));
    bubbles->no_of_regions = no_of_regions;
  }
}/* End of function allocate_sp_bubbles */


/**************************************************************************
  Description: Function that frees memory allocated to the SP_Bubble
               data structure. 
  Inputs : bubbles Pointer to  SP_Bubble.

**************************************************************************/

void free_sp_bubbles(SP_Bubbles *bubbles)
{
  free(bubbles->surface);
  free(bubbles->region_label_array); 
  free(bubbles->zero_level_set);
  free(bubbles->region);
}/* End of function free_sp_bubbles */

/**************************************************************************
  Description: Function that frees memory allocated to the SP_Bubble
               data structure. 
  Inputs : force Pointer to  SP_Bubble.

**************************************************************************/

void free_sp_bubbles_force(SP_Bubbles_Force *force)
{
  free(force->edge_force);
  free(force->statistics_force); 
  free(force->stop);
  free(force->force_direction);

  free(force->snake.Sx);
  free(force->snake.Sy);
  free(force->snake.snake_force);

  free_mem_heap_SP_Cedt(&force->exp_heap);
  free_grid_cedt_data(&force->exp_cedt);
}/* End of function free_sp_bubbles_force */

/**************************************************************************
  Description: Function that frees memory allocated to the QWaves list.
               After each shock computation one needs to free this. 
  Inputs : qwaves - Pointer to QWaves.
           size - no. of elements in qwaves
**************************************************************************/
void free_data_QWaves_list(QWaves *qwaves, int size)

{
  int pos;
  Quench *prevquench;

  for (pos=0;pos<size;pos++){
    if (qwaves[pos].quench != NULL) {
      while(1) {
    prevquench = qwaves[pos].quench->prev;
    free(qwaves[pos].quench);
    
    if (prevquench != NULL) 
      qwaves[pos].quench = prevquench;
    else
      break;
      }
    }
  }
}

/**************************************************************************
  Description: Function that initializes all the necessary structures
               before SP_CEDT/Shock computation is done. 
  Inputs : CEDTWaves waves - Pointer to QWaves.
           SPCedt cedt
       Heap heap
           size - no. of elements in qwaves
**************************************************************************/
void initial_wave_prop_cedt_strucs(Heap *heap, SPCedt *cedt, CEDTWaves *waves,
                   int height, int width)
{
  int i;

  initialize_data_SP_Cedt(cedt, height, width);
  initialize_heap_SPCedt(heap,height*width+5*MAX_BOUNDARY_LENGTH);

  convert_boundary_to_linear(cedt->boundary,cedt->boundary); 
  for (i=0;i<height*width;i++)
    waves->qwaves[i].quench = NULL;
  for (i=0;i<5*MAX_BOUNDARY_LENGTH;i++){
    cedt->ray[i].currPt.x=0.0;
    cedt->ray[i].currPt.y=0.0;
    cedt->ray[i].origin.x=0.0;
    cedt->ray[i].origin.y=0.0;
    cedt->ray[i].m=0.0;
    cedt->ray[i].b0=0.0;
    cedt->ray[i].label=0;
    cedt->ray[i].type=0;
    cedt->ray[i].direction=0;
    cedt->ray[i].xdir=0.0;
    cedt->ray[i].ydir=0.0;
    cedt->ray[i].dx=0.0;
    cedt->ray[i].dy=0.0;
  }
}

#ifdef __cplusplus
}
#endif





