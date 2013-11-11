// This is brcv/seg/dbdet/vis/dbdet_sel_curvelet_linking_tool.h
#ifndef dbdet_sel_curvelet_linking_tool_h
#define dbdet_sel_curvelet_linking_tool_h
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

class dbdet_curvelet_compatibility
{
public:
  int Corder; ///< {C0, C1, C2}
  double area; ///< area of the curve bundle after constraint
  double dk;   ///< degree of change in the model 
  int trans_k; ///< transition model choice
  dbdet_curve_model* model; ///< constrained curve bundle
  double cost; ///< compatibility heaurisic cost

  dbdet_curvelet_compatibility(): Corder(-1), area(0.0), dk(0.0), trans_k(-1), model(0), cost(1000.0) {}
  dbdet_curvelet_compatibility(int order_, double area_, double dk_, int trans_k_, dbdet_curve_model* model_, double cost_): 
    Corder(order_), area(area_), dk(dk_), trans_k(trans_k_), model(model_), cost(cost_) {}

  ~dbdet_curvelet_compatibility(){}

  bool is_legal(){ return Corder>=0; } 
};

//: 
class dbdet_sel_curvelet_linking_tool : public bvis1_tool 
{
public:
  //: Constructor
  dbdet_sel_curvelet_linking_tool();
 
  //: Destructor
  ~dbdet_sel_curvelet_linking_tool() {clear_contour();}
  
  //: Returns the string name of this tool
  vcl_string name() const;

  //: Handle events.
  bool handle( const vgui_event & e, 
               const bvis1_view_tableau_sptr& view );

  //: Set the tableau to work with
  bool set_tableau( const vgui_tableau_sptr& tableau );

  virtual void get_popup( const vgui_popup_params& params, vgui_menu &menu );

  // -------------------------------------------------------------------------
  // Support routines
  // -------------------------------------------------------------------------

  dbdet_curvelet* best_curvelet(dbdet_edgel* e);
  dbdet_curvelet* random_curvelet(dbdet_edgel* e);
  dbdet_curvelet* next_best_curvelet();
  void print_cvlet_info(dbdet_curvelet* cvlet, double cost);

  //compute curvelets at the given edgel if there are none
  void compute_local_curvelets(dbdet_edgel* e);

  // -------------------------------------------------------------------------
  //linking routines
  // -------------------------------------------------------------------------
  
  void clear_contour();

  void add_cvlet(dbdet_curvelet* cvlet);
  bool add_next_curvelet(dbdet_edgel* e=0);
  
  //bool add_next_curvelet(dbdet_edgel* e=0);
  //void add_cvlet(dbdet_curvelet* cvlet, const dbdet_curvelet_compatibility comp=dbdet_curvelet_compatibility());
  //dbdet_curvelet_compatibility compute_compatibility(dbdet_curvelet* cvlet1, dbdet_curvelet* cvlet2);

  //: backtrack from the final curvelet all the way to define a single parametric curve
  void back_track_through_bundles(vgl_point_2d<double> sol);

  void construct_hyp_tree(dbdet_curvelet* cvlet);

  //------------------------------------------------------------------------------------
  //drawing routines
  //------------------------------------------------------------------------------------
  
  void draw_CC_segment(vgl_point_2d<double> pt, double theta, double k, double L, bool col_flag, bool forward=true, double R=1.0, double G=0.0, double B=0.0);
  void draw_CC_bundle(dbdet_CC_curve_model_new* cm, double length, bool col_flag, bool forward=true);
  void draw_edgel_chain(dbdet_edgel_chain* chain);
  void draw_arc_spline();
  void draw_local_hyp_tree(dbdet_hyp_tree* HT);
  void draw_HT();

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

  //the growing contour data structure
  dbdet_hyp_tree HT;

  //various flags and paramters
  double quality_threshold;
  bool enforce_global_consistency;
  bool allow_C0;

  bool draw_bundle;
  bool draw_chain;

};


#endif // dbdet_sel_curvelet_linking_tool_h
