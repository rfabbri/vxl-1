#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "dbseg_subpixel_postscript.h"
#include "dbseg_subpixel_1d_eno_types.h"
#include "dbseg_subpixel_non-uniform-eno.h"
#include "dbseg_subpixel_tbs-utils.h"
#include "dbseg_subpixel_SPCedtGeno.h"
#include "dbseg_subpixel_postscript.h"

#include "dbseg_subpixel_subpixel_bubbles.h"

#define FORCE1 0.1
#define FORCE2 0.05

/**************************************************************************
  Description: Function that finds the total force for the entire image. 
  Inputs :  spbub_deform - The SP_Bubble_Deform structure.
  Output :  spbub_deform 
 **************************************************************************/
void compute_forces(SP_Bubble_Deform *spbub_deform)
{
  int verbose=1,draw=1,write_files=0;
  int coupled=1;
  int x,y,kk,i,j;
  int height,width,boundary_label,region_label;
  double interp_value,first_der,second_der,snake_force;
  double slope,ct,force;
  char shk_file_name[200];
  void *data;
  Point grid_point, boundary_point;
  Point center,Sol;
  FILE *fp,*time_fp;
  SP_Bubbles_Force *SP_force;

  /******* TESTING ROTATION ***/
  double cx=11.907327, cy=13.059254;
  double R=14.3;
  double ocx=30.0,ocy=20.0;
  int sx=36,sy=14;
  double theta=0.4*M_PI/180.0;
  /******* TESTING ROTATION ***/


  /* For Finding Times */
  int time=1;
  clock_t iter_clock,start_clock,clock_fnst,clock_fnend;
  double start_time,time_fnst,time_fnend,time_total=0;
  double shock_time=0,other_time=0,coupled_time=0;

  /*##########################################################*/
  /*##########################################################*/
  
  height = spbub_deform->params.height;
  width = spbub_deform->params.width;
  SP_force=&spbub_deform->force;
  if ((spbub_deform->params.iteration_no % spbub_deform->params.save_surface) == 0)
    draw=1;
  else
    draw=0;
  
  /*Find the Zero level set */
  get_zero_level_set_cedt(&spbub_deform->cedt,spbub_deform->bubbles.zero_level_set, 
              height,width);
  /*If coupling is to be done, find the inter-region shocks. */
  if (coupled){ 
    find_inter_bubble_shocks(spbub_deform);
  }


  /*##########################################################*/
  /*##########################################################*/

  if (draw){
      /*sprintf(shk_file_name,"%s-%04d.ps",spbub_deform->params.outbase,
        spbub_deform->params.iteration_no);*/
    sprintf(shk_file_name,"junk.ps");
    init_draw_ps_original(shk_file_name,5, width,height);
    draw_gridlines_skip(2,2,2,2,width-4,height-4, YELLOW, data);
    if (coupled){
      draw_shock_and_boundary(&spbub_deform->shocks, spbub_deform->cedt.boundary,0);
    }
    else{
      draw_cedt_boundary_segment_zoom(spbub_deform->cedt.boundary,0,0,RED);
    }
  }/*if(draw)*/

  /*##########################################################*/
  /*##########################################################*/

  /* Initialize the forces */
  for (y=0;y<height;y++){
    for (x=0;x<width;x++){
      SP_force->edge_force[y*width+x]=0.0;
      SP_force->statistics_force[y*width+x]=0.0;
    }
  }

  /*
    Main loop for force computation.
    Loops through the image, if the pixel (x,y) is flagged as zero-levelset,
    the forces need to be computed at that point. If force coupling is to be 
    done, "compute_force_at_point" is called.
  */
  for (y=PAD;y<height-PAD;y++){
    for (x=PAD;x<width-PAD;x++){
      if (spbub_deform->bubbles.zero_level_set[y*width+x] == 0){ 
    grid_point.x=(double)x;
    grid_point.y=(double)y;
    
    /* If force coupling needs to be done; */
    if (coupled){
      compute_force_at_point(grid_point,spbub_deform);
    }
    else{ /* This is without coupling of forces */
      boundary_label=spbub_deform->cedt.label[y*width+x];
      assert(boundary_label >= 0 && boundary_label <=500); /* for debug */
      /*Find the point on the boundary that is closest to the grid point*/
      find_distance(&spbub_deform->cedt,(double)x,(double)y,boundary_label,
            &boundary_point);
      if (spbub_deform->params.beta != 0.0){
        spbub_deform->diffusion[y*width+x]=
          compute_curvature_anti_geno_fit(spbub_deform->cedt.boundary,
                          boundary_point,boundary_label,grid_point,
                          spbub_deform->bubbles.surface,
                          spbub_deform->bubbles.region,
                          height,width);
        if (0&draw){
          if (spbub_deform->bubbles.surface[y*width+x] > 0)
        force_draw_ps(boundary_point,grid_point,
                  -spbub_deform->diffusion[y*width+x],RED);
          else
        force_draw_ps(boundary_point,grid_point,
                  spbub_deform->diffusion[y*width+x],RED);
        }
      }/* if (spbub_deform->params.beta != 0.0) */
      else if (spbub_deform->params.alpha != 0.0){
        /*center.x=spbub_deform->cedt.boundary[boundary_label].arc.center_x;
          center.y=spbub_deform->cedt.boundary[boundary_label].arc.center_y;
          find_line_equation(center,boundary_point,&slope,&ct);
        
          force=find_circle_rotate_force(boundary_point,grid_point,slope,ct,
          spbub_deform->params.iteration_no);
          SP_force->statistics_force[y*width+x]=force;*/
        /*Here the exact rotation force is being used.*/
        /*SP_force->statistics_force[y*width+x]=
          compute_rotation_force_line(boundary_point);*/
        /* Here interpolated rotation force will be used */
        SP_force->statistics_force[y*width+x]=
          compute_interp_force(boundary_point,grid_point,spbub_deform->enoimage);
        if (0 && draw){
          if (spbub_deform->bubbles.surface[y*width+x] > 0)
        force_draw_ps(boundary_point,grid_point,
                  -SP_force->statistics_force[y*width+x],RED);
          else
        force_draw_ps(boundary_point,grid_point,
                  SP_force->statistics_force[y*width+x],RED);
        }
      }

    }/* No coupling to be done here*/
      }/* if (spbub_deform->bubbles.zero_level_set[y*width+x] == 0) */ 
    }/*for (x=PAD;x<width-PAD;x++)*/
  }/*for (y=PAD;y<height-PAD;y++)*/
  /* close_draw_ps(); */

  /*sx=0;sy=0;
    theta=0.4*M_PI/180.0;
    theta=(double)spbub_deform->params.iteration_no* theta;
    cx=ocx-R+R*cos(theta);
    cy=ocy+R*sin(theta);
    if (draw){
    point_draw_ps(cx-sx,cy-sy,RED,data);
    point_draw_ps(ocx-sx,ocy-sy,DARK_GREEN,data);
    vector_draw_ps(ocx-R-sx,ocy-sy,-R+3,theta/2,GREY,data);
    vector_draw_ps(ocx-R-sx,ocy-sy,2*R+3,theta/2,GREY,data);
    circle_draw_ps(cx-sx,cy-sy,R,LIGHT_BLUE,data);
    circle_draw_ps(ocx-sx,ocy-sy,R,BLUE,data);
    }*/

  
/*   for (y=PAD;y<height-PAD;y++){ */
/*     for (x=PAD;x<width-PAD;x++){ */
/*       if (SP_force->line_force[y*width+x] > 0.0) */
/*     draw_vector((double)x,y,SP_force->line_force[y*width+x],45,PURPLE,data); */
/*     } */
/*   } */
  if (0&draw){
    close_draw_ps();
  }

  fprintf(stderr,"ITERATION NO. %d\n",spbub_deform->params.iteration_no );
  fprintf(stderr,"Center: (%5.2f %5.2f)\n",cx,cy);
  fprintf(stderr,"SURFACE\n");
  for (j=43;j<=48;j++){
    for (i=20;i<=26;i++){
      fprintf(stderr,"%6.3f ",spbub_deform->bubbles.surface[i*width+j]);
    }
    fprintf(stderr,"\n");
  }
  fprintf(stderr,"\n");

  fprintf(stderr,"FORCE\n");
  for (j=43;j<=48;j++){
    for (i=20;i<=26;i++){
      fprintf(stderr,"%6.3f ",SP_force->statistics_force[i*width+j]);
    }
    fprintf(stderr,"\n");
  }
  fprintf(stderr,"\n");
 /* exit(1); */
  
  /*##########################################################*/
  /*##########################################################*/

    write_files=0;
  if (write_files){ /* For debug*/
    /*fp = fopen("diff-noexp.raw","w");
    fwrite(spbub_deform->diffusion,sizeof(double),height*width,fp);
    fclose(fp);
    fp = fopen("Sx.raw","w");
    fwrite(SP_force->snake.Sx,sizeof(double),height*width,fp);
    fclose(fp);
    fp = fopen("Sy.raw","w");
    fwrite(SP_force->snake.Sy,sizeof(double),height*width,fp);
    fclose(fp);
    fp = fopen("edge_force.raw","w");
    fwrite(SP_force->edge_force,sizeof(double),height*width,fp);
    fclose(fp);
    fp = fopen("stat_force.raw","w");
    fwrite(SP_force->statistics_force,sizeof(double),height*width,fp);
    fclose(fp);
    fp = fopen("snake_force.raw","w");
    fwrite(SP_force->snake.snake_force,sizeof(double),height*width,fp);
    fclose(fp);
    fp = fopen("level_set.raw","w");
    fwrite(spbub_deform->bubbles.zero_level_set,sizeof(double),height*width,fp);
    fclose(fp);
    fp = fopen("surface.raw","w");
    fwrite(spbub_deform->bubbles.surface,sizeof(double),height*width,fp);
    fclose(fp);*/
    
    fflush(stdout);
  }/* if (write_files) */


  /* Expand the statistic forces to all level sets from the zero-level set */
  expand_to_all_level_sets(&SP_force->exp_heap,&SP_force->exp_cedt,
               spbub_deform->bubbles.zero_level_set,
               SP_force->statistics_force, spbub_deform->params);
  /* Expand the smoothing forces to all level sets from the zero-level set */
/*   if (spbub_deform->params.beta != 0) */
/*     expand_to_all_level_sets(&SP_force->exp_heap,&SP_force->exp_cedt, */
/*                  spbub_deform->bubbles.zero_level_set, */
/*                  spbub_deform->diffusion,spbub_deform->params); */
  /*Compute stop function.*/
  for (y=PAD;y<height-PAD;y++){
    for (x=PAD;x<width-PAD;x++){
      SP_force->stop[y*width+x] = SP_force->statistics_force[y*width+x];
      /* SP_force->stop[y*width+x] = 0.2*SP_force->snake.snake_force[y*width+x]; */
      /* SP_force->stop[y*width+x] = SP_force->edge_force[y*width+x]; */
      if (SP_force->stop[y*width+x] > 0.0)
    SP_force->force_direction[y*width+x] = 1;
      else
    SP_force->force_direction[y*width+x] = -1;
    }
  }
  
  adjust_boundaries_pad_double(SP_force->stop,height,width,PAD);
  adjust_boundaries_pad_double(SP_force->snake.Sx,height,width,PAD);
  adjust_boundaries_pad_double(SP_force->snake.Sy,height,width,PAD);
  adjust_boundaries_pad_int(SP_force->force_direction,height,width,PAD);

  /*##########################################################*/
  /*##########################################################*/

}/*End of function compute_forces */




