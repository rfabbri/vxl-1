// This is brcv/seg/dbacm3d/dbacm3d_levelset_driver.cxx
//:
// \file

#include "dbacm3d_geodesic_active_contour.h"

#include <vil3d/vil3d_convert.h>
#include <dbil3d/algo/dbil3d_gauss_filter.h>

#include <dbil/algo/dbil_edt.h>
#include <dbacm/dbacm3d_standard_levelset_func.h>
#include <dbacm/dbacm3d_image_filters.h>


//: Constructor
dbacm3d_geodesic_active_contour::
dbacm3d_geodesic_active_contour()
{
  this->levelset_func_ = new dbacm3d_standard_levelset_func();
}


// -------------------------------------------------------------------
//: Set parameters of levelset function
void dbacm3d_geodesic_active_contour::
compute_levelset_func_params()
{ 
  // set the weights
  // inflation
  this->levelset_func()->set_inflation_weight( -this->direction()*this->inflation_weight());

  vcl_cerr << "setting level set func curvature weight to " << this->curvature_weight_ << vcl_endl;

  this->levelset_func()->set_curvature_weight(this->curvature_weight_);
  
  if (this->use_geodesic_)
  {
    this->levelset_func()->set_advection_weight(1.0f);
  }
  else
  {
    this->levelset_func()->set_advection_weight(0.0f);
  }


  // compute gx, gy, and gmap from the image
  vil3d_image_view<float> gmap = this->levelset_func()->inflation();
  vil3d_image_view<float> gx = this->levelset_func()->advection_x();
  vil3d_image_view<float> gy = this->levelset_func()->advection_y();
  vil3d_image_view<float> gz = this->levelset_func()->advection_z();
  double M = this->gradient_norm();
  
  if (! this->feature_map().origin_ptr())
  {
    dbacm3d_malladi_image_force(this->image_, M, exponent_, this->direction(), sigma_, gmap, gx, gy, gz);
  }
  else
  {
    // testing
    // override gmap at feature map position
    dbacm3d_malladi_image_force_with_feature_map(this->image_, M, exponent_, sigma_, this->feature_map(), gmap, gx, gy);
  }

  this->levelset_func()->set_advection(gx,gy,gz);
  this->levelset_func()->set_inflation(gmap);
  this->levelset_func()->set_curvature_modifier(gmap);
  
  return;
}

// -------------------------------------------------------------------
//: Set initial levelset function
void dbacm3d_geodesic_active_contour::
init_levelset_surf(const vil3d_image_view<float >& init_phi)
{
  this->levelset_func_->set_levelset_surf(init_phi);
  return;
}


//: Initialize levelset surface with binary mask
void dbacm3d_geodesic_active_contour::
init_levelset_surf_with_binary_mask(const vil3d_image_view<bool >& mask_binary)
{
  vcl_cerr << "Not yet implemented" << vcl_endl;
/*
  // convert to type `unsigned' to compute signed EDT
  vil3d_image_view<unsigned > mask_unsigned;
  vil3d_convert_cast(mask_binary, mask_unsigned);

  // signed distance transform
  vil3d_image_view<float > phi;
  dbil_edt_signed(mask_unsigned, phi);

  // smooth distance transform image using gaussian filter
  double gauss_sigma = 0.5;
  vil3d_gauss_filter_5tap_params gauss_params(gauss_sigma);
  vil3d_image_view<float > phi_smoothed;
  vil3d_gauss_filter_5tap(phi, phi_smoothed, gauss_params);

  // assign to levelset function
  this->levelset_func()->set_levelset_surf(phi_smoothed);
  */

  return;
}



// -------------------------------------------------------------------
//: Evolve the levelset function till convergence or reaching maximum 
// number of iterations.
// For now, just fixed number of iterations
void dbacm3d_geodesic_active_contour::
evolve(bool verbose)
{
    vcl_cerr << "evolve " << vcl_endl;;
  float dt = this->timestep();
  //int num_iters_to_initialize = 10;
  for (unsigned int i=0; i<this->num_iterations(); ++i)
  {
  /*
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
    */
    vcl_cerr << " " << i;
    this->levelset_func()->evolve_one_timestep(dt);
  }
  return;
}
