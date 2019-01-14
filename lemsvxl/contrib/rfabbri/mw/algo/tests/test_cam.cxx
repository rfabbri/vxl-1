#include <testlib/testlib_test.h>

#include <limits>
#include <vector>
#include <bmcsd/bmcsd_util.h>
#include <vnl/vnl_double_3x3.h>
#include <algorithm>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_point_2d.h>
#include <bdifd/algo/bdifd_data.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_calibration_matrix.h>
#include <vpgl/algo/vpgl_camera_compute.h>


static const double tolerance=std::numeric_limits<double>::epsilon()*100;
bool synthesize_3d_2d_corresps();
bool
read_calib(
    const std::string &camera_f,
    const std::string &center_direction_f,
    vnl_matrix_fixed<double,3,4> &camera,
    vnl_vector_fixed<double,3> &center,
    vnl_vector_fixed<double,3> &view_direction
);

static bool
read_calib(
    std::vector<vnl_matrix_fixed<double,3,4> > &camera_matrices,
    std::vector<vnl_vector_fixed<double,3> > &camera_center,
    std::vector<vnl_vector_fixed<double,3> > &camera_view_direction
);

//: tests Tom Pollard's calibration utility from 3D to 2D point corresps
MAIN( test_cam )
{
  START ("manual camera calibration from 3D to 2D points");
//  synthesize_3d_2d_corresps();

//  std::string camera_f("/home/rfabbri/lib/data/lafayette-1218/Lafayette_1_v1_cam.txt");
//  std::string center_direction_f("/home/rfabbri/lib/data/lafayette-1218/Lafayette_1_v1_center-direction.txt");


//  vnl_matrix_fixed<double,3,4> camera;
//  vnl_vector_fixed<double,3> center;
//  vnl_vector_fixed<double,3> view_direction;
//  read_calib(camera_f,center_direction_f,camera,center,view_direction);

//  std::cout << "Cam: " << camera << std::endl;
//  std::cout << "Center: " << center << std::endl;
//  std::cout << "Direction: " << view_direction<< std::endl;

  //----------------
  std::vector<vnl_matrix_fixed<double,3,4> > camera_matrices;
  std::vector<vnl_vector_fixed<double,3> > camera_center;
  std::vector<vnl_vector_fixed<double,3> > camera_view_direction;

  read_calib(
      camera_matrices,
      camera_center,
      camera_view_direction
  );

  SUMMARY();
}

//: read just for one view
bool
read_calib(
    const std::string &camera_f,
    const std::string &center_direction_f,
    vnl_matrix_fixed<double,3,4> &camera,
    vnl_vector_fixed<double,3> &center,
    vnl_vector_fixed<double,3> &view_direction
)
{

  {
    std::ifstream fp( camera_f.c_str() );
    if (!fp) {
      std::cerr << "read: error, unable to open file name " << camera_f << std::endl;
      return false;
    }

    fp >> camera;
  }

  {
    std::ifstream fp( center_direction_f.c_str() );
    if (!fp) {
      std::cerr << "read: error, unable to open file name " << center_direction_f << std::endl;
      return false;
    }

    fp >> center;
    fp >> view_direction;
  }
  return true;
}