/**************************************************************************
  Description: Function that finds the edge force at a grid point. 
  Inputs :  first_der - The first derivative of intensity along the normal
  Return :  edge_force
 **************************************************************************/
double compute_edge_force(double first_der)
{
  return (1.0/(1.0 + pow(fabs(first_der),2.0)));
}



/**************************************************************************
  Description: Function that finds the rotation force at a point for a line.
               Force is proportional to the distance from the pivot point.
  Inputs :  Pt - The point where the force is to be computed.
  Return :  Force
 **************************************************************************/
double compute_rotation_force_line(Point Pt)
{
  Point pivot;
  double dist,force;
  double theta=5*M_PI/180.0;

  pivot.y=0.0;
  pivot.x=0.0;
  dist =  euclidean_distance(Pt,pivot);
  force = pow(dist,2)*tan(theta);
  return force/500;
  
}

/**************************************************************************
  Description: Function that finds the interpolated force at a subpixel
               location. Force is calculated using the 2stage ENO  interpolation.
  Inputs :  Pt - The point where the force is to be computed.
  Return :  Force
 **************************************************************************/
double compute_interp_force(Point Pt, Point gridPt, 
                ENO_Interval_Image_t *enoimage)
{
  double first_der, second_der, interp_value;


  compute_subpixel_interp_ders(gridPt,Pt,enoimage,&first_der, 
                   &second_der, &interp_value);
  return interp_value;
  
}

