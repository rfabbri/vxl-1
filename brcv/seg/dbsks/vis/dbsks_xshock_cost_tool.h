// This is dbsks/vis/dbsks_xshock_cost_tool.h
#ifndef dbsks_xshock_cost_tool_h_
#define dbsks_xshock_cost_tool_h_

//:
// \file
// \brief 
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Sep 24, 2008
//
// \verbatim
//  Modifications
// \endverbatim

#include <bvis1/bvis1_tool.h>

//#include <dbsksp/pro/dbsksp_shock_storage_sptr.h>
#include <dbsksp/pro/dbsksp_shock_storage.h>
#include <dbsksp/vis/dbsksp_shock_tableau_sptr.h>
#include <dbsksp/vis/dbsksp_shock_tableau.h>
//#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
//#include <dbsks/pro/dbsks_shapematch_storage_sptr.h>
//#include <dbsks/dbsks_dp_match_sptr.h>
//
#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_command.h>

// ============================================================================
// dbsks_xshock_cost_tool
// ============================================================================
//: 
class dbsks_xshock_cost_tool : public bvis1_tool
{
public:
  //: Constructor
  dbsks_xshock_cost_tool();

  //: Destructor
  virtual ~dbsks_xshock_cost_tool();
  
  //: Set the tableau to work with
  virtual bool set_tableau ( const vgui_tableau_sptr& tableau );
  
  //: Set the storage class for the active tableau
  virtual bool set_storage ( const bpro1_storage_sptr& storage);
  
  //: Return the name of this tool
  virtual vcl_string name() const;

  //: Allow the tool to add to the popup menu as a tableau would
  virtual void get_popup(const vgui_popup_params& params, vgui_menu &menu);

  // DATA ACESS ---------------------------------------------------------------

  //: Get the tableau this tool is working with
  dbsksp_shock_tableau_sptr tableau();

  //: Get the storage associated with the active tableau
  dbsksp_shock_storage_sptr shock_storage() const;

  //: Get selected edge
  dbsksp_xshock_edge_sptr selected_xedge() const;

  //: Set selected edge 
  void set_selected_xedge(const dbsksp_xshock_edge_sptr& xe);

  //: Get selected node
  dbsksp_xshock_node_sptr selected_xnode() const;

  //: Set selected node
  void set_selected_xnode(const dbsksp_xshock_node_sptr& xv);



  ////: Return storage of the image to segment
  //vidpro1_image_storage_sptr image_storage() const 
  //{ return this->image_storage_; }

  ////: Set the image storage
  //bool set_image_storage (const bpro1_storage_sptr& image_storage);

  ////: Return shapematch storage
  //dbsks_shapematch_storage_sptr shapematch_storage() const
  //{ return this->shapematch_storage_; }

  ////: Set shapematch storage
  //bool set_shapematch_storage(const bpro1_storage_sptr& shapematch_storage);

  ////: Retrieve the DP shape matcher from shapematch storage
  //dbsks_dp_match_sptr dp_engine() const;
  
  //: Handle events
  virtual bool handle( const vgui_event & e, 
                       const bvis1_view_tableau_sptr& view );

  //: Handle displaying info of selected objects
  bool handle_display_info();



protected:  
  dbsksp_shock_tableau_sptr tableau_;
  dbsksp_shock_storage_sptr shock_storage_;

  //// storage of image to segment
  //vidpro1_image_storage_sptr image_storage_;

  //// storage of DP shape matchder
  //dbsks_shapematch_storage_sptr shapematch_storage_;

  // Gesture lists
  vgui_event_condition gesture_display_info_;
};



// ============================================================================
// dbsks_xshock_cost_command
// ============================================================================

//: Abstract class for commands used with this tool
class dbsks_xshock_cost_command : public vgui_command
{           
public:  
  dbsks_xshock_cost_command(dbsks_xshock_cost_tool* tool): 
      tool_(tool) {};
  virtual ~dbsks_xshock_cost_command(){}
  dbsks_xshock_cost_tool* tool() {return this->tool_; }
  virtual void execute() = 0;
protected:
  dbsks_xshock_cost_tool* tool_;
};


// ============================================================================
// dbsks_xshock_draw_arc_patch_command
// ============================================================================

//: A vgui command to an image for segmentation
class dbsks_xshock_draw_arc_patch_command : public dbsks_xshock_cost_command
{
public:  
  dbsks_xshock_draw_arc_patch_command(
    dbsks_xshock_cost_tool* tool): dbsks_xshock_cost_command(tool){}
  virtual ~dbsks_xshock_draw_arc_patch_command(){}

  void execute();
};


#endif //dbsks_xshock_cost_tool



