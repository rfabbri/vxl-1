
#ifndef dbil3d_finite_differences_txx_
#define dbil3d_finite_differences_txx_

#include "dbil3d_finite_differences.h"

template <class T>
void dbil3d_finite_differences(const vil3d_image_view<T>& src,
                    vil3d_image_view<T>& dxp,
                    vil3d_image_view<T>& dxm,
                    vil3d_image_view<T>& dxc,
                    vil3d_image_view<T>& dyp,
                    vil3d_image_view<T>& dym,
                    vil3d_image_view<T>& dyc,
                    vil3d_image_view<T>& dzp,
                    vil3d_image_view<T>& dzm,
                    vil3d_image_view<T>& dzc)
{
  unsigned ni = src.ni(),nj = src.nj(),nk = src.nk(),np = src.nplanes();
  vcl_ptrdiff_t istep=src.istep(),jstep=src.jstep(),kstep=src.kstep(),pstep = src.planestep();


  dxp.set_size(ni,nj,nk,np);
  dxm.set_size(ni,nj,nk,np);
  dxc.set_size(ni,nj,nk,np);
  dyp.set_size(ni,nj,nk,np);
  dym.set_size(ni,nj,nk,np);
  dyc.set_size(ni,nj,nk,np);
  dzp.set_size(ni,nj,nk,np);
  dzm.set_size(ni,nj,nk,np);
  dzc.set_size(ni,nj,nk,np);

  //calculate for voxels in central region
  //(no need for out-of-bounds checking)
  //------------------------------------------------------------------
  const T* plane = src.origin_ptr();
  T* dxp_plane   = dxp.origin_ptr();
  T* dxm_plane   = dxm.origin_ptr();
  T* dxc_plane   = dxc.origin_ptr();
  T* dyp_plane   = dyp.origin_ptr();
  T* dym_plane   = dym.origin_ptr();
  T* dyc_plane   = dyc.origin_ptr();
  T* dzp_plane   = dzp.origin_ptr();
  T* dzm_plane   = dzm.origin_ptr();
  T* dzc_plane   = dzc.origin_ptr();

  for (unsigned p=0;p<np;++p,plane += pstep,
                dxp_plane+=pstep, dxm_plane+=pstep, dxc_plane+=pstep,
                dyp_plane+=pstep, dym_plane+=pstep, dyc_plane+=pstep,
                dzp_plane+=pstep, dzm_plane+=pstep, dzc_plane+=pstep)
  {
    const T* slice = plane+kstep;
    T* dxp_slice = dxp_plane+kstep;
    T* dxm_slice = dxm_plane+kstep;
    T* dxc_slice = dxc_plane+kstep;
    T* dyp_slice = dyp_plane+kstep;
    T* dym_slice = dym_plane+kstep;
    T* dyc_slice = dyc_plane+kstep;
    T* dzp_slice = dzp_plane+kstep;
    T* dzm_slice = dzm_plane+kstep;
    T* dzc_slice = dzc_plane+kstep; 
    for (unsigned k=1;k<nk-1;++k,slice += kstep,
                 dxp_slice+=kstep, dxm_slice+=kstep, dxc_slice+=kstep,
                    dyp_slice+=kstep, dym_slice+=kstep, dyc_slice+=kstep,
                    dzp_slice+=kstep, dzm_slice+=kstep, dzc_slice+=kstep )
    {
      const T* row = slice+jstep;
      T* dxp_row = dxp_slice+jstep;
      T* dxm_row = dxm_slice+jstep;
      T* dxc_row = dxc_slice+jstep;
      T* dyp_row = dyp_slice+jstep;
      T* dym_row = dym_slice+jstep;
      T* dyc_row = dyc_slice+jstep;
      T* dzp_row = dzp_slice+jstep;
      T* dzm_row = dzm_slice+jstep;
      T* dzc_row = dzc_slice+jstep;
      for (unsigned j=1;j<nj-1;++j,row += jstep,
                    dxp_row+=jstep, dxm_row+=jstep, dxc_row+=jstep,
                    dyp_row+=jstep, dym_row+=jstep, dyc_row+=jstep,
                    dzp_row+=jstep, dzm_row+=jstep, dzc_row+=jstep)
      {
        const T* pixel = row+istep;
        T* dxp_pixel = dxp_row+istep;
        T* dxm_pixel = dxm_row+istep;
        T* dxc_pixel = dxc_row+istep;
        T* dyp_pixel = dyp_row+istep;
        T* dym_pixel = dym_row+istep;
        T* dyc_pixel = dyc_row+istep;
        T* dzp_pixel = dzp_row+istep;
        T* dzm_pixel = dzm_row+istep;
        T* dzc_pixel = dzc_row+istep;

        for (unsigned i=1;i<ni-1;++i,pixel+=istep,
                      dxp_pixel+=istep, dxm_pixel+=istep, dxc_pixel+=istep,
                      dyp_pixel+=istep, dym_pixel+=istep, dyc_pixel+=istep,
                      dzp_pixel+=istep, dzm_pixel+=istep, dzc_pixel+=istep)
        {
                *dxp_pixel = *(pixel+istep) - *pixel;
                *dxm_pixel = *pixel - *(pixel-istep);
                *dxc_pixel = (*dxp_pixel + *dxm_pixel)/2.;

                *dyp_pixel = *(pixel+jstep) - *pixel;
                *dym_pixel = *pixel - *(pixel-jstep);
                *dyc_pixel = (*dyp_pixel + *dym_pixel)/2.;

                *dzp_pixel = *(pixel+kstep) - *pixel;
                *dzm_pixel = *pixel - *(pixel-kstep);
                *dzc_pixel = (*dzp_pixel + *dzm_pixel)/2.;
        }
      }
    }
  }
  //------------------------------------------------------------------

  //handle boundary cases for i = 0 and i = ni-1 
  //------------------------------------------------------------------
  plane     = src.origin_ptr();
  dxp_plane = dxp.origin_ptr();
  dxm_plane = dxm.origin_ptr();
  dxc_plane = dxc.origin_ptr();
  dyp_plane = dyp.origin_ptr();
  dym_plane = dym.origin_ptr();
  dyc_plane = dyc.origin_ptr();
  dzp_plane = dzp.origin_ptr();
  dzm_plane = dzm.origin_ptr();
  dzc_plane = dzc.origin_ptr();
  for (unsigned p=0;p<np;++p,plane += pstep,
                dxp_plane+=pstep, dxm_plane+=pstep, dxc_plane+=pstep,
                dyp_plane+=pstep, dym_plane+=pstep, dyc_plane+=pstep,
                dzp_plane+=pstep, dzm_plane+=pstep, dzc_plane+=pstep)
  {
    const T* slice = plane;
    T* dxp_slice = dxp_plane;
    T* dxm_slice = dxm_plane;
    T* dxc_slice = dxc_plane;
    T* dyp_slice = dyp_plane;
    T* dym_slice = dym_plane;
    T* dyc_slice = dyc_plane;
    T* dzp_slice = dzp_plane;
    T* dzm_slice = dzm_plane;
    T* dzc_slice = dzc_plane; 
    for (unsigned k=0;k<nk;++k,slice += kstep,
                 dxp_slice+=kstep, dxm_slice+=kstep, dxc_slice+=kstep,
                    dyp_slice+=kstep, dym_slice+=kstep, dyc_slice+=kstep,
                    dzp_slice+=kstep, dzm_slice+=kstep, dzc_slice+=kstep )
    {
      const T* row = slice;
      T* dxp_row = dxp_slice;
      T* dxm_row = dxm_slice;
      T* dxc_row = dxc_slice;
      T* dyp_row = dyp_slice;
      T* dym_row = dym_slice;
      T* dyc_row = dyc_slice;
      T* dzp_row = dzp_slice;
      T* dzm_row = dzm_slice;
      T* dzc_row = dzc_slice;
      for (unsigned j=0;j<nj;++j,row += jstep,
                    dxp_row+=jstep, dxm_row+=jstep, dxc_row+=jstep,
                    dyp_row+=jstep, dym_row+=jstep, dyc_row+=jstep,
                    dzp_row+=jstep, dzm_row+=jstep, dzc_row+=jstep)
      {
        const T* pixel = row;
        T* dxp_pixel = dxp_row;
        T* dxm_pixel = dxm_row;
        T* dxc_pixel = dxc_row;
        T* dyp_pixel = dyp_row;
        T* dym_pixel = dym_row;
        T* dyc_pixel = dyc_row;
        T* dzp_pixel = dzp_row;
        T* dzm_pixel = dzm_row;
        T* dzc_pixel = dzc_row;

        //i == 0 plane
        *dxp_pixel = *(pixel+istep) - *pixel;
        *dxm_pixel = 0;
        *dxc_pixel = (*dxp_pixel + *dxm_pixel)/2.;

        *dyp_pixel = j+1 < nj ? *(pixel+jstep) - *pixel : 0;
        *dym_pixel = j   > 0  ? *pixel - *(pixel-jstep) : 0 ;
        *dyc_pixel = (*dyp_pixel + *dym_pixel)/2.;

        *dzp_pixel = k+1 < nk ? *(pixel+kstep) - *pixel : 0;
        *dzm_pixel = k   > 0  ? *pixel - *(pixel-kstep) : 0;
        *dzc_pixel = (*dzp_pixel + *dzm_pixel)/2.;

        pixel     += (ni-1)*istep;
        dxp_pixel += (ni-1)*istep;
        dxm_pixel += (ni-1)*istep;
        dxc_pixel += (ni-1)*istep;
        dyp_pixel += (ni-1)*istep;
        dym_pixel += (ni-1)*istep;
        dyc_pixel += (ni-1)*istep;
        dzp_pixel += (ni-1)*istep;
        dzm_pixel += (ni-1)*istep;
        dzc_pixel += (ni-1)*istep;

        //i == ni-1 plane
        *dxp_pixel = 0;
        *dxm_pixel = *pixel - *(pixel-istep);
        *dxc_pixel = (*dxp_pixel + *dxm_pixel)/2.;

        *dyp_pixel = j+1 < nj ? *(pixel+jstep) - *pixel : 0;
        *dym_pixel = j   > 0  ? *pixel - *(pixel-jstep) : 0 ;
        *dyc_pixel = (*dyp_pixel + *dym_pixel)/2.;

        *dzp_pixel = k+1 < nk ? *(pixel+kstep) - *pixel : 0;
        *dzm_pixel = k   > 0  ? *pixel - *(pixel-kstep) : 0;
        *dzc_pixel = (*dzp_pixel + *dzm_pixel)/2.;
      }
    }
  }
  //------------------------------------------------------------------

  //handle boundary cases for j = 0 and j = ni-1 
  //------------------------------------------------------------------
  plane     = src.origin_ptr();
  dxp_plane = dxp.origin_ptr();
  dxm_plane = dxm.origin_ptr();
  dxc_plane = dxc.origin_ptr();
  dyp_plane = dyp.origin_ptr();
  dym_plane = dym.origin_ptr();
  dyc_plane = dyc.origin_ptr();
  dzp_plane = dzp.origin_ptr();
  dzm_plane = dzm.origin_ptr();
  dzc_plane = dzc.origin_ptr();

  for (unsigned p=0;p<np;++p,plane += pstep,
                dxp_plane+=pstep, dxm_plane+=pstep, dxc_plane+=pstep,
                dyp_plane+=pstep, dym_plane+=pstep, dyc_plane+=pstep,
                dzp_plane+=pstep, dzm_plane+=pstep, dzc_plane+=pstep)
  {
    const T* slice = plane;
    T* dxp_slice = dxp_plane;
    T* dxm_slice = dxm_plane;
    T* dxc_slice = dxc_plane;
    T* dyp_slice = dyp_plane;
    T* dym_slice = dym_plane;
    T* dyc_slice = dyc_plane;
    T* dzp_slice = dzp_plane;
    T* dzm_slice = dzm_plane;
    T* dzc_slice = dzc_plane; 
    for (unsigned k=0;k<nk;++k,slice += kstep,
                 dxp_slice+=kstep, dxm_slice+=kstep, dxc_slice+=kstep,
                    dyp_slice+=kstep, dym_slice+=kstep, dyc_slice+=kstep,
                    dzp_slice+=kstep, dzm_slice+=kstep, dzc_slice+=kstep )
    {
        const T* pixel = slice+istep;
        T* dxp_pixel = dxp_slice+istep;
        T* dxm_pixel = dxm_slice+istep;
        T* dxc_pixel = dxc_slice+istep;
        T* dyp_pixel = dyp_slice+istep;
        T* dym_pixel = dym_slice+istep;
        T* dyc_pixel = dyc_slice+istep;
        T* dzp_pixel = dzp_slice+istep;
        T* dzm_pixel = dzm_slice+istep;
        T* dzc_pixel = dzc_slice+istep;
        for (unsigned i=1;i<ni-1;++i,pixel+=istep,
                      dxp_pixel+=istep, dxm_pixel+=istep, dxc_pixel+=istep,
                      dyp_pixel+=istep, dym_pixel+=istep, dyc_pixel+=istep,
                      dzp_pixel+=istep, dzm_pixel+=istep, dzc_pixel+=istep)
        {
                *dxp_pixel = *(pixel+istep) - *pixel;
                *dxm_pixel = *pixel - *(pixel-istep);
                *dxc_pixel = (*dxp_pixel + *dxm_pixel)/2.;

                //j == 0 plane
                *dyp_pixel = *(pixel+jstep) - *pixel;
                *dym_pixel = 0;
                *dyc_pixel = (*dyp_pixel + *dym_pixel)/2.;

                *dzp_pixel = k+1 < nk ? *(pixel+kstep) - *pixel : 0 ;
                *dzm_pixel = k   > 0  ? *pixel - *(pixel-kstep) : 0 ;
                *dzc_pixel = (*dzp_pixel + *dzm_pixel)/2.;

        }
        pixel     = slice    +istep + (nj-1)*jstep;
        dxp_pixel = dxp_slice+istep + (nj-1)*jstep;      
        dxm_pixel = dxm_slice+istep + (nj-1)*jstep;      
        dxc_pixel = dxc_slice+istep + (nj-1)*jstep;      
        dyp_pixel = dyp_slice+istep + (nj-1)*jstep;      
        dym_pixel = dym_slice+istep + (nj-1)*jstep;      
        dyc_pixel = dyc_slice+istep + (nj-1)*jstep;      
        dzp_pixel = dzp_slice+istep + (nj-1)*jstep;      
        dzm_pixel = dzm_slice+istep + (nj-1)*jstep;      
        dzc_pixel = dzc_slice+istep + (nj-1)*jstep;      

        for (unsigned i=1;i<ni-1;++i,pixel+=istep,
                      dxp_pixel+=istep, dxm_pixel+=istep, dxc_pixel+=istep,
                      dyp_pixel+=istep, dym_pixel+=istep, dyc_pixel+=istep,
                      dzp_pixel+=istep, dzm_pixel+=istep, dzc_pixel+=istep)
        {
                *dxp_pixel = *(pixel+istep) - *pixel;
                *dxm_pixel = *pixel - *(pixel-istep);
                *dxc_pixel = (*dxp_pixel + *dxm_pixel)/2.;

                //j == nj-1 plane
                *dyp_pixel = 0;
                *dym_pixel = *pixel - *(pixel-jstep);
                *dyc_pixel = (*dyp_pixel + *dym_pixel)/2.;

                *dzp_pixel = k+1 < nk ? *(pixel+kstep) - *pixel : 0 ;
                *dzm_pixel = k   > 0  ? *pixel - *(pixel-kstep) : 0 ;
                *dzc_pixel = (*dzp_pixel + *dzm_pixel)/2.;
        }
      }
    }
 
  //------------------------------------------------------------------
  
  //handle boundary cases for k = 0 and k = ni-1 
  //------------------------------------------------------------------
  plane     = src.origin_ptr();
  dxp_plane = dxp.origin_ptr();
  dxm_plane = dxm.origin_ptr();
  dxc_plane = dxc.origin_ptr();
  dyp_plane = dyp.origin_ptr();
  dym_plane = dym.origin_ptr();
  dyc_plane = dyc.origin_ptr();
  dzp_plane = dzp.origin_ptr();
  dzm_plane = dzm.origin_ptr();
  dzc_plane = dzc.origin_ptr();
  for (unsigned p=0;p<np;++p,plane += pstep,
                dxp_plane+=pstep, dxm_plane+=pstep, dxc_plane+=pstep,
                dyp_plane+=pstep, dym_plane+=pstep, dyc_plane+=pstep,
                dzp_plane+=pstep, dzm_plane+=pstep, dzc_plane+=pstep)
  {
    const T* slice = plane;
    T* dxp_slice = dxp_plane;
    T* dxm_slice = dxm_plane;
    T* dxc_slice = dxc_plane;
    T* dyp_slice = dyp_plane;
    T* dym_slice = dym_plane;
    T* dyc_slice = dyc_plane;
    T* dzp_slice = dzp_plane;
    T* dzm_slice = dzm_plane;
    T* dzc_slice = dzc_plane; 

      const T* row = slice+jstep;
      T* dxp_row = dxp_slice+jstep;
      T* dxm_row = dxm_slice+jstep;
      T* dxc_row = dxc_slice+jstep;
      T* dyp_row = dyp_slice+jstep;
      T* dym_row = dym_slice+jstep;
      T* dyc_row = dyc_slice+jstep;
      T* dzp_row = dzp_slice+jstep;
      T* dzm_row = dzm_slice+jstep;
      T* dzc_row = dzc_slice+jstep;
      for (unsigned j=1;j<nj-1;++j,row += jstep,
                    dxp_row+=jstep, dxm_row+=jstep, dxc_row+=jstep,
                    dyp_row+=jstep, dym_row+=jstep, dyc_row+=jstep,
                    dzp_row+=jstep, dzm_row+=jstep, dzc_row+=jstep)
      {
        const T* pixel = row+istep;
        T* dxp_pixel = dxp_row+istep;
        T* dxm_pixel = dxm_row+istep;
        T* dxc_pixel = dxc_row+istep;
        T* dyp_pixel = dyp_row+istep;
        T* dym_pixel = dym_row+istep;
        T* dyc_pixel = dyc_row+istep;
        T* dzp_pixel = dzp_row+istep;
        T* dzm_pixel = dzm_row+istep;
        T* dzc_pixel = dzc_row+istep;

        for (unsigned i=1;i<ni-1;++i,pixel+=istep,
                      dxp_pixel+=istep, dxm_pixel+=istep, dxc_pixel+=istep,
                      dyp_pixel+=istep, dym_pixel+=istep, dyc_pixel+=istep,
                      dzp_pixel+=istep, dzm_pixel+=istep, dzc_pixel+=istep)
        {
                *dxp_pixel = *(pixel+istep) - *pixel;
                *dxm_pixel = *pixel - *(pixel-istep);
                *dxc_pixel = (*dxp_pixel + *dxm_pixel)/2.;

                *dyp_pixel = *(pixel+jstep) - *pixel;
                *dym_pixel = *pixel - *(pixel-jstep);
                *dyc_pixel = (*dyp_pixel + *dym_pixel)/2.;

                //k == 0 plane
                *dzp_pixel = *(pixel+kstep) - *pixel;
                *dzm_pixel = 0;
                *dzc_pixel = (*dzp_pixel + *dzm_pixel)/2.;
        }
      } 
      row =     slice        +    jstep    +    (nk-1)*kstep;
      dxp_row = dxp_slice    +    jstep    +    (nk-1)*kstep;
      dxm_row = dxm_slice    +    jstep    +    (nk-1)*kstep;
      dxc_row = dxc_slice    +    jstep    +    (nk-1)*kstep;
      dyp_row = dyp_slice    +    jstep    +    (nk-1)*kstep;
      dym_row = dym_slice    +    jstep    +    (nk-1)*kstep;
      dyc_row = dyc_slice    +    jstep    +    (nk-1)*kstep;
      dzp_row = dzp_slice    +    jstep    +    (nk-1)*kstep;
      dzm_row = dzm_slice    +    jstep    +    (nk-1)*kstep;
      dzc_row = dzc_slice    +    jstep    +    (nk-1)*kstep;


      for (unsigned j=1;j<nj-1;++j,row += jstep,
                    dxp_row+=jstep, dxm_row+=jstep, dxc_row+=jstep,
                    dyp_row+=jstep, dym_row+=jstep, dyc_row+=jstep,
                    dzp_row+=jstep, dzm_row+=jstep, dzc_row+=jstep)
      {
        const T* pixel = row+istep;
        T* dxp_pixel = dxp_row+istep;
        T* dxm_pixel = dxm_row+istep;
        T* dxc_pixel = dxc_row+istep;
        T* dyp_pixel = dyp_row+istep;
        T* dym_pixel = dym_row+istep;
        T* dyc_pixel = dyc_row+istep;
        T* dzp_pixel = dzp_row+istep;
        T* dzm_pixel = dzm_row+istep;
        T* dzc_pixel = dzc_row+istep;

        for (unsigned i=1;i<ni-1;++i,pixel+=istep,
                      dxp_pixel+=istep, dxm_pixel+=istep, dxc_pixel+=istep,
                      dyp_pixel+=istep, dym_pixel+=istep, dyc_pixel+=istep,
                      dzp_pixel+=istep, dzm_pixel+=istep, dzc_pixel+=istep)
        {
                *dxp_pixel = *(pixel+istep) - *pixel;
                *dxm_pixel = *pixel - *(pixel-istep);
                *dxc_pixel = (*dxp_pixel + *dxm_pixel)/2.;

                *dyp_pixel = *(pixel+jstep) - *pixel;
                *dym_pixel = *pixel - *(pixel-jstep);
                *dyc_pixel = (*dyp_pixel + *dym_pixel)/2.;

                //k == nk-1 plane
                *dzp_pixel = 0;
                *dzm_pixel = *pixel - *(pixel-kstep);
                *dzc_pixel = (*dzp_pixel + *dzm_pixel)/2.;
        }
      } 
  }
}