/**************************************************************************
  Description: Function that finds the edge force at a grid point. 
  Inputs :  interp - The interpolated intensity value at the closest boundary 
          point.
        region -  region data structure, corresponding to the right region
        label so that the appropriate mean and stdev can be found.
        multi_gauss - 0 -> treat the region as a Gaussian approximation
                      1 -> treat the region as multiple  Gaussian approximation
  Return :  statistics force.
 **************************************************************************/
double compute_statistics_force(double interp, Region_Stats *region, int multi_gauss)
{
  int gauss_prob=0,erf_force=1;
  double prob,force;
  double mean,sigma;
  double mean1,sigma1,alpha1,mean2,sigma2,alpha2;
  double c=10;

 /*  return 0.1; */


  if (multi_gauss && gauss_prob){
    sigma1=region->std_vec[0];
    mean1=region->mean_vec[0];
    alpha1=region->alpha_vec[0];
    sigma2=region->std_vec[1];
    mean2=region->mean_vec[1];
    alpha2=region->alpha_vec[1];
    if (sigma1 != 0 && sigma2 !=0){
      prob=alpha1*(exp(-(pow(interp-mean1,2.0))/(2*pow(sigma1,1.0)))/(sqrt(2*M_PI)*sigma1))+
    alpha2*(exp(-(pow(interp-mean2,2.0))/(2*pow(sigma2,1.0)))/(sqrt(2*M_PI)*sigma2));
    }
    else if ((sigma1 == 0 && fabs(interp-mean1) < 1E-2) ||
         (sigma2 == 0 && fabs(interp-mean2) < 1E-2)){
      prob=1.0;
    }
    else if (sigma1 != 0 && fabs(interp-mean1) > 1E-2){
      prob=alpha1*(exp(-(pow(interp-mean1,2.0))/(2*pow(sigma1,1.0)))/(sqrt(2*M_PI)*sigma1));
    }
    else if (sigma2 != 0 && fabs(interp-mean2) > 1E-2){
      prob=alpha2*(exp(-(pow(interp-mean2,2.0))/(2*pow(sigma2,1.0)))/(sqrt(2*M_PI)*sigma2));
    }
    else{
      prob=0;
    }
    force= -c*(log(1-prob)/log(2));
    /* return 1.0/(1.0 + 100*(1-prob)); */
    if (force > 0.5)
      return 0.5;
    else
      return force;
  }
  else if (multi_gauss && erf_force){
    sigma1=region->std_vec[0];
    mean1=region->mean_vec[0];
    alpha1=region->alpha_vec[0];
    sigma2=region->std_vec[1];
    mean2=region->mean_vec[1];
    alpha2=region->alpha_vec[1];
    if (sigma1 != 0 && sigma2 !=0){
      prob=alpha1*((1+erf((-fabs(interp-mean1)+2.5*sigma1)/(2*sigma1)))/(5*sigma1))+
    alpha2*((1+erf((-fabs(interp-mean2)+2.5*sigma2)/(2*sigma2)))/(5*sigma2));
    }
    else if ((sigma1 == 0 && fabs(interp-mean1) < 1E-2) ||
         (sigma2 == 0 && fabs(interp-mean2) < 1E-2)){
      prob=1.0;
    }
    else if (sigma1 != 0 && fabs(interp-mean1) > 1E-2){
      prob=alpha1*((1+erf((-fabs(interp-mean1)+2.5*sigma1)/(2*sigma1)))/(5*sigma1));
    }
    else if (sigma2 != 0 && fabs(interp-mean2) > 1E-2){
      prob=alpha2*((1+erf((-fabs(interp-mean2)+2.5*sigma2)/(2*sigma2)))/(5*sigma2));
    }
    else{
      prob=0;
    }
    force=5*prob;
    if (force > 0.5)
      return 0.5;
    else
      return force;
  }

  sigma=region->stdev;
  mean=region->mean;
  if (gauss_prob){
    if (sigma != 0)
      prob=exp(-(pow(interp-mean,2.0))/(2*pow(sigma,1.0)))/(sqrt(2*M_PI)*sigma);
    else if (fabs(interp-mean) < 1E-2)
      prob=1.0;
    else
      prob=0;
    /*Can't find log (0). So handle this separately.*/
    if (prob > 0.99)
      prob=0.99;
    /*Force is -c*log2(1-p). c is a contant to keep the force less than 1
      or some meaningful value. Sometimes it is too low.*/
    force= -c*(log(1-prob)/log(2));
    /* return 1.0/(1.0 + 100*(1-prob)); */
    if (force > 0.5)
      return 0.5;
    else
      return force;
  }
  else if (erf_force){
    if (sigma < 1E-1 && fabs(interp-mean) < 1E-1)
      return 0.4;
    else if (sigma < 1E-1 && fabs(interp-mean) >= 1E-1)
      return 0.0;
    prob=(1+erf((-fabs(interp-mean)+2.5*sigma)/(2*sigma)))/(5*sigma);
    force=5*prob; /* To make the value somewhat larger: prob is too low*/
    if (force > 0.4)
      return 0.4;
    else
      return force;
  }
}


