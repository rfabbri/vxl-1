// This is dbsksp/vis/dbsksp_xshock_design_tool.h
#ifndef dbsksp_xshock_design_tool_h_
#define dbsksp_xshock_design_tool_h_
//:
// \file
// \brief 
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date August 18, 2008
//
// \verbatim
//  Modifications
// \endverbatim

#include <bvis1/bvis1_tool.h>

#include <dbsksp/pro/dbsksp_xgraph_storage.h>
#include <dbsksp/pro/dbsksp_xgraph_storage_sptr.h>
#include <dbsksp/vis/dbsksp_xgraph_tableau_sptr.h>
#include <dbsksp/vis/dbsksp_xgraph_tableau.h>

#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_command.h>

// ============================================================================
// dbsksp_xshock_design_tool
// ============================================================================
//: A tool for designing a generative extrinsic shock graph
class dbsksp_xshock_design_tool : public bvis1_tool
{
public:
  //: Constructor
  dbsksp_xshock_design_tool();

  //: Destructor
  virtual ~dbsksp_xshock_design_tool();
  
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
  dbsksp_xgraph_tableau_sptr tableau(){ return this->tableau_; }

  //: Get the storage associated with the active tableau
  dbsksp_xgraph_storage_sptr storage(){ return this->storage_; }

  //: Get selected edge
  dbsksp_xshock_edge_sptr selected_xedge() const { return this->selected_xedge_ ; }

  //: Set selected edge 
  void set_selected_xedge(const dbsksp_xshock_edge_sptr& xe);

  //: Get selected node
  dbsksp_xshock_node_sptr selected_xnode() const { return this->selected_xnode_; }

  //: Set selected node
  void set_selected_xnode(const dbsksp_xshock_node_sptr& xv);

  
  // EVENTS -------------------------------------------------------------------

  //: Handle events
  virtual bool handle( const vgui_event & e, 
                       const bvis1_view_tableau_sptr& view );

  //: Handle displaying info of selected objects
  bool handle_display_info();

  //: Handle translating an extrinsic node
  bool handle_translate_xnode(float dx, float dy);

  //: Handle rotating shock tangents at a node
  bool handle_rotate_xnode(float dtheta);

  //: Handle rotating individual shock tangent
  bool handle_rotate_xtangent(float dtheta);

  //: Handle changing radius at a node
  bool handle_change_radius(double dr);

  //: Handle changing phi of one edge at a node (forcing the others to change too)
  bool handle_change_phi(double dphi);
  
  //: apply similarity transform to the graph
  bool handle_similarity_transform(double dx, double dy, double d_angle, double d_log2scale);

  // UTILITIES -----------------------------------------------------------------

  //: Update display of the xgraph
  void update_xgraph_display();
  

protected:
  
  //: tableau associated with this tool
  dbsksp_xgraph_tableau_sptr tableau_;

  //: shock storage associated with this tool
  dbsksp_xgraph_storage_sptr storage_;

  //: active extrinsic edge this tool is working with
  dbsksp_xshock_edge_sptr selected_xedge_;

  //: active extrinsic node this tool is working with
  dbsksp_xshock_node_sptr selected_xnode_;

  //: function gestures of the tool
  vgui_event_condition gesture_display_info_;

  // change radius at a node
  vgui_event_condition gesture_increase_radius_;
  vgui_event_condition gesture_decrease_radius_;

  // change phi at a node
  vgui_event_condition gesture_increase_phi_;
  vgui_event_condition gesture_decrease_phi_;

  // translation gestures
  vgui_event_condition gesture_increase_node_x_;
  vgui_event_condition gesture_decrease_node_x_;

  vgui_event_condition gesture_increase_node_y_;
  vgui_event_condition gesture_decrease_node_y_;

  // rotion gestures
  // rotate all tangent around one node
  vgui_event_condition gesture_rotate_xnode_cw_;
  vgui_event_condition gesture_rotate_xnode_ccw_;

  // rotate tangent of one edge at one node
  vgui_event_condition gesture_rotate_xtangent_cw_;
  vgui_event_condition gesture_rotate_xtangent_ccw_;

  // change the size (scale) of the shape
  vgui_event_condition gesture_scale_up_;
  vgui_event_condition gesture_scale_down_;

  // rotate the whole shape around the selected node
  vgui_event_condition gesture_rotate_graph_cw_;
  vgui_event_condition gesture_rotate_graph_ccw_;

  // translate the whole shape along x-axis
  vgui_event_condition gesture_increase_graph_x_;
  vgui_event_condition gesture_decrease_graph_x_;

