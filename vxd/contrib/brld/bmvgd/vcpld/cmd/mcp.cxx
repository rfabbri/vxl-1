#include <iterator>
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <buld/buld_arg.h>
#include <bmcsd/bmcsd_util.h>
#include <mw/algo/vcpld_distmap_bundle_adjust.h>
#include <mw/pro/vcpld_bundle_adjust_driver.h>


#define MW_ASSERT(msg, a, b) if ((a) != (b)) { std::cerr << (msg) << std::endl; abort(); }

void 
keep_only_curves_with_sufficient_inlier_views(
    const std::set<unsigned> &viewset,
    bmcsd_curve_3d_sketch *csk);

int
main(int argc, char **argv)
{
  std::string prefix_default(".");

  vul_arg<std::string> a_prefix("-prefix", 
      "path to main directory of files",prefix_default.c_str());
  vul_arg<std::string> a_cam_type("-cam_type",
      "camera type: intrinsic_extrinsic or projcamera","intrinsic_extrinsic");
  vul_arg<std::string> a_csk("-curvesketch", "input 3D curve sketch file or directory", "out/");
  vul_arg<std::string> a_out_dir("-outdir", "output directory relative to -prefix", "out-mcp/");
  vul_arg<unsigned> a_total_support("-totalsupport", 
      "threshold on the total support of the curves", 0);
  vul_arg<unsigned> a_maxviews("-maxviews", 
      "maximum number of views to optimize", 30);
  vul_arg<bool> a_optimize_1cam("-onecam", 
      "Optimize one camera at a time without 3D point optimization", false);

  vul_arg<bool> a_curve_ransac("-curve_ransac", "if -onecam is on, this selects small subsets of curves to run optimization many times and return the best", false);

  vul_arg<std::vector<unsigned> > a_views("-views","comma separated list of views to optimize. By not specifying this,  consecutive views up to maxviews will be used."); // TODO make it unsigned.
 
  vul_arg_parse(argc,argv);
  std::cout << "\n";

  std::cout << "view list arg (size " << a_views.value_.size() << "):\n";
  print_value(std::cout, a_views);
  std::cout << std::endl;

  std::set<unsigned> viewset;

  if (a_views.value_.size()) {
    viewset.insert(a_views.value_.begin(), a_views.value_.end());
  } else {
    for (unsigned i=0; i < a_maxviews(); ++i)
      viewset.insert(i);
  }

  std::cout << "view list (size " << viewset.size() << "):\n";
  std::copy(viewset.begin(), viewset.end(), std::ostream_iterator<unsigned>(std::cout, " "));
  std::cout << std::endl;
  

  bmcsd_util::camera_file_type cam_type;

  if (a_cam_type() == "intrinsic_extrinsic") {
    cam_type = bmcsd_util::MW_INTRINSIC_EXTRINSIC;
  } else {
    if (a_cam_type() == "projcamera")
      cam_type = bmcsd_util::MW_3X4;
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

  // Run Bundle Adjustment

  bmcsd_curve_3d_sketch *csk = new bmcsd_curve_3d_sketch;
  std::string csk_fname = a_prefix() + "/" + a_csk();
  retval  = csk->read_dir_format(csk_fname);
  MW_ASSERT(std::string("Error reading 3D curve sketch: ") + csk_fname, retval, true);

  std::cout << "Pruning " << csk->num_curves() << 
    " curves with cost < " << a_total_support() << std::endl;
  csk->prune_by_total_support(a_total_support());
  std::cout << "Pruned down to " << csk->num_curves() << " curves" << std::endl;

  // Trim all curves in CSK that have no inlier views within the given range
  keep_only_curves_with_sufficient_inlier_views(viewset, csk);

  // Now define vcpld_bundle_adjust_driver.
  vcpld_bundle_adjust_driver b(dpath, csk, viewset, a_optimize_1cam(), a_curve_ransac());

  retval = b.init();
  MW_ASSERT("Bundle adjustment driver init() return value", retval, true);

  // Run bundle adjustment using curve sketch
  retval = b.run();
  MW_ASSERT("Bundle adjustment run() return value", retval, true);

  //: Write out:
  // - optimized cameras
  // - optimized curve sketch

  // TODO: get pref as the prefix the image file names from dpath.


  // fill up cam_fnames:

  assert(b.nviews() == b.optimized_cameras().size());
  assert(b.nviews() == b.views().size());

  std::vector<std::string> cam_fnames_noext(b.nviews());

  for (unsigned i=0; i < b.nviews(); ++i) {
    unsigned v = b.views()[i];
    cam_fnames_noext[i] = vul_file::strip_extension(dpath[v].cam_fname_);
  }

  if (vul_file::make_directory(a_out_dir()))
    std::cout << "Making output directory " << a_out_dir() << std::endl;

  retval = bmcsd_util::write_cams(a_out_dir(), cam_fnames_noext, 
      bmcsd_util::MW_INTRINSIC_EXTRINSIC, b.optimized_cameras());

  MW_ASSERT("Error in writing optimized cameras", retval, true);

// future todo: write out the optmized points. No tangential info -- so use mywritev
// istead of the sequence below.
//
//  bmcsd_curve_3d_sketch csk_new;
//  b.get_optimized_csk(&csk_new);
//  csk_new.write_dir_format(a_out_dir() + "/optimized_curve_sketch");
//  MW_ASSERT("Error in writing optimized curve sketch", retval, true);

  return 0;
}

void 
keep_only_curves_with_sufficient_inlier_views(
    const std::set<unsigned> &viewset,
    bmcsd_curve_3d_sketch *csk)
{
  std::vector< bdifd_1st_order_curve_3d > crv3d_new;
  std::vector< bmcsd_curve_3d_attributes > attr_new;
  crv3d_new.reserve(csk->num_curves());
  attr_new.reserve(csk->num_curves());

  for (unsigned c=0; c < csk->num_curves(); ++c) {
    std::vector<unsigned> inlier_views;

    //    souce views are not automatically an inlier view anymore, since they
    //    must be away from viewport boundaries.
    //
//    unsigned view0 = csk->attributes()[c].v_->stereo0();
//    if (view0 < maxviews)
//      inlier_views.push_back(view0);

//    unsigned view1 = csk->attributes()[c].v_->stereo1();
//    if (view1 < maxviews)
//      inlier_views.push_back(view1);

    for (unsigned k=0; k < csk->attributes()[c].inlier_views_.size(); ++k) {
      unsigned view = csk->attributes()[c].inlier_views_[k];
      if (viewset.find(view) != viewset.end())
        inlier_views.push_back(view);
    }

    if (inlier_views.size() >= 2 ) {
//        && (c == 38 || c == 37 || c == 30)/*XXX hack*/) {
//        && (c == 8 || c == 10 || c == 15)/*XXX hack*/) {
//        && (c == 6 || c == 30 || c == 37)/*XXX hack*/) {
//      std::cout << "XXX Including curve with length: " << csk->curves_3d()[c].size() << std::endl;
      crv3d_new.push_back(csk->curves_3d()[c]);
      attr_new.push_back(csk->attributes()[c]);
    }
  }
  csk->set(crv3d_new, attr_new);
}