/**************************************************************************
  Description : Function that expands the array value from the zero level
                set to all levels sets.
  Inputs : heap - heap structure (for cedt)
           cedt - cedt structure
       zero_level_set - the array representing the zero level set (ones on
       zero level set & zeros e.w.)
       array - array to be expanded.
       params - SP_Bubbles_Params structure.
  Output: array - the expanded version of "array"
**************************************************************************/
void expand_to_all_level_sets(Heap *heap,Cedt *cedt,double *zero_level_set,
                  double *array, SP_Bubbles_Params params)
{
  int height,width;
  float *f_expanded_array;
  float *f_zero_level_set,*f_array;

  height = params.height;
  width = params.width;
  f_expanded_array = (float *)calloc(height*width,sizeof(float));
  f_array = (float *)calloc(height*width,sizeof(float));
  f_zero_level_set = (float *)calloc(height*width,sizeof(float));
  CopyIm_DoubleToFloat(array,f_array,params.height,params.width);
  CopyIm_DoubleToFloat(zero_level_set,f_zero_level_set,params.height,params.width);
  speed_expansion_by_cedt(heap,cedt,f_zero_level_set,f_array,f_expanded_array,
              params.band_size, params.height,params.width);
  CopyIm_FloatToDouble(f_expanded_array,array,params.height,params.width);

  free(f_expanded_array);
  free(f_zero_level_set);
  free(f_array);
}






/**************************************************************************
  Description : Function that finds the distance drop-off for force coupling.
  Inputs :  dist - distance from the shock.
  Return :  the value of the function at dist d
**************************************************************************/
double distance_drop_off_function(double distance)
{

  double sigma=DISTANCE_DROP_SIGMA; /* DISTANCE_DROP_SIGMA =1 */
  double threshold=DISTANCE_DROP_THRESHOLD,val; /*DISTANCE_DROP_THRESHOLD = 0.3*/
  
  val=exp((-pow(threshold,2.0))/pow(sigma,2.0));
  
  if (distance <= 0.0)
    return 1.0/val;
  /*else if (distance <= 0.1)
    return 1.3;*/
  else
    return 1.2*exp((-pow(distance,2.0))/pow(sigma,2.0))/val;
}


/************************************************************************
  Description : Top level function that computes the forces for one point.
                At first, need to find the shock, and then decide is force
        coupling is needed (based on distance to shock). If distance
        is less than a certain value, force coupling is to be done.\
        Otherwise, normal force computation is done.
  Inputs :  grid_point - The grid_point where the update is to be done.
            spbub_deform - SP_Bubble_Deform structure. The following are used from
        spbub_deform structure.
              cedt - SPCedt structure of the surface/boundary.
          shocks - The shock structure
          shock_boundary - the GENO approx. of the shocks. 
          force - the SP_Bubbles_Force structure.
          enoimage - ENO interpolated image (used in image forces computation).
          back_shock_ptr - 
  Output : the force at the point.            
 ************************************************************************/
