// This is dbkpr_projective_reconstruct.h
#ifndef dbkpr_projective_reconstruct_process_h
#define dbkpr_projective_reconstruct_process_h
//:
//\file
//\brief Tool to generate a projective reconstruction from n frames
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date 11/20/06 04:10:52 EST
//

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vnl/vnl_fwd.h>
#include <vgl/vgl_fwd.h>
#include <dbdet/dbdet_keypoint_corr3d.h>
#include <vpgl/vpgl_proj_camera.h>
#include <dbkpr/dbkpr_view_span_tree_sptr.h>
#include <dbdet/dbdet_keypoint_corr3d_sptr.h>

//: 
// 
class dbkpr_projective_reconstruct_process : public bpro1_process {

public:
//  typedef vcl_vector<int> t_corr_set; // XXX

  dbkpr_projective_reconstruct_process();
  virtual ~dbkpr_projective_reconstruct_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  int input_frames();
  int output_frames();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  bool execute();
  bool finish();
private:

  dbkpr_view_span_tree_sptr tree_ptr_;
  unsigned nviews_;

  
  // Lifted from brct_algos; I want homogeneous 3D point output
  static vgl_homg_point_3d<double> 
  triangulate_3d_point(
      const vgl_homg_point_2d<double>& x1, 
      const vnl_double_3x4& P1,
      const vgl_homg_point_2d<double>& x2, 
      const vnl_double_3x4& P2);

  /*
  void select_best_views(const dbkpr_view_span_tree &tree, unsigned &v1, unsigned &v2, unsigned &nmatches) const;

  void 
  get_initial_reconstruction(
      unsigned v1, unsigned v2, 
      vpgl_proj_camera<double> &Pr1, vpgl_proj_camera<double> &Pr2,
      vcl_vector<dbdet_keypoint_corr3d_sptr> &corr3d);

  unsigned pick_new_view(const vcl_vector<unsigned> &view_set);

  void add_new_view_to_reconstruction(unsigned v3, 
      vcl_vector<unsigned> &view_set, 
      vcl_vector<vpgl_proj_camera<double> > &cam_set,
      t_corr_set corr_set);

  void dbkpr_projective_reconstruct_process::
  projective_bundle_adjust(vcl_vector<vpgl_proj_camera<double> > &cameras, t_corr_set &corr_set);
  */

};


#endif // dbkpr_projective_reconstruct_process_h
