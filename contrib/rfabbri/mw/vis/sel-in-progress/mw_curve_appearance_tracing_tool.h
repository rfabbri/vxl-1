// This is mw_curve_appearance_tracing_tool.h
#ifndef mw_curve_appearance_tracing_tool_h
#define mw_curve_appearance_tracing_tool_h
//:
//\file
//\brief Tool for exploring multiview curve correspondences
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date 07/19/2005 12:09:14 AM EDT
//

#include <bvis1/bvis1_tool.h>
#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_style_sptr.h>
#include <bgui/bgui_vsol2D_tableau.h>
#include <bgui/bgui_vsol2D_tableau_sptr.h>
#include <bgui/bgui_vsol_soview2D.h>
#include <bgui/bgui_selector_tableau_sptr.h>

#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_fundamental_matrix.h>
#include <mw/mw_intersection_sets.h>
#include <mw/mw_camera.h>
#include <mw/mw_rig.h>
#include <mw/mw_curves.h>




//: SAME AS MW_CURVE_TRACING_TOOL, BUT READS IN IMAGES AND USES APPEARANCE INFO FOR MATCHING
//
//This tool displays epipolar lines corresponding to user mouse events.
// It acts between three frames; all that is required is a perspective 
// camera storage for each frame. In the future we shall support fundamental
// matrix storage class as well, this should be easy to do. 
//
// The tool currently creates vsol2D tableaus upon activation. These tableaus
// are used for drawing the epipolars. The user can subsequently
// activate/deactivate and change visibility of the epipolars. If you have
// a better design in mind feel free to share it with us.
//
// USAGE
//  1- Define n frames.
//  2- Put an image into each frame; detect edges and transform the edges into
//     Vsol polylines by using pruning process.
//  3- Put a vpgl_perspective_camera into a dvpgl_camera_storage in each frame.
//  4- Define n views, one for each frame.
//  5- Set the edge vsols as active in each frame.
//  6- call the tool and click on the first view's vsol 
//  7- the candidates in the second view will be displayed.
//  8- The remainder views (3 and up) are just used for (i) confirming the
//  two-view selection by reprojection and (ii) to allow the user to select
//  the curves to be reconstructed.
//
// KEYBOARD COMMANDS (KEYSTROKES)
//  - look into switch statement in .cxx file
//
//
// \todo TODO
//- make it work with more than two views
//- when press <ESC>, regenerate the tableaux
//- make it work when changing frames during tool activity
//- rename p1_ to pn_
//- highlight epipolar region between ep1 and ep2
//   - transparent polygon (write an soview and implement draw function using opengl)
//- Add a subenu to the context menu (as matt did in his corresp tool),
//
// \remarks 
//
// - The oldtracer/mw_curve_tracing_tool_3* code have more stereo matching
// functions (such as measuring reprojection error for a given curve match)
// which has not yet been extended for n-views here.
//
// Marginal notes
//  - idea for intersection of line and polyline:
//     - divide polyline segt in two
//     - repeat for the curve segment whose bbox intersect the line
//     
//
class mw_curve_appearance_tracing_tool : public bvis1_tool {
public:
  //: Constructor
  mw_curve_appearance_tracing_tool();
 
  //: Destructor
  virtual ~mw_curve_appearance_tracing_tool() {}
  
  //: Returns the string name of this tool
  vcl_string name() const;

  //: Handle events.
  bool handle( const vgui_event & e, 
               const bvis1_view_tableau_sptr& view );

  bool handle2( const vgui_event & e, 
               const bvis1_view_tableau_sptr& view );

  //: Set the tableau to work with
  bool set_tableau( const vgui_tableau_sptr& tableau );
  
  void activate ();
  void deactivate ();
  bool handle_mouse_click( const bvis1_view_tableau_sptr& view );

protected:

  vgui_event_condition gesture0;
  vcl_vector<mw_camera> cam_; //: cameras for each view
  vcl_vector<vil_image_view<float> > img_; //: cameras for each view
  vcl_vector<vcl_vector<vpgl_fundamental_matrix<double> > > fm_; //: fundamental matrices between each view
  vcl_vector<int> frame_v_;   //: frame number for each view
  unsigned nviews_;

