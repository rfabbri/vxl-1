#ifndef _reaction_flux_h_
#define _reaction_flux_h_


float hamilton_jacobi_flux (float u_l, float u_r, float u_u, float u_d);

void reaction_flux_values (float *surface_array, float *reaction_array, int band_size, 
                           float flow_direction, int height, int width);

#endif 