void compute_force_at_point(Point grid_point, SP_Bubble_Deform *spbub_deform)
{
  int x,y;
  int height,width;
  int boundary_id1,boundary_id2;
  int shock_boundary_id,tmp_id;
  int region_label1,region_label2; 
  int no_coupling_flag=0;
  int draw=1,verbose=0,write_file=0;
  int raph_write_file=0;
  double first_der, second_der, interp_value,F,K,L;
  void *data;
  FILE *fp;
  Point boundary_point1,boundary_point2;
  Point shock_point;
  Point start_pt,end_pt;

  height=spbub_deform->params.height;
  width=spbub_deform->params.width;
  x=(int)grid_point.x;
  y=(int)grid_point.y;

  if ((spbub_deform->params.iteration_no % 50) == 0)
    draw=1;
  else
    draw=0;
  /*draw=0;*/
  
  /* Find boundary id of closest boundary point */
  boundary_id1=spbub_deform->cedt.label[y*width+x];
  assert(boundary_id1 >= 0 && boundary_id1 <=MAX_BOUNDARY_LENGTH); /* for debug */
  find_distance(&spbub_deform->cedt,(double)x,(double)y,boundary_id1,&boundary_point1);
  region_label1 = spbub_deform->cedt.boundary[boundary_id1].segment;

  if (grid_point.x == 21 && grid_point.y == 20){
    F=0;
  }

  if (spbub_deform->num_shocks > 0){
    /* Find the shock point */  
    shock_boundary_id=
      find_shock_point_for_boundary(spbub_deform->shock_boundary,
                    spbub_deform->cedt.boundary,
                    &spbub_deform->shocks,
                    spbub_deform->back_shock_ptr,
                    grid_point,&boundary_point1,
                    boundary_id1,&shock_point,height,width);
    /*Sometimes for waves along grid lines, the boundary_ids are not stable. Two
      intervals minimize the distance, and which one is chosen makes a lot of 
      difference in the shock_point computation. So if the closest boundary_point
      is very close to the end point, I am going to consider one more boundary_id.
      The fact that Huseyin uses lines (instead of lines and arcs, may also 
      exacerbate the above problem. */
    start_pt=get_start_pt(&spbub_deform->cedt.boundary[boundary_id1]);
    end_pt=get_end_pt(&spbub_deform->cedt.boundary[boundary_id1]);
    if (!is_valid_point(shock_point) || !is_valid_label(shock_boundary_id)){
      if (almost_equal_with_tolerance(boundary_point1.x,start_pt.x,0.15) &&
      almost_equal_with_tolerance(boundary_point1.y,start_pt.y,0.15) &&
      is_valid_label(spbub_deform->cedt.boundary[boundary_id1].left)){
    tmp_id=spbub_deform->cedt.boundary[boundary_id1].left;
    find_distance(&spbub_deform->cedt,(double)x,(double)y,tmp_id,&boundary_point1);
    shock_boundary_id=
      find_shock_point_for_boundary(spbub_deform->shock_boundary,
                    spbub_deform->cedt.boundary,
                    &spbub_deform->shocks,
                    spbub_deform->back_shock_ptr,
                    grid_point,&boundary_point1,
                    tmp_id,&shock_point,height,width);
    if (is_valid_point(shock_point) && is_valid_label(shock_boundary_id) )
      boundary_id1=tmp_id;
      }
      if (almost_equal_with_tolerance(boundary_point1.x,end_pt.x,0.15) &&
      almost_equal_with_tolerance(boundary_point1.y,end_pt.y,0.15) &&
      is_valid_label(spbub_deform->cedt.boundary[boundary_id1].right)){
    tmp_id=spbub_deform->cedt.boundary[boundary_id1].right;
    find_distance(&spbub_deform->cedt,(double)x,(double)y,tmp_id,&boundary_point1);
    shock_boundary_id=
      find_shock_point_for_boundary(spbub_deform->shock_boundary,
                    spbub_deform->cedt.boundary,
                    &spbub_deform->shocks,
                    spbub_deform->back_shock_ptr,
                    grid_point,&boundary_point1,
                    tmp_id,&shock_point,height,width);
    if (is_valid_point(shock_point) && is_valid_label(shock_boundary_id))
      boundary_id1=tmp_id;
      }
    }/* if (!is_valid_point(shock_point) || ...)*/
    
    if (verbose){
      printf("Shock Point =(%f,%f)\n",shock_point.x,shock_point.y);
    }
    if ((is_valid_point(shock_point)) &&  (is_valid_label(shock_boundary_id)) &&
    (euclidean_distance(shock_point,boundary_point1) < 50/* MAX_SHOCK_DISTANCE*/)){
      /* Shock point is valid. So find the second boundary point.*/
      boundary_id2=
    find_boundary_point_for_shock(spbub_deform->shock_boundary,&spbub_deform->cedt,
                      &spbub_deform->shocks,shock_point,
                      shock_boundary_id,boundary_point1,
                      boundary_id1,&boundary_point2);
      if (verbose){
    printf("2nd Boundary Point =(%f,%f)\n",boundary_point2.x,boundary_point2.y);
      }
      if ((is_valid_point(boundary_point2)) && is_valid_label(boundary_id2)){
    /*Couple the forces */
    region_label2 = spbub_deform->cedt.boundary[boundary_id2].segment;
    /*if (write_file){
      fp=fopen("coupled-force.dat","a");
      fprintf(fp,"Grid (%d,%d)\n",x,y);
      fclose(fp);
      }*/
    F=compute_coupled_statistics_force(boundary_point1,shock_point,boundary_point2,
                       region_label1,region_label2,
                       spbub_deform->bubbles.region,
                       spbub_deform->params.no_of_classes-1,
                       spbub_deform->enoimage,draw);
    K=compute_coupled_curvature(spbub_deform->cedt.boundary,
                    boundary_point1,boundary_id1,grid_point,
                    boundary_point2,boundary_id2,shock_point,
                    spbub_deform->bubbles.surface,
                    spbub_deform->bubbles.region,height,width);
    if (spbub_deform->params.line_flag == 1){
      L=0.1*compute_coupled_line_force(boundary_point1, shock_point, boundary_point2,
                       height,width, region_label1,region_label2,
                       spbub_deform->force.line_force,draw);
    }
    else{
      L = 0;
    }
    /* printf("Stat=%f Line=%f Curv=%f\n",spbub_deform->params.alpha*F,spbub_deform->params.alpha*L,spbub_deform->params.beta*K); */

    if (0&draw)
      point_draw_ps(grid_point.x,grid_point.y,LIGHT_BLUE,data);
      }/* if ((is_valid_point(boundary_point2)) && */
      else{
    no_coupling_flag=1; 
      }
    }
    else{
      no_coupling_flag=1; /* No coupling to be done */
    }
  }
  else{
    no_coupling_flag=1; /* No coupling to be done */
  }
  
  if (verbose){
    if (no_coupling_flag)
      printf("No coupling needs to be done\n");
    else
      printf("Coupling being done\n");
  }      
    
  /* If no force coupling is to be done, compute local statistic and smoothing
     forces. */
  if (no_coupling_flag){
    compute_subpixel_interp_ders(shock_point,boundary_point1,spbub_deform->enoimage,
                 &first_der, &second_der, &interp_value);
    F=compute_statistics_force(interp_value,&spbub_deform->bubbles.region[region_label1], 
                   spbub_deform->params.no_of_classes-1);

    /*Compute the curvature for doing some smoothing (diffusion).*/
    if (spbub_deform->params.beta != 0.0){
      K=compute_curvature_anti_geno_fit(spbub_deform->cedt.boundary,
                    boundary_point1,boundary_id1,grid_point,
                    spbub_deform->bubbles.surface,
                    spbub_deform->bubbles.region,
                    height,width);
      /*if (draw){
    if (spbub_deform->bubbles.surface[y*width+x] > 0)
    force_draw_ps(boundary_point1,grid_point,-K,RED);
    else
    force_draw_ps(boundary_point1,grid_point,K,BLUE);
    }*/
    }
    /* force_draw_ps(boundary_point1,grid_point,F,GREEN); */
    /* printf("Interp=%f Force=%f\n",interp_value,F); */
    /* if (region_label1 == 1)
      F = FORCE1;
      else
      F= FORCE2;*/
  }/* if (no_coupling_flag)*/

  if (verbose){
    printf("Net Force = %f Net Curvature= %f\n",F,K);
  }
  spbub_deform->diffusion[y*width+x]=K;
  spbub_deform->force.statistics_force[y*width+x]=F+L;
  if (raph_write_file){
    fp=fopen("force.dat","a");
    fprintf(fp,"%d %d %5.2f %5.2f %5.2f %5.2f %5.2f %5.2f %5.2f %5.2f %5.2f \n",
        (int)grid_point.x,(int)grid_point.y,boundary_point1.x,boundary_point1.y,
        shock_point.x,shock_point.y,boundary_point2.x,boundary_point2.y,
        F,K,L);
    fclose(fp);
  }
}
/**************************************************************************
  Description : Function that finds the force coupling. 
  Inputs :  boundary_point1 - The boundary point that is closest to the grid point.
            shock_point - The point on the shock corresponding to the boundary
        point.
        boundary_point2 - The point on the other contour, whose force 
        is to be coupled with boundary_point.
  Return :  the force at the point.
**************************************************************************/
double compute_coupled_statistics_force(Point boundary_point1, Point shock_point, 
                       Point boundary_point2,
                       int region_label1,int region_label2, 
                       Region_Stats *region, int no_of_classes,
                       ENO_Interval_Image_t *enoimage,int draw)
{
  double first_der1, second_der1, interp_value1;
  double first_der2, second_der2, interp_value2;
  double first_der1s, second_der1s, interp_value1s;
  double first_der2s, second_der2s, interp_value2s;
  double F1,F2,F1s,F2s,FsNet,D1,D2;
  double alpha,beta,dist;
  int verbose=0,write_file=0;
  void *data;
  FILE *fp;

  if (write_file){
    fp=fopen("coupled-force.dat","a");
  }

  if (verbose){
    printf("Now Computing Coupled Statistic Forces\n");
  }

  /*Force at boundary point 1*/
  compute_subpixel_interp_ders(shock_point,boundary_point1,enoimage,
                   &first_der1, &second_der1, &interp_value1);
  F1=compute_statistics_force(interp_value1,&region[region_label1],no_of_classes-1);
  
  /*Force at boundary point 2*/
  compute_subpixel_interp_ders(shock_point,boundary_point2,enoimage,
                   &first_der2, &second_der2, &interp_value2);
  F2=compute_statistics_force(interp_value2,&region[region_label2],no_of_classes-1);

  if (verbose){
    printf("F1=%f F2=%f\n",F1,F2);
  }

  /*Force at shock due to region 1*/
  compute_subpixel_interp_ders(boundary_point1,shock_point,enoimage,
                   &first_der1s, &second_der1s, &interp_value1s);
  F1s=compute_statistics_force(interp_value1s,&region[region_label1],no_of_classes-1);

  /*Force at shock due to region 2*/
  compute_subpixel_interp_ders(boundary_point2,shock_point,enoimage,
                   &first_der2s, &second_der2s, &interp_value2s);
  F2s=compute_statistics_force(interp_value2s,&region[region_label2],no_of_classes-1);
  if (verbose){
    printf("F1s=%f F2s=%f\n",F1s,F2s);
  }
  
  
  dist=euclidean_distance(shock_point,boundary_point1);
  alpha=distance_drop_off_function(dist);
  beta=1-alpha;
  if (beta < 0.0) 
    beta=0.0;
  
  
  FsNet=F1s-F2s;  /* Net force at the shock point */
  /* F2=F2+beta*FsNet; */
  /* F1=F1-beta*FsNet; */
  
  if (write_file){
    fprintf(fp, "boundary_point=(%6.3f,%6.3f) \n ",boundary_point1.x,boundary_point1.y);
    fprintf(fp,"Interp1=%6.3f Interp1s=%6.3f Reg1=%d Interp2=%6.3f Interp2s=%6.3fReg2=%d\n",
        interp_value1, interp_value1s, region_label1,
        interp_value2, interp_value2s,region_label2);
    fprintf(fp, "F1=%6.3f F2=%6.3f dist=%6.3f alpha=%6.3f F=%6.3f OldF=%6.3f\n",
        F1,F2,dist,alpha,beta*F1+alpha*(F1s-F2s),F1-alpha*F2+0.5*beta*(F1s-F2s));
    fprintf(fp, "F1s=%6.3f F2s=%6.3f Fs=%6.3f FsNet=%6.3f\n\n",F1s,F2s,FsNet,alpha*(F1s-F2s));
  }
  if (0&draw){
    /*line_draw_ps(boundary_point1.x,boundary_point1.y,shock_point.x,shock_point.y,GREEN,data);
    line_draw_ps(boundary_point2.x,boundary_point2.y,shock_point.x,shock_point.y,BLUE,data);
    force_draw_ps(boundary_point1,shock_point,F1-alpha*F2,RED);*/
  }
  if (write_file){
    fclose(fp);
  }
  /* return F1-alpha*F2+0.5*beta*(F1s-F2s);*/  /*Old version */
  return F1+-alpha*F2+beta*(F1s-F2s); 
}

