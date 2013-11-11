// This is brcv/seg/dbacm3d/dbacm3d_levelset_math.h
#ifndef dbacm3d_levelset_math_h_
#define dbacm3d_levelset_math_h_

//:
// \file
// \brief Various math functions used in levelset segmentation
// \author Jesse Funaro (jdfunaro@lems.brown.edu)
//         (after dbacm/dbacm_levelset_driver by  Nhon Trinh (ntrinh@lems.brown.edu))
// \date 01/05/2006
//
// \verbatim
//  Modifications
// \endverbatim


#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_math.h>

#define MAX_CURVATURE 100
#define MIN_CURVATURE .001
#define MIN_GRADIENT  .001


//: Compute curvature at every point on levelset surface
// assume all input images have the same size
template <class T>
inline void dbacm3d_levelset_math_compute_curvature(vil3d_image_view<T >& curvature,
    const vil3d_image_view<T>& dxc,
    const vil3d_image_view<T>& dyc,
    const vil3d_image_view<T>& dzc,
    const vil3d_image_view<T>& dxx,
    const vil3d_image_view<T>& dyy,
    const vil3d_image_view<T>& dzz,
    const vil3d_image_view<T>& dxy,
    const vil3d_image_view<T>& dxz,
    const vil3d_image_view<T>& dyz)
{
  unsigned ni = dxc.ni(),nj = dxc.nj(),nk = dxc.nk(),np = dxc.nplanes();
  vcl_ptrdiff_t istep=dxc.istep(),jstep=dxc.jstep(),kstep=dxc.kstep(),pstep = dxc.planestep();

  curvature.set_size(ni,nj,nk,np);
  T    xc, yc, zc, xx, yy, zz, xy, xz, yz;
  float H, G,grad_square;

  T* plane = curvature.origin_ptr();
  const T* dxc_plane   = dxc.origin_ptr();
  const T* dyc_plane   = dyc.origin_ptr();
  const T* dzc_plane   = dzc.origin_ptr();
  const T* dxx_plane   = dxx.origin_ptr();
  const T* dyy_plane   = dyy.origin_ptr();
  const T* dzz_plane   = dzz.origin_ptr();
  const T* dxy_plane   = dxy.origin_ptr();
  const T* dxz_plane   = dxz.origin_ptr();
  const T* dyz_plane   = dyz.origin_ptr();

  for (unsigned p=0;p<np;++p,plane += pstep, 
                             dxc_plane+=pstep, dyc_plane+=pstep, dzc_plane+=pstep,
                             dxx_plane+=pstep, dyy_plane+=pstep, dzz_plane+=pstep,
                             dxy_plane+=pstep, dxz_plane+=pstep, dyz_plane+=pstep)
  {
    T* slice =   plane;
    const T* dxc_slice   = dxc_plane;
    const T* dyc_slice   = dyc_plane;
    const T* dzc_slice   = dzc_plane;
    const T* dxx_slice   = dxx_plane;
    const T* dyy_slice   = dyy_plane;
    const T* dzz_slice   = dzz_plane;
    const T* dxy_slice   = dxy_plane;
    const T* dxz_slice   = dxz_plane;
    const T* dyz_slice   = dyz_plane;

    for (unsigned k=0;k<nk;++k,slice += kstep,
                               dxc_slice+=kstep, dyc_slice+=kstep, dzc_slice+=kstep,
                               dxx_slice+=kstep, dyy_slice+=kstep, dzz_slice+=kstep,
                               dxy_slice+=kstep, dxz_slice+=kstep, dyz_slice+=kstep)
    {
      T* row =   slice;
      const T* dxc_row   = dxc_slice;
      const T* dyc_row   = dyc_slice;
      const T* dzc_row   = dzc_slice;
      const T* dxx_row   = dxx_slice;
      const T* dyy_row   = dyy_slice;
      const T* dzz_row   = dzz_slice;
      const T* dxy_row   = dxy_slice;
      const T* dxz_row   = dxz_slice;
      const T* dyz_row   = dyz_slice;
      for (unsigned j=0;j<nj;++j,row += jstep,
                         dxc_row+=jstep, dyc_row+=jstep, dzc_row+=jstep,
                         dxx_row+=jstep, dyy_row+=jstep, dzz_row+=jstep,
                         dxy_row+=jstep, dxz_row+=jstep, dyz_row+=jstep)
      {
        T* pixel =   row;
      const T* dxc_pixel   = dxc_row;
      const T* dyc_pixel   = dyc_row;
      const T* dzc_pixel   = dzc_row;
      const T* dxx_pixel   = dxx_row;
      const T* dyy_pixel   = dyy_row;
      const T* dzz_pixel   = dzz_row;
      const T* dxy_pixel   = dxy_row;
      const T* dxz_pixel   = dxz_row;
      const T* dyz_pixel   = dyz_row;


        for (unsigned i=0;i<ni;++i,pixel+=istep,
                                   dxc_pixel+=istep, dyc_pixel+=istep, dzc_pixel+=istep,
                                   dxx_pixel+=istep, dyy_pixel+=istep, dzz_pixel+=istep,
                                   dxy_pixel+=istep, dxz_pixel+=istep, dyz_pixel+=istep)
       #define EPSILON .001
       #define SIGN(X) (X < 0? -1 : (X==0 ? 0 : 1))
        {

                                xc = *dxc_pixel;
                                yc = *dyc_pixel;
                                zc = *dzc_pixel;

                                
                                float yc_2 = yc*yc;
                                float xc_2 = xc*xc;
                                float zc_2 = zc*zc;
                                grad_square = xc_2 + yc_2 + zc_2;

                                if(grad_square < EPSILON){
                                        *pixel = 0;
                                }
                                else{
                                        xx = *dxx_pixel;
                                        yy = *dyy_pixel;
                                        zz = *dzz_pixel;

                                        xy = *dxy_pixel;
                                        xz = *dxz_pixel;
                                        yz = *dyz_pixel;

                                        H = ( xx*(yc_2 + zc_2) 
                                            + yy*(xc_2 + zc_2) 
                                            + zz*(xc_2 + yc_2)
                                            - 2*(xc*yc*xy + yc*zc*yz + xc*zc*xz))
                                            /(2*vcl_sqrt(grad_square*grad_square*grad_square));


                                        G =((xc_2)*(yy*zz - yz*yz) +            
                                            (yc_2)*(xx*zz - xz*xz) +            
                                            (zc_2)*(xx*yy - xy*xy) +            
                                            -2*xc*yc*(xy*zz - yz*xz) +    
                                            -2*yc*zc*(yz*xx - xz*xy) +    
                                            -2*xc*zc*(xz*yy - xy*yz))
                                                /(grad_square*grad_square);

                                        if(G < 0 ) { 
                                                *pixel = 0;
                                        }
                                        else{
                                                *pixel =  SIGN(H)*vcl_sqrt(G);
                                        }
                                }    

        } 
       #undef EPSILON
       #undef SIGN
      }
    }
  }


}  




