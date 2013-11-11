// This is brcv/seg/dbdet/vis/dbdet_sel_contour_explorer_tool.h
#ifndef dbdet_sel_contour_explorer_tool_h
#define dbdet_sel_contour_explorer_tool_h
//:
//\file
//\brief Tool for querying/exploring contour fragments inside SEL storage class
//\author Amir Tamrakar
//\date 09/17/07
//
// \verbatim
//  Modifications
// \endverbatim

#include <bvis1/bvis1_tool.h>

#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_style_sptr.h>

#include <dbdet/vis/dbdet_sel_tableau_sptr.h>
#include <dbdet/pro/dbdet_sel_storage.h>


//: 
class dbdet_sel_contour_explorer_tool : public bvis1_tool 
{
public:
  //: Constructor
  dbdet_sel_contour_explorer_tool();
 
  //: Destructor
  ~dbdet_sel_contour_explorer_tool() {}
  
  //: Returns the string name of this tool
  vcl_string name() const;

  //: Handle events.
  bool handle( const vgui_event & e, 
               const bvis1_view_tableau_sptr& view );

  //: Set the tableau to work with
  bool set_tableau( const vgui_tableau_sptr& tableau );

  virtual void get_popup( const vgui_popup_params& params, vgui_menu &menu );

  dbdet_edgel* get_nearest_edgel(vgl_point_2d<double> pt);

  void draw_edgel_chain(dbdet_edgel_chain* chain);
  void graph_parameters_along_chain(dbdet_edgel_chain* chain);

protected:

  dbdet_sel_tableau_sptr sel_tab_;
  dbdet_sel_storage_sptr sel_storage_;

  vgui_event_condition gesture0_;

  dbdet_edgel* cur_edgel;         ///< currently selected edgel
  dbdet_edgel_chain* cur_chain;

  //various flags
  bool plot_curvature;
  bool plot_intensity;
  bool plot_strength;
  bool plot_d2f;
  bool dist_wrt_cur;

  bool use_moving_avg; ///< use moving average to smooth contrast along the contour
  unsigned win_len; ///< length of the moving window

  bool auto_scale_val;
  double vmin; //min and max of the range of values
  double vmax;


};


#endif // dbdet_sel_contour_explorer_tool_h
