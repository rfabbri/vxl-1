//: 
// \file  test_vol3d_radius_detection.cxx 
// \brief  testing the radius detection 
// \author    Kongbin Kang
// \date        2005-07-11
// 
#include <vcl_iostream.h>
#include <vbl/vbl_array_3d.h>
#include <testlib/testlib_test.h>
#include <vol3d/vol3d_radius_filter.h>
#include <vol3d/algo/vol3d_radius_detection.h>

static void test_vol3d_algo_radius_detection()
{
  vbl_array_3d<double> vol(80, 80, 80);

  // assign background to be zero
  for(int i = 0; i < 80; i++)
    for(int j = 0; j < 80; j++)
      for(int k = 0; k < 80; k++)
        vol[i][j][k] = 0;

  // asign a few axis aliged cylinders with different radius
  for(int k = 0; k < 20; k++)
    for(int i = 0; i < 80; i++)
      for(int j = 0; j < 80; j++)
        if((i-40)*(i-40)+(j-40)*(j-40) <= 4)
          vol[i][j][k] = 10;

  for(int k = 20; k < 40; k++)
    for(int i = 0; i < 80; i++)
      for(int j = 0; j < 80; j++)
        if((i-40)*(i-40)+(j-40)*(j-40) <= 16)
          vol[i][j][k] = 10;
   
  for(int k = 40; k < 80; k++)
    for(int i = 0; i < 80; i++)
      for(int j = 0; j < 80; j++)
        if((i-40)*(i-40)+(j-40)*(j-40) <= 36)
          vol[i][j][k] = 10;



  vol3d_radius_detection rd(2, 8);

  vbl_array_3d<double> rs = rd.radius(vol, static_cast<double>(10), static_cast<double>(0));
 
  TEST_NEAR("radius of the cylinder at z=17", rs[40][40][17], 3, 1e-6);

  TEST_NEAR("radius of the cylinder at z=50", rs[40][40][50], 8, 1e-6);

  rs = rd.radius(vol, 0);
 
  TEST_NEAR("radius of the cylinder at z=17", rs[40][40][17], 3, 1e-6);

  TEST_NEAR("radius of the cylinder at z=50", rs[40][40][50], 8, 1e-6);
}

TESTMAIN(test_vol3d_algo_radius_detection)

