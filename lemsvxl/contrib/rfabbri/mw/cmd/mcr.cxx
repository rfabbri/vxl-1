#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vsl/vsl_vector_io.h>
#include <dbul/dbul_arg.h>
#include <dbdif/dbdif_camera.h>
#include <dbcsi/dbcsi_curve_distance.h>
#include <mw/mw_util.h>
#include <mw/algo/mw_algo_util.h>
#include <mw/dbmcs_curve_3d_sketch.h>
#include <mw/dbmcs_curve_3d_attributes.h>
#include <mw/algo/mw_data.h>
#include <mw/pro/dbpro_load_camera_source.h>
#include <mw/pro/dbpro_load_edg_source.h>


#define MW_ASSERT(msg, a, b) if ((a) != (b)) { vcl_cerr << (msg) << vcl_endl; abort(); }


//: Stores the concatenation of all inputs from many dbmcs_stereo_jobs
class dvcpl_edgemap_cam_aggregator: public dbpro_sink {
public:

  dvcpl_edgemap_cam_aggregator()
  {}

  dbpro_signal execute()
  {
    assert(input_type_id(0) == typeid(vpgl_perspective_camera<double>));
    assert(input_type_id(1) == typeid(dbdet_edgemap_sptr));
    assert(input_type_id(2) == typeid(vil_image_view<vxl_uint_32>));
    assert(input_type_id(3) == typeid(vil_image_view<unsigned>));

    cam_.set_p(input<vpgl_perspective_camera<double> >(0));
    edge_map_ = input<dbdet_edgemap_sptr>(1);
    dt_ = input<vil_image_view<vxl_uint_32> >(2);
    label_ = input<vil_image_view<unsigned> > (3);

    return DBPRO_VALID;
  }

  dbdif_camera cam_;
  dbdet_edgemap_sptr edge_map_;
  vil_image_view<vxl_uint_32> dt_;
  vil_image_view<unsigned> label_;
};

void dvcpl_reproject_to_find_inliers (
      const mw_curve_stereo_data_path &dpath, 
      dbmcs_curve_3d_sketch *csk,
      double tau_distance,
      double tau_dtheta,
      unsigned tau_support
      );

//: This program reads in a curve sketch, then projects it to all confirmation
// views and writes the inlier views into the attributes file of the curve
// sketch.
//
// The reason this is done after the stereo is that it may have different
// parameters.
//
int
main(int argc, char **argv)
{
  vcl_string prefix_default(".");

  vul_arg<vcl_string> a_prefix("-prefix", 
      "path to main directory of files",prefix_default.c_str());
  vul_arg<vcl_string> a_cam_type("-cam_type",
      "camera type: intrinsic_extrinsic or projcamera","intrinsic_extrinsic");
  vul_arg<vcl_string> a_csk("-curvesketch", "input 3D curve sketch file or directory", "out/");
  vul_arg<unsigned> a_total_support("-totalsupport", 
      "threshold on the total support of the curves", 0);

  vul_arg<double> a_distance_threshold("-dist", 
      "(in pixels) threshold for an edgel to be an inlier to the reprojected curve in each view", 10.0);
  vul_arg<double> a_dtheta_threshold("-dtheta", 
      "(in degrees) threshold in orientation difference for an edgel to be an inlier to reprojected curve in each view", 10.0);

  vul_arg_parse(argc,argv);
  vcl_cout << "\n";

  mw_util::camera_file_type cam_type;

  if (a_cam_type() == "intrinsic_extrinsic") {
    cam_type = mw_util::MW_INTRINSIC_EXTRINSIC;
  } else {
    if (a_cam_type() == "projcamera")
      cam_type = mw_util::MW_3X4;
    else  {
      vcl_cerr << "Error: invalid camera type " << a_cam_type() << vcl_endl;
      return 1;
    }
  }

  mw_curve_stereo_data_path dpath;
  bool retval = 
    mw_data::read_frame_data_list_txt(a_prefix(), &dpath, cam_type);
  if (!retval) return 1;
  vcl_cout << "Dpath:\n" << dpath << vcl_endl;


  dbmcs_curve_3d_sketch *csk = new dbmcs_curve_3d_sketch;
  vcl_string csk_fname = a_prefix() + "/" + a_csk();
  retval  = csk->read_dir_format(csk_fname);
  MW_ASSERT(vcl_string("Error reading 3D curve sketch: ") + csk_fname, retval, true);

  // Now define dvcpl_bundle_adjust_driver.
  dvcpl_reproject_to_find_inliers(dpath, csk, a_distance_threshold(), a_dtheta_threshold(),
      a_total_support());

  //: Write out:
  // - new attributes

  vcl_cout << "Writing new attributes\n";
  vsl_b_ofstream f(csk_fname + "/attributes.vsl");
  vsl_b_write(f, csk->attributes());
  vcl_cout << "num curves in csk: " << csk->num_curves() << vcl_endl;
  vcl_cout << "num curves in attr: " << csk->attributes().size() << vcl_endl;

  return 0;
}

