// This is brcv/seg/dbdet/dbacm_standard_levelset_func.cxx
//:
// \file

#include "dbacm_standard_levelset_func.h"

#include <vnl/vnl_math.h>

#include <vil/vil_convert.h>
#include <vil/algo/vil_gauss_filter.h>
#include <vil/algo/vil_threshold.h>
#include <bil/algo/bil_edt.h>

#include <bil/algo/bil_finite_differences.h>
#include <bil/algo/bil_finite_second_differences.h>


#include <dbacm/dbacm_levelset_math.h>

const float dbacm_min_grad_phi = float(1e-10);


//: Reinitialize levelset surface (to make sure it is a distance transform)
void dbacm_standard_levelset_func::
reinitialize_levelset_surf()
{
  // get binary image
  vil_image_view<bool > mask_binary;
  vil_threshold_above<float >(this->phi_, mask_binary, 0);

  // convert to type `unsigned' to compute signed EDT
  vil_image_view<unsigned > mask_unsigned;
  vil_convert_cast(mask_binary, mask_unsigned);

  // signed distance transform
  vil_image_view<float > sedt_mask;
  bil_edt_signed(mask_unsigned, sedt_mask);

  // smooth distance transform image using gaussian filter
  double gauss_sigma = 0.5;
  vil_gauss_filter_5tap_params gauss_params(gauss_sigma);
  vil_gauss_filter_5tap(sedt_mask, this->phi_, gauss_params);
  return;
}


