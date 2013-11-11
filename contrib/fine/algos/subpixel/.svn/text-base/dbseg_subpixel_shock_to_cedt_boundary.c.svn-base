#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>
#include "dbseg_subpixel_geno-types.h"
#include "dbseg_subpixel_geno.h"
#include "dbseg_subpixel_graphics.h" 
#include "dbseg_subpixel_SPCedtGeno.h"

#include "dbseg_subpixel_subpixel_bubbles.h"

/************************************************************************************
  Description : Fills the SP_CEDT boundary structure using the Traced contours. 
                It is similar to the function in CEDT from arcs. Here the the  
        contour points are labelled and so one has to keep the contour
        ids intact.
                This function call GENO on the traced contours and fills the SPCect 
        structure using GENO Arc interpolation information.
  Inputs : Tr - The traced points.
           height,width - image dimensions
  Output : boundary - SPCedt boundary structure
************************************************************************************/

void cedt_boundary_with_geno_labelled(Tracer *Tr, Boundary *boundary,int height,
                      int width)

{
  double *contour_array;
  int *contour_id_array;
  int tracer_idx,start_pt,pts_ina_contour,contour_idx;
  int label,contour_label;
  int draw=0,verbose=0;
  int boundary_end,boundary_index=0;
  int closed;
  int i,contour_index=0;
  double yf,xf;
  double dist;
  GENO_Curve_List* curve_list;
  GENO_Interpolated_Curve* icurves;
  void *data;

  /*Temporary array for storing contour points and id*/
  contour_array=(double *)malloc(2*sizeof(double)*(Tr->size+2));
  contour_id_array=(int *)malloc(2*sizeof(int)*(Tr->size+2));
  
  if (draw){
    init_draw_ps_original("geno-curves.ps",20.0,height,width);
    draw_gridlines(0,0,height,width,YELLOW,data);
  }

  contour_index=0;
  tracer_idx =0; start_pt=0;  boundary_end = 0; label = 0;
  while(Tr->vert[tracer_idx] != ENDOFLIST) {  
    pts_ina_contour = 0; contour_idx=0; start_pt = tracer_idx; 
    while(Tr->vert[tracer_idx] != ENDOFCONTOUR) {
      contour_array[2*contour_idx]=(double)Tr->horiz[tracer_idx];
      contour_array[2*contour_idx+1]=(double)Tr->vert[tracer_idx];
      contour_id_array[2*contour_idx]=Tr->id[tracer_idx];
      contour_label=Tr->label[tracer_idx];
      pts_ina_contour++;
      tracer_idx++;
      contour_idx++;
    }
    if (verbose){
      printf("ContourId=%d Contour_label=%d Pts=%d\n",contour_index,contour_label,
         pts_ina_contour);
      fflush(stdout);
    }

    /*Handle a point. Can't do any interpolation.*/
    if (pts_ina_contour == 1){
      initialize_cedt_boundary_point_labelled(contour_array,boundary,
                          &boundary_index,contour_label,
                          contour_id_array);
    }
    /*Handle 2 points. Can't do GENO using two points. So 
      simple line assumption*/
    else if (pts_ina_contour == 2 || pts_ina_contour == 3){
      initialize_cedt_boundary_23points_labelled(contour_array,boundary,
                         &boundary_index,
                         contour_label,contour_id_array,
                         pts_ina_contour);
    }
    /* GENO interpolation to be done */
    else if (pts_ina_contour > 3){
      /*I am using a simple heuristic to find if the contour is open 
    or closed. It the distance between them is greater than 1.5 contour
    is open. Also not going to "CLOSE" contours that have less than
    6 points */
      dist=sqrt(sqr((Tr->vert[tracer_idx-1]-Tr->vert[start_pt]))+
        sqr((Tr->horiz[tracer_idx-1]-Tr->horiz[start_pt])));
      if (verbose){
    printf("Start Point (%f,%f) End Point (%f,%f) Dist=%f\n",
           Tr->horiz[start_pt],Tr->vert[start_pt],
           Tr->horiz[tracer_idx-1],Tr->vert[tracer_idx-1],dist);
      }
      if (pts_ina_contour > 6 && dist < 2.0)
    closed = 1;
      else
    closed = 0;
      
      curve_list=make_geno_curve(contour_array,pts_ina_contour,closed);
      if (verbose){
    printf("Made GENO curve %d\n",curve_list);
    fflush(stdout);
      }      
      icurves = geno_intervals_from_curves(curve_list);
      if (verbose){
    printf("Made GENO intervals\n");
    fflush(stdout);
      }      

      if (draw){
    geno_draw_interpolated_curves(icurves);
      }
       
      initialize_cedt_boundary_geno_curve_labelled(contour_array,boundary,icurves,
                           &boundary_index,contour_label,
                           contour_id_array,
                           pts_ina_contour,closed);     
      if (verbose){
    printf("Copied GENO intervals\n");
    fflush(stdout);
      }

      geno_free_curve(curve_list);
      geno_free_interpolated_curve(icurves);
      if (verbose){
    printf("Done all for a contour\n");
    fflush(stdout);
      }      
 
    }
    tracer_idx++;
    contour_index++;
  }
  
  boundary[boundary_index].id=ENDOFBOUNDARY;
  boundary[boundary_index+1].id=ENDOFBOUNDARY;

  if(draw){
    close_draw_ps();
  }

  free(contour_array);
  free(contour_id_array);
}/*End of function cedt_boundary_from_geno_labelled*/