void dvcpl_reproject_to_find_inliers (
      const mw_curve_stereo_data_path &dpath, 
      dbmcs_curve_3d_sketch *csk,
      double tau_distance,
      double tau_dtheta,
      unsigned tau_support
      )
{
  static const unsigned borderwidth=20;

  tau_dtheta *= vnl_math::pi/180.0;
  tau_distance *= tau_distance;

  for (unsigned k=0; k < csk->curves_3d().size(); ++k)
    csk->attr_[k].inlier_views_.clear();

  for (unsigned v=0; v < dpath.nviews(); ++v) {
    // read edge info
    dbpro_process_sptr
      cam_src = new dbpro_load_camera_source<double>(
          dpath[v].cam_full_path(), dpath[v].cam_file_type());

    // 1 Edge map loader
    static const bool my_bSubPixel = true;
    static const double my_scale=1.0;
    dbpro_process_sptr
    edg_src = new dbpro_load_edg_source(dpath[v].edg_full_path(), my_bSubPixel, my_scale);

    // 1 dt and label loader
    dbpro_process_sptr
    edg_dt = new dbpro_edg_dt;
    edg_dt->connect_input(0, edg_src, 0);

    dvcpl_edgemap_cam_aggregator ag;
    ag.connect_input(0, cam_src, 0);
    ag.connect_input(1, edg_src, 0);
    ag.connect_input(2, edg_dt, 0);
    ag.connect_input(3, edg_dt, 1);
    ag.run(1);

    // reproject and count the number of inliers
    
    vcl_cout << "Reprojecting and looking for inliers\n";
    // for each 3d curve 
    for (unsigned k=0; k < csk->curves_3d().size(); ++k) {
      if (csk->attributes()[k].total_support_ < tau_support)
        continue;

      dbdif_1st_order_curve_2d reprojected_curve;
      reprojected_curve.resize(csk->curves_3d()[k].size());
      for (unsigned i=0; i < csk->curves_3d()[k].size(); ++i) {
        // - get image coordinates
        // bool not_degenerate = 
        ag.cam_.project_1st_order_to_image(csk->curves_3d()[k][i], &reprojected_curve[i]);
      }
      if (!mw_util::in_img_bounds(reprojected_curve, ag.dt_, borderwidth))
        continue;

      if (csk->attributes()[k].v_->stereo0() == v ||
          csk->attributes()[k].v_->stereo1() == v) {
        // obviously the source views are an inlier view for calibration
        // if they pass the above conditions.
        csk->attr_[k].inlier_views_.push_back(v);
        continue;
      }

      // mw_util::clip_to_img_bounds(ag.dt_, &reprojected_curve);

      // translate reproj. curve into edgel sequence
      dbcsi_edgel_seq reproj_edgels;
      mw_algo_util::dbdif_to_dbdet(reprojected_curve, &reproj_edgels);

      assert (reproj_edgels.size() == reprojected_curve.size());

      // Compute match cost
      unsigned d_vote = dbcsi_curve_distance::num_inliers_dt_oriented_all_samples(
          reproj_edgels, tau_distance, tau_dtheta, ag.dt_, ag.label_, *ag.edge_map_);

      // Now test if this is an inlier view and record this in attributes

      unsigned tau_min_inliers = static_cast<unsigned>(0.9*reprojected_curve.size());
      if (d_vote > tau_min_inliers) {
        vcl_cout << "Inlier view found.\n";
        csk->attr_[k].inlier_views_.push_back(v);
      }
    }
  }
}
