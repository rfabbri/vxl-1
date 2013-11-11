#include <testlib/testlib_test.h>

#include <vcl_limits.h>
#include <vcl_vector.h>
#include <mw/mw_util.h>
#include <vnl/vnl_double_3x3.h>
#include <vcl_algorithm.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_point_2d.h>
#include <dbdif/algo/dbdif_data.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_calibration_matrix.h>
#include <vpgl/algo/vpgl_camera_compute.h>


static const double tolerance=vcl_numeric_limits<double>::epsilon()*100;
bool synthesize_3d_2d_corresps();
bool
read_calib(
    const vcl_string &camera_f,
    const vcl_string &center_direction_f,
    vnl_matrix_fixed<double,3,4> &camera,
    vnl_vector_fixed<double,3> &center,
    vnl_vector_fixed<double,3> &view_direction
);

static bool
read_calib(
    vcl_vector<vnl_matrix_fixed<double,3,4> > &camera_matrices,
    vcl_vector<vnl_vector_fixed<double,3> > &camera_center,
    vcl_vector<vnl_vector_fixed<double,3> > &camera_view_direction
);

//: tests Tom Pollard's calibration utility from 3D to 2D point corresps
MAIN( test_cam )
{
  START ("manual camera calibration from 3D to 2D points");
//  synthesize_3d_2d_corresps();

//  vcl_string camera_f("/home/rfabbri/lib/data/lafayette-1218/Lafayette_1_v1_cam.txt");
//  vcl_string center_direction_f("/home/rfabbri/lib/data/lafayette-1218/Lafayette_1_v1_center-direction.txt");


//  vnl_matrix_fixed<double,3,4> camera;
//  vnl_vector_fixed<double,3> center;
//  vnl_vector_fixed<double,3> view_direction;
//  read_calib(camera_f,center_direction_f,camera,center,view_direction);

//  vcl_cout << "Cam: " << camera << vcl_endl;
//  vcl_cout << "Center: " << center << vcl_endl;
//  vcl_cout << "Direction: " << view_direction<< vcl_endl;

  //----------------
  vcl_vector<vnl_matrix_fixed<double,3,4> > camera_matrices;
  vcl_vector<vnl_vector_fixed<double,3> > camera_center;
  vcl_vector<vnl_vector_fixed<double,3> > camera_view_direction;

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
    const vcl_string &camera_f,
    const vcl_string &center_direction_f,
    vnl_matrix_fixed<double,3,4> &camera,
    vnl_vector_fixed<double,3> &center,
    vnl_vector_fixed<double,3> &view_direction
)
{

  {
    vcl_ifstream fp( camera_f.c_str() );
    if (!fp) {
      vcl_cerr << "read: error, unable to open file name " << camera_f << vcl_endl;
      return false;
    }

    fp >> camera;
  }

  {
    vcl_ifstream fp( center_direction_f.c_str() );
    if (!fp) {
      vcl_cerr << "read: error, unable to open file name " << center_direction_f << vcl_endl;
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
    vcl_vector<vnl_matrix_fixed<double,3,4> > &camera_matrices,
    vcl_vector<vnl_vector_fixed<double,3> > &camera_center,
    vcl_vector<vnl_vector_fixed<double,3> > &camera_view_direction
)
{
  vcl_string mypath("/vision/video/rfabbri/lafayette-1218/");

  unsigned const n_blocks = 32;
  unsigned const n_directions_per_block = 4;
  camera_matrices.reserve(32);
  camera_center.reserve(32);
  camera_view_direction.reserve(32);
  bool final_retval = true;

  for(unsigned ib=1; ib <= n_blocks; ib++)
    for (unsigned iv=1; iv <= n_directions_per_block; iv++) {
      vcl_string myprefix;

      vcl_stringstream sstr;
      sstr << "Lafayette_" << ib << "_v" << iv;
      sstr >> myprefix;

      vcl_string camera_f = mypath + myprefix + vcl_string("_cam.txt");
      vcl_string center_direction_f = mypath + myprefix + vcl_string("_center-direction.txt");

      vnl_matrix_fixed<double,3,4> camera;
      vnl_vector_fixed<double,3> center;
      vnl_vector_fixed<double,3> view_direction;
      bool retval = read_calib(camera_f,center_direction_f,camera,center,view_direction);
      if (!retval) {
        final_retval = false;
        vcl_cerr << "Skipping file " << myprefix << "*" << vcl_endl;
        camera_matrices.push_back(camera);
        camera_center.push_back(center);
        camera_view_direction.push_back(view_direction);
      } else {
        camera_matrices.push_back(camera);
        camera_center.push_back(center);
        camera_view_direction.push_back(view_direction);

        vcl_cout << "Block " << ib << " view " << iv << vcl_endl;
        vcl_cout << "Cam: " << camera << vcl_endl;
        vcl_cout << "Center: " << center << vcl_endl;
        vcl_cout << "Direction: " << view_direction<< vcl_endl << vcl_endl;
      }
    }
  return final_retval;
}

bool
synthesize_3d_2d_corresps()
{
  // our output files
  vcl_string world_points_file( "world_pts.txt" );
  vcl_string image_points_file( "image_pts.txt" );
  
  // our input files
  vcl_string camera_file( "cameras.txt" );
  vcl_string camera_center_direction_file( "cameras-center-direction.txt" );

  unsigned  crop_origin_x_ = 450;
  unsigned  crop_origin_y_ = 1750;
  double x_max_scaled = 500;
//  bool res; // return values

  vnl_double_3x3 Kmatrix;
  dbdif_turntable::internal_calib_olympus(Kmatrix, x_max_scaled, crop_origin_x_, crop_origin_y_);
  vpgl_calibration_matrix<double> K(Kmatrix);


  vpgl_perspective_camera<double> *P;
  // ground-truth cam
  P = dbdif_turntable::camera_olympus(0, K);

  vcl_vector<vcl_vector<dbdif_3rd_order_point_3d> > crv3d;
  dbdif_data::space_curves_olympus_turntable( crv3d );

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
  vcl_ofstream fp;
  fp.open(world_points_file.c_str(),vcl_ios::out);
  if (!fp) {
    vcl_cerr << "write: error, unable to open file name" << vcl_endl;
    return false;
  }

  fp << pts3d;
  fp.close();
  }


  // Now project the 3D points

  vcl_vector<dbdif_camera> cam_gt_(1);
  cam_gt_[0].set_p(*P);

  vcl_vector<vcl_vector<dbdif_3rd_order_point_2d> > crv2d_gt;
  dbdif_data::project_into_cams(crv3d, cam_gt_, crv2d_gt);

  vnl_matrix<double> pts2d(npts,2);
  for (unsigned i=0; i < crv2d_gt[0].size(); ++i) {
    pts2d(i,0) = crv2d_gt[0][i].gama[0];
    pts2d(i,1) = crv2d_gt[0][i].gama[1];
  }
  {
  vcl_ofstream fp;
  fp.open(image_points_file.c_str(),vcl_ios::out);
  if (!fp) {
    vcl_cerr << "write: error, unable to open file name" << vcl_endl;
    return false;
  }

  fp << pts2d;
  fp.close();
  }

  // todo: show residual
  return true;
}
