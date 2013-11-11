#ifdef __cplusplus
extern "C" {
#endif

#include "dbseg_subpixel_postscript.h"

#include "dbseg_subpixel_subpixel_bubbles.h"


void test_merge(int argc, char *argv[]);

void main(int argc, char *argv[])
{
  test_merge(argc,argv);
}

void test_merge(int argc, char *argv[])
{
  
  SP_Bubble_Deform spbub_deform;
  Point grid_point, boundary_point,Sol;
  Point start_pt,end_pt,second_boundary_point;
  FILE *fp;
  double *nothing;
  int geno_boundary_id,shock_boundary_id,second_boundary_id;
  int i,j,kk,num_shocks,tmp_id;
  int rsh_label,lsh_label;
  int xstart=0,ystart=0;
  int verbose=0;
  void *data;
  int OFFSET=50;
  int height,width;

  initialize_sp_bub_deform_args(argc,argv,&spbub_deform);
  height=spbub_deform.params.height;
  width=spbub_deform.params.width;
  print_parameters(&spbub_deform.params);

  convert_boundary_to_linear(spbub_deform.cedt.boundary,spbub_deform.cedt.boundary);
  inter_boundary_shocks(&spbub_deform.cedt,&spbub_deform.waves,
            &spbub_deform.shocks,1,height,width);
  
  printf("Done with shocks\n");
  convert_higher_shocks(&spbub_deform.shocks,spbub_deform.cedt.boundary);

  free_memory_for_hosh(&spbub_deform.shocks,MAX_SHOCK_LENGTH);
  
  init_draw_ps_original("shocks.ps", 15, width,height);
  draw_gridlines(xstart,ystart, width,height, YELLOW, data);
  
  draw_shock_and_boundary(&spbub_deform.shocks, spbub_deform.cedt.boundary,1);
  close_draw_ps();
  printf("Done drawing shocks.ps\n");
  fflush(stdout);

  init_draw_ps_original("coupled.ps", 10, OFFSET,OFFSET);
  xstart=0;ystart=0;
  draw_gridlines(0,0,OFFSET,OFFSET, YELLOW, data);

  draw_shock_and_boundary(&spbub_deform.shocks, spbub_deform.cedt.boundary,0); 
  printf("Done drawing shocks for coupled.ps\n");
  fflush(stdout);

  update_shock_labels_on_boundary(&spbub_deform.shocks,&spbub_deform.cedt);
  printf("Done updating boundary labels\n");
  fflush(stdout);

  /*allocate_cedt_boundary(&boundary,height*width);
    back_shock_ptr=(int *)calloc(height*width,sizeof(int));*/
  geno_approx_of_shocks(&spbub_deform.shocks,spbub_deform.shock_boundary, 
            spbub_deform.back_shock_ptr, height,width);
  printf("Done geno approx. for labels\n");
  fflush(stdout);

  merge_all_similar_regions(&spbub_deform);


  if (verbose){
    print_shock_labels_all(spbub_deform.cedt.boundary);
  }
  
  close_draw_ps();
  
  free_sp_bub_deform(&spbub_deform);
  /* free(back_shock_ptr); */
  /* free(boundary); */
}

#ifdef __cplusplus
}
#endif





