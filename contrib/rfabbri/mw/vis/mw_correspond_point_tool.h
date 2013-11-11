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
