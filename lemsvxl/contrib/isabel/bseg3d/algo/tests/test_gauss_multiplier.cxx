#include <testlib/testlib_test.h>
#include "../bseg3d_gauss_multiplier.h"
//#include <bvxm/bvxm_voxel_grid.h>

void multiply()
{
 //define some known gaussian pdfs
  typedef bsta_num_obs<bsta_gauss_f1> gauss_type;

  vcl_vector<gauss_type> gauss_vec;
  bsta_gauss_f1 fc1(0,1);
  bsta_gauss_f1 fc2(4,1);
  bsta_gauss_f1 fc3(12,3);
  gauss_vec.push_back(fc1);
  gauss_vec.push_back(fc2);
  gauss_vec.push_back(fc3);
  
  vcl_vector<gauss_type> gauss_vec2;
  bsta_gauss_f1 gc1(0,1.5);
  bsta_gauss_f1 gc2(3,1);
  bsta_gauss_f1 gc3(15,4);
  gauss_vec2.push_back(gc1);
  gauss_vec2.push_back(gc2);
  gauss_vec2.push_back(gc3);
  gauss_vec2.push_back(fc3);

  bseg3d_gauss_multiplier multiplier;

  vcl_pair<double, gauss_type> z1 = multiplier.multiply(gauss_vec);
  vcl_pair<double, gauss_type> z2 = multiplier.multiply(gauss_vec2);
  

  TEST_NEAR("mean1",z1.second.mean(), 3.4286,0.01);
  TEST_NEAR("var1",z1.second.var(), 0.4286,0.01);
  TEST_NEAR("norm_const1",z1.first, 6.8847e-010,0.01);

  TEST_NEAR("mean2",z2.second.mean(), 4.7778,0.01);
  TEST_NEAR("var2",z2.second.var(), 0.4444,0.01);
  TEST_NEAR("norm_const2",z2.first, 3.6308e-016,0.01);

}

MAIN( test_gauss_multiplier)
{
  START ("test gaussian multiplication");
  multiply();
  SUMMARY();
}
