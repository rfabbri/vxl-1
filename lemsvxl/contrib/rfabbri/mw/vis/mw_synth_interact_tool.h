// This is mw_synth_interact_tool.h
#ifndef mw_synth_interact_tool_h
#define mw_synth_interact_tool_h
//:
//\file
//\brief Tool for exploring synthetic data (eg synthcurves-multiview-3d-dataset)
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@gmail.com)
//\date Tue Feb  5 20:38:15 EST 2019
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


//
// Given n views, if you click on an edge in the first view,
// say 's', then it will select the same id in all other views
class mw_synth_interact_tool : public bvis1_tool {
public:
  //: Constructor
  mw_synth_interact_tool();
 
  //: Destructor
  ~mw_synth_interact_tool();
  
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
  void clear_previous_selections();

  //: \see frame_v()
  std::vector<int> frame_v_;

  unsigned nviews() const { return s_->nviews(); }

  // edgels 
  // 
  std::vector<dbdet_edgemap_sptr> edgs_; 

  // ------ GUI Members -------

  struct mycolor {
    float r,g,b;
  } color_p0_, color_pn_, color_aux_;

  unsigned selected_edgel_id_;

  vgui_event_condition gesture0_;

  //: tableaux used to draw in each view 
  std::vector<bgui_edge_tableau_sptr> tab_;

private: 

  void init_tableaux(std::vector< bvis1_view_tableau_sptr > &views);

  //: Get vsols in all views
  void get_curves(std::vector< bvis1_view_tableau_sptr > &views);

  //: Tries to see if there is an active SEL storage in this view. 
  bool get_edgs(const bvis1_view_tableau_sptr &view, 
      std::vector< vsol_polyline_2d_sptr > *pcurves);

  bool handle_mouse_click( const vgui_event & e, const bvis1_view_tableau_sptr& view);
};


#endif // mw_synth_interact_tool_h
