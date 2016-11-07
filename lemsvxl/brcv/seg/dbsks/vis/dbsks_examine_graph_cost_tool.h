// This is contrib/ntrinh/dbsks/vis/dbsks_examine_graph_cost_tool.h
#ifndef dbsks_examine_graph_cost_tool_h_
#define dbsks_examine_graph_cost_tool_h_

//:
// \file
// \brief 
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Dec 18, 2007
//
// \verbatim
//  Modifications
// \endverbatim

#include <bvis1/bvis1_tool.h>

#include <dbsksp/pro/dbsksp_shock_storage_sptr.h>
#include <dbsksp/pro/dbsksp_shock_storage.h>
#include <dbsksp/vis/dbsksp_shock_tableau_sptr.h>
#include <dbsksp/vis/dbsksp_shock_tableau.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <dbsks/pro/dbsks_shapematch_storage_sptr.h>
#include <dbsks/dbsks_dp_match_sptr.h>

#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_command.h>

// ============================================================================
// dbsks_examine_graph_cost_tool
// ============================================================================
//: 
class dbsks_examine_graph_cost_tool : public bvis1_tool
{
public:
  //: Constructor
  dbsks_examine_graph_cost_tool();

  //: Destructor
  virtual ~dbsks_examine_graph_cost_tool();
  
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
  dbsksp_shock_storage_sptr shock_storage();

  //: Return storage of the image to segment
  vidpro1_image_storage_sptr image_storage() const 
  { return this->image_storage_; }

  //: Set the image storage
  bool set_image_storage (const bpro1_storage_sptr& image_storage);

  //: Return shapematch storage
  dbsks_shapematch_storage_sptr shapematch_storage() const
  { return this->shapematch_storage_; }

  //: Set shapematch storage
  bool set_shapematch_storage(const bpro1_storage_sptr& shapematch_storage);

  //: Retrieve the DP shape matcher from shapematch storage
  dbsks_dp_match_sptr dp_engine() const;
  
  //: Handle events
  virtual bool handle( const vgui_event & e, 
                       const bvis1_view_tableau_sptr& view );


  bool handle_change_ref_origin(double dx, double dy);
  bool handle_change_ref_dir(double rot_angle);


  //: Display cost of the clicked component (boundary arc or fragment)
  bool handle_display_fragment_cost();

  //: Display the total cost of a graph
  bool handle_display_graph_cost();
  
  
  //: Check if necessary variables are set
  bool check_necessary_vars_available() const;

protected:
  
  dbsksp_shock_tableau_sptr tableau_;
  dbsksp_shock_storage_sptr shock_storage_;

  // storage of image to segment
  vidpro1_image_storage_sptr image_storage_;

  // storage of DP shape matchder
  dbsks_shapematch_storage_sptr shapematch_storage_;

  // Gesture lists
  vgui_event_condition gesture_display_fragment_cost_;
  vgui_event_condition gesture_display_graph_cost_;

  vgui_event_condition gesture_increase_ref_x_;
  vgui_event_condition gesture_decrease_ref_x_;

  vgui_event_condition gesture_increase_ref_y_;
  vgui_event_condition gesture_decrease_ref_y_;

  vgui_event_condition gesture_increase_ref_psi_;
  vgui_event_condition gesture_decrease_ref_psi_;

};



// ============================================================================
// dbsks_egc_command
// ============================================================================

//: Abstract class for commands used with this tool
class dbsks_egc_command : public vgui_command
{           


public:  
  dbsks_egc_command(dbsks_examine_graph_cost_tool* tool): 
      tool_(tool) {};
  virtual ~dbsks_egc_command(){}
  dbsks_examine_graph_cost_tool* tool() {return this->tool_; }

  //virtual void execute() = 0;
protected:
  dbsks_examine_graph_cost_tool* tool_;
};


// ============================================================================
// dbsks_egc_choose_image_and_shapematch_command
// ============================================================================

//: A vgui command to an image for segmentation
class dbsks_egc_choose_image_and_shapematch_command : public dbsks_egc_command
{
public:  
  dbsks_egc_choose_image_and_shapematch_command(
    dbsks_examine_graph_cost_tool* tool): dbsks_egc_command(tool){}
  virtual ~dbsks_egc_choose_image_and_shapematch_command(){}

  void execute();
};


#endif //dbsks_examine_graph_cost_tool



