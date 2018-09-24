#include <vcl_iomanip.h>
#include <vcl_sstream.h>
#include <vul/vul_file.h>
#include <vnl/vnl_random.h>
#include <bdifd/bdifd_camera.h>
#include <bdifd/algo/bdifd_data.h>
#include <bsold/bsold_file_io.h>
#include <sdet/sdet_edgemap.h>
#include <sdetd/io/sdetd_load_edg.h>
#include <bmcsd/bmcsd_util.h>
#include <bmcsd/algo/bmcsd_algo_util.h>
#include <bmcsd/bmcsd_curve_3d_sketch.h>

// Generate a more complete synthetic sequence of curves
// for the Public // https://github.com/rfabbri/synthcurves-multiview-3d-dataset/#curves
// 
// See also
// https://github.com/rfabbri/synthcurves-multiview-3d-dataset
// 
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
  bdifd_turntable::internal_calib_olympus(Kmatrix, x_max_scaled, crop_origin_x_, crop_origin_y_);

  vpgl_calibration_matrix<double> K(Kmatrix);
  vcl_vector<vpgl_perspective_camera<double> > cam_vpgl;
  vcl_vector<bdifd_camera> cam_gt;
  cam_vpgl.resize(nviews);
  cam_gt.resize(nviews);

  static vnl_random myrand;

  for (unsigned i=0; i < nviews; ++i) {
    vpgl_perspective_camera<double> *P;
    P = bdifd_turntable::camera_olympus(6*i, K);
    cam_gt[i].set_p(*P);
    cam_vpgl[i] = *P;
    delete P;
  }

  // write the cameras out

  vul_file::make_directory(dir);

  bool retval =  
    bmcsd_util::write_cams(dir, prefix, bmcsd_util::BMCS_INTRINSIC_EXTRINSIC, cam_vpgl);

  std::cout << "VPGL CAM 003: " << cam_vpgl[3].get_matrix() << std::endl;
  if (!retval)
    abort();
  

  // crv2d[i][j]  curve i view j
  vcl_vector<vcl_vector<vcl_vector<bdifd_3rd_order_point_2d> > > crv2d;
  vcl_vector<vcl_vector<bdifd_3rd_order_point_3d> > crv3d;