//: Compute Hamilton-Jacobi flux
template <class T>
inline void dbacm3d_levelset_math_compute_hj_flux(vil3d_image_view<T >& hj_flux,
    int direction,
    const vil3d_image_view<T>& dxp,
    const vil3d_image_view<T>& dxm,
    const vil3d_image_view<T>& dyp,
    const vil3d_image_view<T>& dym,
    const vil3d_image_view<T>& dzp,
    const vil3d_image_view<T>& dzm )
{
  unsigned ni = dxp.ni(),nj = dxp.nj(),nk = dxp.nk(),np = dxp.nplanes();
  vcl_ptrdiff_t istep=dxp.istep(),jstep=dxp.jstep(),kstep=dxp.kstep(),pstep = dxp.planestep();

  hj_flux.set_size(ni,nj,nk,np);

  T xp;
  T xm;
  T yp;
  T ym;
  T zp;
  T zm;

  T* plane = hj_flux.origin_ptr();
  const T* dxp_plane   = dxp.origin_ptr();
  const T* dxm_plane   = dxm.origin_ptr();
  const T* dyp_plane   = dyp.origin_ptr();
  const T* dym_plane   = dym.origin_ptr();
  const T* dzp_plane   = dzp.origin_ptr();
  const T* dzm_plane   = dzm.origin_ptr();

  for (unsigned p=0;p<np;++p,plane += pstep,
                dxp_plane+=pstep, dxm_plane+=pstep,
                dyp_plane+=pstep, dym_plane+=pstep,
                dzp_plane+=pstep, dzm_plane+=pstep)
  {
    T* slice =   plane;
    const T* dxp_slice = dxp_plane;
    const T* dxm_slice = dxm_plane;
    const T* dyp_slice = dyp_plane;
    const T* dym_slice = dym_plane;
    const T* dzp_slice = dzp_plane;
    const T* dzm_slice = dzm_plane;
    for (unsigned k=0;k<nk;++k,slice += kstep,
                 dxp_slice+=kstep, dxm_slice+=kstep,
                    dyp_slice+=kstep, dym_slice+=kstep,
                    dzp_slice+=kstep, dzm_slice+=kstep)
    {
      T* row =   slice;
      const T* dxp_row = dxp_slice;
      const T* dxm_row = dxm_slice;
      const T* dyp_row = dyp_slice;
      const T* dym_row = dym_slice;
      const T* dzp_row = dzp_slice;
      const T* dzm_row = dzm_slice;
      for (unsigned j=0;j<nj;++j,row += jstep,
                    dxp_row+=jstep, dxm_row+=jstep,
                    dyp_row+=jstep, dym_row+=jstep,
                    dzp_row+=jstep, dzm_row+=jstep)
      {
        T* pixel =   row;
        const T* dxp_pixel = dxp_row;
        const T* dxm_pixel = dxm_row;
        const T* dyp_pixel = dyp_row;
        const T* dym_pixel = dym_row;
        const T* dzp_pixel = dzp_row;
        const T* dzm_pixel = dzm_row;
        for (unsigned i=0;i<ni;++i,pixel+=istep,
                      dxp_pixel+=istep, dxm_pixel+=istep,
                      dyp_pixel+=istep, dym_pixel+=istep,
                      dzp_pixel+=istep, dzm_pixel+=istep)
        {
          xp = direction*(*dxp_pixel) > 0 ? 0 : *dxp_pixel;
          xm = direction*(*dxm_pixel) < 0 ? 0 : *dxm_pixel;
          yp = direction*(*dyp_pixel) > 0 ? 0 : *dyp_pixel;
          ym = direction*(*dym_pixel) < 0 ? 0 : *dym_pixel;
          zp = direction*(*dzp_pixel) > 0 ? 0 : *dzp_pixel;
          zm = direction*(*dzm_pixel) < 0 ? 0 : *dzm_pixel;

          *pixel = vcl_sqrt(  xp*xp 
                            + xm*xm 
                            + yp*yp 
                            + ym*ym
                            + zp*zp 
                            + zm*zm);

        }
      }
    }
  }


} 


#endif // dbacm3d_levelset_math_h_

