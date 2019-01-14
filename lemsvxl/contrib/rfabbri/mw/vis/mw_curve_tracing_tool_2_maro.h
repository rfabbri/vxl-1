// This is mw_curve_tracing_tool_2_maro.h
#ifndef mw_curve_tracing_tool_2_maro_h
#define mw_curve_tracing_tool_2_maro_h
//:
//\file
//\brief Tool for exploring multiview curve correspondences
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@gmail.com)
//\date 07/19/2005 12:09:14 AM EDT
//

#include <bvis1/bvis1_tool.h>
#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_style_sptr.h>
#include <vgui/vgui_selector_tableau_sptr.h>
#include <bgui/bgui_vsol2D_tableau.h>
#include <bgui/bgui_vsol2D_tableau_sptr.h>
#include <bgui/bgui_vsol_soview2D.h>

#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_fundamental_matrix.h>
#include <sdetd/pro/sdetd_sel_storage_sptr.h>
#include <becld/becld_intersection_sets.h>
#include <bdifd/bdifd_camera.h>
#include <bdifd/bdifd_rig.h>
#include <mw/mw_curves.h>
#include <bmcsd/algo/bmcsd_curve_stereo.h>


//: This tool allows the user to trace linked curve fragments to be matched and
// reconstructed from multiple views. 
//
// USAGE
//  -# Define n frames.
//  -# Put an image into each frame; detect edges and link the edges into
//     Vsol polylines or symbolic edge linker.
//  -# Put a vpgl_perspective_camera into a vpgld_camera_storage in each frame.
//  -# Define n_v views, one for each frame.
//  -# Set the linked curves as active in each frame.
//  -# call the tool and click on a curve fragment in the first view.
//  -# the candidates in the second view will be displayed.
//  -# The remainder views (3 and up) are just used for (i) confirming the
//  two-view selection by reprojection and (ii) to allow the user to select
//  the curves to be reconstructed.
//
// The tool currently creates vsol2D tableaus upon activation. These tableaus
// are used for drawing the epipolars. The user can subsequently
// activate/deactivate and change visibility of the epipolars. 
//
// KEYBOARD COMMANDS (KEYSTROKES)
//  - look into switch statement in .cxx file
//
// DESIGN
//  The mw_curve_tracing_tool_2* hierarchy provide a brown-eyes GUI hierarchy to
//  the basic classes of bmcsd_curve_stereo*. These two hierarchies of GUI and
//  implementation are in 1-1 correspondence by keeping a private pointer in the
//  tools to a base bmcsd_curve_stereo.h class. The pointer is automatically bound to the
//  appropriate bmcsd_curve_stereo subclass depending on which mw_curve_tracing_tool_2
//  is instantiated. The GUI classes are not to be used polymorphically, so
//  don't call a leaf class from a base class pointer. They are in a hierarchy
//  with the sole purpose of implementation reuse.
//
// EXAMPLE
//  The presentation capitol-manual-reconstruction-protocol.odp in the LEMS
//  vision disk describes how to perform a semi-automatic curve fragment
//  reconstruction of the R.I. Capitol building using this tool.
//
// \todo TODO
//  - make it work when changing frames during tool activity
//  - rename p1_ to pn_
//  - Add a submenu to the context menu (as matt did in his corresp tool),
//  - Work only with fundamental matrices / trifocal tensors (i.e.,
//  weakly-calibrated cameras or projective cameras)
//
class mw_curve_tracing_tool_2_maro : public bvis1_tool {
public:
  //: Constructor
  mw_curve_tracing_tool_2_maro();
 
  //: Destructor
  ~mw_curve_tracing_tool_2_maro();
  
  //: Returns the string name of this tool
  std::string name() const;

  //: Handle events.
  bool handle( const vgui_event & e, 
               const bvis1_view_tableau_sptr& view );

  //: Set the tableau to work with
  bool set_tableau( const vgui_tableau_sptr& tableau );
  
  void activate();
  void deactivate();
  bool handle_mouse_click( const bvis1_view_tableau_sptr& view );

  //: frame number for each view, since it is common to take views as a subset of
  // frames of a video
  int frame_v(unsigned i) const { return frame_v_[i]; }

  unsigned view_from_frame_index(unsigned fi) const;

protected:
  //: Implementation. A pointer to the base of the bmcsd_curve_stereo hierarchy of
  // curve-based multiview stereo classes.
  bmcsd_curve_stereo *s_;

  //: Constructor from a specific implementation.
  mw_curve_tracing_tool_2_maro(bmcsd_curve_stereo *impl);
  void init(bmcsd_curve_stereo *impl);
  void clear_previous_selections();

  //: \see frame_v()
  std::vector<int> frame_v_;

  unsigned nviews() const { return s_->nviews(); }

  std::vector<sdetd_sel_storage_sptr> sels_; 

  // ------ GUI Members -------

  struct mycolor {
    float r,g,b;
  } color_p0_, color_pn_, color_aux_;

  bool selected_new_curve_;

  //: \see selected_crv_id_view0()
  unsigned selected_crv_id_view0_;

  bool display_all_intersections_; 
  bool display_all_right_epips_;
  bool display_all_left_epips_;
  bool display_all_nth_view_epips_;
  bool compute_isets_; 
  bool click_selects_whole_curve_;
  vgui_style_sptr ep_style_;

  //: style for curve candidate segts
  vgui_style_sptr cc_style_; 

  //: style for curve segts
  std::vector<vgui_style_sptr> best_match_style_;
  vgui_event_condition gesture0_;

  //: initial point in left curve segment
  vgui_soview2D_point *p0_;

  //: end point in left curve segment
  vgui_soview2D_point *pn_; 

