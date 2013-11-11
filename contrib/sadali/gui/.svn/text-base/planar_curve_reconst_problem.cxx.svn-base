
#include "planar_curve_reconst_problem.h"


void
planar_curve_reconst_problem::f(vnl_vector<double> const& x, vnl_vector<double>& fx)
{ double Y = x[0];
double Z = x[1];
double alp = x[2];


vnl_double_4 threed_point(alp*test_BB_point[0],alp*Y,alp*Z,alp);
vnl_double_3 proj_2d_point = ProjMat*threed_point;

fx[0] =Proj_Pt[0]-proj_2d_point[0];
fx[1] =Proj_Pt[1]-proj_2d_point[1];
fx[2] = 1.0-proj_2d_point[2];


}


void
planar_curve_reconst_problem::set_Proj(const vnl_double_3x4 &ProjMatrix)
{
  ProjMat = ProjMatrix;
}
void
planar_curve_reconst_problem::set_Point(const vnl_double_3 &Point_image)
{
    Proj_Pt = Point_image;
}


