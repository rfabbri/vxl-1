// This is contrib/ntrinh/v2_gui/dbsksp_shock_design_tool.h
#ifndef dbsksp_shock_design_tool_h_
#define dbsksp_shock_design_tool_h_
//:
// \file
// \brief 
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date 10/1/2006
//
// \verbatim
//  Modifications
// \endverbatim

#include <bvis1/bvis1_tool.h>

#include <dbsksp/pro/dbsksp_shock_storage_sptr.h>
#include <dbsksp/vis/dbsksp_shock_tableau_sptr.h>
#include <dbsksp/vis/dbsksp_shock_tableau.h>

#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_command.h>

// ============================================================================
// dbsksp_shock_design_tool
// ============================================================================
//: A tool for designing an A3 v2_shock branch and its boundary
class dbsksp_shock_design_tool : public bvis1_tool
{
public:
  //: Constructor
  dbsksp_shock_design_tool();

  //: Destructor
  virtual ~dbsksp_shock_design_tool();
  
  //: Set the tableau to work with
  virtual bool set_tableau ( const vgui_tableau_sptr& tableau );
  
  //: Set the storage class for the active tableau
  virtual bool set_storage ( const bpro1_storage_sptr& storage);
  
  //: Return the name of this tool
  virtual vcl_string name() const;

  //: Allow the tool to add to the popup menu as a tableau would
  virtual void get_popup(const vgui_popup_params& params, vgui_menu &menu);

  // ----------------- DATA ACESS ---------------------------

  //: Get the tableau this tool is working with
  dbsksp_shock_tableau_sptr tableau() const;

  //: Get the storage associated with the active tableau
  dbsksp_shock_storage_sptr storage() const;

  //: Get and set the selected edge
  dbsksp_shock_edge_sptr selected_edge() const;
  void set_selected_edge(const dbsksp_shock_edge_sptr& e);

  //: Get and set the selected node
  dbsksp_shock_node_sptr selected_node() const;
  void set_selected_node(const dbsksp_shock_node_sptr& node);

  dbsksp_twoshapelet_sptr active_twoshapelet() const 
  {return this->active_twoshapelet_; }

  //: Get and set the operating mode
  vcl_string operating_mode() const {return this->operating_mode_; }
  void set_operating_mode(const vcl_string& mode)
  {this->operating_mode_ = mode; }
  
  //: Handle events
  virtual bool handle( const vgui_event & e, 
                       const bvis1_view_tableau_sptr& view );

  bool handle_display_info();

  // handles related to active nodes and edge
  bool handle_change_m(double dm);
  bool handle_change_length(double dl);
  bool handle_change_phi(double dphi);
  
  

  // handles related to reference point
  bool handle_change_radius(double dr);
  bool handle_change_ref_origin(double dx, double dy);
  bool handle_change_ref_dir(double rot_angle);
  bool handle_set_ref();

  // global scale change
  bool handle_change_scale(double d_log2scale);

  // change part of the graph
  bool handle_change_active_node_geom(double dx, double dy, double dpsi, double dr, double dphi);

  // modify graph structure
  bool handle_insert_node_at_active_edge_midpoint();
  bool handle_extend_branch();

  // misc handles
  bool handle_select_e0();
  bool handle_select_e1();
  bool handle_select_target(float ix, float iy);

protected:
  
  dbsksp_shock_tableau_sptr tableau_;
  dbsksp_shock_storage_sptr storage_;

  dbsksp_twoshapelet_sptr active_twoshapelet_;

  //: operating mode of the tool
  vcl_string operating_mode_;

  // gestures for active nodes and edges
  vgui_event_condition gesture_display_info_;
  
  vgui_event_condition gesture_increase_m_;
  vgui_event_condition gesture_decrease_m_;

  vgui_event_condition gesture_increase_length_;
  vgui_event_condition gesture_decrease_length_;

  vgui_event_condition gesture_increase_phi_;
  vgui_event_condition gesture_decrease_phi_;

  vgui_event_condition gesture_increase_active_node_x_;
  vgui_event_condition gesture_decrease_active_node_x_;

  vgui_event_condition gesture_increase_active_node_y_;
  vgui_event_condition gesture_decrease_active_node_y_;

  vgui_event_condition gesture_increase_active_node_psi_;
  vgui_event_condition gesture_decrease_active_node_psi_;

  vgui_event_condition gesture_increase_active_node_phi_;
  vgui_event_condition gesture_decrease_active_node_phi_;

  vgui_event_condition gesture_increase_active_node_radius_;
  vgui_event_condition gesture_decrease_active_node_radius_;

  // reference point gestures
  vgui_event_condition gesture_increase_radius_;
  vgui_event_condition gesture_decrease_radius_;

  vgui_event_condition gesture_increase_ref_x_;
  vgui_event_condition gesture_decrease_ref_x_;

  vgui_event_condition gesture_increase_ref_y_;
  vgui_event_condition gesture_decrease_ref_y_;

  vgui_event_condition gesture_increase_ref_theta_;
  vgui_event_condition gesture_decrease_ref_theta_;

  vgui_event_condition gesture_set_ref_;

  vgui_event_condition gesture_increase_scale_;
  vgui_event_condition gesture_decrease_scale_;


  vgui_event_condition gesture_select_e0_;
  vgui_event_condition gesture_select_e1_;

  vgui_event_condition gesture_select_target_;

};



// ==============================================================================
// Wrapper functions
// ==============================================================================

// -----------------------------------------------------------------------------
//: Wrapper to call the "handle_insert_node_at_active_edge_midpoint" from outside
void dbsksp_shock_design_insert_node_at_active_edge_midpoint_wrapper(const void* toolref);


