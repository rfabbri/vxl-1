#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vsl/vsl_vector_io.h>
#include <buld/buld_arg.h>
#include <bdifd/bdifd_camera.h>
#include <bcsid/bcsid_curve_distance.h>
#include <bmcsd/bmcsd_util.h>
#include <bmcsd/algo/bmcsd_algo_util.h>
#include <bmcsd/bmcsd_curve_3d_sketch.h>
#include <bmcsd/bmcsd_curve_3d_attributes.h>
#include <bmcsd/algo/bmcsd_data.h>
#include <bmcsd/pro/bmcsd_load_camera_source.h>
#include <bmcsd/pro/bmcsd_load_edg_source.h>

#define MW_ASSERT(msg, a, b) if ((a) != (b)) { std::cerr << (msg) << std::endl; abort(); }

//: Stores the concatenation of all inputs from many bmcsd_stereo_jobs
class vcpld_edgemap_cam_aggregator: public bprod_sink {
public:

  vcpld_edgemap_cam_aggregator()
  {}

  bprod_signal execute()
  {
    assert(input_type_id(0) == typeid(vpgl_perspective_camera<double>));
    assert(input_type_id(1) == typeid(sdet_edgemap_sptr));
    assert(input_type_id(2) == typeid(vil_image_view<vxl_uint_32>));
    assert(input_type_id(3) == typeid(vil_image_view<unsigned>));

    cam_.set_p(input<vpgl_perspective_camera<double> >(0));
    edge_map_ = input<sdet_edgemap_sptr>(1);
    dt_ = input<vil_image_view<vxl_uint_32> >(2);
    label_ = input<vil_image_view<unsigned> > (3);

    return BPROD_VALID;
  }

  bdifd_camera cam_;
  sdet_edgemap_sptr edge_map_;
  vil_image_view<vxl_uint_32> dt_;
  vil_image_view<unsigned> label_;
};

void vcpld_reproject_to_find_inliers (
      const bmcsd_curve_stereo_data_path &dpath, 
      bmcsd_curve_3d_sketch *csk,
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
  std::string prefix_default(".");

  vul_arg<std::string> a_prefix("-prefix", 
      "path to main directory of files",prefix_default.c_str());
  vul_arg<std::string> a_cam_type("-cam_type",
      "camera type: intrinsic_extrinsic or projcamera","intrinsic_extrinsic");
  vul_arg<std::string> a_csk("-curvesketch", "input 3D curve sketch file or directory", "out/");
  vul_arg<unsigned> a_total_support("-totalsupport", 
      "threshold on the total support of the curves", 0);

  vul_arg<double> a_distance_threshold("-dist", 
      "(in pixels) threshold for an edgel to be an inlier to the reprojected curve in each view", 10.0);
  vul_arg<double> a_dtheta_threshold("-dtheta", 
      "(in degrees) threshold in orientation difference for an edgel to be an inlier to reprojected curve in each view", 10.0);

  vul_arg_parse(argc,argv);
  std::cout << "\n";

  bmcsd_util::camera_file_type cam_type;

  if (a_cam_type() == "intrinsic_extrinsic") {
    cam_type = bmcsd_util::BMCS_INTRINSIC_EXTRINSIC;
  } else {
    if (a_cam_type() == "projcamera")
      cam_type = bmcsd_util::BMCS_3X4;
    else  {
      std::cerr << "Error: invalid camera type " << a_cam_type() << std::endl;
      return 1;
    }
  }

  bmcsd_curve_stereo_data_path dpath;
  bool retval = 
    bmcsd_data::read_frame_data_list_txt(a_prefix(), &dpath, cam_type);
  if (!retval) return 1;
  std::cout << "Dpath:\n" << dpath << std::endl;


  bmcsd_curve_3d_sketch *csk = new bmcsd_curve_3d_sketch;
  std::string csk_fname = a_prefix() + "/" + a_csk();
  retval  = csk->read_dir_format(csk_fname);
  MW_ASSERT(std::string("Error reading 3D curve sketch: ") + csk_fname, retval, true);

  // Now define vcpld_bundle_adjust_driver.
  vcpld_reproject_to_find_inliers(dpath, csk, a_distance_threshold(), a_dtheta_threshold(),
      a_total_support());

  //: Write out:
  // - new attributes

  std::cout << "Writing new attributes\n";
  vsl_b_ofstream f(csk_fname + "/attributes.vsl");
  vsl_b_write(f, csk->attributes());
  std::cout << "num curves in csk: " << csk->num_curves() << std::endl;
  std::cout << "num curves in attr: " << csk->attributes().size() << std::endl;

  return 0;
}

void vcpld_reproject_to_find_inliers (
      const bmcsd_curve_stereo_data_path &dpath, 
      bmcsd_curve_3d_sketch *csk,
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
    bprod_process_sptr
      cam_src = new bmcsd_load_camera_source<double>(
          dpath[v].cam_full_path(), dpath[v].cam_file_type());

    // 1 Edge map loader
    static const bool my_bSubPixel = true;
    static const double my_scale=1.0;
    bprod_process_sptr
    edg_src = new bmcsd_load_edg_source(dpath[v].edg_full_path(), my_bSubPixel, my_scale);

    // 1 dt and label loader
    bprod_process_sptr
    edg_dt = new bprod_edg_dt;
    edg_dt->connect_input(0, edg_src, 0);

    vcpld_edgemap_cam_aggregator ag;
    ag.connect_input(0, cam_src, 0);
    ag.connect_input(1, edg_src, 0);
    ag.connect_input(2, edg_dt, 0);
    ag.connect_input(3, edg_dt, 1);
    ag.run(1);

    // reproject and count the number of inliers
    
    std::cout << "Reprojecting and looking for inliers\n";
    // for each 3d curve 
    for (unsigned k=0; k < csk->curves_3d().size(); ++k) {
      if (csk->attributes()[k].total_support_ < tau_support)
        continue;

      bdifd_1st_order_curve_2d reprojected_curve;
      reprojected_curve.resize(csk->curves_3d()[k].size());
      for (unsigned i=0; i < csk->curves_3d()[k].size(); ++i) {
        // - get image coordinates
        // bool not_degenerate = 
        ag.cam_.project_1st_order_to_image(csk->curves_3d()[k][i], &reprojected_curve[i]);
      }
      if (!bmcsd_util::in_img_bounds(reprojected_curve, ag.dt_, borderwidth))
        continue;

      if (csk->attributes()[k].v_->stereo0() == v ||
          csk->attributes()[k].v_->stereo1() == v) {
        // obviously the source views are an inlier view for calibration
        // if they pass the above conditions.
        csk->attr_[k].inlier_views_.push_back(v);
        continue;
      }

      // bmcsd_util::clip_to_img_bounds(ag.dt_, &reprojected_curve);

      // translate reproj. curve into edgel sequence
      bcsid_edgel_seq reproj_edgels;
      bmcsd_algo_util::bdifd_to_sdet(reprojected_curve, &reproj_edgels);

      assert (reproj_edgels.size() == reprojected_curve.size());

      // Compute match cost
      unsigned d_vote = bcsid_curve_distance::num_inliers_dt_oriented_all_samples(
          reproj_edgels, tau_distance, tau_dtheta, ag.dt_, ag.label_, *ag.edge_map_);

      // Now test if this is an inlier view and record this in attributes

      unsigned tau_min_inliers = static_cast<unsigned>(0.9*reprojected_curve.size());
      if (d_vote > tau_min_inliers) {
        std::cout << "Inlier view found.\n";
        csk->attr_[k].inlier_views_.push_back(v);
      }
    }
  }
}
