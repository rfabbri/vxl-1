#include <testlib/testlib_test.h>
#include <vil3d/vil3d_print.h>
#include <vil3d/vil3d_copy.h>
#include <vcl_cstdio.h>
#include <vcl_cmath.h>
#include <vnl/vnl_random.h>
#include <vnl/vnl_math.h>
#include <dbil3d/algo/dbil3d_finite_differences.h>


MAIN( test_dbil3d_finite_differences )
{
  START ("3D Finite Differences");

  int dim = 6; 
  vil3d_image_view<float> testim(dim,dim,dim); 
  vil3d_image_view<float> dxp,dxm,dxc;
  vil3d_image_view<float> dyp,dym,dyc;
  vil3d_image_view<float> dzp,dzm,dzc;

  testim.fill(1); 
  dbil3d_finite_differences(testim, dxp, dxm, dxc, 
                                    dyp, dym, dyc, 
                                    dzp, dzm, dzc); 

  float dxpsum=0;
  float dxmsum=0;
  float dxcsum=0;
  float dypsum=0;
  float dymsum=0;
  float dycsum=0;
  float dzpsum=0;
  float dzmsum=0;
  float dzcsum=0;

  for(unsigned k = 0 ; k < testim.nk(); k++){
  for(unsigned j = 0 ; j < testim.nj(); j++){
  for(unsigned i = 0 ; i < testim.ni(); i++){
    dxpsum += dxp(i,j,k);
    dxmsum += dxm(i,j,k);
    dxcsum += dxc(i,j,k);
    dypsum += dyp(i,j,k);
    dymsum += dym(i,j,k);
    dycsum += dyc(i,j,k);
    dzpsum += dzp(i,j,k);
    dzmsum += dzm(i,j,k);
    dzcsum += dzc(i,j,k);
  }
  }
  }

  vcl_cout << "Testing on constant image, differences should be zero everywhere" << vcl_endl; 
   TEST_NEAR("D_x_plus", dxpsum,0, 0.001);
   TEST_NEAR("D_x_minus", dxmsum,0, 0.001);
   TEST_NEAR("D_x_center", dxcsum,0, 0.001);
   TEST_NEAR("D_y_plus", dypsum,0, 0.001);
   TEST_NEAR("D_y_minus", dymsum,0, 0.001);
   TEST_NEAR("D_y_center", dycsum,0, 0.001);
   TEST_NEAR("D_z_plus", dzpsum,0, 0.001);
   TEST_NEAR("D_z_minus", dzmsum,0, 0.001);
   TEST_NEAR("D_z_center", dzcsum,0, 0.001);


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
  dbil3d_finite_differences(testim, dxp, dxm, dxc, 
                                    dyp, dym, dyc, 
                                    dzp, dzm, dzc); 

  vcl_cout << "testing half-one half-zero (along i-axis) image" << vcl_endl;

//  vil3d_print_all(vcl_cout,dxp);
//  vil3d_print_all(vcl_cout,dxm);
//  vil3d_print_all(vcl_cout,dxc);
  for(unsigned k = 0 ; k < testim.nk(); k++){
  for(unsigned j = 0 ; j < testim.nj(); j++){
          TEST_NEAR("D_x_plus"    ,     dxp(0,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_minus"   ,     dxm(0,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_center"  ,     dxc(0,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_plus"    ,     dyp(0,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_minus"   ,     dym(0,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_center"  ,     dyc(0,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_plus"    ,     dzp(0,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_minus"   ,     dzm(0,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_center"  ,     dzc(0,j,k)    ,    0    ,     0.001);


          TEST_NEAR("D_x_plus"    ,     dxp(dim/2 - 1,j,k)    ,   -1    ,     0.001);
          TEST_NEAR("D_x_minus"   ,     dxm(dim/2 - 1,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_center"  ,     dxc(dim/2 - 1,j,k)    ,   -0.5  ,     0.001);
          TEST_NEAR("D_y_plus"    ,     dyp(dim/2 - 1,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_minus"   ,     dym(dim/2 - 1,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_center"  ,     dyc(dim/2 - 1,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_plus"    ,     dzp(dim/2 - 1,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_minus"   ,     dzm(dim/2 - 1,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_center"  ,     dzc(dim/2 - 1,j,k)    ,    0    ,     0.001);

          TEST_NEAR("D_x_plus"    ,     dxp(dim/2,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_minus"   ,     dxm(dim/2,j,k)    ,   -1    ,     0.001);
          TEST_NEAR("D_x_center"  ,     dxc(dim/2,j,k)    ,   -0.5  ,     0.001);
          TEST_NEAR("D_y_plus"    ,     dyp(dim/2,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_minus"   ,     dym(dim/2,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_center"  ,     dyc(dim/2,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_plus"    ,     dzp(dim/2,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_minus"   ,     dzm(dim/2,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_center"  ,     dzc(dim/2,j,k)    ,    0    ,     0.001);
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

  dbil3d_finite_differences(testim, dxp, dxm, dxc, 
                                    dyp, dym, dyc, 
                                    dzp, dzm, dzc); 


  vcl_cout << "testing half-one half-zero (along j-axis) image" << vcl_endl;
//  vil3d_print_all(vcl_cout,dyp);
//  vil3d_print_all(vcl_cout,dym);
//  vil3d_print_all(vcl_cout,dyc);
  for(unsigned k = 0 ; k < testim.nk(); k++){
  for(unsigned i = 0 ; i < testim.ni(); i++){
          TEST_NEAR("D_x_plus"    ,     dxp(i,0,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_minus"   ,     dxm(i,0,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_center"  ,     dxc(i,0,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_plus"    ,     dyp(i,0,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_minus"   ,     dym(i,0,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_center"  ,     dyc(i,0,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_plus"    ,     dzp(i,0,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_minus"   ,     dzm(i,0,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_center"  ,     dzc(i,0,k)    ,    0    ,     0.001);

          TEST_NEAR("D_x_plus"    ,     dxp(i,dim/2 - 1,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_minus"   ,     dxm(i,dim/2 - 1,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_center"  ,     dxc(i,dim/2 - 1,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_plus"    ,     dyp(i,dim/2 - 1,k)    ,   -1    ,     0.001);
          TEST_NEAR("D_y_minus"   ,     dym(i,dim/2 - 1,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_center"  ,     dyc(i,dim/2 - 1,k)    ,   -0.5  ,     0.001);
          TEST_NEAR("D_z_plus"    ,     dzp(i,dim/2 - 1,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_minus"   ,     dzm(i,dim/2 - 1,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_center"  ,     dzc(i,dim/2 - 1,k)    ,    0    ,     0.001);

          TEST_NEAR("D_x_plus"    ,     dxp(i,dim/2,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_minus"   ,     dxm(i,dim/2,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_center"  ,     dxc(i,dim/2,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_plus"    ,     dyp(i,dim/2,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_minus"   ,     dym(i,dim/2,k)    ,   -1    ,     0.001);
          TEST_NEAR("D_y_center"  ,     dyc(i,dim/2,k)    ,   -0.5  ,     0.001);
          TEST_NEAR("D_z_plus"    ,     dzp(i,dim/2,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_minus"   ,     dzm(i,dim/2,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_center"  ,     dzc(i,dim/2,k)    ,    0    ,     0.001);
  }
  }

for(unsigned k = 0 ; k < testim.nk(); k++){
  for(unsigned j = 0 ; j < testim.nj(); j++){
  for(unsigned i = 0 ; i < testim.ni(); i++){
    if(k < static_cast<unsigned>(dim/2)){ testim(i,j,k) = 1; }
    else         { testim(i,j,k) = 0; }
  }
  }
  }


  dbil3d_finite_differences(testim, dxp, dxm, dxc, 
                                    dyp, dym, dyc, 
                                    dzp, dzm, dzc); 


  vcl_cout << "testing half-one half-zero (along k-axis) image" << vcl_endl;
//  vil3d_print_all(vcl_cout,dzp);
//  vil3d_print_all(vcl_cout,dzm);
//  vil3d_print_all(vcl_cout,dzc);
  for(unsigned j = 0 ; j < testim.nj(); j++){
  for(unsigned i = 0 ; i < testim.ni(); i++){
          unsigned k = 0;
          TEST_NEAR("D_x_plus"    ,     dxp(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_minus"   ,     dxm(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_center"  ,     dxc(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_plus"    ,     dyp(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_minus"   ,     dym(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_center"  ,     dyc(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_plus"    ,     dzp(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_minus"   ,     dzm(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_center"  ,     dzc(i,j,k)    ,    0    ,     0.001);

          k = dim/2 - 1 ;
          TEST_NEAR("D_x_plus"    ,     dxp(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_minus"   ,     dxm(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_center"  ,     dxc(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_plus"    ,     dyp(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_minus"   ,     dym(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_center"  ,     dyc(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_plus"    ,     dzp(i,j,k)    ,   -1    ,     0.001);
          TEST_NEAR("D_z_minus"   ,     dzm(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_center"  ,     dzc(i,j,k)    ,  -0.5     ,     0.001);

          k = dim/2 ;
          TEST_NEAR("D_x_plus"    ,     dxp(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_minus"   ,     dxm(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_center"  ,     dxc(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_plus"    ,     dyp(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_minus"   ,     dym(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_center"  ,     dyc(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_plus"    ,     dzp(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_minus"   ,     dzm(i,j,k)    ,   -1    ,     0.001);
          TEST_NEAR("D_z_center"  ,     dzc(i,j,k)    ,   -0.5    ,     0.001);
  }
  }

for(unsigned k = 0 ; k < testim.nk(); k++){
for(unsigned j = 0 ; j < testim.nj(); j++){
  for(unsigned i = 0 ; i < testim.ni(); i++){
    if(i == 0){
      testim(i,j,k) = 1;
    }
    else{
      testim(i,j,k) = 0;
    }
  }
  }
}

  dbil3d_finite_differences(testim, dxp, dxm, dxc, 
                                    dyp, dym, dyc, 
                                    dzp, dzm, dzc); 

  vcl_cout << "Testing on image at minimal i boundary   " << vcl_endl; 
//  vil3d_print_all(vcl_cout,dxp);
//  vil3d_print_all(vcl_cout,dxm);
//  vil3d_print_all(vcl_cout,dxc);
  for(unsigned k = 0 ; k < testim.nk(); k++){
  for(unsigned j = 0 ; j < testim.nj(); j++){
          unsigned i = 0;
          TEST_NEAR("D_x_plus"    ,     dxp(0,j,k)    ,   -1    ,     0.001);
          TEST_NEAR("D_x_minus"   ,     dxm(0,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_center"  ,     dxc(0,j,k)    ,  -0.5   ,     0.001);
          TEST_NEAR("D_y_plus"    ,     dyp(0,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_minus"   ,     dym(0,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_center"  ,     dyc(0,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_plus"    ,     dzp(0,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_minus"   ,     dzm(0,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_center"  ,     dzc(0,j,k)    ,    0    ,     0.001);

          i = 1;
          TEST_NEAR("D_x_plus"    ,     dxp(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_minus"   ,     dxm(i,j,k)    ,   -1    ,     0.001);
          TEST_NEAR("D_x_center"  ,     dxc(i,j,k)    ,  -0.5   ,     0.001);
          TEST_NEAR("D_y_plus"    ,     dyp(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_minus"   ,     dym(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_center"  ,     dyc(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_plus"    ,     dzp(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_minus"   ,     dzm(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_center"  ,     dzc(i,j,k)    ,    0    ,     0.001);

          i = testim.ni() - 1;
          TEST_NEAR("D_x_plus"    ,     dxp(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_minus"   ,     dxm(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_center"  ,     dxc(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_plus"    ,     dyp(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_minus"   ,     dym(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_center"  ,     dyc(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_plus"    ,     dzp(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_minus"   ,     dzm(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_center"  ,     dzc(i,j,k)    ,    0    ,     0.001);
    }
  }



  for(unsigned k = 0 ; k < testim.nk(); k++){
  for(unsigned j = 0 ; j < testim.nj(); j++){
  for(unsigned i = 0 ; i < testim.ni(); i++){
    if(j == 0){
      testim(i,j,k) = 1;
    }
    else{
      testim(i,j,k) = 0;
    }
  }
  }
  }

  dbil3d_finite_differences(testim, dxp, dxm, dxc, 
                                    dyp, dym, dyc, 
                                    dzp, dzm, dzc); 



  vcl_cout << "Testing on image at minimal j boundary   " << vcl_endl; 
  //vil3d_print_all(vcl_cout,dyp);
  //vil3d_print_all(vcl_cout,dym);
  //vil3d_print_all(vcl_cout,dyc);
  for(unsigned k = 0 ; k < testim.nk(); k++){
  for(unsigned i = 0 ; i < testim.ni(); i++){
          unsigned j = 0;
          TEST_NEAR("D_x_plus"    ,     dxp(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_minus"   ,     dxm(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_center"  ,     dxc(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_plus"    ,     dyp(i,j,k)    ,   -1    ,     0.001);
          TEST_NEAR("D_y_minus"   ,     dym(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_center"  ,     dyc(i,j,k)    ,  -0.5   ,     0.001);
          TEST_NEAR("D_z_plus"    ,     dzp(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_minus"   ,     dzm(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_center"  ,     dzc(i,j,k)    ,    0    ,     0.001);

          j = 1;
          TEST_NEAR("D_x_plus"    ,     dxp(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_minus"   ,     dxm(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_center"  ,     dxc(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_plus"    ,     dyp(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_minus"   ,     dym(i,j,k)    ,   -1    ,     0.001);
          TEST_NEAR("D_y_center"  ,     dyc(i,j,k)    ,  -0.5   ,     0.001);
          TEST_NEAR("D_z_plus"    ,     dzp(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_minus"   ,     dzm(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_center"  ,     dzc(i,j,k)    ,    0    ,     0.001);


          j = testim.nj()-1;
          TEST_NEAR("D_x_plus"    ,     dxp(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_minus"   ,     dxm(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_center"  ,     dxc(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_plus"    ,     dyp(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_minus"   ,     dym(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_center"  ,     dyc(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_plus"    ,     dzp(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_minus"   ,     dzm(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_center"  ,     dzc(i,j,k)    ,    0    ,     0.001);
    }
  }

for(unsigned k = 0 ; k < testim.nk(); k++){
for(unsigned j = 0 ; j < testim.nj(); j++){
  for(unsigned i = 0 ; i < testim.ni(); i++){
    if(k == 0){
      testim(i,j,k) = 1;
    }
    else{
      testim(i,j,k) = 0;
    }
  }
  }
}

  dbil3d_finite_differences(testim, dxp, dxm, dxc, 
                                    dyp, dym, dyc, 
                                    dzp, dzm, dzc); 

  vcl_cout << "Testing on image at minimal k boundary   " << vcl_endl; 
//  vil3d_print_all(vcl_cout,dxp);
//  vil3d_print_all(vcl_cout,dxm);
//  vil3d_print_all(vcl_cout,dxc);
  for(unsigned j = 0 ; j < testim.nj(); j++){
  for(unsigned i = 0 ; i < testim.ni(); i++){
          unsigned k = 0;
          TEST_NEAR("D_x_plus"    ,     dxp(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_minus"   ,     dxm(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_center"  ,     dxc(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_plus"    ,     dyp(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_minus"   ,     dym(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_center"  ,     dyc(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_plus"    ,     dzp(i,j,k)    ,   -1    ,     0.001);
          TEST_NEAR("D_z_minus"   ,     dzm(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_center"  ,     dzc(i,j,k)    ,  -0.5   ,     0.001);

          k = 1;
          TEST_NEAR("D_x_plus"    ,     dxp(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_minus"   ,     dxm(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_center"  ,     dxc(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_plus"    ,     dyp(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_minus"   ,     dym(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_center"  ,     dyc(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_plus"    ,     dzp(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_minus"   ,     dzm(i,j,k)    ,   -1    ,     0.001);
          TEST_NEAR("D_z_center"  ,     dzc(i,j,k)    ,  -0.5   ,     0.001);

          k = testim.nk()-1;
          TEST_NEAR("D_x_plus"    ,     dxp(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_minus"   ,     dxm(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_center"  ,     dxc(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_plus"    ,     dyp(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_minus"   ,     dym(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_center"  ,     dyc(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_plus"    ,     dzp(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_minus"   ,     dzm(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_center"  ,     dzc(i,j,k)    ,    0    ,     0.001);
    }
  }






for(unsigned k = 0 ; k < testim.nk(); k++){
for(unsigned j = 0 ; j < testim.nj(); j++){
  for(unsigned i = 0 ; i < testim.ni(); i++){
    if(i == testim.ni()-1){
      testim(i,j,k) = 1;
    }
    else{
      testim(i,j,k) = 0;
    }
  }
  }
}

  dbil3d_finite_differences(testim, dxp, dxm, dxc, 
                                    dyp, dym, dyc, 
                                    dzp, dzm, dzc); 

  vcl_cout << "Testing on image at extremal i boundary   " << vcl_endl; 
//  vil3d_print_all(vcl_cout,dxp);
//  vil3d_print_all(vcl_cout,dxm);
//  vil3d_print_all(vcl_cout,dxc);
  for(unsigned k = 0 ; k < testim.nk(); k++){
  for(unsigned j = 0 ; j < testim.nj(); j++){
          TEST_NEAR("D_x_plus"    ,     dxp(0,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_minus"   ,     dxm(0,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_center"  ,     dxc(0,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_plus"    ,     dyp(0,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_minus"   ,     dym(0,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_center"  ,     dyc(0,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_plus"    ,     dzp(0,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_minus"   ,     dzm(0,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_center"  ,     dzc(0,j,k)    ,    0    ,     0.001);


          TEST_NEAR("D_x_plus"    ,     dxp(testim.ni() - 2,j,k)    ,    1    ,     0.001);
          TEST_NEAR("D_x_minus"   ,     dxm(testim.ni() - 2,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_center"  ,     dxc(testim.ni() - 2,j,k)    ,   0.5   ,     0.001);
          TEST_NEAR("D_y_plus"    ,     dyp(testim.ni() - 2,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_minus"   ,     dym(testim.ni() - 2,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_center"  ,     dyc(testim.ni() - 2,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_plus"    ,     dzp(testim.ni() - 2,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_minus"   ,     dzm(testim.ni() - 2,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_center"  ,     dzc(testim.ni() - 2,j,k)    ,    0    ,     0.001);

          TEST_NEAR("D_x_plus"    ,     dxp(testim.ni() - 1,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_minus"   ,     dxm(testim.ni() - 1,j,k)    ,    1    ,     0.001);
          TEST_NEAR("D_x_center"  ,     dxc(testim.ni() - 1,j,k)    ,   0.5   ,     0.001);
          TEST_NEAR("D_y_plus"    ,     dyp(testim.ni() - 1,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_minus"   ,     dym(testim.ni() - 1,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_center"  ,     dyc(testim.ni() - 1,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_plus"    ,     dzp(testim.ni() - 1,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_minus"   ,     dzm(testim.ni() - 1,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_center"  ,     dzc(testim.ni() - 1,j,k)    ,    0    ,     0.001);
    }
  }




  for(unsigned k = 0 ; k < testim.nk(); k++){
  for(unsigned j = 0 ; j < testim.nj(); j++){
  for(unsigned i = 0 ; i < testim.ni(); i++){
    if(j == testim.nj()-1){
      testim(i,j,k) = 1;
    }
    else{
      testim(i,j,k) = 0;
    }
  }
  }
  }

  dbil3d_finite_differences(testim, dxp, dxm, dxc, 
                                    dyp, dym, dyc, 
                                    dzp, dzm, dzc); 



  vcl_cout << "Testing on image at extremal j boundary   " << vcl_endl; 
//  vil3d_print_all(vcl_cout,dyp);
//  vil3d_print_all(vcl_cout,dym);
//  vil3d_print_all(vcl_cout,dyc);
  for(unsigned k = 0 ; k < testim.nk(); k++){
  for(unsigned i = 0 ; i < testim.ni(); i++){
          TEST_NEAR("D_x_plus"    ,     dxp(i,0,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_minus"   ,     dxm(i,0,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_center"  ,     dxc(i,0,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_plus"    ,     dyp(i,0,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_minus"   ,     dym(i,0,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_center"  ,     dyc(i,0,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_plus"    ,     dzp(i,0,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_minus"   ,     dzm(i,0,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_center"  ,     dzc(i,0,k)    ,    0    ,     0.001);


          TEST_NEAR("D_x_plus"    ,     dxp(i,testim.nj() - 2,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_minus"   ,     dxm(i,testim.nj() - 2,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_center"  ,     dxc(i,testim.nj() - 2,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_plus"    ,     dyp(i,testim.nj() - 2,k)    ,    1    ,     0.001);
          TEST_NEAR("D_y_minus"   ,     dym(i,testim.nj() - 2,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_center"  ,     dyc(i,testim.nj() - 2,k)    ,   0.5   ,     0.001);
          TEST_NEAR("D_z_plus"    ,     dzp(i,testim.nj() - 2,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_minus"   ,     dzm(i,testim.nj() - 2,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_center"  ,     dzc(i,testim.nj() - 2,k)    ,    0    ,     0.001);


          TEST_NEAR("D_x_plus"    ,     dxp(i,testim.nj() - 1,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_minus"   ,     dxm(i,testim.nj() - 1,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_center"  ,     dxc(i,testim.nj() - 1,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_plus"    ,     dyp(i,testim.nj() - 1,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_minus"   ,     dym(i,testim.nj() - 1,k)    ,    1    ,     0.001);
          TEST_NEAR("D_y_center"  ,     dyc(i,testim.nj() - 1,k)    ,   0.5   ,     0.001);
          TEST_NEAR("D_z_plus"    ,     dzp(i,testim.nj() - 1,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_minus"   ,     dzm(i,testim.nj() - 1,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_center"  ,     dzc(i,testim.nj() - 1,k)    ,    0    ,     0.001);
    }
  }
for(unsigned k = 0 ; k < testim.nk(); k++){
for(unsigned j = 0 ; j < testim.nj(); j++){
  for(unsigned i = 0 ; i < testim.ni(); i++){
    if(k == testim.nk()-1){
      testim(i,j,k) = 1;
    }
    else{
      testim(i,j,k) = 0;
    }
  }
  }
}

  dbil3d_finite_differences(testim, dxp, dxm, dxc, 
                                    dyp, dym, dyc, 
                                    dzp, dzm, dzc); 

  vcl_cout << "Testing on image at extremal k boundary   " << vcl_endl; 
//  vil3d_print_all(vcl_cout,dxp);
//  vil3d_print_all(vcl_cout,dxm);
//  vil3d_print_all(vcl_cout,dxc);
  for(unsigned j = 0 ; j < testim.nj(); j++){
  for(unsigned i = 0 ; i < testim.ni(); i++){
          unsigned k = 0;
          TEST_NEAR("D_x_plus"    ,     dxp(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_minus"   ,     dxm(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_center"  ,     dxc(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_plus"    ,     dyp(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_minus"   ,     dym(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_center"  ,     dyc(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_plus"    ,     dzp(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_minus"   ,     dzm(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_center"  ,     dzc(i,j,k)    ,    0    ,     0.001);

          k = testim.nk()-2;
          TEST_NEAR("D_x_plus"    ,     dxp(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_minus"   ,     dxm(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_center"  ,     dxc(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_plus"    ,     dyp(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_minus"   ,     dym(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_center"  ,     dyc(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_plus"    ,     dzp(i,j,k)    ,    1    ,     0.001);
          TEST_NEAR("D_z_minus"   ,     dzm(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_center"  ,     dzc(i,j,k)    ,   0.5   ,     0.001);

          k = testim.nk()-1;
          TEST_NEAR("D_x_plus"    ,     dxp(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_minus"   ,     dxm(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_x_center"  ,     dxc(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_plus"    ,     dyp(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_minus"   ,     dym(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_y_center"  ,     dyc(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_plus"    ,     dzp(i,j,k)    ,    0    ,     0.001);
          TEST_NEAR("D_z_minus"   ,     dzm(i,j,k)    ,    1    ,     0.001);
          TEST_NEAR("D_z_center"  ,     dzc(i,j,k)    ,   0.5   ,     0.001);
    }
  }

  SUMMARY();
}
