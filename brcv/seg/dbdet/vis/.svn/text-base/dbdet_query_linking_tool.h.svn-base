// This is brcv/seg/dbdet/vis/dbdet_query_linking_tool.h
#ifndef dbdet_query_linking_tool_h_
#define dbdet_query_linking_tool_h_
//:
// \file
// \brief 
// author Amir Tamrakar
// date 03/24/06
//
// \verbatim
//  Modifications
// \endverbatim


#include <bvis1/bvis1_tool.h>
#include <bvis1/tool/bvis1_vsol2D_tools.h>

#include <vgui/vgui.h>
#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_projection_inspector.h>

#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>

#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

#include <bgui/bgui_vsol2D_tableau_sptr.h>
#include <bgui/bgui_vsol2D_tableau.h>

#include <dbdet/algo/dbdet_sel.h>
#include <dbdet/sel/dbdet_edgel.h>


//:  Acts on a vsol tableau containing edgels (e.g. output from non-max
// suppression process). It allows the user to select edgels and explore the
// linking between them. The linking is done inside this class;  this tool does
// not act on 'sel' storage class.
//
class  dbdet_query_linking_tool: public bvis1_vsol2D_tool 
{
public:
  //: Constructor
  dbdet_query_linking_tool();

  //: Destructor
  virtual ~dbdet_query_linking_tool();

  //: Returns the string name of this tool
  vcl_string name() const;

  void deactivate();

  //: handle events
  bool handle( const vgui_event & e, const bvis1_view_tableau_sptr& selector );

  virtual void get_popup( const vgui_popup_params& params, vgui_menu &menu );

  //: form an edgel grouping from the current selection of the edges
  void form_edgel_grouping();

  //: vsol_line to edgel
  dbdet_edgel form_edgel_from_line(vsol_line_2d_sptr line);

protected:
  vgui_event_condition gesture_select_;
  vgui_event_condition gesture_select_ref_;
  vgui_event_condition gesture_new_grouping_;
  vgui_event_condition gesture_zoom_in_;
  vgui_event_condition gesture_zoom_out_;

  vgui_event_condition gesture_pos_1, gesture_pos_2, gesture_pos_3, gesture_pos_4, gesture_pos_5, gesture_pos_6, gesture_pos_7, gesture_pos_8, gesture_pos_9;

private:
  bgui_vsol_soview2D * object_;

  vcl_vector<vsol_line_2d_sptr> edges_; //edges in the grouping (in order)
  vcl_vector<bgui_vsol_soview2D_line_seg*> edge_so_; //corresponding line soviews 

  unsigned ref_edge_ind_; //index of the reference edgel

  bool ref_selected_; //reference edge selected
  int edge_per_; //index of perturbation of the edge
  bool pairs_valid_; //are the individual pairs valid?
  bool grouping_valid_; //is current grouping valid

  bool fit_spline_; //fit a spline through grouping

  vcl_vector<vgl_polygon<double> > polys;
  vgl_polygon<double> p_int;

  vgl_point_2d<double> sel_pt_;
  double sel_tan_;

  int zoom_factor_; //zoom the plot by this factor

  double dtheta_, dpos_; //assumed accuracy of the edge detector

};

#endif // dbdet_query_linking_tool_h_
