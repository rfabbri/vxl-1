// This is mw_curve_tracing_tool_3.h
#ifndef mw_curve_tracing_tool_3.h
#define mw_curve_tracing_tool_3.h
//:
//\file
//\brief Tool for displaying epipolar lines across frames
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date 07/19/2005 12:09:14 AM EDT
//

#include <bvis1/bvis1_tool.h>
#include <bgui/bgui_vsol2D_tableau.h>
#include <bgui/bgui_vsol2D_tableau_sptr.h>
#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_style_sptr.h>
#include <bgui/bgui_vsol_soview2D.h>
#include <bgui/bgui_selector_tableau_sptr.h>

#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_fundamental_matrix.h>
#include <mw/mw_intersection_sets.h>



//: This tool displays epipolar lines corresponding to user mouse events.
// It acts between two frames; all that is required is a perspective 
// camera storage for each frame. In the future we shall support fundamental
// matrix storage class as well, this should be easy to do. 
//
// The tool currently creates vsol2D tableaus upon activation. These tableaus
// are used for drawing the epipolars. The user can subsequently
// activate/deactivate and change visibility of the epipolars. If you have
// a better design in mind feel free to share it with us.
//
// USAGE
//  1- Define two frames.
//  2- Put an image into each frame; detect edges and transform the edges into
//     Vsol polylines by using pruning process.
//  3- Put a vpgl_perspective_camera into a dvpgl_camera_storage in each frame.
//  4- Define two views, one for each frame.
//  5- Set the two edge vsols as active in each frame.
//  6- call the tool and click on the first view's vsol 
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
//  containing a checklist of intersection info to display.
//
//
// \remarks Marginal notes
//  - idea for intersection of line and polyline:
//     - divide polyline segt in two
//     - repeat for the curve segment whose bbox intersect the line
//
class mw_curve_tracing_tool_3 : public bvis1_tool {
public:
  //: Constructor
  mw_curve_tracing_tool_3();
 
  //: Destructor
  virtual ~mw_curve_tracing_tool_3() {}
  
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

protected:

  vgui_event_condition gesture0;
  const vpgl_perspective_camera<double> *cam1_;
  const vpgl_perspective_camera<double> *cam2_;
  vpgl_fundamental_matrix<double> *fm_;

  vgui_soview2D_point *p0_; //:< initial point in left curve segment
  vgui_soview2D_point *pn_; //:< end point in left curve segment
  unsigned p0_idx_; //:< index of p0 in curve
  unsigned pn_idx_;
  unsigned current_curve_id_;
  vsol_polyline_2d_sptr crv_; //:< selected curve
  vsol_polyline_2d_sptr curvelet_; //:< selected curve segment
  bgui_vsol_soview2D_polyline *curvelet_soview_; //:< selected curve segment
  bool selected_new_curve_;

  struct mycolor {
    float r,g,b;
  } color_p0_, color_pn_, color_aux_;

  vgui_soview2D_infinite_line *ep0_soview_;
  vgui_soview2D_infinite_line *epn_soview_;
  vgl_homg_line_2d<double> ep0_;
  vgl_homg_line_2d<double> epn_;

  bgui_vsol2D_tableau_sptr tab_l_; //:< tableau used to draw in left img
  bgui_vsol2D_tableau_sptr tab_r_; //:< tableau used to draw in right img
  bgui_vsol2D_tableau_sptr curve_tableau_l_;

  vcl_vector< vsol_polyline_2d_sptr > vsols_right_;

  vcl_list<unsigned> crv_candidates_; //: index into vsols_right_ of candidate (whole) curves
  vcl_vector<vsol_polyline_2d_sptr> crv_candidates_ptrs_;
  vcl_list<bgui_vsol_soview2D_polyline *> crv_candidates_soviews_; 
  vgui_style_sptr cc_style_;  //:< style for curve segts


  vcl_list<vgui_soview2D_point *> intercept_pts_soviews_;
  vcl_vector<vgui_soview2D_point *> all_intercept_pts_soviews_;
  bool display_all_intersections_; 

  //: epipolar lines of curve segt points in image 1
  vcl_vector<vgl_homg_line_2d<double> > ep_; 
  vcl_list<vgui_soview2D_infinite_line *> ep_soviews_;
  vgui_style_sptr ep_style_;
  bool display_all_epips_;

  bool compute_isets_; 
  mw_intersection_sets  isets_;

private: 
  void update_pn_(const vsol_point_2d_sptr &pt);
  void swap_p0_and_pn_();
  void draw_candidate_curves();
  void show_all_intersection_points();
  void update_display_for_intersection_stuff();
  void update_display_for_epipolar_curve_pencil();
  void reconstruct_possible_matches();
};


#endif // mw_curve_tracing_tool_3.h
