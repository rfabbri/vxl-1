// This is brcv/seg/dbdet/dbacm3d_edge_lowmem_levelset_func.cxx
//:
// \file

#include "dbacm3d_edge_lowmem_levelset_func.h"

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
void dbacm3d_edge_lowmem_levelset_func::
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

inline vnl_vector<float> dbacm3d_edge_lowmem_levelset_func::closest_edge(const float * pixel_edges,
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
void dbacm3d_edge_lowmem_levelset_func::
evolve_one_timestep(float timestep)
{

        //copy of phi for computation purposes
        phi2_.deep_copy(phi_);

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
                istep_phi2=this->phi2_.istep(), 
                jstep_phi2=this->phi2_.jstep(),
                kstep_phi2=this->phi2_.kstep(),
                pstep_phi2=this->phi2_.planestep();



        vcl_ptrdiff_t 
                istep_edges=this->edges_.istep(), 
                jstep_edges=this->edges_.jstep(),
                kstep_edges=this->edges_.kstep(),
                pstep_edges=this->edges_.planestep();

        vcl_ptrdiff_t 
                istep_no_go_image=this->no_go_image_.istep(), 
                jstep_no_go_image=this->no_go_image_.jstep(),
                kstep_no_go_image=this->no_go_image_.kstep(),
                pstep_no_go_image=this->no_go_image_.planestep();

        float* plane_phi = this->phi_.origin_ptr();
        float* plane_phi2 = this->phi2_.origin_ptr();
        const float* plane_edges = this->edges_.origin_ptr();
        const bool* plane_no_go_image = this->no_go_image_.origin_ptr();

        float hj_flux,curvature,dxc,dyc,dzc;

        for (unsigned p=0; p<np; ++p, 
                        plane_phi += pstep_phi,
                        plane_phi2 += pstep_phi2,
                        plane_edges += pstep_edges,
                        plane_no_go_image += pstep_no_go_image)
        {
                float*       slice_phi  = plane_phi;
                float*       slice_phi2  = plane_phi2;
                const float* slice_edges    = plane_edges  ;
                const bool* slice_no_go_image    = plane_no_go_image  ;
                for (unsigned k=0; k<nk; ++k, 
                                slice_phi += kstep_phi,
                                slice_phi2 += kstep_phi2,
                                slice_edges += kstep_edges,
                                slice_no_go_image += kstep_no_go_image)
                { 
                        float* row_phi = slice_phi;
                        float* row_phi2 = slice_phi2;
                        const float* row_edges = slice_edges;
                        const bool* row_no_go_image = slice_no_go_image;

                        for (unsigned j=0; j<nj; ++j,
                                        row_phi += jstep_phi,
                                        row_phi2 += jstep_phi2,
                                        row_edges += jstep_edges,
                                        row_no_go_image += jstep_no_go_image)
                        {
                                float* pixel_phi = row_phi;
                                float* pixel_phi2 = row_phi2;
                                const float* pixel_edges = row_edges;
                                const bool* pixel_no_go_image = row_no_go_image;

                                for (unsigned i=0; i<ni; ++i,
                                                pixel_phi += istep_phi,
                                                pixel_phi2 += istep_phi2,
                                                pixel_edges += istep_edges,
                                                pixel_no_go_image += istep_no_go_image)
                                {
                                        float advection_term, inflation_term, curvature_term, speed;

                                        this->compute_measures(pixel_phi2,
                                                         istep_phi,  jstep_phi,  kstep_phi,
                                                         i,  j,  k, ni,  nj,  nk, 
                                                         hj_flux,  curvature, dxc,  dyc,  dzc);

                                        // advection_term (geodesic)
                                        advection_term = 0;

                                        // inflation term
                                        inflation_term = hj_flux;

                                        // curvature term
                                        curvature_term = curvature*hj_flux;

                                        // overall speed
                                        speed = (inflation_weight_*inflation_term 
                                                        + curvature_weight_*curvature_term);

                                        if(vcl_fabs(*pixel_phi) < 1){
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
                                                                normal.put(0,dxc);
                                                                normal.put(1,dyc);
                                                                normal.put(2,dzc);
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


void dbacm3d_edge_lowmem_levelset_func::compute_measures(float* pixel_phi,
                const vcl_ptrdiff_t& istep, const vcl_ptrdiff_t& jstep, const vcl_ptrdiff_t& kstep,
                const unsigned& i, const unsigned& j, const unsigned& k,
                const unsigned& ni, const unsigned& nj, const unsigned& nk, 
                float& hj_flux, float& curvature,
                float& dxc, float& dyc, float& dzc)
{

      vcl_ptrdiff_t safe_istep_plus = i > ni-2 ? 0 : istep;
      vcl_ptrdiff_t safe_istep_minus = i < 1 ? 0 : -istep;

      vcl_ptrdiff_t safe_jstep_plus= j > nj-2 ? 0 : jstep;
      vcl_ptrdiff_t safe_jstep_minus = j < 1 ? 0 : -jstep; 

      vcl_ptrdiff_t safe_kstep_plus= k > nk-2 ? 0 : kstep;
      vcl_ptrdiff_t safe_kstep_minus = k < 1 ? 0 : -kstep; 


      float dxp = *(pixel_phi+safe_istep_plus) - *pixel_phi;
      float dxm = *pixel_phi - *(pixel_phi+safe_istep_minus);
      dxc = (dxp + dxm)/2.;

      float dyp = *(pixel_phi+safe_jstep_plus) - *pixel_phi;
      float dym = *pixel_phi - *(pixel_phi+safe_jstep_minus);
      dyc = (dyp + dym)/2.;

      float dzp = *(pixel_phi+safe_kstep_plus) - *pixel_phi;
      float dzm = *pixel_phi - *(pixel_phi+safe_kstep_minus);
      dzc = (dzp + dzm)/2.;

      dxp = direction_*(dxp) > 0 ? 0 : dxp;
      dxm = direction_*(dxm) < 0 ? 0 : dxm;
      dyp = direction_*(dyp) > 0 ? 0 : dyp;
      dym = direction_*(dym) < 0 ? 0 : dym;
      dzp = direction_*(dzp) > 0 ? 0 : dzp;
      dzm = direction_*(dzm) < 0 ? 0 : dzm;

      hj_flux =  vcl_sqrt( dxp*dxp 
                      +dxm*dxm 
                      +dyp*dyp 
                      +dym*dym
                      +dzp*dzp 
                      +dzm*dzm);

      float dxx = dxp - dxm;
      float dyy = dyp - dym;
      float dzz = dzp - dzm;
      float dxy =( *(pixel_phi + safe_istep_plus  + safe_jstep_plus)  
                      +*(pixel_phi + safe_istep_minus + safe_jstep_minus)
                      -*(pixel_phi + safe_istep_minus + safe_jstep_plus)  
                      -*(pixel_phi + safe_istep_plus  + safe_jstep_minus))/4.;
      float dxz =( *(pixel_phi + safe_istep_plus  + safe_kstep_plus)  
                      +*(pixel_phi + safe_istep_minus + safe_kstep_minus)
                      -*(pixel_phi + safe_istep_minus + safe_kstep_plus)  
                      -*(pixel_phi + safe_istep_plus  + safe_kstep_minus))/4.;
      float dyz =( *(pixel_phi + safe_jstep_plus  + safe_kstep_plus)  
                      +*(pixel_phi + safe_jstep_minus + safe_kstep_minus)
                      -*(pixel_phi + safe_jstep_minus + safe_kstep_plus)  
                      -*(pixel_phi + safe_jstep_plus  + safe_kstep_minus))/4.;

#define EPSILON .001
#define SIGN(X) (X < 0? -1 : (X==0 ? 0 : 1))

      float yc_2 = dyc*dyc;
      float xc_2 = dxc*dxc;
      float zc_2 = dzc*dzc;
      float grad_square = xc_2 + yc_2 + zc_2;

      if(grad_square < EPSILON){
              curvature = 0;
      }
      else{
              float H = ( dxx*(yc_2 + zc_2) 
                              + dyy*(xc_2 + zc_2) 
                              + dzz*(xc_2 + yc_2)
                              - 2*(dxc*dyc*dxy + dyc*dzc*dyz + dxc*dzc*dxz))
                      /(2*vcl_sqrt(grad_square*grad_square*grad_square));


              float G =((xc_2)*(dyy*dzz - dyz*dyz) +            
                              (yc_2)*(dxx*dzz - dxz*dxz) +            
                              (zc_2)*(dxx*dyy - dxy*dxy) +            
                              -2*dxc*dyc*(dxy*dzz - dyz*dxz) +    
                              -2*dyc*dzc*(dyz*dxx - dxz*dxy) +    
                              -2*dxc*dzc*(dxz*dyy - dxy*dyz))
                      /(grad_square*grad_square);

              if(G < 0 ) { 
                      curvature = 0;
              }
              else{
                      curvature =  SIGN(H)*vcl_sqrt(G);
              }
      }    

#undef EPSILON
#undef SIGN


}


// ------------------------------------------------------------------
//: Compute all internal data necessary to evolve the levelset surface
void dbacm3d_edge_lowmem_levelset_func::
compute_internal_data()
{
        /*
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
         */
} 


