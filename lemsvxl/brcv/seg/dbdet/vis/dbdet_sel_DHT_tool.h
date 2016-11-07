// This is brcv/seg/dbdet/vis/dbdet_sel_DHT_tool.h
#ifndef dbdet_sel_DHT_tool_h
#define dbdet_sel_DHT_tool_h
//:
//\file
//\brief Tool for exploring Edge linking using curvelets
//\author Amir Tamrakar
//\date 01/27/08
//
// \verbatim
//  Modifications
// \endverbatim

#include <bvis1/bvis1_tool.h>

#include <vcl_set.h>
#include <vcl_queue.h>
#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_style_sptr.h>

#include <dbdet/vis/dbdet_sel_tableau_sptr.h>
#include <dbdet/pro/dbdet_sel_storage.h>
#include <dbdet/sel/dbdet_hyp_tree_graph.h>

#include <dbdet/algo/dbdet_sel_sptr.h>

//: A tool to visualize the execution of the DHT algo
class dbdet_sel_DHT_tool : public bvis1_tool 
{
public:
  //: Constructor
  dbdet_sel_DHT_tool();
 
  //: Destructor
  ~dbdet_sel_DHT_tool() {}
  
  //: Returns the string name of this tool
  vcl_string name() const;

  //: Handle events.
  bool handle( const vgui_event & e, 
               const bvis1_view_tableau_sptr& view );

  //: Set the tableau to work with
  bool set_tableau( const vgui_tableau_sptr& tableau );

  virtual void get_popup( const vgui_popup_params& params, vgui_menu &menu );

  // -------------------------------------------------------------------------
  //linking routines
  // -------------------------------------------------------------------------

  void construct_hyp_tree(dbdet_curvelet* cvlet);

  // -------------------------------------------------------------------------
  // Support routines
  // -------------------------------------------------------------------------

  void print_cvlet_info(dbdet_curvelet* cvlet, double cost);

  //------------------------------------------------------------------------------------
  //drawing routines
  //------------------------------------------------------------------------------------
  
  void draw_CC_segment(vgl_point_2d<double> pt, double theta, double k, double L, bool col_flag, bool forward=true, double R=1.0, double G=0.0, double B=0.0);
  void draw_CC_bundle(dbdet_CC_curve_model_new* cm, double length, bool col_flag, bool forward=true);
  void draw_edgel_chain(dbdet_edgel_chain* chain);
  void draw_arc_spline();
  void draw_local_hyp_tree(dbdet_hyp_tree* HT);
  void draw_HT();
  
  void draw_all_hyp_trees();
  void draw_hyp_tree(dbdet_hyp_tree* HT, double R=1.0, double G=0.0, double B=0.0);

  void draw_HTG();

public:

  //various data structures
  dbdet_sel_tableau_sptr sel_tab_;
  dbdet_sel_storage_sptr sel_storage_;

  dbdet_edgemap_sptr EM;
  dbdet_curvelet_map* CM;
  dbdet_edgel_link_graph* ELG;
  dbdet_curve_fragment_graph* CFG;
  dbdet_sel_sptr edge_linker;

  //various parameters
  double rad, dt, dx, maxN, token_len;

  vgui_event_condition gesture0_, gesture1_;
  vgui_event_condition con_start, con_next, con_clear, con_resolve;

  //various data structures and algorithms needed for this tool
  dbdet_edgel* cur_edgel;         ///< currently selected edgel
  dbdet_curvelet* cur_cvlet;

  //various flags and paramters
  double quality_threshold;
  
  bool hide_HTG; 

};


#endif // dbdet_sel_DHT_tool_h