/**************************************************************************
  Description : Function that finds the force coupling. 
  Inputs :  boundary_point1 - The boundary point that is closest to the grid point.
            shock_point - The point on the shock corresponding to the boundary
        point.
        boundary_point2 - The point on the other contour, whose force 
        is to be coupled with boundary_point.
  Return :  the force at the point.
**************************************************************************/
double compute_coupled_line_force(Point boundary_point1, Point shock_point, 
                  Point boundary_point2, int height, int width, 
                  int region_label1,int region_label2, 
                  double *line_force,int draw)
{
  double a00,a01,a10,a11;
  double x,y;
  double F1,F2,F1s,F2s,FsNet,D1,D2;
  double alpha,beta,dist;
  int verbose=0,write_file=0;
  int x0,y0;
  void *data;
  FILE *fp;

  if (write_file){
    fp=fopen("coupled-force.dat","a");
  }

  if (verbose==2){
    printf("Now Computing Coupled Statistic Forces\n");
  }

  
  /*Force at boundary point 1*/
  if (region_label1 == 1){
    F1=bilinear_interpolate_in_array(boundary_point1.x,boundary_point1.y, line_force,height,width);
    if (verbose==2){
      printf("Boundary:a00=%f a01=%f a10=%f a11=%f,x=%f,y=%f\n",a00, a10, a01, a11,x,y);
    }
  }
  else{
    F1=0;
  }
  
  
  /*Force at boundary point 2*/
  if (region_label1 == 1){
    F2=bilinear_interpolate_in_array(boundary_point2.x,boundary_point2.y, line_force,height,width);
  }
  else{
    F2=0;
  }

  /*Force at shock due to region 1*/


  if  (region_label1 == 1){
    F1s=bilinear_interpolate_in_array(shock_point.x,shock_point.y, line_force,height,width);
    F2s=0;
  }
  else if  (region_label2 == 1){
    F1s=0;
    F2s=bilinear_interpolate_in_array(shock_point.x,shock_point.y, line_force,height,width);
  }
  else{
    F2s=0;
    F2s=0;
  }
  
  if (verbose==2){
    printf("Shock:a00=%f a01=%f a10=%f a11=%f,x=%f,y=%f\n",a00, a10, a01, a11,x,y);
  }
  /*bilinear_interpolation(a00, a10, a01, a11,  x, y);*/

  
  dist=euclidean_distance(shock_point,boundary_point1);
  alpha=distance_drop_off_function(dist);
  beta=1-alpha;
  if (beta < 0.0) 
    beta=0.0;
  
  
  FsNet=F1s-F2s;  /* Net force at the shock point */
  /* F2=F2+beta*FsNet; */
  /* F1=F1-beta*FsNet; */
  
  if (write_file){
    fprintf(fp, "boundary_point=(%6.3f,%6.3f) \n ",boundary_point1.x,boundary_point1.y);
    fprintf(fp, "F1=%6.3f F2=%6.3f dist=%6.3f alpha=%6.3f F=%6.3f OldF=%6.3f\n",
        F1,F2,dist,alpha,beta*F1+alpha*(F1s-F2s),F1-alpha*F2+0.5*beta*(F1s-F2s));
    fprintf(fp, "F1s=%6.3f F2s=%6.3f Fs=%6.3f FsNet=%6.3f\n\n",F1s,F2s,FsNet,alpha*(F1s-F2s));
  }
  if (draw){
    /*line_draw_ps(boundary_point1.x,boundary_point1.y,shock_point.x,shock_point.y,GREEN,data);
    line_draw_ps(boundary_point2.x,boundary_point2.y,shock_point.x,shock_point.y,BLUE,data);
    force_draw_ps(boundary_point1,shock_point,F1-alpha*F2,RED);*/
  }
  if (write_file){
    fclose(fp);
  }
  if (verbose){
    printf("F1=%f F2=%f F1s=%f F2s=%f Fnet=%f\n",F1,F2,F1s,F2s,F1-alpha*F2+0.5*beta*(F1s-F2s));
  }

  return F1-alpha*F2+0.5*beta*(F1s-F2s);  /*Old version */
  /* return F1+-alpha*F2+alpha*(F1s-F2s); */
}


