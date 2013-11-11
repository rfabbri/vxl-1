#include <vcl_iomanip.h>
#include <vcl_sstream.h>
#include <vul/vul_file.h>
#include <vnl/vnl_random.h>
#include <dbdif/dbdif_camera.h>
#include <dbdif/algo/dbdif_data.h>
#include <dbsol/dbsol_file_io.h>
#include <dbdet/edge/dbdet_edgemap.h>
#include <dbdet/algo/dbdet_load_edg.h>
#include <mw/mw_util.h>
#include <mw/algo/mw_algo_util.h>
#include <mw/dbmcs_curve_3d_sketch.h>

// Generate a synthetic sequence of curves
int
main(int argc, char **argv)
{
  unsigned  crop_origin_x_ = 450;
  unsigned  crop_origin_y_ = 1750;
//    double x_max_scaled = 255;
  double x_max_scaled = 500;
  unsigned nviews=20;

  vcl_string dir("./out-tmp");
  vcl_string prefix("frame_");

  vnl_double_3x3 Kmatrix;
  dbdif_turntable::internal_calib_olympus(Kmatrix, x_max_scaled, crop_origin_x_, crop_origin_y_);

  vpgl_calibration_matrix<double> K(Kmatrix);
  vcl_vector<vpgl_perspective_camera<double> > cam_vpgl;
  vcl_vector<dbdif_camera> cam_gt;
  cam_vpgl.resize(nviews);
  cam_gt.resize(nviews);

  static vnl_random myrand;


  for (unsigned i=0; i < nviews; ++i) {
    vpgl_perspective_camera<double> *P;
    P = dbdif_turntable::camera_olympus(6*i, K);
    cam_gt[i].set_p(*P);
    delete P;

    P = dbdif_turntable::camera_olympus(6*i+myrand.drand64(-4,4), K);

    cam_vpgl[i] = *P;
    delete P;
  }

  // write the cameras out

  vul_file::make_directory(dir);

  bool retval =  
    mw_util::write_cams(dir, prefix, mw_util::MW_INTRINSIC_EXTRINSIC, cam_vpgl);
  if (!retval)
    abort();
  

  // crv2d[i][j]  curve i view j
  vcl_vector<vcl_vector<vcl_vector<dbdif_3rd_order_point_2d> > > crv2d;
  vcl_vector<vcl_vector<dbdif_3rd_order_point_3d> > crv3d;
//  dbdif_data::space_curves_digicam_turntable_sandbox( crv3d );
  dbdif_data::space_curves_olympus_turntable( crv3d );


  // XXX hack: select specific subset

  {
  vcl_vector<vcl_vector<dbdif_3rd_order_point_3d> > crv3d_ss;
  crv3d_ss.reserve(crv3d.size());
  for (unsigned  i=0; i < crv3d.size(); ++i) {
//    if (i == 8 || i == 10 || i == 15)
    if (i == 38 || i == 37 || i == 30)
      crv3d_ss.push_back(crv3d[i]);
  }
  crv3d = crv3d_ss;
  }

  crv2d.resize(crv3d.size());

  for (unsigned  i=0; i < crv3d.size(); ++i)
    dbdif_data::project_into_cams(crv3d[i], cam_gt, crv2d[i]);

  vcl_vector<vcl_vector<dbdif_1st_order_point_3d> > crv3d_1st(crv3d.size());
  for (unsigned  i=0; i < crv3d.size(); ++i) {
    crv3d_1st[i].resize(crv3d[i].size());
    for (unsigned k=0; k < crv3d[i].size(); ++k) {
      crv3d_1st[i][k] = crv3d[i][k];
    }
  }

  //: image coordinates
  // xi[i][k] == curve i at view k
  unsigned  number_of_curves = crv2d.size();

  for (unsigned  k=0; k < nviews; ++k) {
    vcl_vector< vsol_spatial_object_2d_sptr > polys(number_of_curves);
    for (unsigned i=0; i<number_of_curves; ++i) {
      vcl_vector<vsol_point_2d_sptr> xi; 
      xi.resize(crv2d[i][k].size());
      for (unsigned  j=0; j < crv2d[i][k].size(); ++j) 
        xi[j] = new vsol_point_2d(crv2d[i][k][j].gama[0], crv2d[i][k][j].gama[1]);
      polys[i] = new vsol_polyline_2d(xi);
    }

    vcl_ostringstream v_str;
    v_str << vcl_setw(4) << vcl_setfill('0') << k;
    vcl_string filename = dir + vcl_string("/") + prefix + v_str.str() + vcl_string(".cemv.gz");
    dbsol_save_cem(polys, filename);
  }

  // We now need to generate the edgemaps.

  for (unsigned  k=0; k < nviews; ++k) {
    vcl_vector< dbdet_edgel *> edgels;
    for (unsigned i=0; i<number_of_curves; ++i) {
      for (unsigned  j=0; j < crv2d[i][k].size(); ++j) {
        edgels.push_back(new dbdet_edgel);
        mw_algo_util::dbdif_to_dbdet(crv2d[i][k][j], edgels.back());
      }
    }
    dbdet_edgemap_sptr em = new dbdet_edgemap(520, 380, edgels);

    vcl_ostringstream v_str;
    v_str << vcl_setw(4) << vcl_setfill('0') << k;
    vcl_string filename = dir + vcl_string("/") + prefix + v_str.str() + vcl_string(".edg.gz");
    bool retval = dbdet_save_edg(filename, em);
    if (!retval)
      abort();
  }


  // The 3D Curve Sketch

  vcl_vector< dbmcs_curve_3d_attributes > attr(number_of_curves);
  for (unsigned i=0; i < number_of_curves; ++i) {
    attr[i].set_views(new dbmcs_stereo_views);
    attr[i].v_->set_stereo0(0);
    attr[i].v_->set_stereo1(nviews-1);
  }

  dbmcs_curve_3d_sketch csk(crv3d_1st, attr);
  csk.write_dir_format(dir+vcl_string("/csk"));

  return 0;
}