/************************************************************************************
  Description : Copy a single point (the contour consists of just one point) to 
                SPCedt Boundary structure.
  Inputs : contour_array - Array of list of points.
       index - the index of the current element in the boundary str.
       contour_index - the label of the current contour
  Output : boundary - SPCedt boundary structure
************************************************************************************/

void initialize_cedt_boundary_point_labelled(double *contour_array,
                         Boundary *boundary,
                         int *boundary_index,
                         int contour_index,
                         int *contour_id_array)
{
  int i;
  int tmp_index,start_index;
  
  tmp_index = (*boundary_index);
  /*First two elements specify the point */
  boundary[tmp_index].point.x = contour_array[0]; 
  boundary[tmp_index].point.y = contour_array[1];
  boundary[tmp_index].type = POINT;
  boundary[tmp_index].id = contour_id_array[0];
  boundary[tmp_index].segment = contour_index;
  boundary[tmp_index].left = NOLABEL;
  boundary[tmp_index].right = NOLABEL;
  
  tmp_index++;

  *boundary_index=tmp_index;

}

/************************************************************************************
  Description : Copy contours that consist of just 2 or 3  points to 
                SPCedt Boundary structure. Note: GENO is done only if the
        no. of points in the contour > 3.
  Inputs : contour_array - Array of list of points.
       index - the index of the current element in the boundary str.
       contour_index - the label of the current contour.
       no_of_points - no. of points in the contour.
  Output : boundary - SPCedt boundary structure
************************************************************************************/

