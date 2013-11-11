// This is brcv/seg/dbacm/dbacm_levelset_math.h
#ifndef dbacm_levelset_math_h_
#define dbacm_levelset_math_h_

//:
// \file
// \brief Various math functions used in levelset segmentation
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date 11/30/2005
//
// \verbatim
//  Modifications
// \endverbatim


#include <vil/vil_image_view.h>
#include <vil/vil_math.h>

#define MAX_CURVATURE 100
#define MIN_CURVATURE .001
#define MIN_GRADIENT  .001


//: Compute curvature at every point on levelset surface
// assume all input images have the same size
template <class T>
inline void dbacm_levelset_math_compute_curvature(vil_image_view<T >& curvature,
    const vil_image_view<T>& dxc,
    const vil_image_view<T>& dyc,
    const vil_image_view<T>& dxx,
    const vil_image_view<T>& dyy,
    const vil_image_view<T>& dxy)
{
  unsigned 
    ni=dxc.ni(), 
    nj=dxc.nj(), 
    np=dxc.nplanes();
    
  T xc;
  T xx;
  T yc;
  T yy;
  T xy;
  T numerator;
  T gradient;

  curvature.set_size(ni, nj, np);

  vcl_ptrdiff_t 
    istep_k=curvature.istep(), 
    jstep_k=curvature.jstep(),
    pstep_k=curvature.planestep();

  vcl_ptrdiff_t 
    istep_dxc=dxc.istep(), 
    jstep_dxc=dxc.jstep(),
    pstep_dxc=dxc.planestep();

  vcl_ptrdiff_t 
    istep_dyc=dyc.istep(), 
    jstep_dyc=dyc.jstep(),
    pstep_dyc=dyc.planestep();

  vcl_ptrdiff_t 
    istep_dxx=dxx.istep(), 
    jstep_dxx=dxx.jstep(),
    pstep_dxx=dxx.planestep();

  vcl_ptrdiff_t 
    istep_dyy=dyy.istep(), 
    jstep_dyy=dyy.jstep(),
    pstep_dyy=dyy.planestep();

  vcl_ptrdiff_t 
    istep_dxy=dxy.istep(), 
    jstep_dxy=dxy.jstep(),
    pstep_dxy=dxy.planestep();


  float* plane_k = curvature.top_left_ptr();
  const float* plane_dxc = dxc.top_left_ptr();
  const float* plane_dyc = dyc.top_left_ptr();
  const float* plane_dxx = dxx.top_left_ptr();
  const float* plane_dyy = dyy.top_left_ptr();
  const float* plane_dxy = dxy.top_left_ptr();
  
  for (unsigned p=0; p<np; ++p, 
    plane_k += pstep_k,
    plane_dxc += pstep_dxc,
    plane_dyc += pstep_dyc,
    plane_dxx += pstep_dxx,
    plane_dyy += pstep_dyy,
    plane_dxy += pstep_dxy)
  {
    float* row_k = plane_k;
    const float* row_dxc = plane_dxc;
    const float* row_dyc = plane_dyc;
    const float* row_dxx = plane_dxx;
    const float* row_dyy = plane_dyy;
    const float* row_dxy = plane_dxy;

    for (unsigned j=0; j<nj; ++j,
      row_k += jstep_k,
      row_dxc += jstep_dxc,
      row_dyc += jstep_dyc,
      row_dxx += jstep_dxx,
      row_dyy += jstep_dyy,
      row_dxy += jstep_dxy)
    {
      float* pixel_k = row_k;
      const float* pixel_dxc = row_dxc;
      const float* pixel_dyc = row_dyc;
      const float* pixel_dxx = row_dxx;
      const float* pixel_dyy = row_dyy;
      const float* pixel_dxy = row_dxy;

      for (unsigned i=0; i<ni; ++i,
        pixel_k += istep_k,
        pixel_dxc += istep_dxc,
        pixel_dyc += istep_dyc,
        pixel_dxx += istep_dxx,
        pixel_dyy += istep_dyy,
        pixel_dxy += istep_dxy)
      {
        
        xc = *pixel_dxc;
        yc = *pixel_dyc;
        xx = *pixel_dxx;
        yy = *pixel_dyy;
        xy = *pixel_dxy;
        
        numerator = xx*yc*yc - 2*xc*yc*xy + yy*xc*xc;
        gradient = vcl_sqrt(xc*xc + yc*yc);
          
        if(gradient > MIN_GRADIENT)
        {
          *pixel_k = numerator / (gradient*gradient*gradient);
        }
        else if (vnl_math_abs(numerator) < MIN_CURVATURE)
        {
          *pixel_k = 0;
        }
        else
        {
          *pixel_k = (xx + yy)/2;
        }
        //// limit large curvature
        //result(i, j, p) = (vnl_math_abs(pixel_k) > MAX_CURVATURE) ? 
        //  0 : pixel_k;
      }
    }
  }
}  




