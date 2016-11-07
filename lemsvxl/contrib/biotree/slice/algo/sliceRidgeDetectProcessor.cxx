#include "sliceRidgeDetectProcessor.h"
#include <vnl/vnl_math.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <vnl/vnl_matrix.h>
#include <vil3d/vil3d_trilin_interp.h>
#include <vil3d/vil3d_slice.h>
#include <vil3d/algo/vil3d_grad_1x3.h>
#include <vil/vil_math.h>
#include <vil/algo/vil_sobel_1x3.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>
#include <vcl_string.h>
#include <vcl_cstring.h>


void sliceRidgeDetectProcessor::hessian_decompose_oneslice(
    const vil_image_view<float>& Im_xx,
    const vil_image_view<float>& Im_yy,
    const vil_image_view<float>& Im_zz,
    const vil_image_view<float>& Im_xy,
    const vil_image_view<float>& Im_xz,
    const vil_image_view<float>& Im_yz,
    vil_image_view<float>& e1,
    vil_image_view<float>& e2,
    vil_image_view<float>& e3,
    vil_image_view<float>& l1,
    vil_image_view<float>& l2,
    vil_image_view<float>& l3)
{
  unsigned ni = Im_xx.ni();
  unsigned nj = Im_xx.nj();

  e1.set_size(ni,nj,3);
  e2.set_size(ni,nj,3);
  e3.set_size(ni,nj,3);
  l1.set_size(ni,nj);
  l2.set_size(ni,nj);
  l3.set_size(ni,nj);

    for(int y=0; y<nj; y++) {
      for(int x=0; x<ni; x++) {
        vnl_matrix<float> mat(3,3);
        mat[0][0] = Im_xx(x,y);
        mat[0][1] = Im_xy(x,y);
        mat[0][2] = Im_xz(x,y);
        mat[1][0] = Im_xy(x,y);
        mat[1][1] = Im_yy(x,y); 
        mat[1][2] = Im_yz(x,y);
        mat[2][0] = Im_xz(x,y);
        mat[2][1] = Im_yz(x,y);
        mat[2][2] = Im_zz(x,y);
        vnl_symmetric_eigensystem<float> system(mat);
        //vnl_symmetric_eigensystem stores eigenvectors by increasing
        //eigenvalue

        vnl_vector<float> e1v = system.get_eigenvector(0);
        e3(x,y,0) = e1v[0];
        e3(x,y,1) = e1v[1];
        e3(x,y,2) = e1v[2];
        l3(x,y) = system.get_eigenvalue(0);

        vnl_vector<float> e2v = system.get_eigenvector(1);
        e2(x,y,0) = e2v[0];
        e2(x,y,1) = e2v[1];
        e2(x,y,2) = e2v[2];
        l2(x,y) = system.get_eigenvalue(1);

        vnl_vector<float> e3v = system.get_eigenvector(2);
        e1(x,y,0) = e3v[0];
        e1(x,y,1) = e3v[1];
        e1(x,y,2) = e3v[2];
        l1(x,y) = system.get_eigenvalue(2);
      }
    }
}

