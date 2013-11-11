// This is brcv/dbdet/edge/dbdet_zc_det.cxx

//:
// \file

#include "dbdet_zc_det.h"
#include <vcl_cstdio.h>
#include <vcl_cstdlib.h>   // for vcl_abs(int) and vcl_sqrt()
#include <vnl/vnl_math.h>
#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_matrix_inverse.h>
#include <vnl/algo/vnl_svd.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/algo/vgl_homg_operators_2d.h>

//---------------------------------------------------------------
// Constructors
//----------------------------------------------------------------

//: default constructor
dbdet_zc_det::dbdet_zc_det(): 
  thresh_(0.0), 
  margin_(1),
  option_(1),
  dir_x_(vil_image_view<double>(0,0,1)), 
  dir_y_(vil_image_view<double>(0,0,1)), 
  grad_mag_(vil_image_view<double>(0,0,1)),
  mask_(vil_image_view<double>(0,0,1)),
  mag_(0,0, 0.0)
{
}

//: Constructor from a parameter block, gradient magnitudes given as an image and directions given as component image
dbdet_zc_det::dbdet_zc_det(const dbdet_zc_det_params& zcp, const vil_image_view<double>& dir_x, 
                           const vil_image_view<double>& dir_y, const vil_image_view<double>& grad_mag,
                           const vil_image_view<double>& mask) : 
  thresh_(zcp.thresh_), margin_(zcp.margin_), option_(zcp.option_),
  dir_x_(dir_x), dir_y_(dir_y), grad_mag_(grad_mag), mask_(mask),
  mag_(grad_mag.nj(), grad_mag.ni(), 0.0){}

//-------------------------------------------------------------------------
//: Apply the algorithm
//
void dbdet_zc_det::apply(bool collect_tokens,
                         vcl_vector<vgl_point_2d<double> >& loc, 
                         vcl_vector<double>& orientation, 
                         vcl_vector<double>& mag)
{
  double f[3], s_list[3];

  // run non-maximum suppression at every point
  for (unsigned x = margin_; x < grad_mag_.ni()-margin_; x++){
    for (unsigned y = margin_; y < grad_mag_.nj()-margin_; y++)
    {
      if (mask_(x,y)>thresh_)
      {
        double gx = dir_x_(x,y);
        double gy = dir_y_(x,y);
        vgl_vector_2d<double> direction(gx,gy);
        normalize(direction);

        //if non-degenerate
        if (vcl_abs(direction.x()) > 10e-6 || vcl_abs(direction.y()) > 10e-6)
        {
          int face_num = intersected_face_number(direction); assert(face_num != -1);
          double s = intersection_parameter(direction, face_num); assert(s != -1000);
          f_values(x, y, direction, s, face_num, f);
          s_list[0] = -s; s_list[1] = 0.0; s_list[2] = s;

          //zero crossing check
          if (f[0]*f[1]<0 || f[1]*f[2]<0) 
          {
            double s_star = 5.0; // location of the zero crossing

            //compute location of extrema by fitting a line
            //s_star = (parabola_fit_type_ == dbdet_zc_det_params::PFIT_3_POINTS) ? 
            //                    subpixel_s(s_list, f) : subpixel_s(x, y, direction);

            bool zc_valid = false;

            //using two points only
            if (f[0]*f[1]<0 && f[0]<0){// && ((option_==1 && f[0]>0) || (option_==2 && f[0]<0) || (option_==0))
              double m = (f[1]-f[0])/s;
              s_star = -f[0]/m -s;

              if (s_star<=0 && s_star>-vcl_sqrt(2.0)/2.0)
                zc_valid = true;
            }
            else if (f[1]*f[2]<0 && f[1]<0) {// && ((option_==1 && f[1]>0) || (option_==2 && f[1]<0) || (option_==0))
              double m = (f[2]-f[1])/s;
              s_star = -f[1]/m;

              if (s_star>=0 && s_star<vcl_sqrt(2.0)/2.0)
                zc_valid = true;
            }
 
            if (zc_valid)
            {
              //record this as a max point
              //mag_(y,x) = 100.0; //FIX ME (this is just a dummy value)

              //if seeking tokens, form tokens from this point
              if (collect_tokens){
                loc.push_back(vgl_point_2d<double>(x + s_star * direction.x(), y + s_star * direction.y()));
                orientation.push_back(vcl_atan2(direction.x(), -direction.y()));
                mag.push_back(mask_(x,y)); //this needs to be updated too
              }
            }
          }
        }
      }
    }
  }
}

int dbdet_zc_det::intersected_face_number(const vgl_vector_2d<double>& direction)
{
  if (direction.x() >= 0 && direction.y() >= 0)
  {
    if (direction.x() >= direction.y())
      return 1;
    else
      return 2;
  }
  else if (direction.x() < 0 && direction.y() >= 0)
  {
    if (vcl_abs(direction.x()) < direction.y())
      return 3;
    else
      return 4;
  }
  else if (direction.x() < 0 && direction.y() < 0)
  {
    if (vcl_abs(direction.x()) >= vcl_abs(direction.y()))
      return 5;
    else
      return 6;
  }
  else if (direction.x() >= 0 && direction.y() < 0)
  {
    if (direction.x() < vcl_abs(direction.y()))
      return 7;
    else
      return 8;
  }
  return -1;
}

double dbdet_zc_det::intersection_parameter(const vgl_vector_2d<double>& direction, int face_num)
{
  if (face_num == 1 || face_num == 8)
    return 1.0/direction.x();
  else if (face_num == 2 || face_num == 3)
    return 1.0/direction.y();
  else if (face_num == 4 || face_num == 5)
    return -1.0/direction.x();
  else if (face_num == 6 || face_num == 7)
    return -1.0/direction.y();
  return -1000.0;
}

