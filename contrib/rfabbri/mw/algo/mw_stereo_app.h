// This is mw_stereo_app.h
#ifndef mw_stereo_app_h
#define mw_stereo_app_h
//:
//\file
//\brief Toplevel stereo application class similar to commandline
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date 11/08/06 02:31:09 EST
//

#include <vul/vul_arg.h>
#include <dbul/dbul_arg.h>
#include <dbdif/dbdif_rig.h>
#include <vil/vil_image_resource.h>
#include <mw/mw_discrete_corresp.h>
#include <mw/mw_discrete_corresp_3.h>
#include <mw/mw_discrete_corresp_n.h>
#include <vsol/vsol_line_2d_sptr.h>

struct mw_stereo_app_args {
    vul_arg<vcl_string> *prefix; 
    vul_arg<vcl_string> *out_path; 
    vul_arg<vcl_string> *fname1;
    vul_arg<vcl_string> *fname2; 
    vul_arg<vcl_string> *fname3;
    vul_arg<vcl_vector<vcl_string> > *fnames;
    vul_arg<vcl_string> *corresp_gt;
    vul_arg<vcl_string> *precomputed_epip;
    vul_arg<vcl_string> *edgels1;
    vul_arg<vcl_string> *edgels2;
    vul_arg<vcl_string> *edgels3;
    vul_arg<vcl_string> *edgel_type;
    vul_arg<vcl_vector<vcl_string> > *edgels;
    vul_arg<bool> *run_trinocular;
    vul_arg<bool> *run_trinocular_t_diff;
    vul_arg<bool> *run_trinocular_k_diff;
    vul_arg<bool> *write_epipolar_corresp;
    vul_arg<bool> *read_epipolar_corresp;
    vul_arg<vcl_string> *corresp_in;
    vul_arg<bool> *compare_corresp;
    vul_arg<bool> *compute_epipolars;
    vul_arg<bool> *symmetric_3;
    vul_arg<bool> *symmetric_n;
    vul_arg<bool> *run_sel_geometry;
    vul_arg<bool> *read_sel;
    vul_arg<vcl_string> *sel_in1;
    vul_arg<vcl_string> *sel_in2;
    vul_arg<vcl_string> *sel_in3;
    vul_arg<bool> *synth_data_1;
    vul_arg<bool> *synth_data_2;
    vul_arg<bool> *synth_data_med;
    vul_arg<bool> *synth_data_3;
    vul_arg<bool> *run_synth_geometry;
    vul_arg<bool> *synth_write_edgels;
    vul_arg<double> *angle1;
    vul_arg<double> *angle2;
    vul_arg<double> *angle3;
    vul_arg<vcl_vector<double> > *angles;
    vul_arg<bool> *perturb_camera;
    vul_arg<bool> *write_perturb_camera;
    vul_arg<unsigned> *trinocular_nrad;
    vul_arg<double> *err_pos;
    vul_arg<bool> *remove_epitangency;
    vul_arg<vcl_string> *cam_type;


  //------- Helper functions

  //: OBS: all the arguments have to remain valid; we point to them instead of
  // copying
  void set(
    vul_arg<vcl_string> &a_prefix, 
    vul_arg<vcl_string> &a_out_path, 
    vul_arg<vcl_string> &a_fname1,
    vul_arg<vcl_string> &a_fname2, 
    vul_arg<vcl_string> &a_fname3,
    vul_arg<vcl_vector<vcl_string> > &a_fnames,
    vul_arg<vcl_string> &a_corresp_gt,
    vul_arg<vcl_string> &a_precomputed_epip,
    vul_arg<vcl_string> &a_edgels1,
    vul_arg<vcl_string> &a_edgels2,
    vul_arg<vcl_string> &a_edgels3,
    vul_arg<vcl_string> &a_edgel_type,
    vul_arg<vcl_vector<vcl_string> > &a_edgels,
    vul_arg<bool> &a_run_trinocular,
    vul_arg<bool> &a_run_trinocular_t_diff,
    vul_arg<bool> &a_run_trinocular_k_diff,
    vul_arg<bool> &a_write_epipolar_corresp,
    vul_arg<bool> &a_read_epipolar_corresp,
    vul_arg<vcl_string> &a_corresp_in,
    vul_arg<bool> &a_compare_corresp,
    vul_arg<bool> &a_compute_epipolars,
    vul_arg<bool> &a_symmetric_3,
    vul_arg<bool> &a_symmetric_n,
    vul_arg<bool> &a_run_sel_geometry,
    vul_arg<bool> &a_read_sel,
    vul_arg<vcl_string> &a_sel_in1,
    vul_arg<vcl_string> &a_sel_in2,
    vul_arg<vcl_string> &a_sel_in3,
    vul_arg<bool> &a_synth_data_1,
    vul_arg<bool> &a_synth_data_2,
    vul_arg<bool> &a_synth_data_med,
    vul_arg<bool> &a_synth_data_3,
    vul_arg<bool> &a_run_synth_geometry,
    vul_arg<bool> &a_synth_write_edgels,
    vul_arg<double> &a_angle1,
    vul_arg<double> &a_angle2,
    vul_arg<double> &a_angle3,
    vul_arg<vcl_vector<double> > &a_angles,
    vul_arg<bool> &a_perturb_camera,
    vul_arg<bool> &a_write_perturb_camera,
    vul_arg<unsigned> &a_trinocular_nrad,
    vul_arg<double> &a_err_pos,
    vul_arg<bool> &a_remove_epitangency,
    vul_arg<vcl_string> &a_cam_type
    );
};

