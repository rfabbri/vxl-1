// This is mw_correspond_point_tool.h
#ifndef mw_correspond_point_tool_h
#define mw_correspond_point_tool_h
//:
//\file
//\brief Tool for exploring multiview correspondences between edgels
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date Mon Apr 24 22:36:01 EDT 2006
//

#include <mw/vis/mw_correspond_point_tool_basic.h>
#include <mw/mw_discrete_corresp.h>

#include <vcl_map.h>

class dbgl_eulerspiral;

//: Tool for exploring and editing binocular correspondences between edgels
//
// It is also a stepping stone towards implementing a gui that is general for 
// n-views. 
//
// \sec Design overview
//
// - The base class mw_correspond_point_tool_basic is responsible for displaying
// epipolar lines in all views, for one or more selected edgels. The base class
// does not touch the correspondence data; it is only an epipolar visualization
// tool and a place to store basic data such as the cameras.
//
// - The present class pretty much adds a visual layer on top of the base class.
// The user can click around at will, and the base class will mark the clicked
// edge with a dot  and show epipolar lines to guide the
// user. We call such clicked edgel a "selected" edgel. At the same time, this
// class shows "active" edgels, which are those for which the
// correspondence datastructure is either being explored or edited. The "active"
// edge is shown in different color with a thicker line. The user can
// move the mouse around and the "active" edge will change, but the "selected"
// edge only changes when a click occurs. In summary, "selected" edges are for
// exploring epipolar geometry, and "active" edges are for exploring+editing the
// edge correspondence datastructure. If you want the "active" edge to
// remain in place, you can press 'l' to lock it. You can now move on to a
// second view and select active edgels there.
//
// - You can actually load and save correspondences. You can also load ground truth
// correspondence in order to see which matches are wrong.
//
// \sec Color coding
// 
// We indicate edgels participating in some correspondence in different colors.
//
// Sometimes, for a selected n-tuplet (a pair in this tool's case), a key can be
// pressed in order to print information about that tuplet as stored in the
// correspondence structure, or as computed on-the-fly. See the function
// handle_key for the available keypresses.
//
// \sec Usage
//
class mw_correspond_point_tool : public mw_correspond_point_tool_basic {
public:
  //: Constructor
  mw_correspond_point_tool();
 
  //: Destructor
  virtual ~mw_correspond_point_tool() {}
  
  //: Returns the string name of this tool
  virtual vcl_string name() const;

  //: Handle events.
  virtual bool handle( const vgui_event & e, 
               const bvis1_view_tableau_sptr& view );

  virtual void activate ();
  virtual void deactivate ();

  vgui_event_type gesture_query_corresp_;

protected:

  mw_discrete_corresp *corr_;
  vsol_line_2d_sptr selected_edgel_corresp_;
  vgui_style_sptr corresp_edges_style_;
  vgui_style_sptr wrongly_matched_edgels_style_;
  vcl_vector<vgui_style_sptr> best_match_style_; 
  vcl_string best_match_layer_; 
  vcl_string corresp_edges_layer_; 

  vcl_list<bgui_vsol_soview2D_line_seg *> correspondents_soview_;
  vcl_map<unsigned, vcl_list<bgui_vsol_soview2D_line_seg *>::iterator > correspondents_idx_;
  vcl_vector<vil_image_resource_sptr> images_;
  bool lock_corresp_query_;

  unsigned p0_query_idx_;
  bgui_vsol_soview2D_line_seg *p0_query_soview_; //:< query-selected point in left view
  vgui_style_sptr p0_query_style_; 
  vcl_string p0_query_layer_; 

  unsigned p1_query_idx_;
  bgui_vsol_soview2D_line_seg *p1_query_soview_; //:< query-selected point in left view
  vgui_style_sptr p1_query_style_; 
  //: if selected point in left view is a candidate, heres an iterator for it in the
  // correspondences list:
  vcl_list<mw_attributed_object>::iterator p1_query_itr_; 
  bool p1_query_is_candidate_;
  vcl_string p1_query_layer_; 

  vcl_vector<bgui_vsol_soview2D_line_seg *> p0_corresp_soview_;

  // For manipulating ground-truth /  synthetic data
  bool synthetic_;
  bool synthetic_olympus_;
  vcl_vector<vcl_vector<dbdif_3rd_order_point_2d> > crv2d_gt_;
  vcl_vector<vcl_vector<dbdif_3rd_order_point_3d> > crv3d_gt_;
  mw_discrete_corresp gt_; //:< ground-truth corresp.
  vcl_vector<dbgl_eulerspiral *>es_; //:< srm stands for synthetic reconstruction movie
  vgui_style_sptr es_style_;
  vcl_string es_layer_; 
  vcl_string srm_allcrvs_layer_; 
  double srm_angle_;
  dbdif_camera srm_cam_;
  vnl_double_3x3 srm_K_;
  dbdif_3rd_order_point_3d srm_Prec_;
  bool  srm_display_full_; 
  bool  srm_display_es_; 
  vcl_vector<bgui_vsol_soview2D_polyline *> srm_allcrvs_so_;
  vgui_style_sptr srm_allcrvs_style_;

  vcl_vector<bgui_vsol_soview2D_polyline *> es_so_;
  vcl_vector<double> angle_cam_; //:< degrees; only used in digital cameras's synthetic data for now

  vcl_vector<dbdif_camera> cam_gt_; //: unperturbed cameras for each view

  dbgl_eulerspiral *es_gt_; //:< eulerspiral of reproj. in 3rd view using true cams
  vgui_style_sptr es_style_gt_;
  vcl_string es_layer_gt_;
  bgui_vsol_soview2D_polyline * es_so_gt_;


  bool 
  find_crv3d_idx(unsigned idx, unsigned &i_crv, unsigned &i_pt) const;

  unsigned  nrows_, ncols_;

  virtual bool handle_key(vgui_key key);
  virtual bool handle_mouse_event_whatever_view( 
      const vgui_event & e, 
      const bvis1_view_tableau_sptr& view );

  virtual bool handle_mouse_event_at_view_0( 
    const vgui_event & e, 
    const bvis1_view_tableau_sptr& view );

  virtual bool handle_mouse_event_at_view_1( 
    const vgui_event & e, 
    const bvis1_view_tableau_sptr& view );

  bool handle_corresp_query_whatever_view
      ( const vgui_event & e, 
      const bvis1_view_tableau_sptr& /*view*/ );

  bool handle_corresp_query_at_view_0
      ( const vgui_event & /*e*/, 
      const bvis1_view_tableau_sptr& view );

  bool handle_corresp_query_at_view_1
      ( const vgui_event & e, 
      const bvis1_view_tableau_sptr& view );

  void run_stereo_matcher();
  void toggle_lock_correspondence_query();
//  void get_epipolar_candidates();
//  void run_stereo_matcher_given_epipolar_candidates();
//  void run_stereo_matcher_and_epipolar_candidates();

  dbgl_eulerspiral * 
  get_new_eulerspiral(const dbdif_3rd_order_point_2d &p1) const;

  void srm_draw_eulerspiral();
  void write_energies();
};


#endif // mw_correspond_point_tool_h
