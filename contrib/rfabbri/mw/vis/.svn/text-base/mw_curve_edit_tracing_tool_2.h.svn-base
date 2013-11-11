// This is mw_curve_edit_tracing_tool_2.h
#ifndef mw_curve_edit_tracing_2_h
#define mw_curve_edit_tracing_2_h
//:
//\file
//\brief Tool for exploring multiview curve correspondence
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date Fri Oct 09 2009
//

#include <mw/mw_discrete_corresp.h>
#include <mw/pro/mw_discrete_corresp_storage.h>
#include <mw/vis/mw_curve_tracing_tool_2.h>

//: Same as \c mw_curve_tracing_tool, 
// but adds a layer of functionality that allows the user to edit
// correspondences. 
//
// \see mw_3_correspond_point_tool for the way this works, as the interaction is
// very similar.
//
class mw_curve_edit_tracing_tool_2 : public mw_curve_tracing_tool_2 {
public:
  //: Constructor
  mw_curve_edit_tracing_tool_2();

  //: Destructor
  ~mw_curve_edit_tracing_tool_2() {}

  //: Handle events.
  bool handle( const vgui_event & e,
               const bvis1_view_tableau_sptr& view );
 
  //: Returns the string name of this tool
  vcl_string name() const;

  void activate();
  void deactivate();

  virtual void get_popup( const vgui_popup_params& params, vgui_menu &menu );

protected:

  //: Common work to all constructors
  void init();

private:
  // Data -----
  mw_discrete_corresp *corr_;
  mw_discrete_corresp_storage_sptr c_sto_;

  vsol_polyline_2d_sptr selected_obj_in_corresp_;

  bool lock_corresp_query_;
  unsigned o0_query_idx_;
  mw_discrete_corresp::one_corresp_list_iter o1_query_itr_; 
  unsigned o1_query_idx_;
  bool o1_query_is_corresp_;

  vcl_map<unsigned, vcl_list<bgui_vsol_soview2D *>::iterator > correspondents_idx_;

  //: threshold in cost when asked to show only the matches with this cost.
  double tau_cost_;

  // Styles  -----
  vcl_vector<vgui_style_sptr> best_match_style_; 
  vgui_style_sptr o0_query_style_; 
  vgui_style_sptr o0_query_off_style_; 
  vgui_style_sptr corresp_objs_style_;

  vcl_string best_match_layer_; 
  vcl_string corresp_objs_layer_; 
  vcl_string o0_query_layer_;

  vgui_event_type gesture_query_corresp_;

  // Soviews -----

  //: query-selected object in right view
  bgui_vsol_soview2D *o0_query_soview_; 

  //: mouse_over object in right view
  bgui_vsol_soview2D *o1_soview_; 
  vcl_vector<bgui_vsol_soview2D *> o0_corresp_soview_;
  vcl_vector<bgui_vsol_soview2D *> o1_corresp_soview_;
  vcl_list<bgui_vsol_soview2D *> correspondents_soview_;

  void toggle_lock_correspondence_query();
  bool handle_key(vgui_key key);
  void get_corresp();
  void color_objs_having_correpondents();
  bool handle_corresp_query_at_view_0
      ( const vgui_event & e, 
      const bvis1_view_tableau_sptr& view );
  bool handle_mouse_event_at_view_1(
      const vgui_event & e,
      const bvis1_view_tableau_sptr& view );
};

#endif // mw_curve_edit_tracing_2_h