//  bdifd_data::space_curves_digicam_turntable_sandbox( crv3d );
  bdifd_data::space_curves_olympus_turntable( crv3d );

  vgl_point_3d<double> pt_analyze(crv3d[0][2].Gama[0], crv3d[0][2].Gama[1], crv3d[0][2].Gama[2]);
  std::cout << "VPGL PROJ 003 pt 3: " << "pt " << std::endl << pt_analyze << std::endl <<
    "project: " << std::endl <<  cam_vpgl[3].project(pt_analyze) << std::endl;

  crv2d.resize(crv3d.size());

  for (unsigned  i=0; i < crv3d.size(); ++i)
    bdifd_data::project_into_cams(crv3d[i], cam_gt, crv2d[i]);


  //: image coordinates
  // xi[i][k] == curve i at view k
  // write points as ASCII and also .cemv
  unsigned  number_of_curves = crv2d.size();
  assert(crv3d.size() == crv2d.size());

  vcl_ofstream fp_crv_id;
  
  std::string fname_crv_id = dir + vcl_string("/") + "crv-ids.txt";
  fp_crv_id.open(fname_crv_id.c_str());
  if (!fp_crv_id) {
    vcl_cerr << "generate_synth_sequence: error, unable to open file name " << fname_crv_id << vcl_endl;
    return 1;
  }
    
  for (unsigned  k=0; k < nviews; ++k) {
    vcl_ostringstream v_str;
    v_str << vcl_setw(4) << vcl_setfill('0') << k;
    vcl_string fname_base = dir + vcl_string("/") + prefix + v_str.str();
    
    vcl_ofstream fp_pts2d;
    std::string fname_pts2d = fname_base + "-pts-2D.txt";
    
    fp_pts2d.open(fname_pts2d.c_str());
    if (!fp_pts2d) {
      vcl_cerr << "generate_synth_sequence: error, unable to open file name " << fname_pts2d << vcl_endl;
      return 1;
    }
    
    fp_pts2d << vcl_setprecision(20);

    
    vcl_vector< vsol_spatial_object_2d_sptr > polys(number_of_curves);
    for (unsigned i=0; i<number_of_curves; ++i) {
      vcl_vector<vsol_point_2d_sptr> xi; 
      xi.resize(crv2d[i][k].size());
      for (unsigned  j=0; j < crv2d[i][k].size(); ++j)  {
        if (k == 0)
          fp_crv_id << i << std::endl;
        xi[j] = new vsol_point_2d(crv2d[i][k][j].gama[0], crv2d[i][k][j].gama[1]);
        fp_pts2d << crv2d[i][k][j].gama[0] << " " << crv2d[i][k][j].gama[1] << std::endl;
      }
      polys[i] = new vsol_polyline_2d(xi);
    }
    fp_crv_id.close();

    // bsold_save_cem(polys, fname_base + vcl_string(".cemv.gz"));
  }

  // edgemaps.

  for (unsigned  k=0; k < nviews; ++k) {
    vcl_ostringstream v_str;
    v_str << vcl_setw(4) << vcl_setfill('0') << k;
    vcl_string fname_base = dir + vcl_string("/") + prefix + v_str.str();
    
    vcl_ofstream fp_tgts2d;
    std::string fname_tgts2d = fname_base + "-tgts-2D.txt";
    
    fp_tgts2d.open(fname_tgts2d.c_str());
    if (!fp_tgts2d) {
      vcl_cerr << "generate_synth_sequence: error, unable to open file name " << fname_tgts2d << vcl_endl;
      return 1;
    }
    
    fp_tgts2d << vcl_setprecision(20);

    
    vcl_vector< sdet_edgel *> edgels;
    for (unsigned i=0; i<number_of_curves; ++i) {
      for (unsigned  j=0; j < crv2d[i][k].size(); ++j) {
        edgels.push_back(new sdet_edgel);
        bmcsd_algo_util::bdifd_to_sdet(crv2d[i][k][j], edgels.back());
        fp_tgts2d << crv2d[i][k][j].t[0] << " " << crv2d[i][k][j].t[1] << std::endl;
        assert(fabs(crv2d[i][k][j].t[2]) < 1e-4);
      }
    }
    fp_tgts2d.close();
//    sdet_edgemap_sptr em = new sdet_edgemap(520, 380, edgels);

//    vcl_string filename = dir + vcl_string("/") + prefix + v_str.str() + vcl_string(".edg.gz");
//    bool retval = sdetd_save_edg(filename, em);
//    if (!retval)
//      abort();
  }


  // The 3D Curve Sketch

  /*
  vcl_vector< bmcsd_curve_3d_attributes > attr(number_of_curves);
  for (unsigned i=0; i < number_of_curves; ++i) {
    attr[i].set_views(new bmcsd_stereo_views);
    attr[i].v_->set_stereo0(0);
    attr[i].v_->set_stereo1(nviews-1);
  }
  */
  
  std::string fname_crv_3d_pts = dir + vcl_string("/") + "crv-3D-pts.txt";
  std::string fname_crv_3d_tgts = dir + vcl_string("/") + "crv-3D-tgts.txt";
  
  vcl_ofstream fp_crv_3d_pts;
  vcl_ofstream fp_crv_3d_tgts;
  
  fp_crv_3d_pts.open(fname_crv_3d_pts.c_str());
  if (!fp_crv_3d_pts) {
    vcl_cerr << "generate_synth_sequence: error, unable to open file name " << fname_crv_3d_pts << vcl_endl;
    return 1;
  }
  
  fp_crv_3d_tgts.open(fname_crv_3d_tgts.c_str());
  if (!fp_crv_3d_tgts) {
    vcl_cerr << "generate_synth_sequence: error, unable to open file name " << fname_crv_3d_tgts << vcl_endl;
    return 1;
  }
  
  fp_crv_3d_pts << vcl_setprecision(20);
  fp_crv_3d_tgts << vcl_setprecision(20);
  
  vcl_vector<vcl_vector<bdifd_1st_order_point_3d> > crv3d_1st(crv3d.size());
  for (unsigned  i=0; i < crv3d.size(); ++i) {
    crv3d_1st[i].resize(crv3d[i].size());
    for (unsigned k=0; k < crv3d[i].size(); ++k) {
      crv3d_1st[i][k] = crv3d[i][k];
      fp_crv_3d_pts << crv3d[i][k].Gama[0] << " " << crv3d[i][k].Gama[1]  << " " << crv3d[i][k].Gama[2] << std::endl;
      fp_crv_3d_tgts << crv3d[i][k].T[0] << " " << crv3d[i][k].T[1]  << " " << crv3d[i][k].T[2] << std::endl;
    }
  }
  // bmcsd_curve_3d_sketch csk(crv3d_1st, attr);

  //csk.write_dir_format(dir+vcl_string("/csk"));

  return 0;
}