void sliceRidgeDetectProcessor::pickLargerEigenvalues(const vil_image_view<float>& e1,
                               const vil_image_view<float>& e2,
                               const vil_image_view<float>& e3,
                               const vil_image_view<float>& l1,
                               const vil_image_view<float>& l2,
                               const vil_image_view<float>& l3,
                               vil_image_view<float>& largest_eigen_x,
                               vil_image_view<float>& largest_eigen_y,
                               vil_image_view<float>& largest_eigen_z,
                               vil_image_view<float>& largest_lambda,
                               vil_image_view<float>& second_largest_eigen_x,
                               vil_image_view<float>& second_largest_eigen_y,
                               vil_image_view<float>& second_largest_eigen_z,
                               vil_image_view<float>& second_largest_lambda,
                               vil_image_view<float>& third_eigenvector)
{
        int ni = e1.ni();
        int nj = e1.nj();

        for(int j =0; j < nj; j++) {
        for(int i =0; i < ni; i++) {
                if(vcl_fabs(l1(i,j)) < vcl_fabs(l2(i,j))){
                        if(vcl_fabs(l2(i,j)) < vcl_fabs(l3(i,j))){
                                largest_eigen_x(i,j) = e3(i,j,0);
                                largest_eigen_y(i,j) = e3(i,j,1);
                                largest_eigen_z(i,j) = e3(i,j,2);
                                largest_lambda(i,j) = l3(i,j);

                                second_largest_eigen_x(i,j) = e2(i,j,0);
                                second_largest_eigen_y(i,j) = e2(i,j,1);
                                second_largest_eigen_z(i,j) = e2(i,j,2);
                                second_largest_lambda(i,j) = l2(i,j);

                                third_eigenvector(i,j,0) = e1(i,j,0);
                                third_eigenvector(i,j,1) = e1(i,j,1);
                                third_eigenvector(i,j,2) = e1(i,j,2);
                        }
                        else{
                                largest_eigen_x(i,j) = e2(i,j,0);
                                largest_eigen_y(i,j) = e2(i,j,1);
                                largest_eigen_z(i,j) = e2(i,j,2);
                                largest_lambda(i,j) = l2(i,j);

                                if(vcl_fabs(l1(i,j)) < vcl_fabs(l3(i,j))){
                                        second_largest_eigen_x(i,j) = e3(i,j,0);
                                        second_largest_eigen_y(i,j) = e3(i,j,1);
                                        second_largest_eigen_z(i,j) = e3(i,j,2);
                                        second_largest_lambda(i,j) = l3(i,j);

                                        third_eigenvector(i,j,0) = e1(i,j,0);
                                        third_eigenvector(i,j,1) = e1(i,j,1);
                                        third_eigenvector(i,j,2) = e1(i,j,2);
                                }
                                else{
                                        second_largest_eigen_x(i,j) = e1(i,j,0);
                                        second_largest_eigen_y(i,j) = e1(i,j,1);
                                        second_largest_eigen_z(i,j) = e1(i,j,2);
                                        second_largest_lambda(i,j) = l1(i,j);

                                        third_eigenvector(i,j,0) = e3(i,j,0);
                                        third_eigenvector(i,j,1) = e3(i,j,1);
                                        third_eigenvector(i,j,2) = e3(i,j,2);
                                }

                        }
                }
                else{
                        if(vcl_fabs(l1(i,j)) < vcl_fabs(l3(i,j))){
                                largest_eigen_x(i,j) = e3(i,j,0);
                                largest_eigen_y(i,j) = e3(i,j,1);
                                largest_eigen_z(i,j) = e3(i,j,2);
                                largest_lambda(i,j) = l3(i,j);

                                second_largest_eigen_x(i,j) = e1(i,j,0);
                                second_largest_eigen_y(i,j) = e1(i,j,1);
                                second_largest_eigen_z(i,j) = e1(i,j,2);
                                second_largest_lambda(i,j) = l1(i,j);

                                third_eigenvector(i,j,0) = e2(i,j,0);
                                third_eigenvector(i,j,1) = e2(i,j,1);
                                third_eigenvector(i,j,2) = e2(i,j,2);
                        }
                        else{
                                largest_eigen_x(i,j) = e1(i,j,0);
                                largest_eigen_y(i,j) = e1(i,j,1);
                                largest_eigen_z(i,j) = e1(i,j,2);
                                largest_lambda(i,j) = l1(i,j);

                                if(vcl_fabs(l2(i,j)) < vcl_fabs(l3(i,j))){
                                        second_largest_eigen_x(i,j) = e3(i,j,0);
                                        second_largest_eigen_y(i,j) = e3(i,j,1);
                                        second_largest_eigen_z(i,j) = e3(i,j,2);
                                        second_largest_lambda(i,j) = l3(i,j);

                                        third_eigenvector(i,j,0) = e2(i,j,0);
                                        third_eigenvector(i,j,1) = e2(i,j,1);
                                        third_eigenvector(i,j,2) = e2(i,j,2);
                                }
                                else{
                                        second_largest_eigen_x(i,j) = e2(i,j,0);
                                        second_largest_eigen_y(i,j) = e2(i,j,1);
                                        second_largest_eigen_z(i,j) = e2(i,j,2);
                                        second_largest_lambda(i,j) = l2(i,j);

                                        third_eigenvector(i,j,0) = e3(i,j,0);
                                        third_eigenvector(i,j,1) = e3(i,j,1);
                                        third_eigenvector(i,j,2) = e3(i,j,2);
                                }
                        }
                } 
        }
        }
}