  //: selected curve segments in views #3 and up
  std::vector<bgui_vsol_soview2D_polyline *> selected_crv_soviews_n_; 

  //: selected curve segment
  bgui_vsol_soview2D_polyline *curvelet_soview_;

  //: tableaux used to draw in each view 
  std::vector<bgui_vsol2D_tableau_sptr> tab_;

  //: tableau used for retrieving selected curves
  bgui_vsol2D_tableau_sptr curve_tableau_current_;

  std::list<bgui_vsol_soview2D_polyline *> crv_candidates_soviews_; 

  //: Index to vector designates other views; index 0 is 3rd view, index 1 is 4rth
  // view, and so forth.
  std::vector<bgui_vsol_soview2D_polyline *> reproj_soview_;
  std::vector<std::vector<bgui_vsol_soview2D_polyline *> > p_reproj_soviews_;

  std::list<vgui_soview2D_point *> intercept_pts_soviews_;
  std::vector<vgui_soview2D_point *> all_intercept_pts_soviews_;

  std::vector<vgui_soview2D_infinite_line *> ep0_soview_;
  std::vector<vgui_soview2D_infinite_line *> epn_soview_;
  std::vector<vgui_soview2D_infinite_line *> ep0_soview_2n_;
  std::vector<vgui_soview2D_infinite_line *> epn_soview_2n_;
  std::vector<std::list<vgui_soview2D_infinite_line *> > ep_soviews_;
  std::list<vgui_soview2D_infinite_line *> ep_soviews_left_;
  std::vector<std::list<vgui_soview2D_infinite_line *> > ep_soviews_2n_;

  //: best matches
  std::list<bgui_vsol_soview2D_polyline *> crv_best_matches_soviews_; 

private: 

  //: retrieves the cameras from the repository
  void get_cameras();
  void init_tableaux(std::vector< bvis1_view_tableau_sptr > &views);

  //: Get vsols in all views
  void get_curves(std::vector< bvis1_view_tableau_sptr > &views);

  //: Tries to see if there is an active SEL storage in this view. If so, load the
  // curve fragments and store them as polylines. Also fill up the sels_ std::vector
  // with the storages, for use in children tool as needed.
  bool get_sels(const bvis1_view_tableau_sptr &view, 
      std::vector< vsol_polyline_2d_sptr > *pcurves);

  //: If there is an active VSOL storage with polylines in view v, load them.
  bool get_vsols( const bvis1_view_tableau_sptr &view, 
      unsigned v, std::vector< vsol_polyline_2d_sptr > *pcurves) const;

  bool handle_mouse_click( const vgui_event & e, const bvis1_view_tableau_sptr& view);

  void initialize_curve_selection();

  //: Draws candidate curves in view[1] which are those that intersect an
  // epipolar line of the epipolar beam of the points of the selected subcurve
  void draw_candidate_curves();

  void show_all_intersection_points();

  //: \remarks No computation involved
  void update_display_for_intersection_stuff();

  //: \remarks No computation involved
  void update_display_for_epipolar_curve_pencil();

  void show_reprojections(unsigned crv2_id);
  void add_episegs_to_tool(const std::vector< vsol_polyline_2d_sptr >  &vsols, unsigned frame);
  void add_all_episegs_to_tool(const std::vector<std::vector< vsol_polyline_2d_sptr > > &broken_vsols) const;
  void break_curves_into_episegs();

  void update_pn(const vsol_point_2d_sptr &pt);
  void swap_p0_and_pn();

  //: makes the selected subcurve in view[0] shrink so that all its points has an epipolar
  // line intersecting the selected candidate in view[1].
  void equalize_matching_subcurve();

  //: id of the selected curve in the tableau of view[0]. This is just to keep
  // track if a new curve was selected or not by comparing if the new index is
  // different than the old one.
  unsigned selected_crv_id_view0() const { return selected_crv_id_view0_; }
  void set_selected_crv_id_view0(unsigned id) { selected_crv_id_view0_ = id; }

  //: Paricular case only for focal length manip: reselects curve with id selected_crv_id_view0() in view0
  // Never use it except when curve is wholly selected
  void reselect_curve_view0() {
        clear_previous_selections();
        s_->initialize_subcurve(0);
        initialize_curve_selection();
        s_->update_endpoint(s_->selected_crv(0)->size()-1);
        update_pn(s_->selected_crv(0)->p1());
  }

  void increment_focalength(double val) {
        std::cout << "f mm: " << val*11 << std::endl;

        std::vector<bdifd_camera> newcams;
        for (unsigned v = 0; v < s_->nviews(); ++v) {
          vpgl_calibration_matrix<double> K(s_->cams(v).Pr_.get_calibration());
          std::cout << "Calib matrix: " << K.get_matrix() << std::endl;
          std::cout << "Rot matrix: " << s_->cams(v).Pr_.get_rotation().as_matrix() << std::endl;
          std::cout << "Transl: " << s_->cams(v).Pr_.get_translation() << std::endl;
          //std::cout << "Focal length: " << K.focal_length() << std::endl;
          if (fabs(val) < 1e-6) {
            std::cerr << "cannot decrement focalength";
            return;
          }
          K.set_focal_length(val); // adds delta mm to focal 
          std::cout << "Stored focal length: " << K.focal_length() << std::endl;
          // update P
          vpgl_perspective_camera<double> P(s_->cams(v).Pr_);
          P.set_calibration(K);
          bdifd_camera c;
          c.set_p(P);
          newcams.push_back(c);
        }
        s_->set_cams(newcams);
        reselect_curve_view0();
  }
  void draw_marked_points();
};


#endif // mw_curve_tracing_tool_2_maro_h
