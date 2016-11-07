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


#include "dbseg_subpixel_subpixel_bubbles.h"

/**************************************************************************
  Description : Function that finds the curvature of a boundary point, that 
                part of a GENO approx. boundary. Here instead of using GENO 
        apporximation to curvature, central differences are used to 
        find the curvature, as we want to smooth across corners.
  Inputs :  boundary - boundary segment.
            point - boundary_point
        boundary_id - the id of the contour segment under consideration.
  Return :  curvature
**************************************************************************/
double compute_curvature_anti_geno_fit(Boundary *boundary, Point boundary_point, 
                      int boundary_id, Point grid_point,double *surface, 
                      Region_Stats *region, int height,int width)
{
  Boundary *prev, *next, *curr;
  Boundary fit;
  Point minus2,minus1,plus1,plus2;
  Point boundary_point2,center;
  GENO_Interval intv1,intv2;
  double xss,yss,xs,ys;
  double R;
  double c1,c2,abscurvature,curvature;
  double dist_cent_grid,dist_grid_bdry;
  int x,y,pos;
  int verbose=0;
  int region_label,region_area,curvature_estimate;

  if (boundary[boundary_id].id == ENDOFBOUNDARY || 
      boundary[boundary_id].type == POINT){
    return 0;
  }
  /*For the end points the curvature seems to be causing trouble. So I am
    goind to return 0. */
  if (!is_valid_label(boundary[boundary_id].left))
    return 0;
  if (!is_valid_label(boundary[boundary_id].right))
    return 0;

  /*The curvature tends to shrink the curve. To avoid that, I am adding a term
    that is proportional to the sqrt of area to the curvature term. */
  region_label=boundary[boundary_id].segment;
  if (region_label <= 0){
    curvature_estimate=0;
  }
  else{
    region_area=region[region_label].no_of_pixels;
    if (region_area == 0 || region_area < 0){
      curvature_estimate=0;
    }
    else{
      curvature_estimate=1.0/sqrt(region_area/M_PI);
    }
  }
  
  /*Get the correct points for doing GENO interpolation.*/
  curr=&boundary[boundary_id];
  minus1=get_start_pt(curr);
  plus1=get_end_pt(curr);
  if (is_valid_label(boundary[boundary_id].left)){
    prev=&boundary[boundary[boundary_id].left];
    minus2=get_start_pt(prev);
  }
  else{
    minus2=get_start_pt(curr);
  }
  if (is_valid_label(boundary[boundary_id].right)){
    next=&boundary[boundary[boundary_id].right];
    plus2=get_end_pt(next);
  }
  else{
    plus2=get_end_pt(curr);
  }
  
  /* Fill the GENO interval structures, and compute 
     the two GENO interpolations, one from each side. */
  intv2.pts[0].x=minus1.x; intv2.pts[0].y=minus1.y;
  intv2.pts[1].x=plus1.x;  intv2.pts[1].y=plus1.y;
  intv2.pts[2].x=plus2.x;  intv2.pts[2].y=plus2.y;
  intv2.pts[3].x=-10000.3;  intv2.pts[3].y=567890.2;
  intv2.type=genoIntervalTypeLower;

  geno_compute_intv_info(&intv2);
  c2=intv2.fit.curvature;

  intv1.pts[0].x=minus2.x;  intv1.pts[0].y=minus2.y;
  intv1.pts[1].x=minus1.x;  intv1.pts[1].y=minus1.y;
  intv1.pts[2].x=plus1.x;  intv1.pts[2].y=plus1.y;
  intv1.pts[3].x=-10000.3;  intv1.pts[3].y=567890.2;
  intv1.type=genoIntervalTypeLower;

  geno_compute_intv_info(&intv1);
  c1=intv1.fit.curvature;
  
  if (verbose){
    printf("Curvature2=%f ",intv2.fit.curvature);
    printf("Curvature1=%f \n",intv1.fit.curvature);
  }

  /* If both interpolation are linear return 0*/
  if (c1 == 0 && c2 == 0){
    return curvature_estimate;
  }

  /*For smoothing purposes pick the higher of the two curvatures*/
  if (c1>c2){
    abscurvature=c1;
    copy_geno_intv_to_boundary(&intv1,&fit,0,0);
    R=1.0/intv1.fit.curvature;
    center.x=intv1.fit.center.x;
    center.y=intv1.fit.center.y;
  }
  else{
    abscurvature=c2;
    copy_geno_intv_to_boundary(&intv1,&fit,0,0);
    R=1.0/intv2.fit.curvature;
    center.x=intv2.fit.center.x;
    center.y=intv2.fit.center.y;
  }

  dist_cent_grid=euclidean_distance(center,grid_point);
  find_distance_boundary(&fit,grid_point.x,grid_point.y,&boundary_point2);
  dist_grid_bdry=euclidean_distance(grid_point,boundary_point2);
  x=(int)grid_point.x;
  y=(int)grid_point.y;
  pos=y*width+x;

  if (abscurvature > MAX_CURVATURE){
    abscurvature=MAX_CURVATURE;
  }

  /* Point lies inside the circle and inside the object */
  if (dist_cent_grid <= R && surface[pos] > 0){
    curvature=-abscurvature;
  }
  /* Point is outside the circle, but the distance from the center of
     circle to the grid point is less than the distance from the grid
     point to the boundary fit */
  else if((dist_grid_bdry > dist_cent_grid) && (surface[pos] > 0)){
    curvature=-abscurvature;
  }
  /* Point is outside the circle, outside the object and the the distance 
     from the center of circle to the grid point is more than the distance 
     from the grid point to the boundary fit */
  else if((dist_cent_grid >= R) && (dist_grid_bdry < dist_cent_grid) && 
      (surface[pos] < 0)){
    curvature=-abscurvature;
  }
  else{
    curvature=abscurvature;
  }
  if (verbose){
    printf("Final Curv=%f\n",curvature+curvature_estimate);
  }

  return curvature+curvature_estimate;
}