  // translate the whole shape along y-axis
  vgui_event_condition gesture_increase_graph_y_;
  vgui_event_condition gesture_decrease_graph_y_;
};

// ============================================================================
// dbsksp_xshock_design_command
// ============================================================================

//: A vgui command to set the active group
class dbsksp_xshock_design_command : public vgui_command
{

public:
  dbsksp_xshock_design_tool* tool_;
public:  
  dbsksp_xshock_design_command(dbsksp_xshock_design_tool* tool): tool_(tool) {};
  virtual ~dbsksp_xshock_design_command(){}
  dbsksp_xshock_design_tool* tool() {return this->tool_; }

  void execute() = 0;
};


// ============================================================================
// dbsksp_xshock_split_an_edge_command
// ============================================================================

//: A vgui command to extend an A_\infty branch at the active node
class dbsksp_xshock_split_an_edge_command : 
  public dbsksp_xshock_design_command
{
public:  
  dbsksp_xshock_split_an_edge_command(dbsksp_xshock_design_tool* tool): 
      dbsksp_xshock_design_command(tool){}
  virtual ~dbsksp_xshock_split_an_edge_command(){}

  void execute();
};



//=============================================================================
// dbsksp_xshock_delete_an_A12_node_command
//=============================================================================

//: A vgui command to dextend an A_\infty branch at the active node
class dbsksp_xshock_delete_an_A12_node_command : 
  public dbsksp_xshock_design_command
{
public:  
  dbsksp_xshock_delete_an_A12_node_command(dbsksp_xshock_design_tool* tool): 
      dbsksp_xshock_design_command(tool){}
  virtual ~dbsksp_xshock_delete_an_A12_node_command(){}

  void execute();
};





// ============================================================================
// dbsksp_xshock_extend_A_infty_edge_command
// ============================================================================

//: A vgui command to extend an A_\infty branch at the active node
class dbsksp_xshock_extend_A_infty_edge_command : 
  public dbsksp_xshock_design_command
{
public:  
  dbsksp_xshock_extend_A_infty_edge_command(dbsksp_xshock_design_tool* tool): 
      dbsksp_xshock_design_command(tool){}
  virtual ~dbsksp_xshock_extend_A_infty_edge_command(){}

  void execute();
};



// ============================================================================
// dbsksp_xshock_insert_A_infty_edge_command
// ============================================================================
//: A vgui command to insert an A_\infty branch at the active node right before 
// the active edge
class dbsksp_xshock_insert_A_infty_edge_command : 
  public dbsksp_xshock_design_command
{
public:  
  dbsksp_xshock_insert_A_infty_edge_command(dbsksp_xshock_design_tool* tool): 
      dbsksp_xshock_design_command(tool){}
  virtual ~dbsksp_xshock_insert_A_infty_edge_command(){}

  void execute();
};




//=============================================================================
// dbsksp_xshock_delete_A_infty_edge_command
//=============================================================================
//: A vgui command to delete an A_\infty branch indicated by the active edge
class dbsksp_xshock_delete_A_infty_edge_command : 
  public dbsksp_xshock_design_command
{
public:  
  dbsksp_xshock_delete_A_infty_edge_command(dbsksp_xshock_design_tool* tool): 
      dbsksp_xshock_design_command(tool){}
  virtual ~dbsksp_xshock_delete_A_infty_edge_command(){}

  void execute();
};


//=============================================================================
// dbsksp_xshock_compute_xsamples_command
//=============================================================================
//: A vgui command to compute xsamples for an xgraph
class dbsksp_xshock_compute_xsamples_command : 
  public dbsksp_xshock_design_command
{
public:  
  dbsksp_xshock_compute_xsamples_command(dbsksp_xshock_design_tool* tool): 
      dbsksp_xshock_design_command(tool){}
  virtual ~dbsksp_xshock_compute_xsamples_command(){}

  void execute();
};


//=============================================================================
// dbsksp_xshock_clear_xsamples_command
//=============================================================================
//: A vgui command to clear xsamples for an xgraph
class dbsksp_xshock_clear_xsamples_command : 
  public dbsksp_xshock_design_command
{
public:  
  dbsksp_xshock_clear_xsamples_command(dbsksp_xshock_design_tool* tool): 
      dbsksp_xshock_design_command(tool){}
  virtual ~dbsksp_xshock_clear_xsamples_command(){}

  void execute();
};


#endif //dvis_dbsksp_xshock_design_tool
