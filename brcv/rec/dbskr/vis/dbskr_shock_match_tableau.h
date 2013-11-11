// This is brcv/rec/dbskr/vis/dbskr_shock_match_tableau.h
#ifndef dbskr_shock_match_tableau_h_
#define dbskr_shock_match_tableau_h_

//:
// \file
// \brief A tableau to display dbskr_shock_match_storage objects.
// \author Ozge Can Ozcanli
// \date Sep 30 2005
//
// \verbatim
//  Modifications
//    Amir Tamrakar  03/31/06       Cleaned up code.
//                                  Added methods to draw the visual fragments and 
//                                  the intrinsic coordinate grid of the fragments
//    O. C. Ozcanli  05/06/06       Made into a tool to display costs
//                                  Left click on a shock or boundary point to select that shock curve
//                                  CTRL + Left Click displays splice cost of the first dart for the TREE edge (not Shock edge)
//                                  CTRL + Right Click displays splice cost of second dart for the TREE edge
//
// \endverbatim

#include <vgui/vgui_tableau.h>
#include <vgui/vgui_parent_child_link.h>
#include <dbsk2d/dbsk2d_base_gui_geometry.h>
#include <dbsk2d/dbsk2d_shock_graph_sptr.h>


#include <dbskr/dbskr_tree_sptr.h>
#include <dbskr/dbskr_dpmatch.h>
#include <vgui/vgui_gl.h>
#include <vgui/vgui_event_condition.h>

#include <dbskr/vis/dbskr_shock_match_tableau_sptr.h>
#include <dbskr/pro/dbskr_shock_match_storage.h>

//: A tableau to display dbskr_shock_match_storage objects
class dbskr_shock_match_tableau : public vgui_tableau 
{
private:
  GLenum gl_mode;
protected:
  
  float rnd_colormap[102][3];               ///< random colormap
  float rnd_colormap2[5000][3];             ///< random colormap

  dbskr_sm_cor_sptr sm_cor_;

public:
  
  dbskr_shock_match_tableau();
  virtual ~dbskr_shock_match_tableau() {}
  
  virtual bool handle( const vgui_event & );
  
  //: set the shock matching correspondence
  void set_sm_cor(dbskr_sm_cor_sptr sm_cor); 

  //: get the shock matching correspondence
  dbskr_sm_cor_sptr get_sm_cor(void) { return sm_cor_; }

  //: write out the shock curve alignment map onto a text file
  void write_shock_alignment(vcl_vector<vcl_pair<int,int> > align, vcl_string fname);

  void draw_render();
  void draw_original_shock_graphs();
  void draw_matched_shock_graphs();
  void draw_box(vsol_box_2d_sptr box);

  void draw_boundary_curves(dbskr_scurve_sptr sk_path, int color, double off_x, double off_y);
  void draw_shock_graph(dbsk2d_shock_graph_sptr sg, vcl_map<int, int> &edge_color_map, double off_x, double off_y);
  void draw_visual_frags(dbskr_scurve_sptr sk_path, int color, double off_x, double off_y);
  void draw_intrinsic_coords(dbskr_scurve_sptr sk_path, int color, double off_x, double off_y);

  //: Generate the popup menu for this tableau
  void get_popup(const vgui_popup_params& params, vgui_menu &menu);

  void set_offset_x1(int x) { offset_x1 = x; }
  void set_offset_x2(int x) { offset_x2 = x; }
  void set_offset_y1(int y) { offset_y1 = y; }
  void set_offset_y2(int y) { offset_y2 = y; }

protected:
  //: if the distance of current selected point to this curve is less than 
  //  a threshold than this one is selected 
  //  there might be problems at the corners!!
  double selected(dbskr_scurve_sptr curve, double x, double y);

  double offset_x1, offset_y1; ///< offset of the first shock graph
  double offset_x2, offset_y2; ///< offset of the second shock graph

  bool display_corresponding_bnd_points_; ///< draw corresponding points in the same color
  bool display_boundary_curves_;   ///< display the reconstructed bnd points
  bool display_shock_samples_;     ///< display the shock samples by lines from the shock to the bnd
  bool display_orig_shock_graphs_; ///< display original shock graph OR the common shock graph
  bool display_spliced_edges;      ///< display spliced edges when drawing a matched shock graph
  bool draw_shock_nodes_;          ///< display shock nodes (this can be distracting)
  bool display_visual_frags_;      ///< display visual fragments corresponding to the currently visible shock graph
  bool draw_fragments_with_alpha_; ///< draw the fragments in a semi transparent fashion
  bool display_intrinsic_coords_;  ///< display intrinsic coordinates for the currently visible shock graph
  bool output_selected_scurve_info_; ///< write out scurve info for matching edges onto a file (for debug)
  
  int sampling_ds_; ///< sampling interval along the shock graph for the intrinsic coordinate grid
  int NR_;          ///< sampling interval along the radius for the intrinsic coordinate grid

  unsigned int select_color_;
  dbskr_scurve_sptr selected_curve1_;
  dbskr_scurve_sptr selected_curve2_;
  dbskr_sc_pair_sptr selected_curve1_pair_;
  dbskr_sc_pair_sptr selected_curve2_pair_;
  
  vgl_point_2d<double> selected_plus_pt1_;
  vgl_point_2d<double> selected_plus_pt2_;

  vgl_point_2d<double> selected_minus_pt1_;
  vgl_point_2d<double> selected_minus_pt2_;

  int selected_i;
  int selected_j;

  vgl_point_2d<double> selected_sh_pt1_;
  vgl_point_2d<double> selected_sh_pt2_;
  vgl_point_2d<double> mouse_ppt_;

  vgl_point_2d<double> selected_dart_pt_;
  vgl_point_2d<double> selected_dart_end_;
  int selected_dart_, selected_dart_mate_;
  double delete_cost_mate_, contract_cost_mate_;

  vsol_box_2d_sptr shock1_box_, shock2_box_;

  float ixx_, iyy_;

  vgui_event_condition gesture_select;
  vgui_event_condition gesture_select_next;
  vgui_event_condition gesture_select_previous;
  vgui_event_condition gesture_splice1;
  vgui_event_condition gesture_splice2;
  vgui_event_condition gesture_match_spliced_leaves;

  dbskr_dpmatch* selected_match_;

};


//: Create a smart-pointer to a dbsk2d_shock_tableau.
struct dbskr_shock_match_tableau_new : public dbskr_shock_match_tableau_sptr
{
  typedef dbskr_shock_match_tableau_sptr base;

  //: Constructor - creates a pointer to a dbskr_shock_match_tableau
  dbskr_shock_match_tableau_new() : base(new dbskr_shock_match_tableau()) { }
};

#endif //dbskr_shock_match_tableau_h_
