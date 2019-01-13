#include <testlib/testlib_test.h>

#include <bsold/algo/bsold_curve_3d_algs.h>
#include <bsold/bsold_interp_curve_3d.h>
#include <vsol/vsol_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vnl/vnl_math.h>

#include <iostream> 
#include <cmath>
#include <limits>
#include <cstring>
#include <iostream>
#include <cstdlib>

void loadCON(std::string fileName, std::vector<vsol_point_3d_sptr> &points)
{
  std::ifstream infp(fileName.c_str());
  char magicNum[200];

  infp.getline(magicNum,200);
  if (strncmp(magicNum,"CONTOUR",7))
  {
    std::cerr << "Invalid File " << fileName.c_str() << std::endl;
    std::cerr << "Should be CONTOUR " << magicNum << std::endl;
    std::exit(1);
  }

  char openFlag[200];
  infp.getline(openFlag,200);
  if (!strncmp(openFlag,"OPEN",4))
    std::cout << "Open Curve\n" << std::endl;
  else if (!strncmp(openFlag,"CLOSE",5))
    std::cout << "Closed Curve\n" << std::endl;
  else
  {
    std::cerr << "Invalid File " << fileName.c_str() << std::endl;
    std::cerr << "Should be OPEN/CLOSE " << openFlag << std::endl;
    std::exit(1);
  }

  int i,numOfPoints;
  infp >> numOfPoints;

  double x,y,z;
  for (i=0;i<numOfPoints;i++)
  {
    infp >> x >> y >> z ;
    points.push_back(new vsol_point_3d(x, y, z));
  }
  infp.close();
}

MAIN( test_interp_eno_3d )
{
   START ("3D ENO Interpolation");

   double tolerance = 10e-3;
  
   std::vector<vsol_point_3d_sptr> pts;

   // x(t) = 1 - 2t - 2t^2 + t^3
   // y(t) = -1 + t + t^2 - 2t^3
   // z(t) = 2 + 2t - t^2 + t^3

   // Using
   pts.push_back(new vsol_point_3d(1, -1, 2)); // @ t=0
   pts.push_back(new vsol_point_3d(-0.375, -0.5, 2.875)); // @ t=0.5
   pts.push_back(new vsol_point_3d(-2, -1, 4)); // @ t=1
   pts.push_back(new vsol_point_3d(-3.125, -4, 6.125)); // @ t=1.5
   pts.push_back(new vsol_point_3d(-3, -11, 10)); // @ t=2
   pts.push_back(new vsol_point_3d(-0.875, -23.5, 16.375)); // @ t=2.5
   pts.push_back(new vsol_point_3d(4, -43, 26)); // @ t=3

   std::vector<double> samples;
   samples.push_back(0.0);
   samples.push_back(0.5);
   samples.push_back(1.0);
   samples.push_back(1.5);
   samples.push_back(2.0);
   samples.push_back(2.5);
   samples.push_back(3.0);

   bsold_interp_curve_3d c;
   bsold_curve_3d_algs::interpolate_eno_3d(&c,pts,samples);
   std::cout << c;

   TEST_NEAR("length test: ", c.length_at(0), 0.0, tolerance);
   TEST_NEAR("length test: ", c.length_at(1), 1.7143, tolerance);
   TEST_NEAR("length test: ", c.length_at(2), 3.8136, tolerance);
   TEST_NEAR("length test: ", c.length_at(3), 7.7033, tolerance);
   TEST_NEAR("length test: ", c.length_at(4), 15.719, tolerance);
   TEST_NEAR("length test: ", c.length_at(5), 29.916, tolerance);
   TEST_NEAR("length test: ", c.length_at(6), 52.205, tolerance); 
   TEST_NEAR("length test: ", c.length(), 52.205, tolerance);

   SUMMARY();
}
