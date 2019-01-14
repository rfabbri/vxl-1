#include <testlib/testlib_test.h>
#include <bmcsd/bmcsd_util.h>
#include <vcpld/algo/vcpld_distmap_bundle_adjust.h>
#include <vcpld/algo/vcpld_distmap_optimize_camera.h>
#include <sdetd/io/sdetd_load_edg.h>
#include <sdetd/algo/sdetd_convert_edgemap.h>
#include <bprod/bprod_process.h>
#include <bild/algo/bild_exact_distance_transform.h>


class dvcpl_bundle_ajudt_tester {
public:
  dvcpl_bundle_ajudt_tester (
    const std::vector<std::string> &edgemaps_fnames, 
    const std::vector<std::string> &cams_fnames):
    nviews_(edgemaps_fnames.size()),
    em_(edgemaps_fnames.size()),
    cam_(cams_fnames.size()),
    dt_(cams_fnames.size()),
    label_(cams_fnames.size())
  {
    assert(cam_.size() == nviews_);
    assert(em_.size() == nviews_);

    load_edgemaps(edgemaps_fnames);
    load_cams(cams_fnames);
  }

  void load_edgemaps(const std::vector<std::string> &edgemaps_fnames)
  {
    for (unsigned v=0; v < edgemaps_fnames.size(); ++v) {
      std::cout << "Reading " << edgemaps_fnames[v] << std::endl;
      bool bSubPixel = true;
      double scale=1.0;

      bool retval = sdetd_load_edg(
          edgemaps_fnames[v],
          bSubPixel,
          scale,
          em_[v]);

      if (!retval) {
        std::cerr << "Could not open edge file " << edgemaps_fnames[v] 
          << std::endl;
        abort();
      }
      std::cout << "N edgels: " << em_[v]->num_edgels() << std::endl;
    }
  }

  void load_cams(const std::vector<std::string> &cams_fnames)
  {
    for (unsigned v=0; v < cams_fnames.size(); ++v) {
      bool retval = bmcsd_util::read_cam_anytype(cams_fnames[v], 
          bmcsd_util::BMCS_INTRINSIC_EXTRINSIC, &cam_[v]);
      if (!retval)
        abort();
    }
  }

  //: Write cams.
  void write_cams(std::string dir, std::string prefix)
  {
    bool retval =  
      bmcsd_util::write_cams(dir, prefix, bmcsd_util::BMCS_INTRINSIC_EXTRINSIC, cam_);
    if (!retval)
      abort();
  }

  void write_points()
  {
    mywritev("dat/vcpld_test_pts3d_0-optimized.dat", pts3d_[0]);
    mywritev("dat/vcpld_test_pts3d_1-optimized.dat", pts3d_[1]);
  }

  void compute_dts()
  {
    // convert edgemap to binary image
    for (unsigned v=0; v < nviews_; ++v) {
      vil_image_view<vxl_byte > bw_image;
      //: Assumes the conversion maps edges to 255 and others to 0.
      bool retval = sdetd_convert_edgemap_to_image(*em_[v], bw_image);
      
      if (!retval) abort();

      vil_image_view<vxl_uint_32> dt(bw_image.ni(), bw_image.nj(), 1);

      for(unsigned i=0; i<dt.ni(); i++)
        for(unsigned j=0; j<dt.nj(); j++)
          dt(i,j) = static_cast<vxl_uint_32>(bw_image(i,j)<127);

      vil_image_view<unsigned> imlabel(dt.ni(), dt.nj(), 1);

      retval = bild_exact_distance_transform_maurer_label(dt, imlabel);
      if (!retval) abort();

      dt_[v] = dt;
      label_[v] = imlabel;
    }
  }

  void do_optimization();
  void do_one_cam_optimization();

  void read_3d_curves(std::vector<std::string> pts3d_fnames);

  // Data ----------------------------------------------------------------------

  unsigned nviews_;
  typedef std::vector<vgl_point_3d<double> > single_3d_curve;
  std::vector< single_3d_curve > pts3d_;

  std::vector<sdet_edgemap_sptr> em_;
  std::vector<vpgl_perspective_camera<double> > cam_;
  std::vector<vil_image_view<vxl_uint_32> > dt_;
  std::vector<vil_image_view<unsigned> > label_;
};


