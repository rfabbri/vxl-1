//: 
// \file    bioproj_ridge_detection_example.cxx
// \brief   an example showing how to do ridge detection
//          9 response sets are read from 9 different files
//          It is the user's responsibility to make sure every file has
//          same dimension response sets
// \author  H. Can Aras
// \date    2005-07-19
// 

#include <vcl_ctime.h>
#include <vcl_string.h>
#include <vcl_cstdio.h>
#include <vcl_cstdlib.h>
#include <vcl_fstream.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/algo/vil3d_grad_1x3.h>
#include <vil3d/algo/vil3d_structuring_element.h>
#include <vil3d/algo/vil3d_binary_erode.h>
#include <dbil3d/algo/dbil3d_detect_ridges.h>
#include <dbil3d/algo/dbil3d_gauss_filter.h>
#include <vcl_vector.h>
#include <vsol/vsol_cylinder.h>
#include <vcl_fstream.h>
#include <vcl_cmath.h>
#include <det/det_cylinder_map.h>
#include <det/det_cylinder_detect.h>
#include <det/det_nonmaxium_suppression.h>

#include <slice/sliceFileManager.h>

const double CYL_RADIUS = 0.5;
const double CYL_LENGTH = 1.0;

int main(int argc, char *argv[])
{
  if(argc != 6)
  {
    vcl_cout << "Usage: " << argv[0] << " [input prefix] [output prefix] [epsilon] [aspect_thresh] [lambda_thresh]\n ";
    exit(-1);
  }


  vcl_string fnamebase = argv[1];

  int dimx, dimy, dimz, dummy;
  double val;

  vcl_string fname_s = fnamebase + "_s.out";
  vcl_string fname_x = fnamebase + "_x.out";
  vcl_string fname_y = fnamebase + "_y.out";
  vcl_string fname_z = fnamebase + "_z.out";

  vil3d_image_view<float> I_s = sliceFileManager<float>::read(fname_s);
  vil3d_image_view<float> I_x = sliceFileManager<float>::read(fname_x); 
  vil3d_image_view<float> I_y = sliceFileManager<float>::read(fname_y); 
  vil3d_image_view<float> I_z = sliceFileManager<float>::read(fname_z);



  
  // create the vil3d images needed for the detection
  vil3d_image_view<int> rho(dimx, dimy, dimz);
  vil3d_image_view<float> lambda1(dimx, dimy, dimz);
  vil3d_image_view<float> lambda2(dimx, dimy, dimz);
  vil3d_image_view<float> eigenv3(dimx, dimy, dimz, 3);
  vil3d_image_view<bool> binary(dimx, dimy, dimz);

  ////////////////////////////////////////////////////////////
  // ridge detection
  float epsilon = float(vcl_atof(argv[3]));

  // uses derivatives all computed by the projected method
//  dbil3d_detect_ridges(I_x, I_y, I_z, I_xx, I_yy, I_zz, I_xy, I_xz, I_yz, epsilon, rho, lambda1, lambda2, eigenv3);

  // uses first derivatives computed by the projected method, second derivatives are computed from the first derivatives
  // by taking the gradient, this is the version which should be used at the moment
  dbil3d_detect_ridges(I_x, I_y, I_z, epsilon, rho, lambda1, lambda2, eigenv3);
  
  // not used anymore inside this code
//  dbil3d_detect_ridges<float> (I, float(50/35.4), float(0.7), rho, lambda1, lambda2, eigenv3);

  double ratio_threshold = double( vcl_atof(argv[4]) );
  double lambda_threshold = double( vcl_atof(argv[5]) );

  // find the maximum first eigenvalue in absolute value
  float max_lambda = 0;
  for(int k=0;k<dimz;k++)
  {
    for(int j=0;j<dimy;j++)
    {
      for(int i=0;i<dimx;i++)
      {
        if(max_lambda < vcl_fabs(lambda1(i,j,k)))
          max_lambda = vcl_fabs(lambda1(i,j,k));
      }
    }
  }

  // determine the ridge candidates by applying additional thresholds
  for(int k=0;k<dimz;k++)
  {
    for(int j=0;j<dimy;j++)
    {
      for(int i=0;i<dimx;i++)
      {
        float l1 = lambda1(i,j,k);
        float l2 = lambda2(i,j,k);
        double ratio = vcl_fabs(l1-l2) / (vcl_fabs(l1+l2)/2);
        if (rho(i,j,k) == 1 && ratio <= ratio_threshold && vcl_fabs(l1) >= lambda_threshold*max_lambda)
          binary(i,j,k) = true;
        else
          binary(i,j,k) = false;
      }
    }
  }
  ////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////
  // sub-pixel interpolation

  det_cylinder_detect detector;
  det_cylinder_map cm = detector.apply((unsigned)dimx, (unsigned)dimy, (unsigned)dimz, eigenv3, I_s);

  int neighb_size = 3;
  det_nonmaxium_suppression nms(neighb_size);
  cm = nms.apply(cm);

  vcl_vector<vsol_cylinder_sptr> cylinders;
  vcl_vector<double> strengths;

  // create the cylinders for the surviving ridge voxels
  for(int k=0;k<dimz;k++)
  {
    for(int j=0;j<dimy;j++)
  {
      for(int i=0;i<dimx;i++)
      {
        if (binary(i,j,k) && cm[i][j][k].location_ != vgl_point_3d<double> (0.,0.,0.))
        {
          vcl_cout << vgl_point_3d<double>(double(i), double(j), double(k)) << vcl_endl;
          vcl_cout << 1.0 << vcl_endl;

          double x = i + cm[i][j][k].location_.x();
          double y = j + cm[i][j][k].location_.y();
          double z = k + cm[i][j][k].location_.z();

          // set center, radius, length and orientation
//          vsol_cylinder_sptr cyl = new vsol_cylinder(vgl_point_3d<double> (double(i), double(j), double(k)), CYL_RADIUS, CYL_LENGTH);
          vsol_cylinder_sptr cyl = new vsol_cylinder(vgl_point_3d<double> (x, y, z), CYL_RADIUS, CYL_LENGTH);
          vgl_vector_3d<double> direction ((double)(eigenv3(i,j,k,0)), (double)(eigenv3(i,j,k,1)), (double)(eigenv3(i,j,k,2)));
          normalize(direction);
          cyl->set_orientation(direction);
          cylinders.push_back(cyl);
          strengths.push_back(1.0);
        }
      }
    }
  }
  ////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////
  // write the cylinders to binary stream
  vcl_string o_file = argv[2];
  vsl_b_ofstream stream(o_file.c_str());
  // write the version number
  vsl_b_write(stream, (int) 1);

  // write the number of cylinders
  vsl_b_write(stream, (int) cylinders.size());

  for (unsigned int i=0; i<cylinders.size(); i++){
    vsol_cylinder_sptr cyl = cylinders[i];
    // first write the strength
    vsl_b_write(stream, (double) strengths[i]);
    cyl->b_write(stream);
  }
  stream.close();

  return 0;
}
