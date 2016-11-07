#ifdef __cplusplus
extern "C" {
#endif

#include "dbseg_subpixel_subpixel_bubbles.h"

/*********************************************************************************
  Description: Computes the reaction flux for one element of the surface array 
  (specified by x,y) for Hamilton  Jacobi equations using Osher Sethian scheme.
  ENO derivatives are used to get higher order accuracy.

  Inputs : surface_array - 
           band_size
           x, y
       height,width
       enoimage
       flow_direction - +1 if the surface_value is increasing
                        -1 if the surface_value is decreasing
  Outputs: reaction_array

*********************************************************************************/
void reaction_flux_with_eno(double *surface_array,double *reaction_array,double band_size, 
                double flow_direction, ENO_Interval_Image_t *enoimage, 
                int x, int y, int height, int width)

     
{
  int position; 
  FILE *fopen(), *f1;
  double xm1ym1, xym1, xp1ym1, xm1y, xy, xp1y, xm1yp1,xyp1, xp1yp1;
  double Dxm, Dxp, Dym, Dyp, Dx, Dy, delta_x, delta_y;
  double reaction_flux_value,surface_value;
  
  assert(x>=0 && x<= width-1 && y>=0 && y<=height-1);
  position = y*width+x;
  surface_value = surface_array[position];
  
  if (fabs(surface_value) <= band_size) {
    
    /* Dxm = (double)enointerp_derivative_at(&enoimage->horiz[y*width+(x-1)].interp,
                     enoimage->horiz[y*width+(x-1)].interp.end);
    Dxp = (double)enointerp_derivative_at(&enoimage->horiz[y*width+(x)].interp,
                     enoimage->horiz[y*width+(x)].interp.start);
    Dym = (double)enointerp_derivative_at(&enoimage->vert[(y-1)*width+x].interp,
                     enoimage->vert[(y-1)*width+x].interp.end);
    Dyp = (double)enointerp_derivative_at(&enoimage->vert[y*width+x].interp,
                     enoimage->vert[y*width+x].interp.start);*/

    Dxm = (double)eno_1derivative(&enoimage->horiz[y*width+(x-1)],
                 (double)enoimage->horiz[y*width+(x-1)].interp.end);
    Dxp = (double)eno_1derivative(&enoimage->horiz[y*width+(x)],
                 (double)enoimage->horiz[y*width+(x)].interp.start);
    Dym = (double)eno_1derivative(&enoimage->vert[(y-1)*width+x],
                 (double)enoimage->vert[(y-1)*width+x].interp.end);
    Dyp = (double)eno_1derivative(&enoimage->vert[y*width+x],
                 (double)enoimage->vert[y*width+x].interp.start);

    if (flow_direction < 0) 
      reaction_flux_value = hj_flux_osher_sethian (Dxm, Dxp, Dym, Dyp);
    else if (flow_direction > 0) 
      reaction_flux_value = hj_flux_osher_sethian (-Dxm, -Dxp, -Dym, -Dyp);
    
    reaction_array[position] = reaction_flux_value;
    /* printf("flow=%f Dxm=%f, Dxp=%f, Dym=%f, Dyp=%f Rn=%f\n", */
    /*        flow_direction,Dxm, Dxp, Dym, Dyp,reaction_flux_value); */
  }
  
}/*End of function  reaction_flux_with_eno */


/*********************************************************************************
  Description: Computes the reaction flux for one element of the surface array 
  (specified by x,y) for Hamilton  Jacobi equations using Osher Sethian scheme.

  Inputs : surface_array - 
           band_size
           x, y
       height,width
       flow_direction - +1 if the surface_value is increasing
                        -1 if the surface_value is decreasing
  Outputs: reaction_array

*********************************************************************************/
void reaction_flux(double *surface_array,double *reaction_array,int band_size, 
           double flow_direction, int x, int y, int height, int width)
     
     
{
  int position; 
  FILE *fopen(), *f1;
  double xm1ym1, xym1, xp1ym1, xm1y, xy, xp1y, xm1yp1,xyp1, xp1yp1;
  double Dxm, Dxp, Dym, Dyp, Dx, Dy, delta_x, delta_y;
  double reaction_flux_value,surface_value;
  
  position = y*width+x;
  surface_value = surface_array[position];
  
  if (fabs(surface_value) <= band_size) {
    
    xm1ym1 = surface_array[(y-1)*width+(x-1)];
    xym1   = surface_array[(y-1)*width+x];
    xp1ym1 = surface_array[(y-1)*width+(x+1)];
    xm1y = surface_array[y*width+(x-1)];
    xy = surface_array[y*width+x];
    xp1y = surface_array[y*width+(x+1)];
    xm1yp1 = surface_array[(y+1)*width+(x-1)];
    xyp1 = surface_array[(y+1)*width+x];
    xp1yp1 = surface_array[(y+1)*width+(x+1)]; 
    
    Dxm = (xy - xm1y);
    Dxp = (xp1y - xy);
    Dym = (xy - xym1);
    Dyp = (xyp1 - xy);
    
    if (flow_direction < 0) 
      reaction_flux_value = hj_flux_osher_sethian (Dxm, Dxp, Dym, Dyp);
    else if (flow_direction > 0) 
      reaction_flux_value = hj_flux_osher_sethian (-Dxm, -Dxp, -Dym, -Dyp);
    
    reaction_array[position] = reaction_flux_value;
  }
  
}/*End of function  reaction_flux */


/*                 The Hamilton-Jacobi Flux in 2D
 */

double hj_flux_osher_sethian (double u_l, double u_r, double u_u, double u_d)
   
{
  double
    min_ul_0,
    max_ur_0,
    min_uu_0,
    max_ud_0,flux_1,flux_2;

  if (u_l < 0)  min_ul_0 = u_l;
  else          min_ul_0 = 0.0;
  
  if (u_r > 0)  max_ur_0 = u_r;
  else          max_ur_0 = 0.0;
  
  if (u_u < 0)  min_uu_0 = u_u;
  else          min_uu_0 = 0.0;
  
  if (u_d > 0)  max_ud_0 = u_d;
  else          max_ud_0 = 0.0;
  flux_1 = sqrt (pow (min_ul_0, 2.0) + pow (max_ur_0, 2.0));
  flux_2 = sqrt (pow (min_uu_0, 2.0) + pow (max_ud_0, 2.0));

  return(0.0 - sqrt (flux_1*flux_1 + flux_2*flux_2)); 
}

/*********************************************************************************
  Description: Computes the diffusion flux for one element of the surface array 
  (specified by x,y) for Hamilton  Jacobi equations using Osher Sethian scheme.
  ENO derivatives are used to get higher order accuracy.

  Inputs : surface_array - 
           band_size -
           x, y - 
       height,width - 
       enoimage - 
  Outputs: diffusion_array

*********************************************************************************/
void diffusion_flux_with_eno(double *surface_array,double *diffusion_array,double band_size, 
                 ENO_Interval_Image_t *enosurface, 
                 int x, int y, int height, int width)
{
  int position; 
  FILE *fopen(), *f1;
  double xm1ym1, xym1, xp1ym1, xm1y, xy, xp1y, xm1yp1,xyp1, xp1yp1;
  double Dxm, Dxp, Dym, Dyp, Dx, Dy;
  double delta_x, delta_y;
  double Dxxp,Dxxm,Dyyp,Dyym,Dyy,Dxx;
  double Dx_yp,Dx_ym,Dxy;
  double Dxp_yp,Dxm_yp,Dxp_ym,Dxm_ym;
  double diffusion_flux_value,surface_value;
  double gradient, gradient_squared,curvature_numerator,curvature;
  double max_curvature_warning, min_meaningful_curvature,min_gradient_warning;

  max_curvature_warning = 1000;
  min_meaningful_curvature = 0.001;
  min_gradient_warning = 0.001;

  
  assert(x>=0 && x<= width-1 && y>=0 && y<=height-1);
  position = y*width+x;
  surface_value = surface_array[position];
  
  if (fabs(surface_value) <= band_size) {
    
    Dxm = (double)eno_1derivative(&enosurface->horiz[y*width+(x-1)],
                 (double)enosurface->horiz[y*width+(x-1)].interp.end);
    Dxp = (double)eno_1derivative(&enosurface->horiz[y*width+(x)],
                 (double)enosurface->horiz[y*width+(x)].interp.start);
    Dym = (double)eno_1derivative(&enosurface->vert[(y-1)*width+x],
                 (double)enosurface->vert[(y-1)*width+x].interp.end);
    Dyp = (double)eno_1derivative(&enosurface->vert[y*width+x],
                 (double)enosurface->vert[y*width+x].interp.start);

    Dxxm = (double)eno_2derivative(&enosurface->horiz[y*width+(x-1)],
                 (double)enosurface->horiz[y*width+(x-1)].interp.end);
    Dxxp = (double)eno_2derivative(&enosurface->horiz[y*width+(x)],
                 (double)enosurface->horiz[y*width+(x)].interp.start);
    Dyym = (double)eno_2derivative(&enosurface->vert[(y-1)*width+x],
                 (double)enosurface->vert[(y-1)*width+x].interp.end);
    Dyyp = (double)eno_2derivative(&enosurface->vert[y*width+x],
                 (double)enosurface->vert[y*width+x].interp.start);
    Dx  = (Dxm + Dxp)/2.0;
    Dy  = (Dym + Dyp)/2.0;
    Dxx = (Dxxm + Dxxp)/2.0;
    Dyy = (Dyym + Dyyp)/2.0;

    Dxm_ym = (double)eno_1derivative(&enosurface->horiz[(y-1)*width+(x-1)],
                    (double)enosurface->horiz[(y-1)*width+(x-1)].interp.end);
    Dxp_ym = (double)eno_1derivative(&enosurface->horiz[(y-1)*width+x],
                    (double)enosurface->horiz[(y-1)*width+x].interp.start);
    
    Dxm_yp = (double)eno_1derivative(&enosurface->horiz[(y+1)*width+(x-1)],
                    (double)enosurface->horiz[(y+1)*width+(x-1)].interp.end);
    Dxp_yp = (double)eno_1derivative(&enosurface->horiz[(y+1)*width+x],
                    (double)enosurface->horiz[(y+1)*width+x].interp.start);
    Dx_ym=(Dxm_ym+Dxp_ym)/2.0;
    Dx_yp=(Dxm_yp+Dxp_yp)/2.0;

    Dxy = (Dx_yp-Dx_ym)/2.0;
  
    curvature_numerator = Dyy*Dx*Dx + (-2.0)*Dx*Dy*Dxy + Dxx*Dy*Dy;
    gradient_squared = Dx*Dx+Dy*Dy;
    gradient = sqrt(Dx*Dx+Dy*Dy);
    curvature = curvature_numerator/pow(gradient_squared,1.5);
  
    if ((gradient > min_gradient_warning) &&
    (curvature_numerator < max_curvature_warning)) {
      diffusion_flux_value = curvature_numerator / gradient_squared;
    }
    /* Just a warning to say curvature numerator is high. */
    else if ((gradient > min_gradient_warning) &&
         (curvature_numerator >= max_curvature_warning)) {
      diffusion_flux_value = curvature_numerator / gradient_squared;
    }
    /* When both gradient and curvature numertaor are small! Do not move! */
    else if ((gradient < min_gradient_warning) &&
         (curvature_numerator < min_meaningful_curvature)) {
      diffusion_flux_value = 0.0;
    }/* Else if (gradient < ..... */
    /* printf("%f %f %f\n",curvature_numerator,gradient,diffusion_flux_value); */
    diffusion_array[position] = diffusion_flux_value;
  }
}/*End of function  diffusion_flux_with_eno */



void diffusion_flux(double *surface_array, double *diffusion_array,
            int band_size, int x, int y, int height, int width)    
     
{
  int position;               /* Loop counters. */
  double xm1ym1, xym1, xp1ym1, xm1y, xy,xp1y,xm1yp1,xyp1,xp1yp1;
  double Dx, Dy, Dxx,Dyy,Dxy;
  double gradient, gradient_squared,curvature_numerator,diffusion_flux_value,curvature;
  double max_curvature_warning, min_meaningful_curvature,min_gradient_warning;
  double surface_value;
  
  max_curvature_warning = 1000;
  min_meaningful_curvature = 0.001;
  min_gradient_warning = 0.001;
  
  position = y*width+x;
  surface_value = surface_array[position];

  if (fabs(surface_value) < band_size) { 
    xm1ym1 = surface_array[(y-1)*width+(x-1)];
    xym1   = surface_array[(y-1)*width+x];
    xp1ym1 = surface_array[(y-1)*width+(x+1)];
    xm1y   = surface_array[y*width+(x-1)];
    xy     = surface_array[y*width+x];
    xp1y   = surface_array[y*width+(x+1)];
    xm1yp1 = surface_array[(y+1)*width+(x-1)];
    xyp1   = surface_array[(y+1)*width+x];
    xp1yp1 = surface_array[(y+1)*width+(x+1)]; 
    
    Dx = (xp1y - xm1y) /2.0;
    Dy = (xyp1 - xym1) /2.0;
    Dxx  = (xp1y + ((-2.0)*xy) + xm1y);
    Dyy  = (xyp1 + ((-2.0)*xy) + xym1);
    Dxy = ((xp1yp1 + xm1ym1) - (xp1ym1 + xm1yp1)) / 4.0;
    
    curvature_numerator = Dyy*Dx*Dx + (-2.0)*Dx*Dy*Dxy + Dxx*Dy*Dy;
    
    gradient_squared = Dx*Dx+Dy*Dy;
    gradient = sqrt(Dx*Dx+Dy*Dy);
    curvature = curvature_numerator/pow (gradient_squared,1.5);
    
    if ((gradient > min_gradient_warning) &&
    (curvature_numerator < max_curvature_warning)) {
      diffusion_flux_value = curvature_numerator / gradient_squared;
    }
    /* Just a warning to say curvature numerator is high. */
    
    else if ((gradient > min_gradient_warning) &&
         (curvature_numerator >= max_curvature_warning)) {
      diffusion_flux_value = curvature_numerator / gradient_squared;
    } /* Else if */
    
    /* When both gradient and curvature numertaor are small! Do not move! */
    
    else if ((gradient < min_gradient_warning) &&
         (curvature_numerator < min_meaningful_curvature)) {
      
      diffusion_flux_value = 0.0;
    } /* Else if (gradient < ..... */
    
    
    /* Small gradient, but curvature numerator o.k. The, move by something
     * like mean curvature consider a spherical limit.
     */
    else {
      
      diffusion_flux_value = (Dxx+Dyy)/ 2.0;
    } /* Else */
    
    diffusion_array[position]= diffusion_flux_value;
    
  }
}

#ifdef __cplusplus
}
#endif