//:
// For non-synthetic data, cameras are implicitly given by the image names.
//
class mw_stereo_app {
public:
  mw_stereo_app() 
    : 
    have_stereo_result_corresp_(false),
    have_precomputed_epi_corresp_(false),
    nviews_(3),
    fm_(NULL)
    {} 
  ~mw_stereo_app() { if (fm_) delete fm_; }

  bool 
  init(mw_stereo_app_args & arg);

  bool 
  write_results(vcl_string &path_corresp_out, vcl_string &path_precomputed_epip);

  void run_stereo();

  void build_point_tangents_from_edgels(
      vcl_vector<vcl_vector<dbdif_3rd_order_point_2d> > &crv2d) const;

  bool read_edgels(mw_stereo_app_args &arg);
  bool write_gt_edgels(mw_stereo_app_args &arg);
  bool read_images(mw_stereo_app_args &arg);
  bool read_cams(mw_stereo_app_args &arg);
  void init_sel(mw_stereo_app_args &arg);
  void compute_sel();

public:

  vcl_vector<vil_image_resource_sptr> images_;
  vcl_vector<vcl_string> fnames_;
  vcl_vector<double> angles_;
  vcl_vector<vcl_string> edgel_fnames_;
  vcl_string edgel_type_;
  vcl_vector<dbdif_camera> cam_;
  vcl_vector<dbdif_camera> cam_gt_; //:< ground-truth
  vcl_vector<vcl_vector< vsol_point_2d_sptr > > vsols_;
  vcl_vector<vcl_vector<dbdif_3rd_order_point_2d> > crv2d_gt_;
  vcl_vector<vcl_vector<dbdif_3rd_order_point_3d> > crv3d_gt_;
  mw_discrete_corresp gt_; //:< ground-truth corresp.
  mw_discrete_corresp_3 gt_3_; //:< ground-truth corresp.
  mw_discrete_corresp_n gt_n_; //:< ground-truth corresp.
  mw_discrete_corresp corr_out_; //:< corresp to store stereo results
  mw_discrete_corresp corr_ep_; //:< corresp to store stereo results
  mw_discrete_corresp_3 corr_ep_3_; //:< corresp to store stereo results
  mw_discrete_corresp_3 corr_out_3_; //:< corresp to store stereo results
  mw_discrete_corresp_n corr_ep_n_; //:< corresp to store stereo results
  mw_discrete_corresp_n corr_out_n_; //:< corresp to store stereo results


  vcl_vector<vcl_vector< vsol_line_2d_sptr > > vsols_orig_cache_;

  bool have_stereo_result_corresp_;
  bool have_precomputed_epi_corresp_;
  bool run_trinocular_;
  bool run_trinocular_t_diff_;
  bool run_trinocular_k_diff_;
  bool run_trinocular_DG_;
  bool run_n_view_DG_;
  bool write_epipolar_corresp_;
  bool compare_correspondences_;
  bool compute_epipolars_;
  bool symmetric_3_;
  bool symmetric_n_;
  bool compute_sel_;
  bool run_sel_geometry_;
  bool run_synth_geometry_;
  bool synth_data_1_;
  bool synth_data_2_;
  bool synth_data_med_;
  bool synth_data_3_;
  bool crop_;  //: select random edgels within crop region of real data?
  unsigned nviews_;
  vpgl_fundamental_matrix<double> *fm_;
  unsigned  nrows_;
  unsigned  ncols_;
  unsigned  trinocular_nrad_;
  double err_pos_;
};


#endif // mw_stereo_app_h

