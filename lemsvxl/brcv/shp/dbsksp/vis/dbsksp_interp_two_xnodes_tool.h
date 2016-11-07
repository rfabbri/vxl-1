// This is shp/dbsksp/vis/dbsksp_interp_two_xnodes_tool.h
#ifndef dbsksp_interp_two_xnodes_tool_h_
#define dbsksp_interp_two_xnodes_tool_h_
//:
// \file
// \brief Tool to visualize interpolation between two xnodes
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Mar 4, 2007
//
// \verbatim
//  Modifications
// \endverbatim


#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_command.h>
#include <bvis1/bvis1_tool.h>
#include <dbsksp/pro/dbsksp_shock_storage.h>
#include <dbsksp/pro/dbsksp_shock_storage_sptr.h>
#include <dbsksp/vis/dbsksp_shock_tableau_sptr.h>
#include <dbsksp/vis/dbsksp_shock_tableau.h>
#include <dbgl/algo/dbgl_biarc.h>
#include <dbsksp/dbsksp_xshock_node.h>







// ============================================================================
// dbsksp_interp_two_xnodes_tool
// ============================================================================

class dbsksp_interp_two_xnodes_tool : public bvis1_tool
{
public:
  //: Constructor
  dbsksp_interp_two_xnodes_tool();

  //: Destructor
  virtual ~dbsksp_interp_two_xnodes_tool();
  
  //: Set the tableau to work with
  virtual bool set_tableau ( const vgui_tableau_sptr& tableau );
  
  //: Set the storage class for the active tableau
  virtual bool set_storage ( const bpro1_storage_sptr& storage);
  
  //: Return the name of this tool
  virtual vcl_string name() const {return "Interpolate xnodes"; }

  //: Allow the tool to add to the popup menu as a tableau would
  virtual void get_popup(const vgui_popup_params& params, vgui_menu &menu);

  // ----------------- DATA ACESS ---------------------------

  //: Get the tableau this tool is working with
  dbsksp_shock_tableau_sptr tableau();

  //: Get the storage associated with the active tableau
  dbsksp_shock_storage_sptr storage();

  // the two extrinsic nodes
  dbsksp_xshock_node_descriptor xnode_start();
  dbsksp_xshock_node_descriptor xnode_end();

  // fixed param
  double param_t();

  dbsksp_twoshapelet_sptr active_twoshapelet()
  { return this->active_twoshapelet_; }
  void set_active_twoshapelet(const dbsksp_twoshapelet_sptr& ss)
  { this->active_twoshapelet_ = ss; }

  // --------------------------------------------------------------------------
  //: Handle events
  virtual bool handle( const vgui_event & e, 
                       const bvis1_view_tableau_sptr& view );

  bool handle_display_info();
  bool handle_change_param_value(double increment);
  bool handle_draw_overlay();

  void draw_extrinsic_node(const dbsksp_xshock_node_descriptor& node);
  
protected:
  
  dbsksp_shock_tableau_sptr tableau_;
  dbsksp_shock_storage_sptr storage_;
  

  //: function gestures of the tool
  vgui_event_condition gesture_display_info_;

  // change the value of 
  vgui_event_condition gesture_increase_param_value_;
  vgui_event_condition gesture_decrease_param_value_;

  // change the value of increment (used in increase_param_value)
  vgui_event_condition gesture_increase_increment_value_;
  vgui_event_condition gesture_decrease_increment_value_;

  

  // cache objects for drawing
  dbsksp_twoshapelet_sptr active_twoshapelet_;
  // estimation of boundary and shocks
  dbgl_biarc shock_estimate_;
  dbgl_biarc left_bnd_estimate_;
  dbgl_biarc right_bnd_estimate_;

  /////////////////////////////////////////////////////
  // demo_type specific parameters

  // construct a two shapelet from two end extrinsic nodes and a phi
  vcl_map<vcl_string, double > param_list_;
  vcl_string active_param_name_;
  double increment_;

  // interpolation modes
  vcl_vector<vcl_string > interp_mode_list_;
  vcl_string active_interp_mode_;

  
};



// ============================================================================
// dbsksp_shock_design_command
// ============================================================================

//: A vgui command to set the active group
template< class T >
class dbsksp_set_param_command : public vgui_command
{
public:  
  dbsksp_set_param_command(T* param, const T& param_value ): 
      param_(param), param_value_(param_value) {};
  virtual ~dbsksp_set_param_command(){}

  void execute(){ *param_ = param_value_; }

  T* param_;
  T param_value_;
};




#endif //dvis_dbsksp_interp_two_xnodes_tool
