// This is brcv/seg/dbdet/dbacm3d_edge_trough_levelset_func.cxx
//:
// \file

#include "dbacm3d_edge_trough_levelset_func.h"

#include <vnl/vnl_math.h>
#include <vcl_iostream.h>

#include <dbil3d/algo/dbil3d_finite_differences.h>
#include <dbil3d/algo/dbil3d_finite_second_differences.h>

#include <vnl/vnl_vector.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/algo/vil3d_threshold.h>
#include <vil3d/algo/vil3d_distance_transform.h>
#include <dbil3d/algo/dbil3d_gauss_filter.h> 
#include <dbacm3d/dbacm3d_levelset_math.h>

const float dbacm3d_min_grad_phi = float(1e-10);


//: Reinitialize levelset surface (to make sure it is a distance transform)
void dbacm3d_edge_trough_levelset_func::
reinitialize_levelset_surf()
{
//        vcl_cerr << "reinitializing levelset surface" << "\n" ;
//        vil3d_image_view<bool> thresholded; 
//        vil3d_threshold_below(phi_,thresholded,0.f);
//        vil3d_signed_distance_transform(thresholded,
//                        phi_,
//                        1000);
//        dbil3d_gauss_filter(phi_,1,phi_);
//
//
//

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

inline vnl_vector<float> dbacm3d_edge_trough_levelset_func::closest_edge(const float * pixel_edges,
                                      const  vcl_ptrdiff_t & pstep_edges,
                                      const  vcl_ptrdiff_t & kstep_edges,
                                      const  vcl_ptrdiff_t & jstep_edges,
                                      const  vcl_ptrdiff_t & istep_edges)
{
    vnl_vector<float> closest_edge_offset(3,-1.0f);
    float min_magnitude = -1;

    if(this->twoD_hack_){
    int z = 0;
    for(int y = -1; y <= 1; y++){
    for(int x = -1; x <= 1; x++){
            const float* edge_check =  pixel_edges + x*istep_edges + y*jstep_edges + z*kstep_edges;
            if((*edge_check) != -1){
                   vnl_vector<float> edgeloc(3,0.0f);
                   edgeloc.put(0,x+0.5+*edge_check);
                   edgeloc.put(1,y+0.5+*(edge_check+pstep_edges));
                   edgeloc.put(2,z+0.5+*(edge_check+2*pstep_edges));
                   if(edgeloc.magnitude() < min_magnitude || min_magnitude == -1){
                            min_magnitude = edgeloc.magnitude();
                            closest_edge_offset = edgeloc;
                   }
            }
    }
    }
    }
    else{
    //int z = 0;
    for(int z = -1; z <= 1; z++){
    for(int y = -1; y <= 1; y++){
    for(int x = -1; x <= 1; x++){
            const float* edge_check =  pixel_edges + x*istep_edges + y*jstep_edges + z*kstep_edges;
            if((*edge_check) != -1){
                   vnl_vector<float> edgeloc(3,0.0f);
                   edgeloc.put(0,x+0.5+*edge_check);
                   edgeloc.put(1,y+0.5+*(edge_check+pstep_edges));
                   edgeloc.put(2,z+0.5+*(edge_check+2*pstep_edges));
                   if(edgeloc.magnitude() < min_magnitude || min_magnitude == -1){
                            min_magnitude = edgeloc.magnitude();
                            closest_edge_offset = edgeloc;
                   }
            }
    }
    }
    }
    }
    return closest_edge_offset;
}

// ------------------------------------------------------------------
//: evolve the levelset function given a time step
void dbacm3d_edge_trough_levelset_func::
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
                istep_k=this->curvature_.istep(), 
                jstep_k=this->curvature_.jstep(),
                kstep_k=this->curvature_.kstep(),
                pstep_k=this->curvature_.planestep();

        vcl_ptrdiff_t 
                istep_edges=this->edges_.istep(), 
                jstep_edges=this->edges_.jstep(),
                kstep_edges=this->edges_.kstep(),
                pstep_edges=this->edges_.planestep();

        vcl_ptrdiff_t 
                istep_troughs=this->troughs_.istep(), 
                jstep_troughs=this->troughs_.jstep(),
                kstep_troughs=this->troughs_.kstep(),
                pstep_troughs=this->troughs_.planestep();

        vcl_ptrdiff_t 
                istep_no_go_image=this->no_go_image_.istep(), 
                jstep_no_go_image=this->no_go_image_.jstep(),
                kstep_no_go_image=this->no_go_image_.kstep(),
                pstep_no_go_image=this->no_go_image_.planestep();

        float* plane_phi = this->phi_.origin_ptr();
        const float* plane_hj = this->hj_flux_.origin_ptr();
        const float* plane_k = this->curvature_.origin_ptr();
        const float* plane_edges = this->edges_.origin_ptr();
        const float* plane_troughs = this->troughs_.origin_ptr();
        const bool* plane_no_go_image = this->no_go_image_.origin_ptr();
        const float* plane_dxc = this->dxc_.origin_ptr();
        const float* plane_dyc = this->dyc_.origin_ptr();
        const float* plane_dzc = this->dzc_.origin_ptr();

        for (unsigned p=0; p<np; ++p, 
                        plane_phi += pstep_phi,
                        plane_hj += pstep_hj,
                        plane_dxc += pstep_dxc,
                        plane_dyc += pstep_dyc,
                        plane_dzc += pstep_dzc,
                        plane_k += pstep_k,
                        plane_edges += pstep_edges,
                        plane_troughs += pstep_troughs,
                        plane_no_go_image += pstep_no_go_image)
        {
                float*       slice_phi  = plane_phi;
                const float* slice_hj   = plane_hj ;
                const float* slice_k    = plane_k  ;
                const float* slice_dxc  = plane_dxc;
                const float* slice_dyc  = plane_dyc;
                const float* slice_dzc  = plane_dzc;
                const float* slice_edges    = plane_edges  ;
                const float* slice_troughs    = plane_troughs  ;
                const bool* slice_no_go_image    = plane_no_go_image  ;
                for (unsigned k=0; k<nk; ++k, 
                                slice_phi += kstep_phi,
                                slice_hj += kstep_hj,
                                slice_dxc += kstep_dxc,
                                slice_dyc += kstep_dyc,
                                slice_dzc += kstep_dzc, 
                                slice_k += kstep_k,
                                slice_edges += kstep_edges,
                                slice_troughs += kstep_troughs,
                                slice_no_go_image += kstep_no_go_image)
                { 
                        float* row_phi = slice_phi;
                        const float* row_hj = slice_hj;
                        const float* row_dxc = slice_dxc;
                        const float* row_dyc = slice_dyc;
                        const float* row_dzc = slice_dzc; 
                        const float* row_k = slice_k;
                        const float* row_edges = slice_edges;
                        const float* row_troughs = slice_troughs;
                        const bool* row_no_go_image = slice_no_go_image;

                        for (unsigned j=0; j<nj; ++j,
                                        row_phi += jstep_phi,
                                        row_hj += jstep_hj,
                                        row_dxc += jstep_dxc,
                                        row_dyc += jstep_dyc,
                                        row_dzc += jstep_dzc, 
                                        row_k += jstep_k,
                                        row_edges += jstep_edges,
                                        row_troughs += jstep_troughs,
                                        row_no_go_image += jstep_no_go_image)
                        {
                                float* pixel_phi = row_phi;
                                const float* pixel_hj = row_hj;
                                const float* pixel_dxc = row_dxc;
                                const float* pixel_dyc = row_dyc;
                                const float* pixel_dzc = row_dzc; 
                                const float* pixel_k = row_k;
                                const float* pixel_edges = row_edges;
                                const float* pixel_troughs = row_troughs;
                                const bool* pixel_no_go_image = row_no_go_image;

                                for (unsigned i=0; i<ni; ++i,
                                                pixel_phi += istep_phi,
                                                pixel_hj += istep_hj,
                                                pixel_dxc += istep_dxc,
                                                pixel_dyc += istep_dyc,
                                                pixel_dzc += istep_dzc,
                                                pixel_k += istep_k,
                                                pixel_edges += istep_edges,
                                                pixel_troughs += istep_troughs,
                                                pixel_no_go_image += istep_no_go_image)
                                {
                                        float advection_term, inflation_term, curvature_term, speed;
                                        // advection_term (geodesic)
                                        advection_term = 0;

                                        // inflation term
                                        inflation_term = *pixel_hj;

                                        // curvature term
                                        curvature_term = (*pixel_k)*(*pixel_hj);

                                        // overall speed
                                        speed = (inflation_weight_*inflation_term 
                                               + curvature_weight_*curvature_term);

                                        if(vcl_fabs(*pixel_phi) < 1){
                                        //modulate by trough value
                                        //(slow to zero as levelset approaches the
                                        //trough)
                                        speed *= 0.4-*pixel_troughs < 0 ? 0 : 0.4-*pixel_troughs;

                                        if(i >=1 && i < ni-1 
                                           && j >=1 && j < nj-1 
                                           && (( k >=1 && k < nk-1)|| this->twoD_hack_))
                                        {
                                                vnl_vector<float> edgevec = closest_edge(pixel_edges,
                                                                                      pstep_edges,
                                                                                      kstep_edges,
                                                                                      jstep_edges,
                                                                                      istep_edges);
                                                if(edgevec[0] > -1){
                                                        bool debug = 0;
                                                if(debug) vcl_cerr <<"\n" <<  i << " " << j << " " << k << "\n" ;
                                                if(debug) vcl_cerr << *pixel_phi << "\n";
                                                if(debug) vcl_cerr << "edge is at : ";
                                                if(debug) vcl_cerr << edgevec[0] << " " << edgevec[1] << " " <<edgevec[2] << "\n" ;
                                                vnl_vector<float> normal(3,0.0f);
                                                normal.put(0,*pixel_dxc);
                                                normal.put(1,*pixel_dyc);
                                                normal.put(2,*pixel_dzc);
                                                normal.normalize();

                                                if(debug) vcl_cerr << "normal is  : ";
                                                if(debug) vcl_cerr << normal[0] << " " << normal[1] << " " <<normal[2] << "\n" ;
                                                if(debug) vcl_cerr << "dot is  : " << dot_product(normal,edgevec) << "\n";
                                                float dist = dot_product(normal,edgevec) - vcl_fabs(*pixel_phi);

                                                if(debug) vcl_cerr << "dist is  : " << dist << "\n";
                                                if(debug) vcl_cerr << "speed was  : " << speed << "\n";
                                                speed *= dist*dist/(dist*dist + edgeT_*edgeT_);
                                                if(debug) vcl_cerr << "multiplied by  : " << dist*dist/(dist*dist + edgeT_*edgeT_) << "\n";
                                                if(debug) vcl_cerr << "new speed is  : " << speed << "\n";
//                                                char g;
//                                                vcl_cin >> g;
                                                speed *= (float)*pixel_no_go_image;
                                                }

                                        }
                                        }

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
        void dbacm3d_edge_trough_levelset_func::
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

                        if(this->twoD_hack_){
                                dzp.fill(0);
                                dzm.fill(0);
                        }

                        dbacm3d_levelset_math_compute_hj_flux(hj_flux_, 
                                                             (this->inflation_weight()>=0) ? 1 : -1,
                                                             dxp, dxm,
                                                             dyp, dym,
                                                             dzp, dzm); 


                        if(this->curvature_weight() > 0){
                                curvature_.set_size(phi_.ni(),phi_.nj(),phi_.nk());
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
                                dbacm3d_levelset_math_compute_curvature(this->curvature_,
                                                                        dxc_,dyc_,dzc_,
                                                                        dxx,dyy,dzz,dxy,dxz,dyz);
                        }
                        else{
                          if(curvature_.ni() != phi_.ni() || 
                              curvature_.nj() != phi_.nj() || 
                              curvature_.nk() != phi_.nk() )
                          {
                                float* tmp = new float;
                                *tmp = -1;
                                this->curvature_ = vil3d_image_view<float>(tmp,
                                    phi_.ni(),
                                    phi_.nj(),
                                    phi_.nk(),
                                    phi_.nplanes()
                                    ,0,0,0,0);
                          }

                        }
                        return;
                } 


