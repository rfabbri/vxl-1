#include "dbil3d_hessian_decompose.h"
#include <vil3d/algo/vil3d_grad_1x3.h>
#include <vnl/vnl_math.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <vnl/vnl_matrix.h>

void dbil3d_hessian_decompose( const vil3d_image_view<float>& gradient_x,
                               const vil3d_image_view<float>& gradient_y,
                               const vil3d_image_view<float>& gradient_z,
                               vil3d_image_view<float>& e1,
                               vil3d_image_view<float>& e2,
                               vil3d_image_view<float>& e3,
                               vil3d_image_view<float>& l1,
                               vil3d_image_view<float>& l2,
                               vil3d_image_view<float>& l3)
{
unsigned ni = gradient_x.ni();
unsigned nj = gradient_x.nj();
unsigned nk = gradient_x.nk();


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

 dbil3d_hessian_decompose(Im_xx, Im_yy, Im_zz,
                          Im_xy, Im_xz, Im_yz,
                          e1, e2, e3,
                          l1, l2, l3);

}
void dbil3d_hessian_decompose( const vil3d_image_view<float>& Im_xx,
                               const vil3d_image_view<float>& Im_yy,
                               const vil3d_image_view<float>& Im_zz,
                               const vil3d_image_view<float>& Im_xy,
                               const vil3d_image_view<float>& Im_xz,
                               const vil3d_image_view<float>& Im_yz,
                               vil3d_image_view<float>& e1,
                               vil3d_image_view<float>& e2,
                               vil3d_image_view<float>& e3,
                               vil3d_image_view<float>& l1,
                               vil3d_image_view<float>& l2,
                               vil3d_image_view<float>& l3)
{
unsigned ni = Im_xx.ni();
unsigned nj = Im_xx.nj();
unsigned nk = Im_xx.nk();


e1.set_size(ni,nj,nk,3);
e2.set_size(ni,nj,nk,3);
e3.set_size(ni,nj,nk,3);
l1.set_size(ni,nj,nk);
l2.set_size(ni,nj,nk);
l3.set_size(ni,nj,nk);

vil3d_image_view<float> result(ni,nj,nk);

  for(int z=0; z<nk; z++) {
    for(int y=0; y<nj; y++) {
      for(int x=0; x<ni; x++) {
        vnl_matrix<float> mat(3,3);
        mat[0][0] = Im_xx(x,y,z);
        mat[0][1] = Im_xy(x,y,z);
        mat[0][2] = Im_xz(x,y,z);
        mat[1][0] = Im_xy(x,y,z);
        mat[1][1] = Im_yy(x,y,z); 
        mat[1][2] = Im_yz(x,y,z);
        mat[2][0] = Im_xz(x,y,z);
        mat[2][1] = Im_yz(x,y,z);
        mat[2][2] = Im_zz(x,y,z);
        vnl_symmetric_eigensystem<float> system(mat);
        //vnl_symmetric_eigensystem stores eigenvectors by increasing
        //eigenvalue

        vnl_vector<float> e1v = system.get_eigenvector(0);
        e3(x,y,z,0) = e1v[0];
        e3(x,y,z,1) = e1v[1];
        e3(x,y,z,2) = e1v[2];
        l3(x,y,z) = system.get_eigenvalue(0);

        vnl_vector<float> e2v = system.get_eigenvector(1);
        e2(x,y,z,0) = e2v[0];
        e2(x,y,z,1) = e2v[1];
        e2(x,y,z,2) = e2v[2];
        l2(x,y,z) = system.get_eigenvalue(1);

        vnl_vector<float> e3v = system.get_eigenvector(2);
        e1(x,y,z,0) = e3v[0];
        e1(x,y,z,1) = e3v[1];
        e1(x,y,z,2) = e3v[2];
        l1(x,y,z) = system.get_eigenvalue(2);
      }
    }
  }
}