  vgui_soview2D_point *p0_; //:< initial point in left curve segment
  vgui_soview2D_point *pn_; //:< end point in left curve segment
  unsigned p0_idx_; //:< index of p0 in curve
  unsigned pn_idx_;
  unsigned current_curve_id_;

  vcl_vector<dbdet_edgel_chain *> selected_crv_; //:< selected curve in each view (for reconstruction, etc)

  vcl_vector<bgui_vsol_soview2D_polyline *> selected_crv_soviews_n_; //:< selected curve segments in views #3 and up

  vsol_polyline_2d_sptr curvelet_; //:< selected curve segment; it is the polyline to be visualized.

  bgui_vsol_soview2D_polyline *curvelet_soview_; //:< selected curve segment
  bool selected_new_curve_;

  struct mycolor {
    float r,g,b;
  } color_p0_, color_pn_, color_aux_;

  vcl_vector<bgui_vsol2D_tableau_sptr> tab_; //:< tableaux used to draw in each view 

  //:< tableau containing original vsols in each view
  vcl_vector<bgui_vsol2D_tableau_sptr> curve_tableau_;
  bgui_vsol2D_tableau_sptr curve_tableau_current_;

  vcl_vector<vcl_vector< vsol_polyline_2d_sptr > > vsols_; //:< set of polylines at each view

  vcl_list<unsigned> crv_candidates_; //: index into vsols_[i2] of candidate (whole) curves
  vcl_vector<vsol_polyline_2d_sptr> crv_candidates_ptrs_;
  vcl_list<bgui_vsol_soview2D_polyline *> crv_candidates_soviews_; 
  vcl_vector<unsigned> crv_candidates_idx_;  //: v[i] == index into isets_.L_ of crv_candidates_[i]
  vgui_style_sptr cc_style_;  //:< style for curve segts


  //: Index to vector designates other views; index 0 is 3rd view, index 1 is 4rth
  // view, and so forth.
  vcl_vector<bgui_vsol_soview2D_polyline *> reproj_soview_;
  
  vcl_vector<vcl_vector<bgui_vsol_soview2D_polyline *> > p_reproj_soviews_;


  vcl_list<vgui_soview2D_point *> intercept_pts_soviews_;
  vcl_vector<vgui_soview2D_point *> all_intercept_pts_soviews_;
  bool display_all_intersections_; 


  //: These store epip. lines for endponts only, from view 1 to each other view. (vector index
  // indicates view) 
  vcl_vector<vgui_soview2D_infinite_line *> ep0_soview_;
  vcl_vector<vgui_soview2D_infinite_line *> epn_soview_;
  vcl_vector<vgl_homg_line_2d<double> > ep0_;
  vcl_vector<vgl_homg_line_2d<double> > epn_;

  //: Same but for epipolar lines from view [1] to i_view >= 2 (0-index)
  // index to vector designates other views; index 0 is 3rd view, index 1 is 4rth
  // view, and so forth.
  vcl_vector<vgui_soview2D_infinite_line *> ep0_soview_2n_;
  vcl_vector<vgui_soview2D_infinite_line *> epn_soview_2n_;
  vcl_vector<vgl_homg_line_2d<double> > ep0_2n_;
  vcl_vector<vgl_homg_line_2d<double> >  epn_2n_;


  //: Epipolar lines of curve segt points in image 1 in all other views.
  // index to vector designates other views; index 0 is 2nd view and index 1 is 3rd
  // view
  vcl_vector<vcl_vector<vgl_homg_line_2d<double> > > ep_; 
  vcl_vector<vcl_list<vgui_soview2D_infinite_line *> > ep_soviews_;

  vcl_vector<vgl_homg_line_2d<double> > ep_left_; 
  vcl_list<vgui_soview2D_infinite_line *> ep_soviews_left_;