void sliceRidgeDetectProcessor::computeRho3d( 
                     const vil3d_image_view<float>& gradient_x,
                     const vil3d_image_view<float>& gradient_y,
                     const vil3d_image_view<float>& gradient_z, 
                     const vil_image_view<float>& eigen_x,
                     const vil_image_view<float>& eigen_y,
                     const vil_image_view<float>& eigen_z,
                     const vil_image_view<float>& lambda,
                     const int & z_index,
                     vil_image_view<int>& rho)
{
        int ni = gradient_x.ni();
        int nj = gradient_x.nj();
        int nk = gradient_x.nk();

        vil_image_view<float> Ix_plus_e(ni,nj);
        vil_image_view<float> Ix_minus_e(ni,nj);
        vil_image_view<float> Iy_plus_e(ni,nj);
        vil_image_view<float> Iy_minus_e(ni,nj);
        vil_image_view<float> Iz_plus_e(ni,nj);
        vil_image_view<float> Iz_minus_e(ni,nj);

        const float* xdata = gradient_x.origin_ptr();
        const float* ydata = gradient_y.origin_ptr();
        const float* zdata = gradient_z.origin_ptr();

        vcl_ptrdiff_t x_istep = gradient_x.istep();
        vcl_ptrdiff_t x_jstep = gradient_x.jstep();
        vcl_ptrdiff_t x_kstep = gradient_x.kstep();
        vcl_ptrdiff_t y_istep = gradient_y.istep();
        vcl_ptrdiff_t y_jstep = gradient_y.jstep();
        vcl_ptrdiff_t y_kstep = gradient_y.kstep();
        vcl_ptrdiff_t z_istep = gradient_z.istep();
        vcl_ptrdiff_t z_jstep = gradient_z.jstep();
        vcl_ptrdiff_t z_kstep = gradient_z.kstep();


        int z =  static_cast<int>(nk/2.);

        for(int j =0; j < nj; j++) {
        for(int i =0; i < ni; i++) {

                double x_ind_p = i + epsilon_*eigen_x(i,j);
                double x_ind_m = i - epsilon_*eigen_x(i,j);

                double y_ind_p = j + epsilon_*eigen_y(i,j);
                double y_ind_m = j - epsilon_*eigen_y(i,j);

                double z_ind_p = z + epsilon_*eigen_z(i,j);
                double z_ind_m = z - epsilon_*eigen_z(i,j);

                Ix_plus_e(i,j)  = vil3d_trilin_interp_safe(x_ind_p, y_ind_p, z_ind_p, xdata,
                                ni,nj,nk, x_istep, x_jstep, x_kstep);
                Iy_plus_e(i,j)  = vil3d_trilin_interp_safe(x_ind_p, y_ind_p, z_ind_p, ydata,
                                ni,nj,nk, y_istep, y_jstep, y_kstep);
                Iz_plus_e(i,j)  = vil3d_trilin_interp_safe(x_ind_p, y_ind_p, z_ind_p, zdata,
                                ni,nj,nk, z_istep, z_jstep, z_kstep);

                Ix_minus_e(i,j)  = vil3d_trilin_interp_safe(x_ind_m, y_ind_m, z_ind_m, xdata,
                                ni,nj,nk, x_istep, x_jstep, x_kstep);
                Iy_minus_e(i,j)  = vil3d_trilin_interp_safe(x_ind_m, y_ind_m, z_ind_m, ydata,
                                ni,nj,nk, y_istep, y_jstep, y_kstep);
                Iz_minus_e(i,j)  = vil3d_trilin_interp_safe(x_ind_m, y_ind_m, z_ind_m, zdata,
                                ni,nj,nk, z_istep, z_jstep, z_kstep);
        }
        }

        // grad(I).nu (at +epsilon)
        vil_image_view<float> gI_dot_nu_plus_e;
        vil_image_view<float> Ix_p_times_ex;
        vil_image_view<float> Iy_p_times_ey;
        vil_image_view<float> Iz_p_times_ez;
        vil_math_image_product(Ix_plus_e,eigen_x,Ix_p_times_ex);
        vil_math_image_product(Iy_plus_e,eigen_y,Iy_p_times_ey);
        vil_math_image_product(Iz_plus_e,eigen_z,Iz_p_times_ez);

        vil_math_image_sum(Iy_p_times_ey,Iz_p_times_ez,gI_dot_nu_plus_e);
        vil_math_image_sum(Ix_p_times_ex,gI_dot_nu_plus_e,gI_dot_nu_plus_e);

        // grad(I).nu (at -epsilon)
        vil_image_view<float> gI_dot_nu_minus_e;
        vil_image_view<float> Ix_m_times_ex;
        vil_image_view<float> Iy_m_times_ey;
        vil_image_view<float> Iz_m_times_ez;
        vil_math_image_product(Ix_minus_e,eigen_x,Ix_m_times_ex);
        vil_math_image_product(Iy_minus_e,eigen_y,Iy_m_times_ey);
        vil_math_image_product(Iz_minus_e,eigen_z,Iz_m_times_ez);

        vil_math_image_sum(Iy_m_times_ey,Iz_m_times_ez,gI_dot_nu_minus_e);
        vil_math_image_sum(Ix_m_times_ex,gI_dot_nu_minus_e,gI_dot_nu_minus_e);

        // compute rho
        
        rho.set_size(ni,nj);


#define SIGN(X) (X < 0? -1 : (X==0 ? 0 : 1))
        for(int j =0; j < nj; j++) {
        for(int i =0; i < ni; i++) {
        rho(i,j) = (int)(-0.5*SIGN(lambda(i,j))*vcl_abs(SIGN(gI_dot_nu_plus_e(i,j)) - SIGN(gI_dot_nu_minus_e(i,j))));
        }
        }

#undef SIGN
}