// -----------------------------------------------------------------------------
//: Wrapper to call the "handle_insert_node_at_active_edge_midpoint" from outside
void dbsksp_shock_design_extend_branch_wrapper(const void* toolref);








// ============================================================================
// dbsksp_shock_design_command
// ============================================================================

//: A vgui command to set the active group
class dbsksp_shock_design_command : public vgui_command
{

public:
  dbsksp_shock_design_tool* tool_;
public:  
  dbsksp_shock_design_command(dbsksp_shock_design_tool* tool): tool_(tool) {};
  virtual ~dbsksp_shock_design_command(){}
  dbsksp_shock_design_tool* tool() {return this->tool_; }

  void execute(){}
};

// ============================================================================
// dbsksp_tool_form_shapelet_command
// ============================================================================

//: A vgui command to form one shapelet from the active edge and active node
class dbsksp_tool_form_shapelet_command : 
  public dbsksp_shock_design_command
{
public:  
  dbsksp_tool_form_shapelet_command(dbsksp_shock_design_tool* tool): 
      dbsksp_shock_design_command(tool){}
  virtual ~dbsksp_tool_form_shapelet_command(){}

  void execute();
};

// ============================================================================
// dbsksp_tool_clear_shapelet_list_command
// ============================================================================
//: A vgui command to clear the shapelet list
class dbsksp_tool_clear_shapelet_list_command : 
  public dbsksp_shock_design_command
{
public:  
  dbsksp_tool_clear_shapelet_list_command(dbsksp_shock_design_tool* tool): 
      dbsksp_shock_design_command(tool){}
  virtual ~dbsksp_tool_clear_shapelet_list_command(){}

  void execute();
};




// ============================================================================
// dbsksp_tool_insert_A_infty_branch_command
// ============================================================================
//: A vgui command to insert an A_\infty branch at the active node
// right before the active edge
class dbsksp_tool_insert_A_infty_branch_command : 
  public dbsksp_shock_design_command
{
public:  
  dbsksp_tool_insert_A_infty_branch_command(dbsksp_shock_design_tool* tool): 
      dbsksp_shock_design_command(tool){}
  virtual ~dbsksp_tool_insert_A_infty_branch_command(){}

  void execute();
};




// ============================================================================
// dbsksp_tool_delete_A_infty_branch_command
// ============================================================================
//: A vgui command to delete an A_\infty branch indicated by the active node
// active edge. The active node should have degree > = 3
// This command is typically used when the angle phi of the edge is very small
// Its boundary is just a point, making is a degenerate edge.
class dbsksp_tool_delete_A_infty_branch_command : 
  public dbsksp_shock_design_command
{
public:  
  dbsksp_tool_delete_A_infty_branch_command(dbsksp_shock_design_tool* tool): 
      dbsksp_shock_design_command(tool){}
  virtual ~dbsksp_tool_delete_A_infty_branch_command(){}

  void execute();
};





// ============================================================================
// dbsksp_tool_select_point_mode_command
// ============================================================================
//: A vgui command to enter point selection mode
class dbsksp_tool_select_point_mode_command : 
  public dbsksp_shock_design_command
{
public:  
  dbsksp_tool_select_point_mode_command(dbsksp_shock_design_tool* tool): 
      dbsksp_shock_design_command(tool){}
  virtual ~dbsksp_tool_select_point_mode_command(){}

  void execute();
};



// ============================================================================
// dbsksp_tool_squeeze_shock_edge_command
// ============================================================================
//: A vgui command to squeeze a (short) shock edge and convert it to a node and
// two new terminal edges
class dbsksp_tool_squeeze_shock_edge_command : 
  public dbsksp_shock_design_command
{
public:  
  dbsksp_tool_squeeze_shock_edge_command(dbsksp_shock_design_tool* tool): 
      dbsksp_shock_design_command(tool){}
  virtual ~dbsksp_tool_squeeze_shock_edge_command(){}

  //: Execute the command
  void execute();
};





// ============================================================================
// dbsksp_tool_remove_leaf_edge_command
// ============================================================================
//: A vgui command to remove a leaf shock edge
class dbsksp_tool_remove_leaf_edge_command : 
  public dbsksp_shock_design_command
{
public:  
  dbsksp_tool_remove_leaf_edge_command(dbsksp_shock_design_tool* tool): 
      dbsksp_shock_design_command(tool){}
  virtual ~dbsksp_tool_remove_leaf_edge_command(){}

  //: Execute the command
  void execute();
};


// ============================================================================
// dbsksp_tool_remove_internal_edge_command
// ============================================================================
//: A vgui command to remove an internal shock edge
class dbsksp_tool_remove_internal_edge_command : 
  public dbsksp_shock_design_command
{
public:  
  dbsksp_tool_remove_internal_edge_command(dbsksp_shock_design_tool* tool): 
      dbsksp_shock_design_command(tool){}
  virtual ~dbsksp_tool_remove_internal_edge_command(){}

  //: Execute the command
  void execute();
};





// ============================================================================
// dbsksp_tool_split_node_command
// ============================================================================
//: A vgui command to split a high-order node into two nodes and put an edge
// in between them
// Typically used in A_^4 transition
class dbsksp_tool_split_node_command : public dbsksp_shock_design_command
{
public:  
  dbsksp_tool_split_node_command(dbsksp_shock_design_tool* tool): 
      dbsksp_shock_design_command(tool){}
  virtual ~dbsksp_tool_split_node_command(){}

  //: Execute the command
  void execute();
};


#endif //dvis_dbsksp_shock_design_tool
