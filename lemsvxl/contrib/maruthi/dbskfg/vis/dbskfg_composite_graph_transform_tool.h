// This is dbsksp/vis/dbskfg_composite_graph_transform_tool.h
#ifndef dbskfg_composite_graph_transform_tool_h_
#define dbskfg_composite_graph_transform_tool_h_
//:
// \file
// \brief 
// \author Maruthi Narayanan (mn@lems.brown.edu)
// \date July 27th, 2010
//
// \verbatim
//  Modifications
// \endverbatim

#include <dbskfg/pro/dbskfg_composite_graph_storage.h>
#include <dbskfg/pro/dbskfg_composite_graph_storage_sptr.h>
#include <dbskfg/vis/dbskfg_composite_graph_tableau.h>
#include <dbskfg/vis/dbskfg_composite_graph_tableau_sptr.h>

#include <bvis1/bvis1_tool.h>

#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_command.h>
#include <bxml/bxml_write.h>

// ============================================================================
// dbskfg_composite_graph_transform_tool
// ============================================================================
//: A tool for interactively applying transforms to composite graph
class dbskfg_composite_graph_transform_tool : public bvis1_tool
{
public:

  //: Constructor
  dbskfg_composite_graph_transform_tool();

  //: Destructor
  /* virtual */ ~dbskfg_composite_graph_transform_tool();
  
  //: Set the tableau to work with
  /* virtual */ bool set_tableau ( const vgui_tableau_sptr& tableau );
  
  //: Set the storage class for the active tableau
  bool set_storage ( const bpro1_storage_sptr& storage);
  
  //: Return the name of this tool
  /* virtual */ vcl_string name() const;
  
  //: Allow the tool to add to the popup menu as a tableau would
  /* virtual */ void get_popup(
      const vgui_popup_params& params, vgui_menu &menu);
  
  // EVENTS -------------------------------------------------------------------

  //: Handle events
  /* virtual */ bool handle( const vgui_event & e, 
                             const bvis1_view_tableau_sptr& view );

  //: Handle displaying info of selected objects
  bool handle_apply_transform();

  // UTILITIES -----------------------------------------------------------------

  //: Update display of the xgraph
  void update_composite_graph_display();
  

private:
  
  //: tableau associated with this tool
  dbskfg_composite_graph_tableau_sptr tableau_;

  //: composite graph storage associated with this tool
  dbskfg_composite_graph_storage_sptr storage_;
  
  //: vgui event to change condition
  vgui_event_condition apply_transform_;

  //: vgui event to delete all shocks
  vgui_event_condition delete_shocks_;

  //: keep current highlighted view
  vgui_soview* current_;
 
  // See if we are in training mode
  bool training_mode_;

  // Keep name of training file name
  vcl_string training_filename_;
  
  // Document pointer of xml
  bxml_data_sptr root_xml_;

};



#endif //dvis_dbskfg_composite_graph_transform_tool