  //: Index to vector designates other views; index 0 is 3rd view, index 1 is 4rth
  // view, and so forth.
  vcl_vector<vcl_vector<vgl_homg_line_2d<double> > > ep_2n_; 
  vcl_vector<vcl_list<vgui_soview2D_infinite_line *> > ep_soviews_2n_;


  vgui_style_sptr ep_style_;
  bool display_all_right_epips_;
  bool display_all_left_epips_;
  bool display_all_nth_view_epips_;

  bool compute_isets_; 
  mw_intersection_sets  isets_;

  bool click_selects_whole_curve_;

  // Best matches
  vcl_list<bgui_vsol_soview2D_polyline *> crv_best_matches_soviews_; 
  vcl_vector<vgui_style_sptr> best_match_style_;  //:< style for curve segts

private: 
  //: Some definitions are in mw_curve_appearance_tracing_tool_rec.cxx
  void get_cameras();
  void get_images();
  void init_tableaux(vcl_vector< bvis1_view_tableau_sptr > &views);
  void get_curves(vcl_vector< bvis1_view_tableau_sptr > &views);
  void get_corresponding_point_v0_to_vn(unsigned v,unsigned di0, vsol_point_2d_sptr &pt_img1) const;
  bool handle_mouse_click( const vgui_event & e, const bvis1_view_tableau_sptr& view);
  void initialize_curve_selection(unsigned pt_idx);
  void update_pn_(const vsol_point_2d_sptr &pt);
  void swap_p0_and_pn_();
  void draw_candidate_curves();
  void show_all_intersection_points();
  void update_display_for_intersection_stuff();
  void update_display_for_epipolar_curve_pencil();
  void reconstruct_possible_matches();
  void reconstruct_multiview();

  void linearly_reconstruct_pts(
      const vcl_vector<vsol_point_2d_sptr> &pt_img,
      const vcl_vector<unsigned> &views,
      vgl_point_3d<double> *pt_3D) const;

  void nonlinearly_optimize_reconstruction(
      const vcl_vector<vsol_point_2d_sptr> &pt_img,
      const vcl_vector<unsigned> &views,
      const vgl_point_3d<double> &pt_3D_initial,
      vgl_point_3d<double> *pt_3D) const;

  void get_reconstructions(
      const vcl_vector<unsigned> &views, 
      unsigned ini_idx, 
      unsigned di0, 
      mw_vector_3d *pt_3D, 
      mw_vector_3d *pt_3D_linear) const;

  bool get_index_of_candidate_curve(
      const vsol_polyline_2d_sptr & selected_crv, 
      unsigned *jnz);

  void break_curves_into_episegs();
  void break_curves_into_episegs(
      const vcl_vector< vsol_polyline_2d_sptr >  &vsols,
      const vgl_homg_point_2d<double> &e, int frame, int view) const;

  void show_reprojections(unsigned jnz);

  void project(
      unsigned view, 
      vcl_vector<vsol_point_2d_sptr> &proj, 
      const vcl_vector<mw_vector_3d> &crv3d, 
      mw_rig &/*rig*/) const;

  void reconstruct_and_reproject(
      unsigned jnz, 
      unsigned view, 
      vcl_vector<vsol_point_2d_sptr> &reproj, 

      vcl_vector<mw_vector_3d> &crv3d, 
      vcl_vector<unsigned> &crv1_idx,
      vcl_vector<unsigned> &crv2_idx,
      mw_rig &rig) const;

  void reconstruct_one_candidate(
      unsigned jnz, 
      vcl_vector<mw_vector_3d> &crv3d, 
      const vcl_vector<unsigned> &crv1_idx,
      const vcl_vector<unsigned> &crv2_idx,
      mw_rig &rig) const;

  void 
  define_match_for_reconstruction(
      unsigned jnz,
      vcl_vector<unsigned> &crv1_idx,
      vcl_vector<unsigned> &crv2_idx,
      mw_rig &rig
      ) const;

  static vgl_homg_line_2d<double> l_epipolar_line(
      const vpgl_fundamental_matrix<double> &F, 
      const vgl_homg_line_2d<double> &ep_r);
};


#endif // mw_curve_appearance_tracing_tool_h