//: Compute Hamilton-Jacobi flux
template <class T>
inline void dbacm_levelset_math_compute_hj_flux(vil_image_view<T >& hj_flux,
    int direction,
    const vil_image_view<T>& dxp,
    const vil_image_view<T>& dxm,
    const vil_image_view<T>& dyp,
    const vil_image_view<T>& dym)
{

  unsigned 
    ni=dxp.ni(), 
    nj=dxp.nj(), 
    np=dxp.nplanes();
    
  T xp;
  T xm;
  T yp;
  T ym;
  
  hj_flux.set_size(ni, nj, np);

  vcl_ptrdiff_t 
    istep_hj=hj_flux.istep(), 
    jstep_hj=hj_flux.jstep(),
    pstep_hj=hj_flux.planestep();

  vcl_ptrdiff_t 
    istep_dxp=dxp.istep(), 
    jstep_dxp=dxp.jstep(),
    pstep_dxp=dxp.planestep();

  vcl_ptrdiff_t 
    istep_dxm=dxm.istep(), 
    jstep_dxm=dxm.jstep(),
    pstep_dxm=dxm.planestep();

  vcl_ptrdiff_t 
    istep_dyp=dyp.istep(), 
    jstep_dyp=dyp.jstep(),
    pstep_dyp=dyp.planestep();

  
  vcl_ptrdiff_t 
    istep_dym=dym.istep(), 
    jstep_dym=dym.jstep(),
    pstep_dym=dym.planestep();

  float* plane_hj = hj_flux.top_left_ptr();
  const float* plane_dxp = dxp.top_left_ptr();
  const float* plane_dxm = dxm.top_left_ptr();
  const float* plane_dyp = dyp.top_left_ptr();
  const float* plane_dym = dym.top_left_ptr();
  
  
  for (unsigned p=0; p<np; ++p, 
    plane_hj += pstep_hj,
    plane_dxp += pstep_dxp,
    plane_dxm += pstep_dxm,
    plane_dyp += pstep_dyp,
    plane_dym += pstep_dym)
  {
    float* row_hj = plane_hj;
    const float* row_dxp = plane_dxp;
    const float* row_dxm = plane_dxm;
    const float* row_dyp = plane_dyp;
    const float* row_dym = plane_dym;

    for (unsigned j=0; j<nj; ++j,
      row_hj += jstep_hj,
      row_dxp += jstep_dxp,
      row_dxm += jstep_dxm,
      row_dyp += jstep_dyp,
      row_dym += jstep_dym)
    {
      float* pixel_hj = row_hj;
      const float* pixel_dxp = row_dxp;
      const float* pixel_dxm = row_dxm;
      const float* pixel_dyp = row_dyp;
      const float* pixel_dym = row_dym;

      for (unsigned i=0; i<ni; ++i,
        pixel_hj += istep_hj,
        pixel_dxp += istep_dxp,
        pixel_dxm += istep_dxm,
        pixel_dyp += istep_dyp,
        pixel_dym += istep_dym)
      {
        xp = (direction* (*pixel_dxp)) > 0 ? 0 : (*pixel_dxp);
        xm = (direction* (*pixel_dxm)) < 0 ? 0 : (*pixel_dxm);
        yp = (direction* (*pixel_dyp)) > 0 ? 0 : (*pixel_dyp);
        ym = (direction* (*pixel_dym)) < 0 ? 0 : (*pixel_dym);
        *pixel_hj = vcl_sqrt(xp*xp + xm*xm + yp*yp + ym*ym);
      }
    }
  }
} 


#endif // dbacm_levelset_math_h_