vcl_vector<float*> sliceRidgeDetectProcessor::process(const vcl_vector< vcl_vector<float*> >& slice_sets, 
int w, int h, int z)
{
        int ni = w;
        int nj = h;

        vil_image_view<float> largest_eigen_x(ni,nj);
        vil_image_view<float> largest_eigen_y(ni,nj);
        vil_image_view<float> largest_eigen_z(ni,nj);
        vil_image_view<float> second_largest_eigen_x(ni,nj);
        vil_image_view<float> second_largest_eigen_y(ni,nj);
        vil_image_view<float> second_largest_eigen_z(ni,nj);

        vcl_vector<float*> toreturn;
        toreturn.push_back( new float[w*h]);
        toreturn.push_back( new float[w*h]);
        toreturn.push_back( new float[w*h]);
        vil_image_view<float> rho(toreturn[0],w,h,1,1,w,w*h);
        vil_image_view<float> lambda1(toreturn[1],w,h,1,1,w,w*h);
        vil_image_view<float> lambda2(toreturn[2],w,h,1,1,w,w*h);
        vil_image_view<float> eigenv3(ni,nj,3);

        vil3d_image_view<float> local_gx(w,h,this->nslices());
        vil3d_image_view<float> local_gy(w,h,this->nslices());
        vil3d_image_view<float> local_gz(w,h,this->nslices());

        {
        vil_image_view<float> e1;
        vil_image_view<float> e2;
        vil_image_view<float> e3;
        vil_image_view<float> l1;
        vil_image_view<float> l2;
        vil_image_view<float> l3;

        vil_image_view<float> Im_xx;
        vil_image_view<float> Im_yy;
        vil_image_view<float> Im_zz;
        vil_image_view<float> Im_xy;
        vil_image_view<float> Im_xz;
        vil_image_view<float> Im_yz;



        for(int i = 0; i < this->nslices(); i++){
                vcl_memcpy(local_gx.origin_ptr() + i*local_gx.kstep(),slice_sets[0][i],w*h*sizeof(float));
                vcl_memcpy(local_gy.origin_ptr() + i*local_gy.kstep(),slice_sets[1][i],w*h*sizeof(float));
                vcl_memcpy(local_gz.origin_ptr() + i*local_gz.kstep(),slice_sets[2][i],w*h*sizeof(float));

        }

        vil3d_image_view<float> gxx;
        vil3d_image_view<float> gyy;
        vil3d_image_view<float> gzz;
        vil3d_image_view<float> gxy;
        vil3d_image_view<float> gxz;
        vil3d_image_view<float> gyz;
        vil3d_image_view<float> crap;

        vil3d_grad_1x3(local_gx,gxx,gxy,gxz);
        vil3d_grad_1x3(local_gy,crap,gyy,gyz);
        vil3d_grad_1x3(local_gz,crap,crap,gzz);

        Im_xx = vil3d_slice_ij(gxx,(int)(this->nslices()/2.));
        Im_yy = vil3d_slice_ij(gyy,(int)(this->nslices()/2.));
        Im_zz = vil3d_slice_ij(gzz,(int)(this->nslices()/2.));
        Im_xy = vil3d_slice_ij(gxy,(int)(this->nslices()/2.));
        Im_xz = vil3d_slice_ij(gxz,(int)(this->nslices()/2.));
        Im_yz = vil3d_slice_ij(gyz,(int)(this->nslices()/2.));

        vcl_cout << " Hessian Decompose ...";
        this->hessian_decompose_oneslice( 
                        Im_xx, Im_yy, Im_zz, 
                        Im_xy, Im_xz, Im_yz,
                        e1, e2, e3,
                        l1, l2, l3);

        vcl_cout << "done\n";

        vcl_cout << " Pick Larger Eigenvalues ...";
        this->pickLargerEigenvalues(e1, e2, e3, l1, l2, l3,
                              largest_eigen_x, largest_eigen_y, largest_eigen_z, 
                              lambda1, 
                              second_largest_eigen_x, second_largest_eigen_y, second_largest_eigen_z, 
                              lambda2,
                              eigenv3);
        vcl_cout << "done\n";
        }
        vil_image_view<int> rho_largest(ni,nj,1);
        vil_image_view<int> rho_second_largest(ni,nj,1);

        vcl_cout << " Compute Rho3d (largest) ...";
     this->computeRho3d(local_gx,local_gy,local_gz, largest_eigen_x, largest_eigen_y, largest_eigen_z, 
                  lambda1, z,
                  rho_largest);
     vcl_cout << "done\n";

     vcl_cout << " Compute Rho3d (2nd largest) ...";
     computeRho3d( local_gx,local_gy,local_gz, second_largest_eigen_x, second_largest_eigen_y, second_largest_eigen_z, 
                  lambda2,z,
                  rho_second_largest);
     vcl_cout << "done\n";

     vcl_cout << " Combine results ...";
     vil_math_image_sum(rho_largest,rho_second_largest,rho);
     vil_math_scale_values(rho, 0.5);
     vcl_cout << "done\n";

     return toreturn;
}

