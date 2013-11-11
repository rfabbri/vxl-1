// This is brcv/rec/dbskr/vis/dbskr_visualize_path_scurve_tool.h
#ifndef dbskr_visualize_path_scurve_tool_h_
#define dbskr_visualize_path_scurve_tool_h_
//:
// \file
// \brief To debug the path computation between given two nodes
// \author Ozge C Ozcanli
// \date 03/12/2008
//
// \verbatim
//  Modifications
//
// \endverbatim

#include <dbsk2d/vis/dbsk2d_ishock_highlight_tool.h>

#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_projection_inspector.h>
#include <dbsk2d/dbsk2d_shock_edge_sptr.h>
#include <dbsk2d/dbsk2d_shock_node_sptr.h>
#include <dbskr/dbskr_scurve_sptr.h>
#include <dbskr/algo/dbskr_shock_path_finder_sptr.h>
#include <dbskr/dbskr_tree_sptr.h>
#include <dbskr/dbskr_shock_patch_sptr.h>
#include <dbskr/dbskr_v_graph_sptr.h>

class dbskr_visualize_path_scurve_tool : public dbsk2d_ishock_highlight_tool 
{
public:

  dbskr_visualize_path_scurve_tool();
  virtual ~dbskr_visualize_path_scurve_tool();

  bool handle( const vgui_event & e, 
               const bvis1_view_tableau_sptr& view );

  //: This is called when the tool is activated
  virtual void activate();

  vcl_string name() const;

  virtual void get_popup( const vgui_popup_params& params, vgui_menu &menu );

  void output_scurve();
  dbsk2d_shock_node_sptr get_selected_node();
  dbsk2d_shock_edge_sptr get_selected_edge();
  dbskr_scurve_sptr get_scurve();
  void draw_scurve();
  void draw_an_scurve(dbskr_scurve_sptr cur_scurve, 
                      float rp, float gp, float bp, 
                      float rm, float gm, float bm,
                      float rs, float gs, float bs);

  void draw_shock_path(vcl_vector<dbsk2d_shock_edge_sptr> &edges, float r, float g, float b);

  void draw_v_graph_nodes(dbskr_v_graph_sptr v, float r, float g, float b, float point_size);
  void draw_patch(dbskr_shock_patch_sptr shock_patch, float r, float g, float b);
  void draw_box(vsol_box_2d_sptr box, float r, float g, float b);

protected:
  vgui_event_condition select_node1; ///< to select an edge and reset the grouping
  vgui_event_condition select_node2; ///< to select an edge and reset the grouping
  vgui_event_condition next_curve; ///< to select an edge and reset the grouping
  vgui_event_condition next_graph; ///< to select an edge and reset the grouping
  vgui_event_condition create_shock_storage; ///< to select an edge and reset the grouping
  

  dbskr_shock_path_finder_sptr pf_;
  
  dbsk2d_shock_node_sptr node1;
  dbsk2d_shock_node_sptr node2;

  //given two patches find the paths, the root nodes of the patches are assumed to be node1 and node2 respectively
  bool use_patches_;
  double patch_depth1_;  // double will be cast to int, has to be double to be set from the gui
  double patch_depth2_;
  dbskr_shock_patch_sptr sp1_;
  dbskr_shock_patch_sptr sp2_;
  dbskr_shock_patch_sptr sp_middle_;
  vsol_box_2d_sptr patch_box_;
  double length_thres_;

  dbskr_scurve_sptr cur_scurve; ///< this is the scurve formed from the path of the selected nodes
  vcl_vector<dbskr_scurve_sptr> scurves_;
  vcl_vector<vcl_vector<dbskr_v_node_sptr> > paths_;
  vcl_vector<dbskr_v_graph_sptr> graphs_;
  vcl_vector<dbskr_v_edge_sptr> outer_edges_;
  unsigned current_ind_, current_graph_ind_;
  dbskr_v_graph_sptr current_graph_;

  vcl_vector<float> abs_lengths_;

  bool binterpolate_, subsample_;
  double interpolate_ds_, subsample_ds_;
  
  bool construct_circular_ends_;
  bool display_outer_boundary_;
  bool display_traced_boudary_;
  bool display_patches_box_;

};

#endif //dbskr_visualize_path_scurve_tool_h
