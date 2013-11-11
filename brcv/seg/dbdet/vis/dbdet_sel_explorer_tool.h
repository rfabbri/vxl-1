// This is brcv/seg/dbdet/vis/dbdet_sel_explorer_tool.h
#ifndef dbdet_sel_explorer_tool_h
#define dbdet_sel_explorer_tool_h
//:
//\file
//\brief Tool for querying/exploring an existing SEL storage object
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date Sun Apr 23 13:25:40 EDT 2006
//
// \verbatim
//  Modifications
//    Amir Tamrakar  10/17/06          Removed the auxillary tableau so that the 
//                                     tool can work with multiple frames 
//                            
// \endverbatim

#include <bvis1/bvis1_tool.h>
#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_style_sptr.h>
#include <bgui/bgui_vsol2D_tableau.h>
#include <bgui/bgui_vsol2D_tableau_sptr.h>

#include <dbdet/pro/dbdet_sel_storage.h>

class dbdet_edgel;
class dbdet_curvelet;

//: This tools differs from the 'dbdet_query_linking_tool' because it acts on a
// given sel storage class. This tool enables the user to select an edgel and
// display/print the properties of the selected edgel.
//
// \remarks The functionalities of the present tool could be implemented in the
// tableau for the sel storage class as well, but I decided to make it
// independent so we have more freedom to add features. Also, there are some
// members of this tool that are vectors, in such a way that this tool is
// extensible for working with more than a single view.
//
// \todo Option to graphically display the different groupings around the
// present edgel
//
class dbdet_sel_explorer_tool : public bvis1_tool {
public:
  //: Constructor
  dbdet_sel_explorer_tool();
 
  //: Destructor
  ~dbdet_sel_explorer_tool() {}
  
  //: Returns the string name of this tool
  vcl_string name() const;

  //: Handle events.
  bool handle( const vgui_event & e, 
               const bvis1_view_tableau_sptr& view );

  //: Set the tableau to work with
  bool set_tableau( const vgui_tableau_sptr& tableau );
  
  void activate ();
  void deactivate ();

  virtual void get_popup( const vgui_popup_params& params, vgui_menu &menu );

  dbdet_edgel* get_nearest_edgel(vgl_point_2d<double> pt);
  void draw_grouping(const dbdet_curvelet* cvlet);
  void draw_ES_ft(dbdet_curvelet* cvlet, dbdet_edgel* eA);

protected:

  vgui_event_condition gesture0_;
  vcl_vector<int> frame_v_;   //: frame number for each view
  const unsigned nviews_;
  dbdet_sel_storage_sptr sel_storage_;

  vgui_soview2D_point *p0_; //:< edgel position in left curve segment
  vgui_style_sptr p0_style_;

  dbdet_edgel* cur_edgel; ///< currently selected edgel
  bool display_ES_; //: display ES fit along with the links
  unsigned min_order_to_display_; //minimum length of grouping to display
  bool display_immediate_links_only_; 

  vcl_vector<bgui_vsol2D_tableau_sptr> tab_; //:< tableaux used to draw in each view 

  void print_sel_stats() const;
  void print_complete_edgel_stats(dbdet_edgel * e) const;
  void print_edgel_stats(dbdet_edgel * e) const;
  void print_curvelets_around_edgel(dbdet_edgel *e) const;
  bool differential_geometry_at(dbdet_edgel *e1, vcl_vector<double> &k, vcl_vector<double> &kdot) const;
};


#endif // dbdet_sel_explorer_tool_h
