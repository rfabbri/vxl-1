// This is mw_curve_dt_tracing_tool.h
#ifndef mw_curve_dt_tracing_tool_h
#define mw_curve_dt_tracing_tool_h
//:
//\file
//\brief Tool for exploring multiview curve correspondences
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@gmail.com)
//\date 07/19/2005 12:09:14 AM EDT
//

#include <vil/vil_image_view.h>
#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_style_sptr.h>
#include <bgui/bgui_vsol2D_tableau.h>
#include <bgui/bgui_vsol2D_tableau_sptr.h>
#include <bgui/bgui_vsol_soview2D.h>
#include <bgui/bgui_selector_tableau_sptr.h>
#include <bvis1/bvis1_tool.h>

#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_fundamental_matrix.h>
#include <becld/becld_intersection_sets.h>
#include <bdifd/bdifd_camera.h>
#include <bdifd/bdifd_rig.h>
#include <mw/mw_curves.h>


//: SAME AS MW_CURVE_TRACING_TOOL, BUT READS IN DISTANCE TRANSFORM / LABEL
// IMAGES AS COMPUTED BY THE DISTANCE TRANSFORM PROCESS FROM BINARIZED EDGE MAPS
// AND USES THEM FOR MATCHING
//
class mw_curve_dt_tracing_tool : public bvis1_tool { 
public:
  //: Constructor
  mw_curve_dt_tracing_tool();
 
  //: Destructor
  virtual ~mw_curve_dt_tracing_tool() {}
  
  //: Returns the string name of this tool
  std::string name() const;

  //: Handle events.
  bool handle( const vgui_event & e, const bvis1_view_tableau_sptr& view );

  //: Set the tableau to work with
  bool set_tableau( const vgui_tableau_sptr& tableau );
  
  void activate (); 
  void deactivate (); 
  bool handle_mouse_click( const bvis1_view_tableau_sptr& view);

protected:

  vgui_event_condition gesture0;
  std::vector<bdifd_camera> cam_; //: cameras for each view
  std::vector<vil_image_view<vxl_uint_32> > dt_; //: distance transform images for each view
  std::vector<vil_image_view<unsigned> > label_; //: closest label transform images for each view

  //: fundamental matrices between each view 
  std::vector<std::vector<vpgl_fundamental_matrix<double> > > fm_; 
  std::vector<int> frame_v_;   //: frame number for each view 
  unsigned nviews_;

  vgui_soview2D_point *p0_; //:< initial point in left curve segment 
  vgui_soview2D_point *pn_; //:< end point in left curve segment 
  unsigned p0_idx_; //:< index of p0 in curve 
  unsigned pn_idx_;
  unsigned current_curve_id_;

  //:< selected curve in each view (for reconstruction, etc)
  std::vector<vsol_polyline_2d_sptr> selected_crv_; 

  //: selected curve segments in views #3 and up
  std::vector<bgui_vsol_soview2D_polyline *> selected_crv_soviews_n_; 

  vsol_polyline_2d_sptr subcurve_; //:< selected curve segment

  bgui_vsol_soview2D_polyline *curvelet_soview_; //:< selected curve segment
  bgui_vsol_soview2D_polyline *best_match_soview_; //:< best curve fragment after a stereo match.
  bool selected_new_curve_;

  struct mycolor { float r,g,b; } color_p0_, color_pn_, color_aux_;

  std::vector<bgui_vsol2D_tableau_sptr> tab_; //:< tableaux used to draw in each view 

  //:< tableau containing original vsols in each view
  std::vector<bgui_vsol2D_tableau_sptr> curve_tableau_; 
  bgui_vsol2D_tableau_sptr curve_tableau_current_;

  std::vector<std::vector< vsol_polyline_2d_sptr > > vsols_; //:< set of polylines at each view

  std::list<unsigned> crv_candidates_; //: index into vsols_[i2] of candidate (whole) curves
  std::vector<vsol_polyline_2d_sptr> crv_candidates_ptrs_; 

  std::list<bgui_vsol_soview2D_polyline *> crv_candidates_soviews_; 

  //: v[i] == index into isets_.L_ of crv_candidates_[i] 
  std::vector<unsigned> crv_candidates_idx_;  
  vgui_style_sptr cc_style_;  //:< style for curve segts


  //: Index to vector designates other views; index 0 is 3rd view, index 1 is
  //4rth view, and so forth.
  std::vector<bgui_vsol_soview2D_polyline *> reproj_soview_;
  
  std::vector<std::vector<bgui_vsol_soview2D_polyline *> > p_reproj_soviews_;

  std::list<vgui_soview2D_point *> intercept_pts_soviews_; 
  std::vector<vgui_soview2D_point *> all_intercept_pts_soviews_; 
  bool display_all_intersections_; 


  //: These store epip. lines for endponts only, from view 1 to each other view.
  // (vector index indicates view) 
  std::vector<vgui_soview2D_infinite_line *> ep0_soview_; 
  std::vector<vgui_soview2D_infinite_line *> epn_soview_; 
  std::vector<vgl_homg_line_2d<double> > ep0_; 
  std::vector<vgl_homg_line_2d<double> > epn_;

  //: Same but for epipolar lines from view [1] to i_view >= 2 (0-index) index to vector designates
  //other views; index 0 is 3rd view, index 1 is 4rth view, and so forth.
  std::vector<vgui_soview2D_infinite_line *> ep0_soview_2n_; 
  std::vector<vgui_soview2D_infinite_line *> epn_soview_2n_; 
  std::vector<vgl_homg_line_2d<double> > ep0_2n_;
  std::vector<vgl_homg_line_2d<double> >  epn_2n_;


