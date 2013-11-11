// This is brcv/seg/dbacm/dbacm_levelset_driver.cxx
//:
// \file

#include "dbacm_geodesic_active_contour.h"

#include <vil/vil_convert.h>
#include <vil/algo/vil_gauss_filter.h>

#include <bil/algo/bil_edt.h>
#include <dbacm/dbacm_standard_levelset_func.h>
#include <bil/algo/bil_edge_indicator.h>


//: Constructor
dbacm_geodesic_active_contour::
dbacm_geodesic_active_contour()
{
  this->levelset_func_ = new dbacm_standard_levelset_func();
}


// -------------------------------------------------------------------
//: Set parameters of levelset function
void dbacm_geodesic_active_contour::
compute_levelset_func_params()
{ 
  // set the weights
  // inflation
  this->levelset_func()->set_inflation_weight(
    -this->direction()*this->inflation_weight());

  this->levelset_func()->set_curvature_weight(this->curvature_weight_);
  
  if (this->use_geodesic_)
  {
    this->levelset_func()->set_avection_weight(1.0f);
  }
  else
  {
    this->levelset_func()->set_avection_weight(0.0f);
  }


  // compute gx, gy, and gmap from the image
  vil_image_view<float > gmap = this->levelset_func()->inflation();
  vil_image_view<float > gx = this->levelset_func()->advection_x();
  vil_image_view<float > gy = this->levelset_func()->advection_y();
  double sigma = 0.8;
  double M = this->gradient_norm();
  int exponent = 2;
  
  if (! this->feature_map().top_left_ptr())
  {
    bil_malladi_image_force(this->image_, M, exponent, sigma, gmap);
    //compute gradients of gmap
    vil_sobel_3x3(gmap, gx, gy);
  }
  else
  {
    // testing
    // override gmap at feature map position
    bil_malladi_image_force_with_feature_map(this->image_, M, exponent, sigma, this->feature_map(), gmap);
    //compute gradients of gmap
    vil_sobel_3x3(gmap, gx, gy);
  }

  this->levelset_func()->set_advection(gx, gy);
  this->levelset_func()->set_inflation(gmap);
  this->levelset_func()->set_curvature_modifier(gmap);
  
  return;
}

// -------------------------------------------------------------------
//: Set initial levelset function
void dbacm_geodesic_active_contour::
init_levelset_surf(const vil_image_view<float >& init_phi)
{
  this->levelset_func_->set_levelset_surf(init_phi);
  return;
}


//: Initialize levelset surface with binary mask
void dbacm_geodesic_active_contour::
init_levelset_surf_with_binary_mask(const vil_image_view<bool >& mask_binary)
{
  // convert to type `unsigned' to compute signed EDT
  vil_image_view<unsigned > mask_unsigned;
  vil_convert_cast(mask_binary, mask_unsigned);

  // signed distance transform
  vil_image_view<float > phi;
  bil_edt_signed(mask_unsigned, phi);

  // smooth distance transform image using gaussian filter
  double gauss_sigma = 0.5;
  vil_gauss_filter_5tap_params gauss_params(gauss_sigma);
  vil_image_view<float > phi_smoothed;
  vil_gauss_filter_5tap(phi, phi_smoothed, gauss_params);

  // assign to levelset function
  this->levelset_func()->set_levelset_surf(phi_smoothed);

  return;
}



// -------------------------------------------------------------------
//: Evolve the levelset function till convergence or reaching maximum 
// number of iterations.
// For now, just fixed number of iterations
void dbacm_geodesic_active_contour::
evolve(bool verbose)
{
  float dt = this->timestep();
  int num_iters_to_initialize = 10;
  for (unsigned int i=0; i<this->num_iterations(); ++i)
  {
    if (i % num_iters_to_initialize == 0)
    {
      this->levelset_func()->reinitialize_levelset_surf();
      if (verbose)
        vcl_cout << " \n- ";
    }
    if (verbose)
    {
      vcl_cout << " " << i;
    }
    this->levelset_func()->evolve_one_timestep(dt);
  }
  return;
}
