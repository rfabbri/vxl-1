// This is brcv/rec/dbskr/vis/dbskr_subgraph_scurve_tool.h
#ifndef dbskr_subgraph_scurve_tool_h_
#define dbskr_subgraph_scurve_tool_h_
//:
// \file
// \brief To draw the scurves of a shock graph starting from the selected node
// \author Ozge C. Ozcanli
// \date Jan 09, 2007
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
#include <vil/vil_rgb.h>

class dbskr_subgraph_scurve_tool : public dbsk2d_ishock_highlight_tool 
{
public:

  dbskr_subgraph_scurve_tool();
  virtual ~dbskr_subgraph_scurve_tool();

  bool handle( const vgui_event & e, 
               const bvis1_view_tableau_sptr& view );

  //: This is called when the tool is activated
  virtual void activate();

  vcl_string name() const;

  virtual void get_popup( const vgui_popup_params& params, vgui_menu &menu );
#if 0
  void get_scurves(dbsk2d_shock_node_sptr start_node, dbsk2d_shock_edge_sptr parent_edge, 
                   vcl_vector<dbskr_scurve_sptr>& cur_scurves,
                   vcl_vector<dbsk2d_shock_node_sptr>& visited_nodes, 
                   vcl_vector<bool>& end_lines, int depth = 1);
#endif
  void draw_scurves();
  void draw_an_scurve(dbskr_scurve_sptr cur_scurve, 
                      float rp, float gp, float bp, 
                      float rm, float gm, float bm, bool end_scurve);
  void draw_boundary();

protected:
  vgui_event_condition select_edge; ///< to select an edge and reset the grouping
  vgui_event_condition increase_depth;
  vgui_event_condition decrease_depth;
  vgui_event_condition reset_depth;
  vgui_event_condition switch_drawing;

  vcl_vector<dbskr_scurve_sptr> cur_scurves_; ///< this is the scurve formed from the selected edges
  //vcl_vector<dbsk2d_shock_node_sptr> visited_nodes_;
  vcl_vector<bool> end_scurve_; // will be drawn in yellow if the scurve is right at the end

  bool draw_boundary_;
  vsol_polygon_2d_sptr poly_;
  bool draw_with_circular_completions_;

  bool binterpolate_, subsample_;
  double interpolate_ds_, subsample_ds_;
  bool reverse_sampling_;
  double depth_;
  double poly_area_threshold_;
  double color_r_, color_g_, color_b_;
  double line_width_;
};

#endif //dbskr_subgraph_scurve_tool_h
