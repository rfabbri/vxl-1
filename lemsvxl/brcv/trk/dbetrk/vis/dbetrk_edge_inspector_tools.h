// This is breye1/dbvis1/tool/dbetrk/vis/dbetrk_edge_inspector_tools.h
#ifndef dbetrk_edge_inspector_tools_h_
#define dbetrk_edge_inspector_tools_h_
//:
// \file
// \brief Tools that work on ctrk_matching 
// \author Vishal Jain (vj@lems.brown.edu)
// \date 6/1/04
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_iostream.h>
#include <bvis1/bvis1_tool.h>
#include <vcl_map.h>
#include <vgui/vgui_style_sptr.h>
#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <dbetrk/pro/dbetrk_storage_sptr.h>
#include <dbetrk/dbetrk_edge_sptr.h>
#include <bgui/bgui_vsol2D_tableau.h>
#include <dbetrk/vis/dbetrk_soview2D.h>
  
//: A tool for inspecting nodes in the matching
class dbetrk_edge_inspector_tools : public bvis1_tool
{
public:
  dbetrk_edge_inspector_tools();
  virtual ~dbetrk_edge_inspector_tools();

  virtual bool set_tableau ( const vgui_tableau_sptr& tableau );
  virtual bool set_storage ( const bpro1_storage_sptr& storage);

  virtual vcl_string name() const;



  //: handle events
  bool handle( const vgui_event & e, const bvis1_view_tableau_sptr& view );

protected:

  bgui_vsol2D_tableau_sptr tableau();
  dbetrk_storage_sptr storage();

  bgui_vsol2D_tableau_sptr tableau_;
  dbetrk_storage_sptr storage_;

  dbetrk_soview2D * object_;
  dbetrk_edge_sptr curr_edge_node_;
 vgui_style_sptr neighbor_style_;
  vgui_event_condition gesture_info_;

};



#endif // dbetrk/vis/dbetrk_edge_inspector_tools