#undef DBIL3D_FINITE_DIFFERENCES_INSTANTIATE
#define DBIL3D_FINITE_DIFFERENCES_INSTANTIATE(T) \
template void dbil3d_finite_differences(const vil3d_image_view<T>& src_im,\
            vil3d_image_view<T>& dxp,\
            vil3d_image_view<T>& dxm,\
            vil3d_image_view<T>& dxc,\
            vil3d_image_view<T>& dyp,\
            vil3d_image_view<T>& dym,\
            vil3d_image_view<T>& dyc,\
            vil3d_image_view<T>& dzp,\
            vil3d_image_view<T>& dzm,\
            vil3d_image_view<T>& dzc);
#endif // dbil3d_finite_differences_txx_


/*
* a more concise but slightly slower version of the above
  for (unsigned p=0;p<np;++p,plane += pstep,
                dxp_plane+=pstep, dxm_plane+=pstep, dxc_plane+=pstep,
                dyp_plane+=pstep, dym_plane+=pstep, dyc_plane+=pstep,
                dzp_plane+=pstep, dzm_plane+=pstep, dzc_plane+=pstep)
  {
    const T* slice = plane;
    T* dxp_slice = dxp_plane;
    T* dxm_slice = dxm_plane;
    T* dxc_slice = dxc_plane;
    T* dyp_slice = dyp_plane;
    T* dym_slice = dym_plane;
    T* dyc_slice = dyc_plane;
    T* dzp_slice = dzp_plane;
    T* dzm_slice = dzm_plane;
    T* dzc_slice = dzc_plane; 
    for (unsigned k=0;k<nk;++k,slice += kstep,
                 dxp_slice+=kstep, dxm_slice+=kstep, dxc_slice+=kstep,
                    dyp_slice+=kstep, dym_slice+=kstep, dyc_slice+=kstep,
                    dzp_slice+=kstep, dzm_slice+=kstep, dzc_slice+=kstep )
    {
      const T* row = slice;
      T* dxp_row = dxp_slice;
      T* dxm_row = dxm_slice;
      T* dxc_row = dxc_slice;
      T* dyp_row = dyp_slice;
      T* dym_row = dym_slice;
      T* dyc_row = dyc_slice;
      T* dzp_row = dzp_slice;
      T* dzm_row = dzm_slice;
      T* dzc_row = dzc_slice;
      for (unsigned j=0;j<nj;++j,row += jstep,
                    dxp_row+=jstep, dxm_row+=jstep, dxc_row+=jstep,
                    dyp_row+=jstep, dym_row+=jstep, dyc_row+=jstep,
                    dzp_row+=jstep, dzm_row+=jstep, dzc_row+=jstep)
      {
        const T* pixel = row;
        T* dxp_pixel = dxp_row;
        T* dxm_pixel = dxm_row;
        T* dxc_pixel = dxc_row;
        T* dyp_pixel = dyp_row;
        T* dym_pixel = dym_row;
        T* dyc_pixel = dyc_row;
        T* dzp_pixel = dzp_row;
        T* dzm_pixel = dzm_row;
        T* dzc_pixel = dzc_row;

        for (unsigned i=0;i<ni;++i,pixel+=istep,
                      dxp_pixel+=istep, dxm_pixel+=istep, dxc_pixel+=istep,
                      dyp_pixel+=istep, dym_pixel+=istep, dyc_pixel+=istep,
                      dzp_pixel+=istep, dzm_pixel+=istep, dzc_pixel+=istep)
        {
                *dxp_pixel = i+1<ni ? *(pixel+istep) - *pixel : 0 ;
                *dxm_pixel = i > 0 ? *pixel - *(pixel-istep) : 0;
                *dxc_pixel = (*dxp_pixel + *dxm_pixel)/2.;

                *dyp_pixel = j+1<nj ? *(pixel+jstep) - *pixel: 0;
                *dym_pixel = j > 0 ? *pixel - *(pixel-jstep) : 0 ;
                *dyc_pixel = (*dyp_pixel + *dym_pixel)/2.;

                *dzp_pixel = k+1 < nk ? *(pixel+kstep) - *pixel : 0;
                *dzm_pixel = k > 0 ? *pixel - *(pixel-kstep) : 0;
                *dzc_pixel = (*dzp_pixel + *dzm_pixel)/2.;
        }
      }
    }
  }

*/

