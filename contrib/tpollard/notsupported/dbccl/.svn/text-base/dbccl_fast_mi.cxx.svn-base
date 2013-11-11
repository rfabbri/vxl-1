#ifndef _dbccl_fast_mi_cxx_
#define _dbccl_fast_mi_cxx_

#include "dbccl_fast_mi.h"

#include <vnl/vnl_vector_fixed.h>


//---------------------------------------------
dbccl_fast_mi::dbccl_fast_mi(
  dbccl_fast_mi_params* params )
{
  params_ = params;
  log_search_volume_ = log( (float)params_->mi_mask_volume() );
  log_table_ = vnl_vector<float>( params_->mi_mask_volume() );
  for( int i = 1; i < params_->mi_mask_volume(); i++ )
    log_table_(i) = log((float)i);
};


//----------------------------------------------
float
dbccl_fast_mi::compute_mi(
  const vil_image_view<vxl_byte>& image,
  const vgl_point_2d<int>& image_window_ul_corner,
  const vnl_matrix<int>& mi_mask )
{
  vnl_matrix<int> pab( params_->histogram_size, params_->histogram_size, 0 );
  vnl_vector<int> pa( params_->histogram_size, 0 );
  vnl_vector<int> pb( params_->histogram_size, 0 );
  float ha = 0, hb = 0, hab = 0;

  // Fill the histograms.
  for( int nx = 0; nx < params_->mi_mask_diameter(); nx++ ){
    for( int ny = 0; ny < params_->mi_mask_diameter(); ny++ ){
      int base_bucket = (int)floor( 
        params_->histogram_size*mi_mask(ny,nx)/255.1 );
      int image_bucket = (int)floor(
        params_->histogram_size*
      image( image_window_ul_corner.x() + nx, image_window_ul_corner.y()+ ny )/255.1 );
      pab( image_bucket, base_bucket )++;
      pa( image_bucket )++;
      pb( base_bucket )++;
    }  
  }

  // Compute the entropy and MI.
  for( int i = 0; i < params_->histogram_size; i++ ){
    if( pa(i) > 0 ) ha-= (pa(i)/(float)params_->mi_mask_volume())*
      (log_table_(pa(i))-log_search_volume_);
    if( pb(i) > 0 ) hb-= (pb(i)/(float)params_->mi_mask_volume())*
      (log_table_(pb(i))-log_search_volume_);
    for( int j = 0; j < params_->histogram_size; j++ )
      if( pab(i,j) > 0 ) hab-= (pab(i,j)/(float)params_->mi_mask_volume())*
        (log_table_(pab(i,j))-log_search_volume_);
  }
  return ha + hb - hab;
};


//----------------------------------------------
void 
dbccl_fast_mi::compute_mi_region(
  const vil_image_view<vxl_byte>& image,
  const vgl_point_2d<int>& starting_point,
  const vnl_matrix<int>& mi_mask,
  vnl_matrix<float>& mi_region )
{
  mi_region = vnl_matrix<float>( 
    params_->mi_region_diameter(), params_->mi_region_diameter(), -1 );
 
  // Compute the bounds on the search region that we can compute MI on, due to
  // image size.
  int min_x = 0, min_y = 0, 
      max_x = params_->mi_region_diameter(), max_y = params_->mi_region_diameter();
  int min_x_pixel = starting_point.x()-(params_->mi_region_rad+params_->mi_mask_rad);
  if( min_x_pixel < 0 ) min_x = -min_x_pixel;
  int max_x_pixel = starting_point.x()+(params_->mi_region_rad+params_->mi_mask_rad);
  if( max_x_pixel >= image.ni() ) max_x -= 1+max_x_pixel-image.ni();
  int min_y_pixel = starting_point.y()-(params_->mi_region_rad+params_->mi_mask_rad);
  if( min_y_pixel < 0 ) min_y = -min_y_pixel;
  int max_y_pixel = starting_point.y()+(params_->mi_region_rad+params_->mi_mask_rad);
  if( max_y_pixel >= image.nj() ) max_y -= 1+max_y_pixel-image.nj();

  // Compute the MI at each pixel in the search area.
  for( int y = min_y; y < max_y; y++ )
    for( int x = min_x; x < max_x; x++ )
      mi_region(y,x) = compute_mi( image, 
        vgl_point_2d<int>( min_x_pixel + x, min_y_pixel + y ), mi_mask );
};


#endif // _dbccl_fast_mi_cxx_