// ------------------------------------------------------------------
//: evolve the levelset function given a time step
void dbacm_standard_levelset_func::
evolve_one_timestep(float timestep)
{
  // compute finite difference, hj-flux, and curvature images
  this->compute_internal_data();

  // iterate through all points on image
  unsigned 
    ni=this->phi_.ni(), 
    nj=this->phi_.nj(), 
    np=this->phi_.nplanes();

  vcl_ptrdiff_t 
    istep_phi=this->phi_.istep(), 
    jstep_phi=this->phi_.jstep(),
    pstep_phi=this->phi_.planestep();

  vcl_ptrdiff_t 
    istep_hj=this->hj_flux_.istep(), 
    jstep_hj=this->hj_flux_.jstep(),
    pstep_hj=this->hj_flux_.planestep();

  vcl_ptrdiff_t 
    istep_k=this->curvature_.istep(), 
    jstep_k=this->curvature_.jstep(),
    pstep_k=this->curvature_.planestep();

  vcl_ptrdiff_t 
    istep_dxc=this->dxc_.istep(), 
    jstep_dxc=this->dxc_.jstep(),
    pstep_dxc=this->dxc_.planestep();

  vcl_ptrdiff_t 
    istep_dyc=this->dyc_.istep(), 
    jstep_dyc=this->dyc_.jstep(),
    pstep_dyc=this->dyc_.planestep();

  vcl_ptrdiff_t 
    istep_gmap=this->gmap_.istep(), 
    jstep_gmap=this->gmap_.jstep(),
    pstep_gmap=this->gmap_.planestep();

  vcl_ptrdiff_t 
    istep_gx=this->gx_.istep(), 
    jstep_gx=this->gx_.jstep(),
    pstep_gx=this->gx_.planestep();

  vcl_ptrdiff_t 
    istep_gy=this->gy_.istep(), 
    jstep_gy=this->gy_.jstep(),
    pstep_gy=this->gy_.planestep();

  vcl_ptrdiff_t 
    istep_kmap=this->kmap_.istep(), 
    jstep_kmap=this->kmap_.jstep(),
    pstep_kmap=this->kmap_.planestep();



  float* plane_phi = this->phi_.top_left_ptr();
  const float* plane_hj = this->hj_flux_.top_left_ptr();
  const float* plane_k = this->curvature_.top_left_ptr();
  const float* plane_dxc = this->dxc_.top_left_ptr();
  const float* plane_dyc = this->dyc_.top_left_ptr();

  const float* plane_gmap = this->gmap_.top_left_ptr();
  const float* plane_gx = this->gx_.top_left_ptr();
  const float* plane_gy = this->gy_.top_left_ptr();
  const float* plane_kmap = this->kmap_.top_left_ptr();

  for (unsigned p=0; p<np; ++p, 
    plane_phi += pstep_phi,
    plane_hj += pstep_hj,
    plane_k += pstep_k,
    plane_dxc += pstep_dxc,
    plane_dyc += pstep_dyc,
    plane_gmap += pstep_gmap,
    plane_gx += pstep_gx,
    plane_gy += pstep_gy,
    plane_kmap += pstep_kmap)
  {
    float* row_phi = plane_phi;
    const float* row_hj = plane_hj;
    const float* row_k = plane_k;
    const float* row_dxc = plane_dxc;
    const float* row_dyc = plane_dyc;

    const float* row_gmap = plane_gmap;
    const float* row_gx = plane_gx;
    const float* row_gy = plane_gy;
    const float* row_kmap = plane_kmap;

    for (unsigned j=0; j<nj; ++j,
      row_phi += jstep_phi,
      row_hj += jstep_hj,
      row_k += jstep_k,
      row_dxc += jstep_dxc,
      row_dyc += jstep_dyc,
      row_gmap += jstep_gmap,
      row_gx += jstep_gx,
      row_gy += jstep_gy,
      row_kmap += jstep_kmap)
    {
      float* pixel_phi = row_phi;
      const float* pixel_hj = row_hj;
      const float* pixel_k = row_k;
      const float* pixel_dxc = row_dxc;
      const float* pixel_dyc = row_dyc;

      const float* pixel_gmap = row_gmap;
      const float* pixel_gx = row_gx;
      const float* pixel_gy = row_gy;
      const float* pixel_kmap = row_kmap;

      for (unsigned i=0; i<ni; ++i,
        pixel_phi += istep_phi,
        pixel_hj += istep_hj,
        pixel_k += istep_k,
        pixel_dxc += istep_dxc,
        pixel_dyc += istep_dyc,
        pixel_gmap += istep_gmap,
        pixel_gx += istep_gx,
        pixel_gy += istep_gy,
        pixel_kmap += istep_kmap)
      {
        float advection_term, inflation_term, curvature_term, speed;
        float grad_phi_mag = vnl_math_hypot(*pixel_dxc, *pixel_dyc);
        

        // advection_term (geodesic)
        advection_term = (grad_phi_mag < dbacm_min_grad_phi) ? 0 :
        ((*pixel_gx)*(*pixel_dxc) + (*pixel_gy)*(*pixel_dyc))/grad_phi_mag;

        // inflation term
        inflation_term = (*pixel_gmap)*(*pixel_hj);

        // curvature term
         curvature_term = (*pixel_kmap)*(*pixel_k)*(*pixel_hj);

        // overall speed
        speed = this->advection_weight()*advection_term - 
          this->inflation_weight()*inflation_term +
          this->curvature_weight()*curvature_term;

        // update
        *pixel_phi += timestep * speed;
      }
    }
  }
  
  return;
}



// ------------------------------------------------------------------
//: Compute all internal data necessary to evolve the levelset surface
void dbacm_standard_levelset_func::
compute_internal_data()
{
  // 1. Compute Hamilton-Jacobi flux

  // Update finite difference images
  bil_finite_differences(this->phi_,
    this->dxp_,
    this->dxm_,
    this->dxc_,
    this->dyp_,
    this->dym_,
    this->dyc_);

  // Compute hj-flux using finite difference images
  dbacm_levelset_math_compute_hj_flux<float >(this->hj_flux_,
    (this->inflation_weight()>=0) ? 1 : -1,
    this->dxp_,
    this->dxm_,
    this->dyp_,
    this->dym_);

  // 2. Compute mean curvature on the surface

  // Update 2nd derivative images using finite difference
  bil_finite_second_differences<float >(this->phi_,
    this->dxp_,
    this->dxm_,
    this->dyp_,
    this->dym_,
    this->dxx_,
    this->dyy_,
    this->dxy_);

  // Compute curvature using finite difference images
  dbacm_levelset_math_compute_curvature<float >(this->curvature_,
    this->dxc_,
    this->dyc_,
    this->dxx_,
    this->dyy_,
    this->dxy_);
  return;
}


