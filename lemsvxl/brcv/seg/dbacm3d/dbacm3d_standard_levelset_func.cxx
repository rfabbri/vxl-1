// This is brcv/seg/dbdet/dbacm3d_standard_levelset_func.cxx
//:
// \file

#include "dbacm3d_standard_levelset_func.h"

#include <vnl/vnl_math.h>

#include <dbil3d/algo/dbil3d_finite_differences.h>
#include <dbil3d/algo/dbil3d_finite_second_differences.h>


#include <vil3d/vil3d_image_view.h>
#include <dbacm3d/dbacm3d_levelset_math.h>

const float dbacm3d_min_grad_phi = float(1e-10);


//: Reinitialize levelset surface (to make sure it is a distance transform)
void dbacm3d_standard_levelset_func::
reinitialize_levelset_surf()
{
        vcl_cerr << "Not yet implemented" << vcl_endl;
        /*
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
        */
}


// ------------------------------------------------------------------
//: evolve the levelset function given a time step
void dbacm3d_standard_levelset_func::
evolve_one_timestep(float timestep)
{
        // compute finite difference, hj-flux, and curvature images
        this->compute_internal_data();

        // iterate through all points on image
        unsigned ni=this->phi_.ni(), 
                nj=this->phi_.nj(), 
                nk=this->phi_.nk(), 
                np=this->phi_.nplanes();

        vcl_ptrdiff_t 
                istep_phi=this->phi_.istep(), 
                jstep_phi=this->phi_.jstep(),
                kstep_phi=this->phi_.kstep(),
                pstep_phi=this->phi_.planestep();

        vcl_ptrdiff_t 
                istep_hj=this->hj_flux_.istep(), 
                jstep_hj=this->hj_flux_.jstep(),
                kstep_hj=this->hj_flux_.kstep(),
                pstep_hj=this->hj_flux_.planestep();

        vcl_ptrdiff_t 
                istep_k=this->curvature_.istep(), 
                jstep_k=this->curvature_.jstep(),
                kstep_k=this->curvature_.kstep(),
                pstep_k=this->curvature_.planestep();

        vcl_ptrdiff_t 
                istep_dxc=this->dxc_.istep(), 
                jstep_dxc=this->dxc_.jstep(),
                kstep_dxc=this->dxc_.kstep(),
                pstep_dxc=this->dxc_.planestep();

        vcl_ptrdiff_t 
                istep_dyc=this->dyc_.istep(), 
                jstep_dyc=this->dyc_.jstep(),
                kstep_dyc=this->dyc_.kstep(),
                pstep_dyc=this->dyc_.planestep();

        vcl_ptrdiff_t 
                istep_dzc=this->dzc_.istep(), 
                jstep_dzc=this->dzc_.jstep(),
                kstep_dzc=this->dzc_.kstep(),
                pstep_dzc=this->dzc_.planestep();

        vcl_ptrdiff_t 
                istep_gmap=this->gmap_.istep(), 
                jstep_gmap=this->gmap_.jstep(),
                kstep_gmap=this->gmap_.kstep(),
                pstep_gmap=this->gmap_.planestep();

        vcl_ptrdiff_t 
                istep_gx=this->gx_.istep(), 
                jstep_gx=this->gx_.jstep(),
                kstep_gx=this->gx_.kstep(),
                pstep_gx=this->gx_.planestep();

        vcl_ptrdiff_t 
                istep_gy=this->gy_.istep(), 
                jstep_gy=this->gy_.jstep(),
                kstep_gy=this->gy_.kstep(),
                pstep_gy=this->gy_.planestep();
        vcl_ptrdiff_t 
                istep_gz=this->gz_.istep(), 
                jstep_gz=this->gz_.jstep(),
                kstep_gz=this->gz_.kstep(),
                pstep_gz=this->gz_.planestep();


        /*
        vcl_ptrdiff_t 
                istep_kmap=this->kmap_.istep(), 
                jstep_kmap=this->kmap_.jstep(),
                kstep_kmap=this->kmap_.kstep(),
                pstep_kmap=this->kmap_.planestep();
                */

        float* plane_phi = this->phi_.origin_ptr();
        const float* plane_hj = this->hj_flux_.origin_ptr();
        const float* plane_k = this->curvature_.origin_ptr();
        const float* plane_dxc = this->dxc_.origin_ptr();
        const float* plane_dyc = this->dyc_.origin_ptr();
        const float* plane_dzc = this->dzc_.origin_ptr();

        const float* plane_gmap = this->gmap_.origin_ptr();
        const float* plane_gx = this->gx_.origin_ptr();
        const float* plane_gy = this->gy_.origin_ptr();
        const float* plane_gz = this->gz_.origin_ptr();
        /*
           const float* plane_kmap = this->kmap_.origin_ptr();
           */

        for (unsigned p=0; p<np; ++p, 
                        plane_phi += pstep_phi,
                        plane_hj += pstep_hj,
                        plane_k += pstep_k,
                        plane_dxc += pstep_dxc,
                        plane_dyc += pstep_dyc,
                        plane_dzc += pstep_dzc,
                        plane_gmap += pstep_gmap,
                        plane_gx += pstep_gx,
                        plane_gy += pstep_gy,
                        plane_gz += pstep_gz)
        {
                float*       slice_phi  = plane_phi;
                const float* slice_hj   = plane_hj ;
                const float* slice_k    = plane_k  ;
                const float* slice_dxc  = plane_dxc;
                const float* slice_dyc  = plane_dyc;
                const float* slice_dzc  = plane_dzc;

                const float* slice_gmap = plane_gmap;
                const float* slice_gx   = plane_gx  ;
                const float* slice_gy   = plane_gy  ;
                const float* slice_gz   = plane_gz  ;
                for (unsigned k=0; k<nk; ++k, 
                                slice_phi += kstep_phi,
                                slice_hj += kstep_hj,
                                slice_k += kstep_k,
                                slice_dxc += kstep_dxc,
                                slice_dyc += kstep_dyc,
                                slice_dzc += kstep_dzc,
                                slice_gmap += kstep_gmap,
                                slice_gx += kstep_gx,
                                slice_gy += kstep_gy,
                                slice_gz += kstep_gz)
                { 
                        float* row_phi = slice_phi;
                        const float* row_hj = slice_hj;
                        const float* row_k = slice_k;
                        const float* row_dxc = slice_dxc;
                        const float* row_dyc = slice_dyc;
                        const float* row_dzc = slice_dzc;
                        const float* row_gmap = slice_gmap;
                        const float* row_gx = slice_gx;
                        const float* row_gy = slice_gy;
                        const float* row_gz = slice_gz;

                        for (unsigned j=0; j<nj; ++j,
                                        row_phi += jstep_phi,
                                        row_hj += jstep_hj,
                                        row_k += jstep_k,
                                        row_dxc += jstep_dxc,
                                        row_dyc += jstep_dyc,
                                        row_dzc += jstep_dzc,
                                        row_gmap += jstep_gmap,
                                        row_gx += jstep_gx,
                                        row_gy += jstep_gy,
                                        row_gz += jstep_gz)
                        {
                                float* pixel_phi = row_phi;
                                const float* pixel_hj = row_hj;
                                const float* pixel_k = row_k;
                                const float* pixel_dxc = row_dxc;
                                const float* pixel_dyc = row_dyc;
                                const float* pixel_dzc = row_dzc;

                                const float* pixel_gmap = row_gmap;
                                const float* pixel_gx = row_gx;
                                const float* pixel_gy = row_gy;
                                const float* pixel_gz = row_gz;

                                for (unsigned i=0; i<ni; ++i,
                                                pixel_phi += istep_phi,
                                                pixel_hj += istep_hj,
                                                pixel_k += istep_k,
                                                pixel_dxc += istep_dxc,
                                                pixel_dyc += istep_dyc,
                                                pixel_dzc += istep_dzc,
                                                pixel_gmap += istep_gmap,
                                                pixel_gx += istep_gx,
                                                pixel_gy += istep_gy,
                                                pixel_gz += istep_gz)
                                {
                                        float advection_term, inflation_term, curvature_term, speed;
                                        float grad_phi_mag = vcl_sqrt((*pixel_dxc)*(*pixel_dxc) +
                                                                      (*pixel_dyc)*(*pixel_dyc) +
                                                                      (*pixel_dzc)*(*pixel_dzc) );

                                        // advection_term (geodesic)
                                        advection_term = (grad_phi_mag < dbacm3d_min_grad_phi) ? 0 :
                                                ((*pixel_gx)*(*pixel_dxc) + (*pixel_gy)*(*pixel_dyc) + (*pixel_gz)*(*pixel_dzc))/grad_phi_mag;

                                        // inflation term
                                        inflation_term = (*pixel_gmap)*(*pixel_hj);

                                        // curvature term
                                        curvature_term = (*pixel_k)*(*pixel_hj);

                                        // overall speed
                                        speed = advection_weight_*advection_term + 
                                                inflation_weight_*inflation_term +
                                                curvature_weight_*curvature_term;

                                        // update
                                        *pixel_phi -= timestep * speed;
                                }
                        }
                }
        }

        return;
}



        // ------------------------------------------------------------------
        //: Compute all internal data necessary to evolve the levelset surface
        void dbacm3d_standard_levelset_func::
                compute_internal_data()
                {
                        vil3d_image_view<float> dxp;
                        vil3d_image_view<float> dxm;
                        vil3d_image_view<float> dyp;
                        vil3d_image_view<float> dym;
                        vil3d_image_view<float> dzp;
                        vil3d_image_view<float> dzm;

                        //vcl_cerr << "dbil3d_finite_differences..." ;
                        dbil3d_finite_differences(phi_,
                                        dxp,dxm,this->dxc_,
                                        dyp,dym,this->dyc_,
                                        dzp,dzm,this->dzc_);
                        dbacm3d_levelset_math_compute_hj_flux(hj_flux_, 
                                                             (this->inflation_weight()>=0) ? 1 : -1,
                                                             dxp, dxm,
                                                             dyp, dym,
                                                             dzp, dzm); 

                        curvature_.set_size(phi_.ni(),phi_.nj(),phi_.nk());

                        if(this->curvature_weight() > 0){
                                vil3d_image_view<float> dxx;
                                vil3d_image_view<float> dyy;
                                vil3d_image_view<float> dzz;
                                vil3d_image_view<float> dxy;
                                vil3d_image_view<float> dxz;
                                vil3d_image_view<float> dyz;

                                dbil3d_finite_second_differences(phi_,dxp,dxm,
                                                                      dyp,dym,
                                                                      dzp,dzm,
                                                                      dxx,dyy,dzz,dxy,dxz,dyz);
                                dbacm3d_levelset_math_compute_curvature(curvature_,
                                                                        dxc_,dyc_,dzc_,
                                                                        dxx,dyy,dzz,dxy,dxz,dyz);
                        }
                        else{
                                this->curvature_.fill(0);
                        }
                        return;
                } 