  //: Epipolar lines of curve segt points in image 1 in all other views.  index to vector designates
  //other views; index 0 is 2nd view and index 1 is 3rd view
  std::vector<std::vector<vgl_homg_line_2d<double> > > ep_;
  std::vector<std::list<vgui_soview2D_infinite_line *> > ep_soviews_;

  std::vector<vgl_homg_line_2d<double> > ep_left_; 
  std::list<vgui_soview2D_infinite_line *> ep_soviews_left_;

  //: Index to vector designates other views; index 0 is 3rd view, index 1 is 4rth view, and so
  //forth.
  std::vector<std::vector<vgl_homg_line_2d<double> > > ep_2n_;
  std::vector<std::list<vgui_soview2D_infinite_line *> > ep_soviews_2n_;


  vgui_style_sptr ep_style_; 
  bool display_all_right_epips_; 
  bool display_all_left_epips_; 
  bool display_all_nth_view_epips_;

  bool compute_isets_; 
  becld_intersection_sets  isets_;

  bool click_selects_whole_curve_;

  // Best matches
  std::list<bgui_vsol_soview2D_polyline *> crv_best_matches_soviews_; 
  std::vector<vgui_style_sptr> best_match_style_;  //:< style for curve segts

private: 
  //: Some definitions are in mw_curve_dt_tracing_tool_rec.cxx
  void get_cameras ();
  void get_images ();
  void init_tableaux (std::vector < bvis1_view_tableau_sptr > &views);
  void get_curves (std::vector < bvis1_view_tableau_sptr > &views);
  void get_corresponding_point_v0_to_vn (unsigned v, unsigned di0,
            vsol_point_2d_sptr & pt_img1) const;
  bool handle_mouse_click (const vgui_event & e,
         const bvis1_view_tableau_sptr & view);
  void initialize_curve_selection (unsigned pt_idx);
  void update_pn_ (const vsol_point_2d_sptr & pt);
  void swap_p0_and_pn_ ();
  void draw_candidate_curves ();
  void show_all_intersection_points ();
  void update_display_for_intersection_stuff ();
  void update_display_for_epipolar_curve_pencil ();
  void reconstruct_possible_matches ();
  void reconstruct_multiview ();
  bool match_using_hog ();

  void linearly_reconstruct_pts( const std::vector<vsol_point_2d_sptr> &pt_img, const
      std::vector<unsigned> &views, vgl_point_3d<double> *pt_3D) const;

  void nonlinearly_optimize_reconstruction( const std::vector<vsol_point_2d_sptr> &pt_img, const
      std::vector<unsigned> &views, const vgl_point_3d<double> &pt_3D_initial, vgl_point_3d<double>
      *pt_3D) const;

  void get_reconstructions( const std::vector<unsigned> &views, unsigned ini_idx, unsigned di0,
      bmcsd_vector_3d *pt_3D, bmcsd_vector_3d *pt_3D_linear) const;

  bool get_index_of_candidate_curve( const vsol_polyline_2d_sptr & selected_crv, unsigned *jnz);

  void break_curves_into_episegs ();

  void break_curves_into_episegs (const std::vector <vsol_polyline_2d_sptr > &vsols,
           const vgl_homg_point_2d < double >&e, int frame,
           int view) const;

  void show_reprojections(unsigned jnz);

  void project( unsigned view, std::vector<vsol_point_2d_sptr> &proj, const std::vector<bmcsd_vector_3d>
      &crv3d, bdifd_rig &/*rig*/) const;

  void reconstruct_and_reproject( unsigned jnz, unsigned view, std::vector<vsol_point_2d_sptr>
      &reproj, 
      std::vector<bmcsd_vector_3d> &crv3d, std::vector<unsigned> &crv1_idx, std::vector<unsigned>
      &crv2_idx, bdifd_rig &rig) const;

  void reconstruct_one_candidate( unsigned jnz, std::vector<bmcsd_vector_3d> &crv3d, const
      std::vector<unsigned> &crv1_idx, const std::vector<unsigned> &crv2_idx, bdifd_rig &rig) const;

  void define_match_for_reconstruction( unsigned jnz, std::vector<unsigned> &crv1_idx,
      std::vector<unsigned> &crv2_idx, bdifd_rig &rig) const;

  void get_matching_subcurve(
      unsigned candidate_index,
      unsigned ini_idx,
      unsigned end_idx,
      unsigned *ini_idx_sub,
      unsigned *end_idx_sub
      ) const;

  bool match_using_dt();

  void reconstruct_subcurve(
      unsigned ini_idx_sub, 
      unsigned end_idx_sub, 
      std::vector<bmcsd_vector_3d> *curve_3d) const;

  void
  get_reconstructions_optimal_kanatani(
      const std::vector<unsigned> &views, 
      unsigned ini_idx, 
      unsigned di0, 
      bmcsd_vector_3d *pt_3D, 
      bmcsd_vector_3d *pt_3D_linear) const ;

  void reconstruct_pts_2view_kanatani(
      const std::vector<vsol_point_2d_sptr> &pt_img,
      const std::vector<unsigned> &views,
      vgl_point_3d<double> *pt_3D
      ) const;
};


#endif // mw_curve_dt_tracing_tool_h