//: read for all views
bool
read_calib(
    std::vector<vnl_matrix_fixed<double,3,4> > &camera_matrices,
    std::vector<vnl_vector_fixed<double,3> > &camera_center,
    std::vector<vnl_vector_fixed<double,3> > &camera_view_direction
)
{
  std::string mypath("/vision/video/rfabbri/lafayette-1218/");

  unsigned const n_blocks = 32;
  unsigned const n_directions_per_block = 4;
  camera_matrices.reserve(32);
  camera_center.reserve(32);
  camera_view_direction.reserve(32);
  bool final_retval = true;

  for(unsigned ib=1; ib <= n_blocks; ib++)
    for (unsigned iv=1; iv <= n_directions_per_block; iv++) {
      std::string myprefix;

      std::stringstream sstr;
      sstr << "Lafayette_" << ib << "_v" << iv;
      sstr >> myprefix;

      std::string camera_f = mypath + myprefix + std::string("_cam.txt");
      std::string center_direction_f = mypath + myprefix + std::string("_center-direction.txt");

      vnl_matrix_fixed<double,3,4> camera;
      vnl_vector_fixed<double,3> center;
      vnl_vector_fixed<double,3> view_direction;
      bool retval = read_calib(camera_f,center_direction_f,camera,center,view_direction);
      if (!retval) {
        final_retval = false;
        std::cerr << "Skipping file " << myprefix << "*" << std::endl;
        camera_matrices.push_back(camera);
        camera_center.push_back(center);
        camera_view_direction.push_back(view_direction);
      } else {
        camera_matrices.push_back(camera);
        camera_center.push_back(center);
        camera_view_direction.push_back(view_direction);

        std::cout << "Block " << ib << " view " << iv << std::endl;
        std::cout << "Cam: " << camera << std::endl;
        std::cout << "Center: " << center << std::endl;
        std::cout << "Direction: " << view_direction<< std::endl << std::endl;
      }
    }
  return final_retval;
}

bool
synthesize_3d_2d_corresps()
{
  // our output files
  std::string world_points_file( "world_pts.txt" );
  std::string image_points_file( "image_pts.txt" );
  
  // our input files
  std::string camera_file( "cameras.txt" );
  std::string camera_center_direction_file( "cameras-center-direction.txt" );

  unsigned  crop_origin_x_ = 450;
  unsigned  crop_origin_y_ = 1750;
  double x_max_scaled = 500;
//  bool res; // return values

  vnl_double_3x3 Kmatrix;
  bdifd_turntable::internal_calib_olympus(Kmatrix, x_max_scaled, crop_origin_x_, crop_origin_y_);
  vpgl_calibration_matrix<double> K(Kmatrix);


  vpgl_perspective_camera<double> *P;
  // ground-truth cam
  P = bdifd_turntable::camera_olympus(0, K);

  std::vector<std::vector<bdifd_3rd_order_point_3d> > crv3d;
  bdifd_data::space_curves_olympus_turntable( crv3d );

  unsigned npts=0;
  for (unsigned nc=0; nc < crv3d.size(); nc++)
    npts+=crv3d[nc].size();

  unsigned ipt=0;
  vnl_matrix<double> pts3d(npts,3);
  for (unsigned nc=0; nc < crv3d.size(); nc++)
    for (unsigned i=0; i < crv3d[nc].size(); ++i) {
      pts3d(ipt,0) = crv3d[nc][i].Gama[0];
      pts3d(ipt,1) = crv3d[nc][i].Gama[1];
      pts3d(ipt,2) = crv3d[nc][i].Gama[2];
      ++ipt;
    }

  {
  std::ofstream fp;
  fp.open(world_points_file.c_str(),std::ios::out);
  if (!fp) {
    std::cerr << "write: error, unable to open file name" << std::endl;
    return false;
  }

  fp << pts3d;
  fp.close();
  }


  // Now project the 3D points

  std::vector<bdifd_camera> cam_gt_(1);
  cam_gt_[0].set_p(*P);

  std::vector<std::vector<bdifd_3rd_order_point_2d> > crv2d_gt;
  bdifd_data::project_into_cams(crv3d, cam_gt_, crv2d_gt);

  vnl_matrix<double> pts2d(npts,2);
  for (unsigned i=0; i < crv2d_gt[0].size(); ++i) {
    pts2d(i,0) = crv2d_gt[0][i].gama[0];
    pts2d(i,1) = crv2d_gt[0][i].gama[1];
  }
  {
  std::ofstream fp;
  fp.open(image_points_file.c_str(),std::ios::out);
  if (!fp) {
    std::cerr << "write: error, unable to open file name" << std::endl;
    return false;
  }

  fp << pts2d;
  fp.close();
  }

  // todo: show residual
  return true;
}
