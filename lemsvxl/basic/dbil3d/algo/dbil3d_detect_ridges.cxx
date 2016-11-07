#include "dbil3d_detect_ridges.h"
#include "dbil3d_hessian_decompose.h"
#include <vnl/vnl_math.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <vnl/vnl_matrix.h>
#include <vil3d/vil3d_switch_axes.h>
#include <vil3d/vil3d_trilin_interp.h>
#include <vil3d/algo/vil3d_grad_1x3.h>
#include <vil3d/vil3d_convert.h>
#include <vil3d/vil3d_math.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/io/vil3d_io_image_view.h>

void pickLargerEigenvalues(const vil3d_image_view<float>& e1,
                           const vil3d_image_view<float>& e2,
                           const vil3d_image_view<float>& e3,
                           const vil3d_image_view<float>& l1,
                           const vil3d_image_view<float>& l2,
                           const vil3d_image_view<float>& l3,
                           vil3d_image_view<float>& largest_eigen_x,
                           vil3d_image_view<float>& largest_eigen_y,
                           vil3d_image_view<float>& largest_eigen_z,
                           vil3d_image_view<float>& largest_lambda,
                           vil3d_image_view<float>& second_largest_eigen_x,
                           vil3d_image_view<float>& second_largest_eigen_y,
                           vil3d_image_view<float>& second_largest_eigen_z,
                           vil3d_image_view<float>& second_largest_lambda,
                           vil3d_image_view<float>& third_eigenvector)
{
        int ni = e1.ni();
        int nj = e1.nj();
        int nk = e1.nk();

        for(int k =0; k < nk; k++) {
        for(int j =0; j < nj; j++) {
        for(int i =0; i < ni; i++) {
                if(vcl_fabs(l1(i,j,k)) < vcl_fabs(l2(i,j,k))){
                        if(vcl_fabs(l2(i,j,k)) < vcl_fabs(l3(i,j,k))){
                                largest_eigen_x(i,j,k) = e3(i,j,k,0);
                                largest_eigen_y(i,j,k) = e3(i,j,k,1);
                                largest_eigen_z(i,j,k) = e3(i,j,k,2);
                                largest_lambda(i,j,k) = l3(i,j,k);

                                second_largest_eigen_x(i,j,k) = e2(i,j,k,0);
                                second_largest_eigen_y(i,j,k) = e2(i,j,k,1);
                                second_largest_eigen_z(i,j,k) = e2(i,j,k,2);
                                second_largest_lambda(i,j,k) = l2(i,j,k);

                                third_eigenvector(i,j,k,0) = e1(i,j,k,0);
                                third_eigenvector(i,j,k,1) = e1(i,j,k,1);
                                third_eigenvector(i,j,k,2) = e1(i,j,k,2);
                        }
                        else{
                                largest_eigen_x(i,j,k) = e2(i,j,k,0);
                                largest_eigen_y(i,j,k) = e2(i,j,k,1);
                                largest_eigen_z(i,j,k) = e2(i,j,k,2);
                                largest_lambda(i,j,k) = l2(i,j,k);

                                if(vcl_fabs(l1(i,j,k)) < vcl_fabs(l3(i,j,k))){
                                        second_largest_eigen_x(i,j,k) = e3(i,j,k,0);
                                        second_largest_eigen_y(i,j,k) = e3(i,j,k,1);
                                        second_largest_eigen_z(i,j,k) = e3(i,j,k,2);
                                        second_largest_lambda(i,j,k) = l3(i,j,k);

                                        third_eigenvector(i,j,k,0) = e1(i,j,k,0);
                                        third_eigenvector(i,j,k,1) = e1(i,j,k,1);
                                        third_eigenvector(i,j,k,2) = e1(i,j,k,2);
                                }
                                else{
                                        second_largest_eigen_x(i,j,k) = e1(i,j,k,0);
                                        second_largest_eigen_y(i,j,k) = e1(i,j,k,1);
                                        second_largest_eigen_z(i,j,k) = e1(i,j,k,2);
                                        second_largest_lambda(i,j,k) = l1(i,j,k);

                                        third_eigenvector(i,j,k,0) = e3(i,j,k,0);
                                        third_eigenvector(i,j,k,1) = e3(i,j,k,1);
                                        third_eigenvector(i,j,k,2) = e3(i,j,k,2);
                                }

                        }
                }
                else{
                        if(vcl_fabs(l1(i,j,k)) < vcl_fabs(l3(i,j,k))){
                                largest_eigen_x(i,j,k) = e3(i,j,k,0);
                                largest_eigen_y(i,j,k) = e3(i,j,k,1);
                                largest_eigen_z(i,j,k) = e3(i,j,k,2);
                                largest_lambda(i,j,k) = l3(i,j,k);

                                second_largest_eigen_x(i,j,k) = e1(i,j,k,0);
                                second_largest_eigen_y(i,j,k) = e1(i,j,k,1);
                                second_largest_eigen_z(i,j,k) = e1(i,j,k,2);
                                second_largest_lambda(i,j,k) = l1(i,j,k);

                                third_eigenvector(i,j,k,0) = e2(i,j,k,0);
                                third_eigenvector(i,j,k,1) = e2(i,j,k,1);
                                third_eigenvector(i,j,k,2) = e2(i,j,k,2);
                        }
                        else{
                                largest_eigen_x(i,j,k) = e1(i,j,k,0);
                                largest_eigen_y(i,j,k) = e1(i,j,k,1);
                                largest_eigen_z(i,j,k) = e1(i,j,k,2);
                                largest_lambda(i,j,k) = l1(i,j,k);

                                if(vcl_fabs(l2(i,j,k)) < vcl_fabs(l3(i,j,k))){
                                        second_largest_eigen_x(i,j,k) = e3(i,j,k,0);
                                        second_largest_eigen_y(i,j,k) = e3(i,j,k,1);
                                        second_largest_eigen_z(i,j,k) = e3(i,j,k,2);
                                        second_largest_lambda(i,j,k) = l3(i,j,k);

                                        third_eigenvector(i,j,k,0) = e2(i,j,k,0);
                                        third_eigenvector(i,j,k,1) = e2(i,j,k,1);
                                        third_eigenvector(i,j,k,2) = e2(i,j,k,2);
                                }
                                else{
                                        second_largest_eigen_x(i,j,k) = e2(i,j,k,0);
                                        second_largest_eigen_y(i,j,k) = e2(i,j,k,1);
                                        second_largest_eigen_z(i,j,k) = e2(i,j,k,2);
                                        second_largest_lambda(i,j,k) = l2(i,j,k);

                                        third_eigenvector(i,j,k,0) = e3(i,j,k,0);
                                        third_eigenvector(i,j,k,1) = e3(i,j,k,1);
                                        third_eigenvector(i,j,k,2) = e3(i,j,k,2);
                                }
                        }
                } 
        }
        }
        }
}

