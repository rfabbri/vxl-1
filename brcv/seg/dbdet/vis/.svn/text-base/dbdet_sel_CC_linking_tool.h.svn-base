// This is brcv/seg/dbdet/vis/dbdet_sel_CC_linking_tool.h
#ifndef dbdet_sel_CC_linking_tool_h
#define dbdet_sel_CC_linking_tool_h
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

#include <dbdet/algo/dbdet_sel_sptr.h>
#include <dbdet/sel/dbdet_EHT.h>

//: 
class dbdet_sel_CC_linking_tool : public bvis1_tool 
{
public:
  //: Constructor
  dbdet_sel_CC_linking_tool();
 
  //: Destructor
  ~dbdet_sel_CC_linking_tool() { clear_contour(); }
  
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
  
  void clear_contour() { HTF.delete_tree(); HTR.delete_tree();}

  // -------------------------------------------------------------------------
  // Local hyp tree functions
  // -------------------------------------------------------------------------

  void construct_hyp_tree(dbdet_edgel* e);

  void resolve_contour();

  //drawing routines
  void draw_edgel_chain(vcl_vector<dbdet_edgel*>& edgel_chain, float R=1.0, float G=0.0, float B=0.0);
  void draw_arc_spline(vcl_vector<dbdet_edgel*>& edgel_chain,  float R=1.0, float G=0.0, float B=0.0);
  
  void draw_hyp_tree();
  void draw_link(dbdet_EHT_node* cur_node);

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

  //various flags and paramters
  bool draw_chain;
  bool draw_fits;

  dbdet_EHT HTF;
  dbdet_EHT HTR;

  double tolerance;
  double smooth_thresh;

  int depth; 

};


#endif // dbdet_sel_CC_linking_tool_h