/**************************************************************************
  Description : Function that finds the edge force coupling. 
  Inputs :  boundary_point1 - The boundary point that is closest to the grid point.
            shock_point - The point on the shock corresponding to the boundary
        point.
        boundary_point2 - The point on the other contour, whose force 
        is to be coupled with boundary_point.
  Return :  the edge force at the point.
**************************************************************************/
double compute_coupled_edge_force(Point boundary_point1, Point shock_point, 
                 Point boundary_point2,
                 ENO_Interval_Image_t *enoimage)
{
  double first_der1, second_der1, interp_value1;
  double first_der2, second_der2, interp_value2;
  double F1,F2,D1,D2;
  double alpha,dist;
  int verbose=0,draw=1;
  void *data;
  FILE *fp;
  
  fp=fopen("coupled-force.dat","a");
  
  compute_subpixel_interp_ders(shock_point,boundary_point1,enoimage,
                   &first_der1, &second_der1, &interp_value1);
  F1=compute_edge_force(first_der1);
  
  compute_subpixel_interp_ders(shock_point,boundary_point2,enoimage,
                   &first_der2, &second_der2, &interp_value2);
  F2=compute_edge_force(first_der2);
  
  dist=euclidean_distance(shock_point,boundary_point1);
  alpha=distance_drop_off_function(dist);
  
  if (verbose){
    fprintf(fp, "boundary_point=(%f,%f) \n ",boundary_point1.x,boundary_point1.y);
    fprintf(fp,"Interp1=%f  Interp2=%f \n",first_der1,first_der2);
    fprintf(fp, "F1=%f F2=%f dist=%f alpha=%f F=%f\n",F1,F2,dist,alpha,F1-alpha*F2);
  }
  if (draw){
    /*line_draw_ps(boundary_point1.x,boundary_point1.y,
      shock_point.x,shock_point.y,GREEN,data);
      line_draw_ps(boundary_point2.x,boundary_point2.y,
      shock_point.x,shock_point.y,BLUE,data);*/
    /*force_draw_ps(boundary_point1,shock_point,F1-alpha*F2,RED);*/
  }
  fclose(fp);
  return F1-alpha*F2;
}/* End of function compute_coupled_edge_force */

void force_draw_ps(Point boundary_point, Point shock_point,double force, int color)
{
  double angle;
  void *data;
  int verbose=0;

  if(force > 0)
    angle=atan2(boundary_point.y-shock_point.y,boundary_point.x-shock_point.x);
  else
    angle=atan2((boundary_point.y-shock_point.y),(boundary_point.x-shock_point.x));
  
  if (verbose){
    printf("Angle=%f Force=%f\n",angle,force);
    printf("Boundary Point=(%f,%f)\n",boundary_point.x,boundary_point.y);
    printf("Shock_point=(%f,%f)\n",shock_point.x,shock_point.y);
  }
  draw_vector((double)boundary_point.x,(double)boundary_point.y,
          -1*force,180*angle/M_PI,color,data);
}

#ifdef __cplusplus
}
#endif