void computeRho3d(const vil3d_image_view<float>& Ix,
                  const vil3d_image_view<float>& Iy,
                  const vil3d_image_view<float>& Iz,
                  const vil3d_image_view<float>& eigen_x,
                  const vil3d_image_view<float>& eigen_y,
                  const vil3d_image_view<float>& eigen_z,
                  const vil3d_image_view<float>& lambda,
                  const float& epsilon,
                  vil3d_image_view<int>& rho)
{
        int ni = Ix.ni();
        int nj = Ix.nj();
        int nk = Ix.nk();

        vil3d_image_view<float> Ix_plus_e(ni,nj,nk);
        vil3d_image_view<float> Ix_minus_e(ni,nj,nk);
        vil3d_image_view<float> Iy_plus_e(ni,nj,nk);
        vil3d_image_view<float> Iy_minus_e(ni,nj,nk);
        vil3d_image_view<float> Iz_plus_e(ni,nj,nk);
        vil3d_image_view<float> Iz_minus_e(ni,nj,nk);

        const float* xdata = Ix.origin_ptr();
        const float* ydata = Iy.origin_ptr();
        const float* zdata = Iz.origin_ptr();

        vcl_ptrdiff_t x_istep = Ix.istep();
        vcl_ptrdiff_t x_jstep = Ix.jstep();
        vcl_ptrdiff_t x_kstep = Ix.kstep();
        vcl_ptrdiff_t y_istep = Iy.istep();
        vcl_ptrdiff_t y_jstep = Iy.jstep();
        vcl_ptrdiff_t y_kstep = Iy.kstep();
        vcl_ptrdiff_t z_istep = Iz.istep();
        vcl_ptrdiff_t z_jstep = Iz.jstep();
        vcl_ptrdiff_t z_kstep = Iz.kstep();

        for(int k =0; k < nk; k++) {
        for(int j =0; j < nj; j++) {
        for(int i =0; i < ni; i++) {

                double x_ind_p = i + epsilon*eigen_x(i,j,k);
                double x_ind_m = i - epsilon*eigen_x(i,j,k);

                double y_ind_p = j + epsilon*eigen_y(i,j,k);
                double y_ind_m = j - epsilon*eigen_y(i,j,k);

                double z_ind_p = k + epsilon*eigen_z(i,j,k);
                double z_ind_m = k - epsilon*eigen_z(i,j,k);

                Ix_plus_e(i,j,k)  = vil3d_trilin_interp_safe(x_ind_p, y_ind_p, z_ind_p, xdata,
                                ni,nj,nk, x_istep, x_jstep, x_kstep);
                Iy_plus_e(i,j,k)  = vil3d_trilin_interp_safe(x_ind_p, y_ind_p, z_ind_p, ydata,
                                ni,nj,nk, y_istep, y_jstep, y_kstep);
                Iz_plus_e(i,j,k)  = vil3d_trilin_interp_safe(x_ind_p, y_ind_p, z_ind_p, zdata,
                                ni,nj,nk, z_istep, z_jstep, z_kstep);

                Ix_minus_e(i,j,k)  = vil3d_trilin_interp_safe(x_ind_m, y_ind_m, z_ind_m, xdata,
                                ni,nj,nk, x_istep, x_jstep, x_kstep);
                Iy_minus_e(i,j,k)  = vil3d_trilin_interp_safe(x_ind_m, y_ind_m, z_ind_m, ydata,
                                ni,nj,nk, y_istep, y_jstep, y_kstep);
                Iz_minus_e(i,j,k)  = vil3d_trilin_interp_safe(x_ind_m, y_ind_m, z_ind_m, zdata,
                                ni,nj,nk, z_istep, z_jstep, z_kstep);
        }
        }
        }

        // grad(I).nu (at +epsilon)
        vil3d_image_view<float> gI_dot_nu_plus_e;
        vil3d_image_view<float> Ix_p_times_ex;
        vil3d_image_view<float> Iy_p_times_ey;
        vil3d_image_view<float> Iz_p_times_ez;
        vil3d_math_image_product(Ix_plus_e,eigen_x,Ix_p_times_ex);
        vil3d_math_image_product(Iy_plus_e,eigen_y,Iy_p_times_ey);
        vil3d_math_image_product(Iz_plus_e,eigen_z,Iz_p_times_ez);

        vil3d_math_image_sum(Iy_p_times_ey,Iz_p_times_ez,gI_dot_nu_plus_e);
        vil3d_math_image_sum(Ix_p_times_ex,gI_dot_nu_plus_e,gI_dot_nu_plus_e);

        // grad(I).nu (at -epsilon)
        vil3d_image_view<float> gI_dot_nu_minus_e;
        vil3d_image_view<float> Ix_m_times_ex;
        vil3d_image_view<float> Iy_m_times_ey;
        vil3d_image_view<float> Iz_m_times_ez;
        vil3d_math_image_product(Ix_minus_e,eigen_x,Ix_m_times_ex);
        vil3d_math_image_product(Iy_minus_e,eigen_y,Iy_m_times_ey);
        vil3d_math_image_product(Iz_minus_e,eigen_z,Iz_m_times_ez);

        vil3d_math_image_sum(Iy_m_times_ey,Iz_m_times_ez,gI_dot_nu_minus_e);
        vil3d_math_image_sum(Ix_m_times_ex,gI_dot_nu_minus_e,gI_dot_nu_minus_e);

        // compute rho
        
        rho.set_size(ni,nj,nk);


#define SIGN(X) (X < 0? -1 : (X==0 ? 0 : 1))
        for(int k =0; k < nk; k++) {
        for(int j =0; j < nj; j++) {
        for(int i =0; i < ni; i++) {
        rho(i,j,k) = (int)(-0.5*SIGN(lambda(i,j,k))*vcl_abs(SIGN(gI_dot_nu_plus_e(i,j,k)) - SIGN(gI_dot_nu_minus_e(i,j,k))));
        }
        }
        }

#undef SIGN

}