void dbdet_zc_det::f_values(int x, int y, const vgl_vector_2d<double>& direction, double s, int face_num, double *f)
{
  int corners[4];
  get_relative_corner_coordinates(face_num, corners);

  vgl_vector_2d<double> intersection_point = s * direction;
  vgl_vector_2d<double> corner1(corners[0], corners[1]); //have to convert to double for subtraction
  vgl_vector_2d<double> corner2(corners[2], corners[3]); //have to convert to double for subtraction
  double distance1 = length(intersection_point - corner1);
  double distance2 = length(intersection_point - corner2);
  double value1 = grad_mag_(x+corners[0], y+corners[1]);
  double value2 = grad_mag_(x+corners[2], y+corners[3]);
  double f_plus = value1 * distance2 + value2 * distance1;

  intersection_point = -s * direction;
  corner1.set(-corners[0], -corners[1]); //have to convert to double for subtraction
  corner2.set(-corners[2], -corners[3]); //have to convert to double for subtraction
  distance1 = length(intersection_point - corner1);
  distance2 = length(intersection_point - corner2);
  value1 = grad_mag_(x-corners[0], y-corners[1]);
  value2 = grad_mag_(x-corners[2], y-corners[3]);
  double f_minus = value1 * distance2 + value2 * distance1;

  f[0] = f_minus;
  f[1] = grad_mag_(x,y);
  f[2] = f_plus;
}

void dbdet_zc_det::get_relative_corner_coordinates(int face_num, int *corners)
{
  switch (face_num)
  {
   case 1:
     corners[0] = 1;
     corners[1] = 0;
     corners[2] = 1;
     corners[3] = 1;
     break;
   case 2:
     corners[0] = 1;
     corners[1] = 1;
     corners[2] = 0;
     corners[3] = 1;
     break;
   case 3:
     corners[0] = 0;
     corners[1] = 1;
     corners[2] = -1;
     corners[3] = 1;
    break;
   case 4:
     corners[0] = -1;
     corners[1] = 1;
     corners[2] = -1;
     corners[3] = 0;
    break;
   case 5:
     corners[0] = -1;
     corners[1] = 0;
     corners[2] = -1;
     corners[3] = -1;
    break;
   case 6:
     corners[0] = -1;
     corners[1] = -1;
     corners[2] = 0;
     corners[3] = -1;
    break;
   case 7:
     corners[0] = 0;
     corners[1] = -1;
     corners[2] = 1;
     corners[3] = -1;
    break;
   case 8:
     corners[0] = 1;
     corners[1] = -1;
     corners[2] = 1;
     corners[3] = 0;
    break;
   default:
     corners[0] = 0;
     corners[1] = 0;
     corners[2] = 0;
     corners[3] = 0;
  }
}

double dbdet_zc_det::subpixel_s(double *s, double *f)
{
  //linear regression
  double Sx = s[0] + s[1] + s[2];
  double Sy = f[0] + f[1] + f[2];
  double Sxx = s[0]*s[0] + s[1]*s[1] + s[2]*s[2];
  double Sxy = s[0]*f[0] + s[1]*f[1] + s[2]*f[2];

  double m = (3*Sxy-Sx*Sy)/(3*Sxx-Sx*Sx);
  double b = (Sy-m*Sx)/3;

  //determine zero crossing
  double s_star=10.0; //large number
  if (vcl_abs(m)>1e-5)
    s_star = -b/m;

  return s_star;
}

double dbdet_zc_det::subpixel_s(int x, int y, const vgl_vector_2d<double>& direction)
{
  double d;
  double s;
  double f;
  vgl_homg_point_2d<double> p1(0.0, 0.0);
  vgl_homg_point_2d<double> p2(direction.x(), direction.y());
  vgl_homg_line_2d<double> line1(p1,p2);
  //construct the matrices
  vnl_matrix<double> A(9, 3);
  vnl_matrix<double> B(9, 1);
  vnl_matrix<double> P(3, 1);
  int index = 0;
  for (int j = -1; j <= 1; j++)
  {
    for (int i = -1; i <= 1; i++)
    {
      find_distance_s_and_f_for_point(i, j, line1, d, s, direction);
      f = grad_mag_(x+i,y+j);
      A(index, 0) = vcl_pow(s,2.0);
      A(index, 1) = s;
      A(index, 2) = 1.0;
      B(index, 0) = f;
      index++;
    }
  }
//  vnl_matrix<double> A_trans = A.transpose();
//  vnl_matrix<double> temp = vnl_matrix_inverse<double> (A_trans*A);
//  vnl_matrix<double> temp2 = temp * A_trans;
//  P = temp2 * B;
  vnl_svd<double> svd(A);
  P = svd.solve(B);
  double s_star = -P(1,0)/(2*P(0,0));
  return s_star;
}

void dbdet_zc_det::find_distance_s_and_f_for_point(int x, int y, vgl_homg_line_2d<double> line,
                                                double &d, double &s, const vgl_vector_2d<double>& direction)
{
  vgl_homg_point_2d<double> point(x,y);
  vgl_homg_line_2d<double> perp_line = vgl_homg_operators_2d<double>::perp_line_through_point(line, point);
  vgl_homg_point_2d<double> intersection_point_homg = vgl_homg_operators_2d<double>::intersection(line, perp_line);
  vgl_point_2d<double> intersection_point(intersection_point_homg);
  vgl_vector_2d<double> d_helper(x-intersection_point.x(), y-intersection_point.y());
  d = length(d_helper);
  s = intersection_point.x() / direction.x();
}

//----------------------------------------------------------
//: Clear internal storage
//
void dbdet_zc_det::clear()
{
  mag_.clear();
}