/**************************************************************************
  Description : Function that finds the curvature of a boundary point, that 
                part of a GENO approx. boundary.
  Inputs :  boundary - boundary segment.
            point - boundary_point
        surface - the surface being evolved.
  Return :  curvature
**************************************************************************/
double compute_curvature_geno_boundary(Boundary *boundary, Point boundary_point, 
                      Point grid_point,double *surface, int height,
                      int width)
{
  Point center;
  double normal_angle,rad_vect_angle;
  double d,R;
  double curvature;
  int verbose=0;
  int x,y,pos;

  if (boundary->type == LINE || boundary->type == POINT)
    return 0;
  else if (boundary->type == ARC){
    R=boundary->arc.radius;
    center.x=boundary->arc.center_x;
    center.y=boundary->arc.center_y;
    d=euclidean_distance(center,grid_point);
    x=(int)grid_point.x;
    y=(int)grid_point.y;
    pos=y*width+x;
    curvature=-boundary->arc.curvature;
    if (curvature < -0.5)
      curvature=-0.5;
    /* Point lies inside the circle, but outside the object */
    if (d < R && surface[pos] < 0){
      curvature=-curvature;
    }
    /* Point lies outside the circle, but inside the object */
    else if (d >= R && surface[pos] > 0){
      curvature=-curvature;
    }
  }
  return curvature;
}


/**************************************************************************
  Description : Function that does curvature coupling.
  Inputs :  boundary_point1 - The boundary point that is closest to the grid point.
            boundary_id1 - The id that contains boundary_point1.
            shock_point - The point on the shock corresponding to the boundary
                point.
        boundary_point2 - The point on the other contour, whose force 
                is to be coupled with boundary_point.
            boundary_id2 - The id that contains boundary_point2.
        boundary - the SPCedt boundary structure.
        surface - surface being evolved.
        height,width - image dimensions.
  Output:   net curvature at the grid point.
**************************************************************************/

double compute_coupled_curvature(Boundary *boundary, Point boundary_point1, int boundary_id1,
                Point grid_point, Point boundary_point2, int boundary_id2,
                Point shock_point,double *surface, Region_Stats *region,
                int height,int width)
{
  double K1,K2;
  double dist,alpha;
  int verbose=0,draw=0,write_file=0;
  int x,y;
  void *data;
  FILE *fp;

  if (write_file){
    fp=fopen("coupled-curv.dat","a");
  }

  dist=euclidean_distance(shock_point,boundary_point1);
  alpha=distance_drop_off_function(dist);

  K1=compute_curvature_anti_geno_fit(boundary,boundary_point1,boundary_id1,grid_point,
                     surface,region,height,width);
  K2=compute_curvature_anti_geno_fit(boundary,boundary_point2,boundary_id2,grid_point,
                     surface,region,height,width);
  if (draw){
    y=(int)grid_point.y;
    x=(int)grid_point.x;
    if (surface[y*width+x] > 0){
      force_draw_ps(boundary_point1,grid_point,-K1,BLUE);
      force_draw_ps(boundary_point2,shock_point,-K2,RED);
    }
    else{
      force_draw_ps(boundary_point1,grid_point,K1,BLUE);
      force_draw_ps(boundary_point2,shock_point,K2,RED);
    }
  }

  if (write_file){
    fprintf(fp, "boundary_point=(%f,%f) \n ",boundary_point1.x,boundary_point1.y);
    fprintf(fp, "F1=%f F2=%f dist=%f alpha=%f F=%f\n",K1,K2,dist,alpha,K1-alpha*K2);
  }
  if (write_file){
    fclose(fp);
  }
  return K1-alpha*K2;
}

#ifdef __cplusplus
}
#endif





