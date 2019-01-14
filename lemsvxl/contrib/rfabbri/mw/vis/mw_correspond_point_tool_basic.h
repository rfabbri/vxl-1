// This is mw_correspond_point_tool_basic.h
#ifndef mw_correspond_point_tool_basic_h
#define mw_correspond_point_tool_basic_h
//:
//\file
//\brief Tool for exploring multiview correspondences between edgels
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@gmail.com)
//\date Mon Apr 24 22:36:01 EDT 2006
//

#include <map>

#include <bvis1/bvis1_tool.h>
#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_style_sptr.h>
#include <bgui/bgui_vsol2D_tableau.h>
#include <bgui/bgui_vsol2D_tableau_sptr.h>

#include <bdifd/bdifd_camera.h>

//: Many of the basic operations this tool does, as far as displaying epipolar
//lines is concerned, could be moved to a vpgld_epipolar_tool
//

class mw_correspond_point_tool_basic : public bvis1_tool {
public:
  //: Constructor
  mw_correspond_point_tool_basic();
 
  //: Destructor
  virtual ~mw_correspond_point_tool_basic() {}
  
  //: Returns the string name of this tool
  std::string name() const;

  //: Handle events.
  bool handle( const vgui_event & e, 
               const bvis1_view_tableau_sptr& view );

  //: Set the tableau to work with
  bool set_tableau( const vgui_tableau_sptr& tableau );
  
  virtual void activate ();
  virtual void deactivate ();

  vgui_event_condition gesture0_;

protected:

  std::vector<bdifd_camera> cam_; //: cameras for each view
  std::vector<int> frame_v_;   //: frame number for each view
  std::map<int,unsigned> iv_frame_; //: view number for each frame
  unsigned nviews_;

  std::vector<std::vector< vsol_point_2d_sptr > > vsols_; //:< pointsets in each view
  std::vector<std::vector< vsol_line_2d_sptr > > vsols_orig_cache_; //:< edgels in each view

  bgui_vsol2D_tableau_sptr edgel_tableau_current_;

  std::vector<bgui_vsol2D_tableau_sptr> tab_; //:< tableaux used to draw in each view 
  
  //: p_[i] == selected point  in view i
  // if the i-th view point has not been selected at all, p_[i] == 0 
  std::vector<vgui_soview2D_point *>p_; 
  bool any_selected_point_;
  std::vector<unsigned> p_idx_; //:< idx of p_[i] in vsols[i]
  std::vector<vgui_style_sptr> p_style_; //:< style of p_[i]
  vsol_line_2d_sptr selected_edgel_;

  //: ep_[i][k] : epipolar line from a point in view i, shown at view k
  std::vector<std::vector<vgl_homg_line_2d<double> > > ep_;
  std::vector<std::vector<vgui_soview2D_infinite_line *> > ep_soview_;

  //: ep_style_[i] == style of ep_[i][k] for all k
  std::vector<vgui_style_sptr> ep_style_;

//  std::list<unsigned> intercepts_id_;

//  std::vector<vgui_soview2D_point *> all_intercept_pts_soviews_;
//  bool display_all_intersections_; 
//  bool compute_isets_; 

protected: 
  void handle_point_selection_whatever_view(vsol_point_2d_sptr pt, unsigned v);

  void update_display_for_epipolars( bool redraw , unsigned starting_view);
  void show_all_intersection_points();
//  void update_display_for_intersection_stuff();
//  void compute_intersections();
//  bool get_index_of_candidate_point(
//          const vsol_point_2d_sptr& selected_point, unsigned *idx);
//  bool get_index_of_candidate_point(
//          const vsol_line_2d_sptr& selected_edgel, unsigned *idx);
  bool get_index_of_point(
          const vsol_point_2d_sptr& selected_point, 
          const std::vector<vsol_point_2d_sptr> &pv,
          unsigned *idx);
  bool get_index_of_point(
          const vsol_line_2d_sptr& selected_edgel, 
          const std::vector<vsol_line_2d_sptr> &pv,
          unsigned *idx);

  virtual bool handle_key(vgui_key key);
  virtual bool handle_mouse_event_whatever_view( 
      const vgui_event & e, 
      const bvis1_view_tableau_sptr& view );
};


#endif // mw_correspond_point_tool_basic_h