void dbil3d_detect_ridges( const vil3d_image_view<float>& gradient_x,
                           const vil3d_image_view<float>& gradient_y,
                           const vil3d_image_view<float>& gradient_z,
                           const float& epsilon,
                           vil3d_image_view<int>& rho,
                           vil3d_image_view<float>& lambda1,
                           vil3d_image_view<float>& lambda2,
                           vil3d_image_view<float>& eigenv3)
{
vil3d_image_view<float> Im_xx;
vil3d_image_view<float> Im_xy;
vil3d_image_view<float> Im_xz;
vil3d_grad_1x3(gradient_x,Im_xx,Im_xy,Im_xz);

vil3d_image_view<float> Im_yy;
vil3d_image_view<float> Im_yz;
vil3d_image_view<float> garbage;
vil3d_grad_1x3(gradient_y,garbage,Im_yy,Im_yz);

vil3d_image_view<float> Im_zz;
vil3d_grad_1x3(gradient_z,garbage,garbage,Im_zz);
dbil3d_detect_ridges(gradient_x, gradient_y, gradient_z, 
                Im_xx, Im_yy, Im_zz,
                Im_xy, Im_xz, Im_yz, 
                epsilon,
                rho, lambda1, lambda2, eigenv3);
}

void dbil3d_detect_ridges( const vil3d_image_view<float>& gradient_x,
                const vil3d_image_view<float>& gradient_y,
                const vil3d_image_view<float>& gradient_z, 
                const vil3d_image_view<float>& Im_xx,
                const vil3d_image_view<float>& Im_yy,
                const vil3d_image_view<float>& Im_zz,
                const vil3d_image_view<float>& Im_xy,
                const vil3d_image_view<float>& Im_xz,
                const vil3d_image_view<float>& Im_yz, 
                const float& epsilon,
                vil3d_image_view<int>& rho,
                vil3d_image_view<float>& lambda1,
                vil3d_image_view<float>& lambda2,
                vil3d_image_view<float>& eigenv3)
{
        int ni = gradient_x.ni();
        int nj = gradient_x.nj();
        int nk = gradient_x.nk();

        vil3d_image_view<float> largest_eigen_x(ni,nj,nk);
        vil3d_image_view<float> largest_eigen_y(ni,nj,nk);
        vil3d_image_view<float> largest_eigen_z(ni,nj,nk);
        vil3d_image_view<float> second_largest_eigen_x(ni,nj,nk);
        vil3d_image_view<float> second_largest_eigen_y(ni,nj,nk);
        vil3d_image_view<float> second_largest_eigen_z(ni,nj,nk);
        lambda1.set_size(ni,nj,nk);
        lambda2.set_size(ni,nj,nk);
        eigenv3.set_size(ni,nj,nk,3);


        {
        vil3d_image_view<float> e1;
        vil3d_image_view<float> e2;
        vil3d_image_view<float> e3;
        vil3d_image_view<float> l1;
        vil3d_image_view<float> l2;
        vil3d_image_view<float> l3;

        vcl_cerr << " Hessian Decompose ...";
        dbil3d_hessian_decompose( Im_xx, Im_yy, Im_zz, 
                                  Im_xy, Im_xz, Im_yz,
                                  e1, e2, e3,
                                  l1, l2, l3);

        vcl_cerr << "done\n";

        vcl_cerr << " Pick Larger Eigenvalues ...";
        pickLargerEigenvalues(e1, e2, e3, l1, l2, l3,
                              largest_eigen_x, largest_eigen_y, largest_eigen_z, 
                              lambda1, 
                              second_largest_eigen_x, second_largest_eigen_y, second_largest_eigen_z, 
                              lambda2,
                              eigenv3);
        vcl_cerr << "done\n";
        }
        vil3d_image_view<int> rho_largest(ni,nj,nk);
        vil3d_image_view<int> rho_second_largest(ni,nj,nk);

        vcl_cerr << " Compute Rho3d (largest) ...";
     computeRho3d(gradient_x, gradient_y, gradient_z,
                  largest_eigen_x, largest_eigen_y, largest_eigen_z, 
                  lambda1,
                  epsilon, 
                  rho_largest);
     vcl_cerr << "done\n";

     vcl_cerr << " Compute Rho3d (2nd largest) ...";
     computeRho3d(gradient_x, gradient_y, gradient_z,
                  second_largest_eigen_x, second_largest_eigen_y, second_largest_eigen_z, 
                  lambda2,
                  epsilon,
                  rho_second_largest);
     vcl_cerr << "done\n";

     vcl_cerr << " Combine results ...";
     vil3d_math_image_sum(rho_largest,rho_second_largest,rho);
     vil3d_math_scale_and_offset_values(rho, 0.5, 0);
     vcl_cerr << "done\n";
}