MAIN( test_dvcpl_distmap_bundle_adjust )
{
  START ("vcpld bundle adjustment of curves using distance transforms");
  // Specify the data
  std::vector<std::string> edgemaps_fnames, cams_fnames;

  std::string path="/usr/local/moredata/subset-local/";
  edgemaps_fnames.push_back(path + std::string("frame_00001-grad_thresh5.edg.gz"));
  edgemaps_fnames.push_back(path + std::string("frame_00030-grad_thresh5.edg.gz"));
  edgemaps_fnames.push_back(path + std::string("frame_00066-grad_thresh5.edg.gz"));
  edgemaps_fnames.push_back(path + std::string("frame_00100-grad_thresh5.edg.gz"));
  edgemaps_fnames.push_back(path + std::string("frame_00137-grad_thresh5.edg.gz"));
  edgemaps_fnames.push_back(path + std::string("frame_00177-grad_thresh5.edg.gz"));

  cams_fnames.push_back(path + std::string("frame_00001.extrinsic"));
  cams_fnames.push_back(path + std::string("frame_00030.extrinsic"));
  cams_fnames.push_back(path + std::string("frame_00066.extrinsic"));
  cams_fnames.push_back(path + std::string("frame_00100.extrinsic"));
  cams_fnames.push_back(path + std::string("frame_00137.extrinsic"));
  cams_fnames.push_back(path + std::string("frame_00177.extrinsic"));

  dvcpl_bundle_ajudt_tester t(
    edgemaps_fnames, 
    cams_fnames);

  t.compute_dts();
  
  std::vector<std::string> pts3d_fnames;
//  pts3d_fnames.push_back("dat/vcpld_test_pts3d_0.dat");
  pts3d_fnames.push_back("dat/vcpld_test_pts3d_1.dat");
  pts3d_fnames.push_back("dat/vcpld_test_pts3d_2.dat");
  // Read in two 3D curves.
  t.read_3d_curves(pts3d_fnames);

  // At this point we have two 3D curves and 3 views with edgemaps.
  // For now, the 3 curves show up in the 3 views.

  // Now call the optimization.  
  t.do_optimization();

  t.write_cams("dat", "result-cams");
  t.write_points();

  // Another test - optimize 1 camera.
  t.do_one_cam_optimization();

  SUMMARY();
}

void dvcpl_bundle_ajudt_tester::
read_3d_curves(std::vector<std::string> pts3d_fnames)
{
  pts3d_.resize(2);
//  pts3d_.resize(1);

  if (!myreadv(pts3d_fnames[0], pts3d_[0]))
    abort();
  if (!myreadv(pts3d_fnames[1], pts3d_[1]))
    abort();

  std::cout << "Curve 0 size: " << pts3d_[0].size() << std::endl;
  std::cout << "Curve 1 size: " << pts3d_[1].size() << std::endl;
}

void dvcpl_bundle_ajudt_tester::
do_optimization()
{
  vcpld_distmap_bundle_adjust o;

  unsigned total_npts = pts3d_[0].size() + pts3d_[1].size();
//  unsigned total_npts = pts3d_[0].size();

  // all curves pts in all views
  std::vector<std::vector<bool> > mask(nviews_,
                                     std::vector<bool>(total_npts,true));

  for (unsigned i=0; i < pts3d_[1].size(); ++i)
    mask[3][pts3d_[0].size()+i] = false;

  o.optimize(
      cam_,
      pts3d_,
      dt_,
      label_,
      em_,
      mask);

  // o.print_report();

  TEST("Error improved", o.start_error() > o.end_error(), true);
}

void dvcpl_bundle_ajudt_tester::
do_one_cam_optimization()
{
  std::cout << "------- Optimizing first camera only -------\n";

  vpgl_perspective_camera<double> opt = 
  vcpld_distmap_optimize_camera::opt_orient_pos(
      cam_[5],
      pts3d_,
      dt_[5],
      label_[5],
      em_[5]);


  // write out.
  std::vector<vpgl_perspective_camera<double> > cams;
  cams.push_back(opt);
  bool retval =  
    bmcsd_util::write_cams("dat", "result-1cam", bmcsd_util::BMCS_INTRINSIC_EXTRINSIC, cams);
  if (!retval)
    abort();
}
