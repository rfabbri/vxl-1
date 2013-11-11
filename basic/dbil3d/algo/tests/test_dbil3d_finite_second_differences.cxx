#include <testlib/testlib_test.h>
#include <vil3d/vil3d_print.h>
#include <vcl_cstdio.h>
#include <vcl_cmath.h>
#include <vnl/vnl_random.h>
#include <vnl/vnl_math.h>
#include <dbil3d/algo/dbil3d_finite_differences.h>
#include <dbil3d/algo/dbil3d_finite_second_differences.h>


MAIN( test_dbil3d_finite_second_differences )
{
  START ("2D Finite Second Differences");

  int dim = 5; 
  vil3d_image_view<float> testim(dim,dim,dim); 
  vil3d_image_view<float> dxp,dxm,dxc;
  vil3d_image_view<float> dyp,dym,dyc;
  vil3d_image_view<float> dzp,dzm,dzc;
  vil3d_image_view<float> dxx,dyy,dzz;
  vil3d_image_view<float> dxy,dxz,dyz;

  testim.fill(1); 
  dbil3d_finite_differences(testim, dxp, dxm, dxc, dyp, dym, dyc ,dzp, dzm, dzc); 
  dbil3d_finite_second_differences(testim, dxp, dxm, dyp, dym,dzp,dzm,dxx,dyy,dzz,dxy,dxz,dyz); 

  float dxxsum=0;
  float dyysum=0;
  float dzzsum=0;
  float dxysum=0;
  float dxzsum=0;
  float dyzsum=0;

  for(unsigned k = 0 ; k < testim.nk(); k++){
  for(unsigned j = 0 ; j < testim.nj(); j++){
  for(unsigned i = 0 ; i < testim.ni(); i++){
                  dxxsum+=dxx(i,j,k);
                  dyysum+=dyy(i,j,k);
                  dxysum+=dxy(i,j,k); 
                  dzzsum+=dzz(i,j,k);
                  dxzsum+=dxz(i,j,k); 
                  dyzsum+=dyz(i,j,k); 
  }
  }
  }

  TEST_NEAR("D_xx constant image", dxxsum,0, 0.001);
  TEST_NEAR("D_yy constant image", dyysum,0, 0.001);
  TEST_NEAR("D_zz constant image", dzzsum,0, 0.001);
  TEST_NEAR("D_xy constant image", dxysum,0, 0.001);
  TEST_NEAR("D_xz constant image", dxzsum,0, 0.001);
  TEST_NEAR("D_yz constant image", dyzsum,0, 0.001);


  for(unsigned k = 0 ; k < testim.nk(); k++){
  for(unsigned j = 0 ; j < testim.nj(); j++){
  for(unsigned i = 0 ; i < testim.ni(); i++){
    if(i < static_cast<unsigned>(dim/2)){
      testim(i,j,k) = 1;
    }
    else{
      testim(i,j,k) = 0;
    }

  }
  }
  } 
  dbil3d_finite_differences(testim, dxp, dxm, dxc, dyp, dym, dyc ,dzp, dzm, dzc); 
  dbil3d_finite_second_differences(testim, dxp, dxm, dyp, dym,dzp,dzm,dxx,dyy,dzz,dxy,dxz,dyz); 

  vcl_cout << "testing half-one half-zero (along i-axis) image" << vcl_endl;
  for(unsigned k = 0 ; k < testim.nk(); k++){
  for(unsigned j = 0 ; j < testim.nj(); j++){
          unsigned i = 0;
          TEST_NEAR("D_xx"  ,     dxx(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_yy"  ,     dyy(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_zz"  ,     dzz(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_xy"  ,     dxy(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_xz"  ,     dxz(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_yz"  ,     dyz(i,j,k)    ,    0    ,     0.001);

          i = testim.ni() - 1;
          TEST_NEAR("D_xx"  ,     dxx(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_yy"  ,     dyy(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_zz"  ,     dzz(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_xy"  ,     dxy(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_xz"  ,     dxz(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_yz"  ,     dyz(i,j,k)    ,    0    ,     0.001);


          i = dim/2 - 1;
          TEST_NEAR("D_xx"  ,     dxx(i,j,k)    ,    -1    ,     0.001);
          TEST_NEAR("D_yy"  ,     dyy(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_zz"  ,     dzz(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_xy"  ,     dxy(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_xz"  ,     dxz(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_yz"  ,     dyz(i,j,k)    ,    0    ,     0.001);

          i = dim/2;
          TEST_NEAR("D_xx"  ,     dxx(i,j,k)    ,    1    ,     0.001);
          TEST_NEAR("D_yy"  ,     dyy(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_zz"  ,     dzz(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_xy"  ,     dxy(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_xz"  ,     dxz(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_yz"  ,     dyz(i,j,k)    ,    0    ,     0.001);
  }
  }

  for(unsigned k = 0 ; k < testim.nk(); k++){
  for(unsigned j = 0 ; j < testim.nj(); j++){
          for(unsigned i = 0 ; i < testim.ni(); i++){
                  if(j < static_cast<unsigned>(dim/2)){
                          testim(i,j,k) = 1;
                  }
                  else{
                          testim(i,j,k) = 0;
                  }

          }
  }
  }

  dbil3d_finite_differences(testim, dxp, dxm, dxc, dyp, dym, dyc ,dzp, dzm, dzc); 
  dbil3d_finite_second_differences(testim, dxp, dxm, dyp, dym,dzp,dzm,dxx,dyy,dzz,dxy,dxz,dyz); 

  vcl_cout << "testing half-one half-zero (along j-axis) image" << vcl_endl;
  for(unsigned k = 0 ; k < testim.nk(); k++){
  for(unsigned i = 0 ; i < testim.ni(); i++){
          unsigned j = 0;
          TEST_NEAR("D_xx"  ,     dxx(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_yy"  ,     dyy(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_zz"  ,     dzz(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_xy"  ,     dxy(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_xz"  ,     dxz(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_yz"  ,     dyz(i,j,k)    ,    0    ,     0.001);

          j = testim.ni() - 1;
          TEST_NEAR("D_xx"  ,     dxx(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_yy"  ,     dyy(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_zz"  ,     dzz(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_xy"  ,     dxy(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_xz"  ,     dxz(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_yz"  ,     dyz(i,j,k)    ,    0    ,     0.001);


          j = dim/2 - 1;
          TEST_NEAR("D_xx"  ,     dxx(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_yy"  ,     dyy(i,j,k)    ,   -1    ,     0.001);
          TEST_NEAR("D_zz"  ,     dzz(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_xy"  ,     dxy(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_xz"  ,     dxz(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_yz"  ,     dyz(i,j,k)    ,    0    ,     0.001);

          j = dim/2;
          TEST_NEAR("D_xx"  ,     dxx(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_yy"  ,     dyy(i,j,k)    ,    1    ,     0.001);
          TEST_NEAR("D_zz"  ,     dzz(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_xy"  ,     dxy(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_xz"  ,     dxz(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_yz"  ,     dyz(i,j,k)    ,    0    ,     0.001);
  }
  }
  

  for(unsigned k = 0 ; k < testim.nk(); k++){
  for(unsigned j = 0 ; j < testim.nj(); j++){
          for(unsigned i = 0 ; i < testim.ni(); i++){
                  if(k < static_cast<unsigned>(dim/2)){
                          testim(i,j,k) = 1;
                  }
                  else{
                          testim(i,j,k) = 0;
                  }

          }
  }
  }

  dbil3d_finite_differences(testim, dxp, dxm, dxc, dyp, dym, dyc ,dzp, dzm, dzc); 
  dbil3d_finite_second_differences(testim, dxp, dxm, dyp, dym,dzp,dzm,dxx,dyy,dzz,dxy,dxz,dyz); 

  vcl_cout << "testing half-one half-zero (along k-axis) image" << vcl_endl;
  for(unsigned j = 0 ; j < testim.nj(); j++){
  for(unsigned i = 0 ; i < testim.ni(); i++){
          unsigned k = 0;
          TEST_NEAR("D_xx"  ,     dxx(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_yy"  ,     dyy(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_zz"  ,     dzz(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_xy"  ,     dxy(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_xz"  ,     dxz(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_yz"  ,     dyz(i,j,k)    ,    0    ,     0.001);

          k = testim.ni() - 1;
          TEST_NEAR("D_xx"  ,     dxx(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_yy"  ,     dyy(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_zz"  ,     dzz(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_xy"  ,     dxy(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_xz"  ,     dxz(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_yz"  ,     dyz(i,j,k)    ,    0    ,     0.001);


          k = dim/2 - 1;
          TEST_NEAR("D_xx"  ,     dxx(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_yy"  ,     dyy(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_zz"  ,     dzz(i,j,k)    ,   -1    ,     0.001);
          TEST_NEAR("D_xy"  ,     dxy(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_xz"  ,     dxz(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_yz"  ,     dyz(i,j,k)    ,    0    ,     0.001);

          k = dim/2;
          TEST_NEAR("D_xx"  ,     dxx(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_yy"  ,     dyy(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_zz"  ,     dzz(i,j,k)    ,    1    ,     0.001);
          TEST_NEAR("D_xy"  ,     dxy(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_xz"  ,     dxz(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_yz"  ,     dyz(i,j,k)    ,    0    ,     0.001);
  }
  }
  


  for(unsigned k = 0 ; k < testim.nk(); k++){
  for(unsigned j = 0 ; j < testim.nj(); j++){
          for(unsigned i = 0 ; i < testim.ni(); i++){
                  if(i==j){
                          testim(i,j,k) = 1;
                  }
                  else{
                          testim(i,j,k) = 0;
                  }

          }
  }
  }

  dbil3d_finite_differences(testim, dxp, dxm, dxc, dyp, dym, dyc ,dzp, dzm, dzc); 
  dbil3d_finite_second_differences(testim, dxp, dxm, dyp, dym,dzp,dzm,dxx,dyy,dzz,dxy,dxz,dyz); 

  vcl_cout << "testing i/j diagonal image" << vcl_endl;
  for(unsigned k = 0 ; k < testim.nk(); k++){
  for(unsigned j = 0 ; j < testim.nj(); j++){
  for(unsigned i = 0 ; i < testim.ni(); i++){
          if(i == 0 && j == 0 || i == testim.ni()-1 && j == testim.nj()-1){
                  vcl_cout << "corners on diagonal " << vcl_endl;
                  TEST_NEAR("D_xx"  ,     dxx(i,j,k)    ,    -1    ,     0.001);
                  TEST_NEAR("D_yy"  ,     dyy(i,j,k)    ,    -1    ,     0.001);
                  TEST_NEAR("D_zz"  ,     dzz(i,j,k)    ,     0    ,     0.001);
          }
          else if(i == j){
                  vcl_cout << "non-corner on diagonal " << vcl_endl;
                  TEST_NEAR("D_xx"  ,     dxx(i,j,k)    ,    -2    ,     0.001);
                  TEST_NEAR("D_yy"  ,     dyy(i,j,k)    ,    -2    ,     0.001);
                  TEST_NEAR("D_zz"  ,     dzz(i,j,k)    ,     0    ,     0.001);
          }
          else if(i == j + 1 || i == j-1 ){
                  vcl_cout << "super/sub diagonal " << vcl_endl;
                  TEST_NEAR("D_xx"  ,     dxx(i,j,k)    ,     1    ,     0.001);
                  TEST_NEAR("D_yy"  ,     dyy(i,j,k)    ,     1    ,     0.001);
                  TEST_NEAR("D_zz"  ,     dzz(i,j,k)    ,     0    ,     0.001);
          }
          else{
                  vcl_cout << "elsewhere " << vcl_endl;
                  TEST_NEAR("D_xx"  ,     dxx(i,j,k)    ,     0    ,     0.001);
                  TEST_NEAR("D_yy"  ,     dyy(i,j,k)    ,     0    ,     0.001);
                  TEST_NEAR("D_zz"  ,     dzz(i,j,k)    ,     0    ,     0.001);
          }
  }
  }
  }


  for(unsigned k = 0 ; k < testim.nk(); k++){
  for(unsigned j = 0 ; j < testim.nj(); j++){
  for(unsigned i = 0 ; i < testim.ni(); i++){
          if(i == j){
                  vcl_cout << "corner on diagonal " << vcl_endl;
                  vcl_cout << "(" << i << "," << j << ")" << vcl_endl;;
                  TEST_NEAR("D_xy"  ,     dxy(i,j,k)    ,    0.5   ,     0.001);
          }
          else if(i == j - 2 || j == i- 2 ){
                  vcl_cout << "second super/sub diagonals" << vcl_endl;
                  vcl_cout << "(" << i << "," << j << ")" << vcl_endl;;
                  TEST_NEAR("D_xy"  ,     dxy(i,j,k)    ,    -0.25   ,     0.001);
          }
 
          else if(i + j == 1 || i + j == testim.ni() + testim.nj() - 3 ){
                  vcl_cout << "i + j == 1 || i + j == testim.ni() + testim.nj() - 1" << vcl_endl;
                  vcl_cout << "(" << i << "," << j << ")" << vcl_endl;;
                  TEST_NEAR("D_xy"  ,     dxy(i,j,k)    ,    0.25   ,     0.001);
          }
          else if(i + j == 2 || i + j == testim.ni() + testim.nj() - 4 ){
                  vcl_cout << "i + j == 2 || i + j == testim.ni() + testim.nj() - 2" << vcl_endl;
                  vcl_cout << "(" << i << "," << j << ")" << vcl_endl;;
                  TEST_NEAR("D_xy"  ,     dxy(i,j,k)    ,    -0.25   ,     0.001);
          }
          else{
                  vcl_cout << "elsewhere " << vcl_endl;
                  vcl_cout << "(" << i << "," << j << ")" << vcl_endl;;
                  TEST_NEAR("D_xy"  ,     dxy(i,j,k)    ,    0   ,     0.001);
          }
          TEST_NEAR("D_xz"  ,     dxz(i,j,k)    ,     0    ,     0.001);
          TEST_NEAR("D_yz"  ,     dxz(i,j,k)    ,     0    ,     0.001);
  }
  }
  }


  for(unsigned k = 0 ; k < testim.nk(); k++){
  for(unsigned j = 0 ; j < testim.nj(); j++){
          for(unsigned i = 0 ; i < testim.ni(); i++){
                  if(i==k){
                          testim(i,j,k) = 1;
                  }
                  else{
                          testim(i,j,k) = 0;
                  }

          }
  }
  }

  dbil3d_finite_differences(testim, dxp, dxm, dxc, dyp, dym, dyc ,dzp, dzm, dzc); 
  dbil3d_finite_second_differences(testim, dxp, dxm, dyp, dym,dzp,dzm,dxx,dyy,dzz,dxy,dxz,dyz); 

  vcl_cout << "testing i/k diagonal image" << vcl_endl;
  for(unsigned k = 0 ; k < testim.nk(); k++){
  for(unsigned j = 0 ; j < testim.nj(); j++){
  for(unsigned i = 0 ; i < testim.ni(); i++){
          if(i == 0 && k == 0 || i == testim.ni()-1 && k == testim.nk()-1){
                  vcl_cout << "corners on diagonal " << vcl_endl;
                  TEST_NEAR("D_xx"  ,     dxx(i,j,k)    ,    -1    ,     0.001);
                  TEST_NEAR("D_yy"  ,     dyy(i,j,k)    ,     0    ,     0.001);
                  TEST_NEAR("D_zz"  ,     dzz(i,j,k)    ,    -1    ,     0.001);
          }
          else if(i == k){
                  vcl_cout << "non-corner on diagonal " << vcl_endl;
                  TEST_NEAR("D_xx"  ,     dxx(i,j,k)    ,    -2    ,     0.001);
                  TEST_NEAR("D_yy"  ,     dyy(i,j,k)    ,     0    ,     0.001);
                  TEST_NEAR("D_zz"  ,     dzz(i,j,k)    ,    -2    ,     0.001);
          }
          else if(i == k + 1 || i == k-1 ){
                  vcl_cout << "super/sub diagonal " << vcl_endl;
                  TEST_NEAR("D_xx"  ,     dxx(i,j,k)    ,     1    ,     0.001);
                  TEST_NEAR("D_yy"  ,     dyy(i,j,k)    ,     0    ,     0.001);
                  TEST_NEAR("D_zz"  ,     dzz(i,j,k)    ,     1    ,     0.001);
          }
          else{
                  vcl_cout << "elsewhere " << vcl_endl;
                  TEST_NEAR("D_xx"  ,     dxx(i,j,k)    ,     0    ,     0.001);
                  TEST_NEAR("D_yy"  ,     dyy(i,j,k)    ,     0    ,     0.001);
                  TEST_NEAR("D_zz"  ,     dzz(i,j,k)    ,     0    ,     0.001);
          }
  }
  }
  }


  for(unsigned k = 0 ; k < testim.nk(); k++){
  for(unsigned j = 0 ; j < testim.nj(); j++){
  for(unsigned i = 0 ; i < testim.ni(); i++){
          if(i == k){
                  vcl_cout << "corner on diagonal " << vcl_endl;
                  TEST_NEAR("D_xz"  ,     dxz(i,j,k)    ,    0.5   ,     0.001);
          }
          else if(i == k - 2 || k == i- 2 ){
                  vcl_cout << "second super/sub diagonals" << vcl_endl;
                  TEST_NEAR("D_xz"  ,     dxz(i,j,k)    ,    -0.25   ,     0.001);
          }
 
          else if(i + k == 1 || i + k == testim.ni() + testim.nk() - 3 ){
                  vcl_cout << "i + k == 1 || i + k == testim.ni() + testim.nk() - 3" << vcl_endl;
                  TEST_NEAR("D_xz"  ,     dxz(i,j,k)    ,    0.25   ,     0.001);
          }
          else if(i + k == 2 || i + k == testim.ni() + testim.nk() - 4 ){
                  vcl_cout << "i + k == 2 || i + k == testim.ni() + testim.nk() - 4" << vcl_endl;
                  TEST_NEAR("D_xz"  ,     dxz(i,j,k)    ,    -0.25   ,     0.001);
          }
          else{
                  vcl_cout << "elsewhere " << vcl_endl;
                  TEST_NEAR("D_xz"  ,     dxz(i,j,k)    ,    0   ,     0.001);
          }
          TEST_NEAR("D_xy"  ,     dxy(i,j,k)    ,     0    ,     0.001);
          TEST_NEAR("D_yz"  ,     dyz(i,j,k)    ,     0    ,     0.001);
  }
  }
  }


  for(unsigned k = 0 ; k < testim.nk(); k++){
  for(unsigned j = 0 ; j < testim.nj(); j++){
          for(unsigned i = 0 ; i < testim.ni(); i++){
                  if(j==k){
                          testim(i,j,k) = 1;
                  }
                  else{
                          testim(i,j,k) = 0;
                  }

          }
  }
  }

  dbil3d_finite_differences(testim, dxp, dxm, dxc, dyp, dym, dyc ,dzp, dzm, dzc); 
  dbil3d_finite_second_differences(testim, dxp, dxm, dyp, dym,dzp,dzm,dxx,dyy,dzz,dxy,dxz,dyz); 

  vcl_cout << "testing j/k diagonal image" << vcl_endl;
  for(unsigned k = 0 ; k < testim.nk(); k++){
  for(unsigned j = 0 ; j < testim.nj(); j++){
  for(unsigned i = 0 ; i < testim.ni(); i++){
          if(j == 0 && k == 0 || j == testim.nj()-1 && k == testim.nk()-1){
                  vcl_cout << "corners on diagonal " << vcl_endl;
                  TEST_NEAR("D_xx"  ,     dxx(i,j,k)    ,     0    ,     0.001);
                  TEST_NEAR("D_yy"  ,     dyy(i,j,k)    ,    -1    ,     0.001);
                  TEST_NEAR("D_zz"  ,     dzz(i,j,k)    ,    -1    ,     0.001);
          }
          else if(j == k){
                  vcl_cout << "non-corner on diagonal " << vcl_endl;
                  TEST_NEAR("D_xx"  ,     dxx(i,j,k)    ,     0    ,     0.001);
                  TEST_NEAR("D_yy"  ,     dyy(i,j,k)    ,    -2    ,     0.001);
                  TEST_NEAR("D_zz"  ,     dzz(i,j,k)    ,    -2    ,     0.001);
          }
          else if(j == k + 1 || j == k-1 ){
                  vcl_cout << "super/sub diagonal " << vcl_endl;
                  TEST_NEAR("D_xx"  ,     dxx(i,j,k)    ,     0    ,     0.001);
                  TEST_NEAR("D_yy"  ,     dyy(i,j,k)    ,     1    ,     0.001);
                  TEST_NEAR("D_zz"  ,     dzz(i,j,k)    ,     1    ,     0.001);
          }
          else{
                  vcl_cout << "elsewhere " << vcl_endl;
                  TEST_NEAR("D_xx"  ,     dxx(i,j,k)    ,     0    ,     0.001);
                  TEST_NEAR("D_yy"  ,     dyy(i,j,k)    ,     0    ,     0.001);
                  TEST_NEAR("D_zz"  ,     dzz(i,j,k)    ,     0    ,     0.001);
          }
  }
  }
  }


  for(unsigned k = 0 ; k < testim.nk(); k++){
  for(unsigned j = 0 ; j < testim.nj(); j++){
  for(unsigned i = 0 ; i < testim.ni(); i++){
          if(j == k){
                  vcl_cout << "corner on diagonal " << vcl_endl;
                  TEST_NEAR("D_yz"  ,     dyz(i,j,k)    ,    0.5   ,     0.001);
          }
          else if(j == k - 2 || k == j - 2 ){
                  vcl_cout << "second super/sub diagonals" << vcl_endl;
                  TEST_NEAR("D_yz"  ,     dyz(i,j,k)    ,    -0.25   ,     0.001);
          }
 
          else if(j + k == 1 || j + k == testim.nj() + testim.nk() - 3 ){
                  vcl_cout << "j + k == 1 || j + k == testim.nj() + testim.nk() - 3" << vcl_endl;
                  TEST_NEAR("D_yz"  ,     dyz(i,j,k)    ,    0.25   ,     0.001);
          }
          else if(j + k == 2 || j + k == testim.nj() + testim.nk() - 4 ){
                  vcl_cout << "j + k == 2 || j + k == testim.nj() + testim.nk() - 4" << vcl_endl;
                  TEST_NEAR("D_yz"  ,     dyz(i,j,k)    ,    -0.25   ,     0.001);
          }
          else{
                  vcl_cout << "elsewhere " << vcl_endl;
                  TEST_NEAR("D_yz"  ,     dyz(i,j,k)    ,    0   ,     0.001);
          }
          TEST_NEAR("D_xy"  ,     dxy(i,j,k)    ,     0    ,     0.001);
          TEST_NEAR("D_xz"  ,     dxz(i,j,k)    ,     0    ,     0.001);
  }
  }
  }

  SUMMARY();
}
