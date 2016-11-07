// This is brcv/rec/dbskr/vis/dbskr_debug_scurve_tool.h
#ifndef dbskr_debug_scurve_tool_h_
#define dbskr_debug_scurve_tool_h_
//:
// \file
// \brief To debug the scurve computations from shock edges 
// \author Amir Tamrakar
// \date 05/15/2006
//
// \verbatim
//  Modifications
//  O. C. Ozcanli 09/06/06, added dbskr_tree functionalities
//                          press u to update splices after changing threshold
// \endverbatim

#include <dbsk2d/vis/dbsk2d_ishock_highlight_tool.h>

#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_projection_inspector.h>
#include <dbsk2d/dbsk2d_shock_edge_sptr.h>
#include <dbsk2d/dbsk2d_shock_node_sptr.h>
#include <dbskr/dbskr_scurve_sptr.h>
#include <dbskr/dbskr_tree_sptr.h>

class dbskr_debug_scurve_tool : public dbsk2d_ishock_highlight_tool 
{
public:

  dbskr_debug_scurve_tool();
  virtual ~dbskr_debug_scurve_tool();

  bool handle( const vgui_event & e, 
               const bvis1_view_tableau_sptr& view );

  //: This is called when the tool is activated
  virtual void activate();

  vcl_string name() const;

  virtual void get_popup( const vgui_popup_params& params, vgui_menu &menu );

  void output_scurve();
  dbsk2d_shock_edge_sptr get_selected_edge();
  dbskr_scurve_sptr get_scurve();
  void draw_scurves();
  void draw_an_scurve(dbskr_scurve_sptr cur_scurve, 
                      float rp, float gp, float bp, 
                      float rm, float gm, float bm);
  void draw_spliced_shock_branches();

protected:
  vgui_event_condition select_edge; ///< to select an edge and reset the grouping
  vgui_event_condition add_edges;   ///< to add to the current chain of shock edges
  vgui_event_condition reverse_sampling_cmd; ///< to reverse the sampling of the scurve
  vgui_event_condition select_tree_edge;
  vgui_event_condition update_splice_map;

  dbskr_tree_sptr tree_;
  vgl_point_2d<double> selected_dart_pt_;
  vgl_point_2d<double> selected_dart_end_;
  int selected_dart_; unsigned dart_cnt_;
  //: keep a sorted map of all darts based on splice costs to determine which get pruned 
  //  and which stay
  vcl_map< vcl_pair<double, unsigned>, bool> dart_splice_map_;
  bool add_splice_;
  double threshold_;
  void update_dart_splice_map();

  vcl_vector<dbsk2d_shock_edge_sptr> selected_edges;
  dbsk2d_shock_node_sptr start_node;

  dbskr_scurve_sptr cur_scurve; ///< this is the scurve formed from the selected edges

  bool binterpolate_, subsample_;
  double interpolate_ds_, subsample_ds_;
  bool reverse_sampling_;
  bool display_spliced_contours_;
  bool output_selected_scurve_info_; ///< write out scurve info for the selected edges onto a file (for debug)

  bool construct_circular_ends_;
  bool elastic_splice_cost_;
  bool find_splice_cost_using_dpmatch_combined_;
  double R_;

};

#endif //dbskr_debug_scurve_tool_h
