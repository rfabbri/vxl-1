//: 
// \file  test_vol3d_radius.cxx 
// \brief  testing the radius filter
// \author    Kongbin Kang
// \date        2005-05-29
// 
#include <vcl_iostream.h>
#include <vbl/vbl_array_3d.h>
#include <testlib/testlib_test.h>
#include <vol3d/vol3d_radius_filter.h>

static void test_vol3d_radius()
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


  vol3d_radius_filter rf(10);

  vcl_valarray<double> dens = rf.densities(vol, 40, 40, 50);


  for(int i = 0; i < dens.size(); i++)
    vcl_cout << dens[i] << '\t';
  vcl_cout << '\n';

  double density = rf.density(vol, 40, 40, 50, 8);
  vcl_cout << "density within radius 4 = " << density << '\t';
  vcl_cout << '\n';

  vcl_valarray<int> rp(5);

  rp[0] = 2;
  rp[1] = 3;
  rp[2] = 4;
  rp[3] = 6;
  rp[4] = 8;

  vcl_valarray<double> dens_rp = rf.densities(vol, rp, 40, 40, 17);
 
  for(int i = 0; i < dens_rp.size(); i++)
    vcl_cout << dens_rp[i] << '\t';
  vcl_cout << '\n';

  TEST_NEAR("density on radius 3 shell ", dens_rp[1], 3.777, 0.001);

  dens_rp = rf.densities(vol, rp, 40, 40, 50);

  vcl_cout << "\ndensities at (40, 40, 50) are ";
  for(int i = 0; i < dens_rp.size(); i++)
    vcl_cout << dens_rp[i] << '\t';
  vcl_cout << '\n';

/*  --------------------------------------------------------------------*/
  vcl_valarray<int> rp1(4);

  rp1[0] = 3;
  rp1[1] = 4;
  rp1[2] = 6;
  rp1[3] = 8;

  dens_rp = rf.densities(vol, rp1, 40, 40, 58);
  
  vcl_cout << "\ndensities at (40, 40, 58) are ";
  for(int i = 0; i < dens_rp.size(); i++)
    vcl_cout << dens_rp[i] << '\t';
  vcl_cout << '\n';

}

TESTMAIN(test_vol3d_radius)

