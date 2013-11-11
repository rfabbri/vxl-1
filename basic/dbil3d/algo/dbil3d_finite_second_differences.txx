
#ifndef dbil3d_finite_second_differences_txx_
#define dbil3d_finite_second_differences_txx_

#include "dbil3d_finite_second_differences.h"
#include <vcl_iostream.h>

template <class T>
void dbil3d_finite_second_differences(const vil3d_image_view<T>& src,
                    const vil3d_image_view<T>& dxp,
                    const vil3d_image_view<T>& dxm,
                    const vil3d_image_view<T>& dyp,
                    const vil3d_image_view<T>& dym,
                    const vil3d_image_view<T>& dzp,
                    const vil3d_image_view<T>& dzm,
                    vil3d_image_view<T>& dxx,
                    vil3d_image_view<T>& dyy,
                    vil3d_image_view<T>& dzz,
                    vil3d_image_view<T>& dxy,
                    vil3d_image_view<T>& dxz,
                    vil3d_image_view<T>& dyz)
{
  unsigned ni = src.ni(),nj = src.nj(),nk = src.nk(),np = src.nplanes();
  vcl_ptrdiff_t istep=src.istep(),jstep=src.jstep(),kstep=src.kstep(),pstep = src.planestep();

  dxx.set_size(ni,nj,nk,np);
  dyy.set_size(ni,nj,nk,np);
  dzz.set_size(ni,nj,nk,np);
  dxy.set_size(ni,nj,nk,np);
  dxz.set_size(ni,nj,nk,np);
  dyz.set_size(ni,nj,nk,np);

  const T* plane        = src.origin_ptr();
  const T* dxp_plane   = dxp.origin_ptr();
  const T* dxm_plane   = dxm.origin_ptr();
  const T* dyp_plane   = dyp.origin_ptr();
  const T* dym_plane   = dym.origin_ptr();
  const T* dzp_plane   = dzp.origin_ptr();
  const T* dzm_plane   = dzm.origin_ptr();
  T* dxx_plane           = dxx.origin_ptr();
  T* dyy_plane           = dyy.origin_ptr();
  T* dzz_plane           = dzz.origin_ptr();
  T* dxy_plane           = dxy.origin_ptr();
  T* dxz_plane           = dxz.origin_ptr();
  T* dyz_plane           = dyz.origin_ptr();


  //calculate for voxels in central region
  //(no need for out-of-bounds checking)
  //------------------------------------------------------------------
  for (unsigned p=0;p<np;++p,plane += pstep,
                dxp_plane+=pstep, dxm_plane+=pstep, 
                dyp_plane+=pstep, dym_plane+=pstep, 
                dzp_plane+=pstep, dzm_plane+=pstep,
                dxx_plane+=pstep, dyy_plane+=pstep, dzz_plane+=pstep,
                dxy_plane+=pstep, dxz_plane+=pstep, dyz_plane+=pstep ) 
  {
    const T* slice = plane+kstep;
    const T* dxp_slice = dxp_plane+kstep;
    const T* dxm_slice = dxm_plane+kstep;
    const T* dyp_slice = dyp_plane+kstep;
    const T* dym_slice = dym_plane+kstep;
    const T* dzp_slice = dzp_plane+kstep;
    const T* dzm_slice = dzm_plane+kstep;
    T* dxx_slice       = dxx_plane+kstep;
    T* dyy_slice       = dyy_plane+kstep;
    T* dzz_slice       = dzz_plane+kstep;
    T* dxy_slice       = dxy_plane+kstep;
    T* dxz_slice       = dxz_plane+kstep;
    T* dyz_slice       = dyz_plane+kstep;
  for (unsigned k=1;k<nk-1;++k,slice += kstep,
                dxp_slice+=kstep, dxm_slice+=kstep, 
                dyp_slice+=kstep, dym_slice+=kstep, 
                dzp_slice+=kstep, dzm_slice+=kstep,
                dxx_slice+=kstep, dyy_slice+=kstep, dzz_slice+=kstep,
                dxy_slice+=kstep, dxz_slice+=kstep, dyz_slice+=kstep ) 
  {
    const T* row = slice+jstep;
    const T* dxp_row = dxp_slice+jstep;
    const T* dxm_row = dxm_slice+jstep;
    const T* dyp_row = dyp_slice+jstep;
    const T* dym_row = dym_slice+jstep;
    const T* dzp_row = dzp_slice+jstep;
    const T* dzm_row = dzm_slice+jstep;
    T* dxx_row       = dxx_slice+jstep;
    T* dyy_row       = dyy_slice+jstep;
    T* dzz_row       = dzz_slice+jstep;
    T* dxy_row       = dxy_slice+jstep;
    T* dxz_row       = dxz_slice+jstep;
    T* dyz_row       = dyz_slice+jstep;
  for (unsigned j=1;j<nj-1;++j,row += jstep,
                dxp_row+=jstep, dxm_row+=jstep, 
                dyp_row+=jstep, dym_row+=jstep, 
                dzp_row+=jstep, dzm_row+=jstep,
                dxx_row+=jstep, dyy_row+=jstep, dzz_row+=jstep,
                dxy_row+=jstep, dxz_row+=jstep, dyz_row+=jstep ) 
  {
    const T* pixel = row+istep;
    const T* dxp_pixel = dxp_row+istep;
    const T* dxm_pixel = dxm_row+istep;
    const T* dyp_pixel = dyp_row+istep;
    const T* dym_pixel = dym_row+istep;
    const T* dzp_pixel = dzp_row+istep;
    const T* dzm_pixel = dzm_row+istep;
    T* dxx_pixel       = dxx_row+istep;
    T* dyy_pixel       = dyy_row+istep;
    T* dzz_pixel       = dzz_row+istep;
    T* dxy_pixel       = dxy_row+istep;
    T* dxz_pixel       = dxz_row+istep;
    T* dyz_pixel       = dyz_row+istep;
  for (unsigned i=1;i<ni-1;++i,pixel += istep,
                dxp_pixel+=istep, dxm_pixel+=istep, 
                dyp_pixel+=istep, dym_pixel+=istep, 
                dzp_pixel+=istep, dzm_pixel+=istep,
                dxx_pixel+=istep, dyy_pixel+=istep, dzz_pixel+=istep,
                dxy_pixel+=istep, dxz_pixel+=istep, dyz_pixel+=istep ) 
  {
              *dxx_pixel = *dxp_pixel - *dxm_pixel;
              *dyy_pixel = *dyp_pixel - *dym_pixel;
              *dzz_pixel = *dzp_pixel - *dzm_pixel;
            *dxy_pixel =( *(pixel+istep+jstep) + *(pixel-istep-jstep)
                         -*(pixel-istep+jstep) - *(pixel+istep-jstep))/4.;
            *dxz_pixel =( *(pixel+istep+kstep) + *(pixel-istep-kstep)
                         -*(pixel-istep+kstep) - *(pixel+istep-kstep))/4.;
            *dyz_pixel =( *(pixel+jstep+kstep) + *(pixel-jstep-kstep)
                         -*(pixel-jstep+kstep) - *(pixel+jstep-kstep))/4.;
  }//end loop over i 
  }//end loop over j
  }//end loop over k
  }//end loop over p 

  //------------------------------------------------------------------
  
  
  vcl_ptrdiff_t safe_istep_plus  =  istep;
  vcl_ptrdiff_t safe_istep_minus = -istep;
  vcl_ptrdiff_t safe_jstep_plus  =  jstep;
  vcl_ptrdiff_t safe_jstep_minus = -jstep;
  vcl_ptrdiff_t safe_kstep_plus  =  kstep;
  vcl_ptrdiff_t safe_kstep_minus = -kstep;

  //handle boundary cases for i = 0 and i = ni-1 
  //------------------------------------------------------------------
  plane = src.origin_ptr();
  dxp_plane = dxp.origin_ptr();
  dxm_plane = dxm.origin_ptr();
  dyp_plane = dyp.origin_ptr();
  dym_plane = dym.origin_ptr();
  dzp_plane = dzp.origin_ptr();
  dzm_plane = dzm.origin_ptr();
  dxx_plane = dxx.origin_ptr();
  dyy_plane = dyy.origin_ptr();
  dzz_plane = dzz.origin_ptr();
  dxy_plane = dxy.origin_ptr();
  dxz_plane = dxz.origin_ptr();
  dyz_plane = dyz.origin_ptr();
 for (unsigned p=0;p<np;++p,plane += pstep,
                dxp_plane+=pstep, dxm_plane+=pstep, 
                dyp_plane+=pstep, dym_plane+=pstep, 
                dzp_plane+=pstep, dzm_plane+=pstep,
                dxx_plane+=pstep, dyy_plane+=pstep, dzz_plane+=pstep,
                dxy_plane+=pstep, dxz_plane+=pstep, dyz_plane+=pstep ) 
  {
    const T* slice = plane;
    const T* dxp_slice = dxp_plane;
    const T* dxm_slice = dxm_plane;
    const T* dyp_slice = dyp_plane;
    const T* dym_slice = dym_plane;
    const T* dzp_slice = dzp_plane;
    const T* dzm_slice = dzm_plane;
    T* dxx_slice       = dxx_plane;
    T* dyy_slice       = dyy_plane;
    T* dzz_slice       = dzz_plane;
    T* dxy_slice       = dxy_plane;
    T* dxz_slice       = dxz_plane;
    T* dyz_slice       = dyz_plane;
 for (unsigned k=0;k<nk;++k,slice += kstep,
                dxp_slice+=kstep, dxm_slice+=kstep, 
                dyp_slice+=kstep, dym_slice+=kstep, 
                dzp_slice+=kstep, dzm_slice+=kstep,
                dxx_slice+=kstep, dyy_slice+=kstep, dzz_slice+=kstep,
                dxy_slice+=kstep, dxz_slice+=kstep, dyz_slice+=kstep ) 
  {
   const T* row = slice;
    const T* dxp_row = dxp_slice;
    const T* dxm_row = dxm_slice;
    const T* dyp_row = dyp_slice;
    const T* dym_row = dym_slice;
    const T* dzp_row = dzp_slice;
    const T* dzm_row = dzm_slice;
    T* dxx_row       = dxx_slice;
    T* dyy_row       = dyy_slice;
    T* dzz_row       = dzz_slice;
    T* dxy_row       = dxy_slice;
    T* dxz_row       = dxz_slice;
    T* dyz_row       = dyz_slice;

    safe_kstep_plus  = (k+1 < nk ?  kstep: 0);
    safe_kstep_minus = (k   > 0  ? -kstep: 0);
 for (unsigned j=0;j<nj;++j,row += jstep,
                dxp_row+=jstep, dxm_row+=jstep, 
                dyp_row+=jstep, dym_row+=jstep, 
                dzp_row+=jstep, dzm_row+=jstep,
                dxx_row+=jstep, dyy_row+=jstep, dzz_row+=jstep,
                dxy_row+=jstep, dxz_row+=jstep, dyz_row+=jstep ) 
  {
    const T* pixel = row;
    const T* dxp_pixel = dxp_row;
    const T* dxm_pixel = dxm_row;
    const T* dyp_pixel = dyp_row;
    const T* dym_pixel = dym_row;
    const T* dzp_pixel = dzp_row;
    const T* dzm_pixel = dzm_row;
    T* dxx_pixel       = dxx_row;
    T* dyy_pixel       = dyy_row;
    T* dzz_pixel       = dzz_row;
    T* dxy_pixel       = dxy_row;
    T* dxz_pixel       = dxz_row;
    T* dyz_pixel       = dyz_row;

    safe_jstep_plus  = (j+1 < nj ?  jstep: 0);
    safe_jstep_minus = (j   > 0  ? -jstep: 0);

    //i == 0 plane
    *dxx_pixel = *dxp_pixel - *dxm_pixel;
    *dyy_pixel = *dyp_pixel - *dym_pixel;
    *dzz_pixel = *dzp_pixel - *dzm_pixel;

    //don't reference pixel-istep, it doesn't exist
    safe_istep_plus  = istep;
    safe_istep_minus = 0;
    *dxy_pixel =( *(pixel + safe_istep_plus  + safe_jstep_plus)  
                 +*(pixel + safe_istep_minus + safe_jstep_minus)
                   -*(pixel + safe_istep_minus + safe_jstep_plus)  
                 -*(pixel + safe_istep_plus  + safe_jstep_minus))/4.;
    *dxz_pixel =( *(pixel + safe_istep_plus  + safe_kstep_plus)  
                 +*(pixel + safe_istep_minus + safe_kstep_minus)
                   -*(pixel + safe_istep_minus + safe_kstep_plus)  
                 -*(pixel + safe_istep_plus  + safe_kstep_minus))/4.;
    *dyz_pixel =( *(pixel + safe_jstep_plus  + safe_kstep_plus)  
                 +*(pixel + safe_jstep_minus + safe_kstep_minus)
                   -*(pixel + safe_jstep_minus + safe_kstep_plus)  
                 -*(pixel + safe_jstep_plus  + safe_kstep_minus))/4.;

    //i == ni-1 plane
    pixel     += (ni-1)*istep;
    dxp_pixel += (ni-1)*istep;
    dxm_pixel += (ni-1)*istep;
    dyp_pixel += (ni-1)*istep;
    dym_pixel += (ni-1)*istep;
    dzp_pixel += (ni-1)*istep;
    dzm_pixel += (ni-1)*istep;
    dxx_pixel += (ni-1)*istep;
    dyy_pixel += (ni-1)*istep;
    dzz_pixel += (ni-1)*istep;
    dxy_pixel += (ni-1)*istep;
    dxz_pixel += (ni-1)*istep;
    dyz_pixel += (ni-1)*istep;


    *dxx_pixel = *dxp_pixel - *dxm_pixel;
    *dyy_pixel = *dyp_pixel - *dym_pixel;
    *dzz_pixel = *dzp_pixel - *dzm_pixel;

    //don't reference pixel+istep, it doesn't exist
    safe_istep_plus  = 0;
    safe_istep_minus = -istep;

    *dxy_pixel =( *(pixel + safe_istep_plus  + safe_jstep_plus)  
                 +*(pixel + safe_istep_minus + safe_jstep_minus)
                   -*(pixel + safe_istep_minus + safe_jstep_plus)  
                 -*(pixel + safe_istep_plus  + safe_jstep_minus))/4.;
    *dxz_pixel =( *(pixel + safe_istep_plus  + safe_kstep_plus)  
                 +*(pixel + safe_istep_minus + safe_kstep_minus)
                   -*(pixel + safe_istep_minus + safe_kstep_plus)  
                 -*(pixel + safe_istep_plus  + safe_kstep_minus))/4.;
    *dyz_pixel =( *(pixel + safe_jstep_plus  + safe_kstep_plus)  
                 +*(pixel + safe_jstep_minus + safe_kstep_minus)
                   -*(pixel + safe_jstep_minus + safe_kstep_plus)  
                 -*(pixel + safe_jstep_plus  + safe_kstep_minus))/4.;
        }//end loop over j
      }//end loop over k
    }//end loop over p

 safe_istep_plus  =  istep;
 safe_istep_minus = -istep;

  //handle boundary cases for j = 0 and j = nj-1 
  //------------------------------------------------------
  plane = src.origin_ptr();
  dxp_plane = dxp.origin_ptr();
  dxm_plane = dxm.origin_ptr();
  dyp_plane = dyp.origin_ptr();
  dym_plane = dym.origin_ptr();
  dzp_plane = dzp.origin_ptr();
  dzm_plane = dzm.origin_ptr();
  dxx_plane = dxx.origin_ptr();
  dyy_plane = dyy.origin_ptr();
  dzz_plane = dzz.origin_ptr();
  dxy_plane = dxy.origin_ptr();
  dxz_plane = dxz.origin_ptr();
  dyz_plane = dyz.origin_ptr();
 for (unsigned p=0;p<np;++p,plane += pstep,
                dxp_plane+=pstep, dxm_plane+=pstep, 
                dyp_plane+=pstep, dym_plane+=pstep, 
                dzp_plane+=pstep, dzm_plane+=pstep,
                dxx_plane+=pstep, dyy_plane+=pstep, dzz_plane+=pstep,
                dxy_plane+=pstep, dxz_plane+=pstep, dyz_plane+=pstep ) 
  {
    const T* slice = plane;
    const T* dxp_slice = dxp_plane;
    const T* dxm_slice = dxm_plane;
    const T* dyp_slice = dyp_plane;
    const T* dym_slice = dym_plane;
    const T* dzp_slice = dzp_plane;
    const T* dzm_slice = dzm_plane;
    T* dxx_slice       = dxx_plane;
    T* dyy_slice       = dyy_plane;
    T* dzz_slice       = dzz_plane;
    T* dxy_slice       = dxy_plane;
    T* dxz_slice       = dxz_plane;
    T* dyz_slice       = dyz_plane;
 for (unsigned k=0;k<nk;++k,slice += kstep,
                dxp_slice+=kstep, dxm_slice+=kstep, 
                dyp_slice+=kstep, dym_slice+=kstep, 
                dzp_slice+=kstep, dzm_slice+=kstep,
                dxx_slice+=kstep, dyy_slice+=kstep, dzz_slice+=kstep,
                dxy_slice+=kstep, dxz_slice+=kstep, dyz_slice+=kstep ) 
  {
    const T* pixel     = slice+istep;
    const T* dxp_pixel = dxp_slice+istep;
    const T* dxm_pixel = dxm_slice+istep;
    const T* dyp_pixel = dyp_slice+istep;
    const T* dym_pixel = dym_slice+istep;
    const T* dzp_pixel = dzp_slice+istep;
    const T* dzm_pixel = dzm_slice+istep;
    T* dxx_pixel       = dxx_slice+istep;
    T* dyy_pixel       = dyy_slice+istep;
    T* dzz_pixel       = dzz_slice+istep;
    T* dxy_pixel       = dxy_slice+istep;
    T* dxz_pixel       = dxz_slice+istep;
    T* dyz_pixel       = dyz_slice+istep;

    safe_kstep_plus  = (k+1 < nk ?  kstep: 0);
    safe_kstep_minus = (k   > 0  ? -kstep: 0);
    //j == 0 plane
    //don't reference pixel-jstep, it doesn't exist
    safe_jstep_plus  = jstep;
    safe_jstep_minus = 0;

    for (unsigned i=1;i<ni-1;++i,pixel += istep,
                    dxp_pixel+=istep, dxm_pixel+=istep, 
                    dyp_pixel+=istep, dym_pixel+=istep, 
                    dzp_pixel+=istep, dzm_pixel+=istep,
                    dxx_pixel+=istep, dyy_pixel+=istep, dzz_pixel+=istep,
                    dxy_pixel+=istep, dxz_pixel+=istep, dyz_pixel+=istep ) 
    {

              *dxx_pixel = *dxp_pixel - *dxm_pixel;
              *dyy_pixel = *dyp_pixel - *dym_pixel;
              *dzz_pixel = *dzp_pixel - *dzm_pixel;

            *dxy_pixel =( *(pixel + safe_istep_plus  + safe_jstep_plus)  
                         +*(pixel + safe_istep_minus + safe_jstep_minus)
                           -*(pixel + safe_istep_minus + safe_jstep_plus)  
                         -*(pixel + safe_istep_plus  + safe_jstep_minus))/4.;
            *dxz_pixel =( *(pixel + safe_istep_plus  + safe_kstep_plus)  
                         +*(pixel + safe_istep_minus + safe_kstep_minus)
                           -*(pixel + safe_istep_minus + safe_kstep_plus)  
                         -*(pixel + safe_istep_plus  + safe_kstep_minus))/4.;
            *dyz_pixel =( *(pixel + safe_jstep_plus  + safe_kstep_plus)  
                         +*(pixel + safe_jstep_minus + safe_kstep_minus)
                           -*(pixel + safe_jstep_minus + safe_kstep_plus)  
                         -*(pixel + safe_jstep_plus  + safe_kstep_minus))/4.;
    }

    pixel     =     slice+istep+(nj-1)*jstep;
    dxp_pixel = dxp_slice+istep+(nj-1)*jstep;
    dxm_pixel = dxm_slice+istep+(nj-1)*jstep;
    dyp_pixel = dyp_slice+istep+(nj-1)*jstep;
    dym_pixel = dym_slice+istep+(nj-1)*jstep;
    dzp_pixel = dzp_slice+istep+(nj-1)*jstep;
    dzm_pixel = dzm_slice+istep+(nj-1)*jstep;
    dxx_pixel = dxx_slice+istep+(nj-1)*jstep;
    dyy_pixel = dyy_slice+istep+(nj-1)*jstep;
    dzz_pixel = dzz_slice+istep+(nj-1)*jstep;
    dxy_pixel = dxy_slice+istep+(nj-1)*jstep;
    dxz_pixel = dxz_slice+istep+(nj-1)*jstep;
    dyz_pixel = dyz_slice+istep+(nj-1)*jstep;

    //j == nj-1 plane
    //don't reference pixel+jstep, it doesn't exist
    safe_jstep_plus  = 0;
    safe_jstep_minus = -jstep;

    for (unsigned i=1;i<ni-1;++i,pixel += istep,
                    dxp_pixel+=istep, dxm_pixel+=istep, 
                    dyp_pixel+=istep, dym_pixel+=istep, 
                    dzp_pixel+=istep, dzm_pixel+=istep,
                    dxx_pixel+=istep, dyy_pixel+=istep, dzz_pixel+=istep,
                    dxy_pixel+=istep, dxz_pixel+=istep, dyz_pixel+=istep ) 
    {

              *dxx_pixel = *dxp_pixel - *dxm_pixel;
              *dyy_pixel = *dyp_pixel - *dym_pixel;
              *dzz_pixel = *dzp_pixel - *dzm_pixel;

            *dxy_pixel =( *(pixel + safe_istep_plus  + safe_jstep_plus)  
                         +*(pixel + safe_istep_minus + safe_jstep_minus)
                           -*(pixel + safe_istep_minus + safe_jstep_plus)  
                         -*(pixel + safe_istep_plus  + safe_jstep_minus))/4.;
            *dxz_pixel =( *(pixel + safe_istep_plus  + safe_kstep_plus)  
                         +*(pixel + safe_istep_minus + safe_kstep_minus)
                           -*(pixel + safe_istep_minus + safe_kstep_plus)  
                         -*(pixel + safe_istep_plus  + safe_kstep_minus))/4.;
            *dyz_pixel =( *(pixel + safe_jstep_plus  + safe_kstep_plus)  
                         +*(pixel + safe_jstep_minus + safe_kstep_minus)
                           -*(pixel + safe_jstep_minus + safe_kstep_plus)  
                         -*(pixel + safe_jstep_plus  + safe_kstep_minus))/4.;
    }
  }//end loop over k
  //------------------------------------------------------


  //handle boundary cases for k = 0 and k = nk-1 
  //------------------------------------------------------
  plane = src.origin_ptr();
  dxp_plane = dxp.origin_ptr();
  dxm_plane = dxm.origin_ptr();
  dyp_plane = dyp.origin_ptr();
  dym_plane = dym.origin_ptr();
  dzp_plane = dzp.origin_ptr();
  dzm_plane = dzm.origin_ptr();
  dxx_plane = dxx.origin_ptr();
  dyy_plane = dyy.origin_ptr();
  dzz_plane = dzz.origin_ptr();
  dxy_plane = dxy.origin_ptr();
  dxz_plane = dxz.origin_ptr();
  dyz_plane = dyz.origin_ptr();

  safe_istep_plus  =  istep;
  safe_istep_minus =  -istep;
  safe_jstep_plus  =  jstep;
  safe_jstep_minus =  -jstep;

 for (unsigned p=0;p<np;++p,plane += pstep,
                dxp_plane+=pstep, dxm_plane+=pstep, 
                dyp_plane+=pstep, dym_plane+=pstep, 
                dzp_plane+=pstep, dzm_plane+=pstep,
                dxx_plane+=pstep, dyy_plane+=pstep, dzz_plane+=pstep,
                dxy_plane+=pstep, dxz_plane+=pstep, dyz_plane+=pstep ) 
  {
    //k == 0 plane
    //don't reference pixel-kstep, it doesn't exist
    safe_kstep_plus  = kstep;
    safe_kstep_minus = 0;

    const T* row = plane+jstep;
    const T* dxp_row = dxp_plane+jstep;
    const T* dxm_row = dxm_plane+jstep;
    const T* dyp_row = dyp_plane+jstep;
    const T* dym_row = dym_plane+jstep;
    const T* dzp_row = dzp_plane+jstep;
    const T* dzm_row = dzm_plane+jstep;
    T* dxx_row       = dxx_plane+jstep;
    T* dyy_row       = dyy_plane+jstep;
    T* dzz_row       = dzz_plane+jstep;
    T* dxy_row       = dxy_plane+jstep;
    T* dxz_row       = dxz_plane+jstep;
    T* dyz_row       = dyz_plane+jstep;
    for (unsigned j=1;j<nj-1;++j,row += jstep,
                dxp_row+=jstep, dxm_row+=jstep, 
                dyp_row+=jstep, dym_row+=jstep, 
                dzp_row+=jstep, dzm_row+=jstep,
                dxx_row+=jstep, dyy_row+=jstep, dzz_row+=jstep,
                dxy_row+=jstep, dxz_row+=jstep, dyz_row+=jstep ) 
    {
            const T* pixel = row+istep;
            const T* dxp_pixel = dxp_row+istep;
            const T* dxm_pixel = dxm_row+istep;
            const T* dyp_pixel = dyp_row+istep;
            const T* dym_pixel = dym_row+istep;
            const T* dzp_pixel = dzp_row+istep;
            const T* dzm_pixel = dzm_row+istep;
            T* dxx_pixel       = dxx_row+istep;
            T* dyy_pixel       = dyy_row+istep;
            T* dzz_pixel       = dzz_row+istep;
            T* dxy_pixel       = dxy_row+istep;
            T* dxz_pixel       = dxz_row+istep;
            T* dyz_pixel       = dyz_row+istep;
    for (unsigned i=1;i<ni-1;++i,pixel += istep,
                    dxp_pixel+=istep, dxm_pixel+=istep, 
                    dyp_pixel+=istep, dym_pixel+=istep, 
                    dzp_pixel+=istep, dzm_pixel+=istep,
                    dxx_pixel+=istep, dyy_pixel+=istep, dzz_pixel+=istep,
                    dxy_pixel+=istep, dxz_pixel+=istep, dyz_pixel+=istep ) 
    {
              *dxx_pixel = *dxp_pixel - *dxm_pixel;
              *dyy_pixel = *dyp_pixel - *dym_pixel;
              *dzz_pixel = *dzp_pixel - *dzm_pixel;

            *dxy_pixel =( *(pixel + safe_istep_plus  + safe_jstep_plus)  
                         +*(pixel + safe_istep_minus + safe_jstep_minus)
                           -*(pixel + safe_istep_minus + safe_jstep_plus)  
                         -*(pixel + safe_istep_plus  + safe_jstep_minus))/4.;
            *dxz_pixel =( *(pixel + safe_istep_plus  + safe_kstep_plus)  
                         +*(pixel + safe_istep_minus + safe_kstep_minus)
                           -*(pixel + safe_istep_minus + safe_kstep_plus)  
                         -*(pixel + safe_istep_plus  + safe_kstep_minus))/4.;
            *dyz_pixel =( *(pixel + safe_jstep_plus  + safe_kstep_plus)  
                         +*(pixel + safe_jstep_minus + safe_kstep_minus)
                           -*(pixel + safe_jstep_minus + safe_kstep_plus)  
                         -*(pixel + safe_jstep_plus  + safe_kstep_minus))/4.;
    }
    }
    //k == nk-1 plane
    //don't reference pixel+kstep, it doesn't exist
    safe_kstep_plus  = 0;
    safe_kstep_minus = -kstep;



    row     =     plane+jstep+(nk-1)*kstep;
    dxp_row = dxp_plane+jstep+(nk-1)*kstep;
    dxm_row = dxm_plane+jstep+(nk-1)*kstep;
    dyp_row = dyp_plane+jstep+(nk-1)*kstep;
    dym_row = dym_plane+jstep+(nk-1)*kstep;
    dzp_row = dzp_plane+jstep+(nk-1)*kstep;
    dzm_row = dzm_plane+jstep+(nk-1)*kstep;
    dxx_row = dxx_plane+jstep+(nk-1)*kstep;
    dyy_row = dyy_plane+jstep+(nk-1)*kstep;
    dzz_row = dzz_plane+jstep+(nk-1)*kstep;
    dxy_row = dxy_plane+jstep+(nk-1)*kstep;
    dxz_row = dxz_plane+jstep+(nk-1)*kstep;
    dyz_row = dyz_plane+jstep+(nk-1)*kstep;

    for (unsigned j=1;j<nj-1;++j,row += jstep,
                dxp_row+=jstep, dxm_row+=jstep, 
                dyp_row+=jstep, dym_row+=jstep, 
                dzp_row+=jstep, dzm_row+=jstep,
                dxx_row+=jstep, dyy_row+=jstep, dzz_row+=jstep,
                dxy_row+=jstep, dxz_row+=jstep, dyz_row+=jstep ) 
    {
            const T* pixel = row+istep;
            const T* dxp_pixel = dxp_row+istep;
            const T* dxm_pixel = dxm_row+istep;
            const T* dyp_pixel = dyp_row+istep;
            const T* dym_pixel = dym_row+istep;
            const T* dzp_pixel = dzp_row+istep;
            const T* dzm_pixel = dzm_row+istep;
            T* dxx_pixel       = dxx_row+istep;
            T* dyy_pixel       = dyy_row+istep;
            T* dzz_pixel       = dzz_row+istep;
            T* dxy_pixel       = dxy_row+istep;
            T* dxz_pixel       = dxz_row+istep;
            T* dyz_pixel       = dyz_row+istep;
    for (unsigned i=1;i<ni-1;++i,pixel += istep,
                    dxp_pixel+=istep, dxm_pixel+=istep, 
                    dyp_pixel+=istep, dym_pixel+=istep, 
                    dzp_pixel+=istep, dzm_pixel+=istep,
                    dxx_pixel+=istep, dyy_pixel+=istep, dzz_pixel+=istep,
                    dxy_pixel+=istep, dxz_pixel+=istep, dyz_pixel+=istep ) 
    {
              *dxx_pixel = *dxp_pixel - *dxm_pixel;
              *dyy_pixel = *dyp_pixel - *dym_pixel;
              *dzz_pixel = *dzp_pixel - *dzm_pixel;

            *dxy_pixel =( *(pixel + safe_istep_plus  + safe_jstep_plus)  
                         +*(pixel + safe_istep_minus + safe_jstep_minus)
                           -*(pixel + safe_istep_minus + safe_jstep_plus)  
                         -*(pixel + safe_istep_plus  + safe_jstep_minus))/4.;
            *dxz_pixel =( *(pixel + safe_istep_plus  + safe_kstep_plus)  
                         +*(pixel + safe_istep_minus + safe_kstep_minus)
                           -*(pixel + safe_istep_minus + safe_kstep_plus)  
                         -*(pixel + safe_istep_plus  + safe_kstep_minus))/4.;
            *dyz_pixel =( *(pixel + safe_jstep_plus  + safe_kstep_plus)  
                         +*(pixel + safe_jstep_minus + safe_kstep_minus)
                           -*(pixel + safe_jstep_minus + safe_kstep_plus)  
                         -*(pixel + safe_jstep_plus  + safe_kstep_minus))/4.;
    }
    }

  }//end loop over p
}
}

#undef DBIL3D_FINITE_SECOND_DIFFERENCES_INSTANTIATE
#define DBIL3D_FINITE_SECOND_DIFFERENCES_INSTANTIATE(T) \
template void dbil3d_finite_second_differences(const vil3d_image_view<T>& src,\
                    const vil3d_image_view<T>& dxp,\
                    const vil3d_image_view<T>& dxm,\
                    const vil3d_image_view<T>& dyp,\
                    const vil3d_image_view<T>& dym,\
                    const vil3d_image_view<T>& dzp,\
                    const vil3d_image_view<T>& dzm,\
                    vil3d_image_view<T>& dxx,\
                    vil3d_image_view<T>& dyy,\
                    vil3d_image_view<T>& dzz,\
                    vil3d_image_view<T>& dxy,\
                    vil3d_image_view<T>& dxz,\
                    vil3d_image_view<T>& dyz);
#endif // dbil3d_finite_second_differences_txx_