void initialize_cedt_boundary_23points_labelled(double *contour_array,
                         Boundary *boundary,
                         int *boundary_index,
                         int contour_index,
                         int *contour_id_array,         
                         int no_of_points)
{
  int verbose=0;
  int i;
  int tmp_index,start_index;
  
  tmp_index = (*boundary_index);
  if (almost_equal(contour_array[0],contour_array[2]) &&
      almost_equal(contour_array[1],contour_array[3]) )
    printf("This should be a point\n");
  
  boundary[tmp_index].line.x1 = contour_array[0]; 
  boundary[tmp_index].line.y1 = contour_array[1];
  boundary[tmp_index].line.x2 = contour_array[2]; 
  boundary[tmp_index].line.y2 = contour_array[3];
  boundary[tmp_index].type = LINE;
  boundary[tmp_index].id = contour_id_array[0];
  boundary[tmp_index].left = NOLABEL;
  boundary[tmp_index].right = contour_id_array[2];
  boundary[tmp_index].segment = contour_index;
  if (verbose){
    printf("Bdry_idx=%d ID=%d\n",tmp_index,
       contour_id_array[0]);
  }
  tmp_index++;
  
  if (no_of_points == 3){
    if (almost_equal(contour_array[2],contour_array[4]) &&
    almost_equal(contour_array[3],contour_array[5]) )
      printf("This should be a point\n");
    boundary[tmp_index].line.x1 = contour_array[2]; 
    boundary[tmp_index].line.y1 = contour_array[3];
    boundary[tmp_index].line.x2 = contour_array[4]; 
    boundary[tmp_index].line.y2 = contour_array[5];
    boundary[tmp_index].type = LINE;
    boundary[tmp_index].id =  contour_id_array[2];
    boundary[tmp_index].left = contour_id_array[0];
    boundary[tmp_index].right = contour_id_array[4];
    boundary[tmp_index-1].right = contour_id_array[2];
    boundary[tmp_index].segment = contour_index;
    if (verbose){
      printf("Bdry_idx=%d ID=%d\n",tmp_index,
         contour_id_array[2]);
    }
    tmp_index++;
  }
  *boundary_index=tmp_index;
}
/************************************************************************************
  Description : Copy GENO Arc interpolation information to SPCedt Boundary structure
                for one contour.
  Inputs : contour_array - The list of traced points.
           icurves - Structure having all the GENO interpolation information.
       boundary_index - the index of the current element in the boundary str.
       contour_index - the label of the current contour
       closed - 1 -> closed contour
                0 -> open contour
       no_of_points - no. of points in the current contour
  Output : boundary - SPCedt boundary structure
************************************************************************************/
void 
initialize_cedt_boundary_geno_curve_labelled(double *contour_array,
                         Boundary *boundary,
                         GENO_Interpolated_Curve *icurves,
                         int *boundary_index,
                         int contour_index,
                         int *contour_id_array,
                         int no_of_points,
                         int closed)
{
  int verbose=0;
  int i,contour_idx;
  int tmp_index,start_index,shock_index;
  Point start_pt, end_pt, shock_loc;

  tmp_index = (*boundary_index);
  start_index = tmp_index;

  contour_idx=0;
  for (i=0;i<icurves->num_intv;i++){
    start_pt.x = icurves->interval[i].pts[1].x;
    start_pt.y = icurves->interval[i].pts[1].y;
    end_pt.x = icurves->interval[i].pts[2].x;
    end_pt.y = icurves->interval[i].pts[2].y;
    if (verbose){
      printf("GENO : Start=(%f,%f) End=%f,%f)\n",start_pt.x,start_pt.y,
         end_pt.x,end_pt.y);
      printf("ACT%02d: Start=(%f,%f) End=%f,%f)\nID=%d\n",contour_idx,
    contour_array[contour_idx],
    contour_array[contour_idx+1],contour_array[contour_idx+2],
    contour_array[contour_idx+3],contour_id_array[contour_idx]);
    }
    /*If the start point and end points are almost the same, then don't do anything*/
    if (almost_equal_with_tolerance(start_pt.x,end_pt.x,1E-6) && 
    almost_equal_with_tolerance(start_pt.y,end_pt.y,1E-6)){
      if (verbose){
    printf("This should be a point\n");
      }
      contour_idx++;contour_idx++;
    }
    else {
      /*Note: Going to ignore geno shocks for 
    a while. This labelled version is mainly 
    for shock to geno to boundary transition;
    and there shocks are irrelevant.*/
      copy_geno_intv_to_boundary(&(icurves->interval[i]),&(boundary[tmp_index]),
                 tmp_index,contour_index);
      boundary[tmp_index].id=contour_id_array[contour_idx];
      if (contour_idx != 0)
    boundary[tmp_index].left=contour_id_array[contour_idx-2];
      boundary[tmp_index].right=contour_id_array[contour_idx+2];
      
      if (verbose){
    printf("Bdry_idx=%d contour_idx=%d ID=%d\n",tmp_index,contour_idx,
           contour_id_array[contour_idx]);
      }
      contour_idx++;contour_idx++;
      tmp_index++;
    }
  }/* for (i=0;i<icurves->num_intv;i++) */
  
  /* If the contour is closed the left and right labels of the 
     end points have to be set */
  if (closed == 1){
    boundary[tmp_index-1].right=boundary[start_index].id;
    boundary[start_index].left=boundary[tmp_index-1].id;
  }
  else{
    boundary[tmp_index-1].right=contour_id_array[contour_idx];
    boundary[start_index].left=NOLABEL;
  }

  *boundary_index=tmp_index;
}/* End of function initialize_cedt_boundary_geno_curve*/

#ifdef __cplusplus
}
#endif





