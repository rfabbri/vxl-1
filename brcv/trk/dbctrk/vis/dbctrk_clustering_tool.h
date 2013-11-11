// This is breye1/dbctrk/vis/dbctrk_clustering_tools.h
#ifndef dbctrk_clustering_tools_h_
#define dbctrk_clustering_tools_h_
//:
// \file
// \brief Tools that work on dbctrk_clustering 
// \author Vishal Jain (vj@lems.brown.edu)
// \date 5/26/04
//
// \verbatim
//  Modifications
// \endverbatim

#include <bvis1/bvis1_tool.h>
#include <vcl_map.h>
#include <vgui/vgui_style_sptr.h>
#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <dbctrk/vis/dbctrk_soview2d.h>
#include <dbctrk/pro/dbctrk_storage_sptr.h>
#include <dbctrk/dbctrk_tracker_curve_sptr.h>
#include <bgui/bgui_vsol2D_tableau.h>
//: An abstract base class for tools in this file
class dbctrk_clustering_tool : public bvis1_tool
{
public:
  //: Set the tableau to work with
  virtual bool set_tableau ( const vgui_tableau_sptr& tableau );

  //: Set the storage class for the active tableau
  virtual bool set_storage ( const bpro1_storage_sptr& storage);
  
protected:
  dbctrk_clustering_tool();
  
  bgui_vsol2D_tableau_sptr tableau_;
  dbctrk_storage_sptr storage_;
};

  
//: A tool for inspecting nodes in the clustering
class dbctrk_inspector_tool : public dbctrk_clustering_tool
{
public:
  dbctrk_inspector_tool();
  virtual ~dbctrk_inspector_tool();

  //: Return the name of this tool
  virtual vcl_string name() const;

  //: Handle events
  virtual bool handle( const vgui_event & e, 
                       const bvis1_view_tableau_sptr& view );
  
protected:
  vgui_style_sptr neighbor_style_;
  
private:
  dbctrk_soview2D* object_;
  dbctrk_tracker_curve_sptr curr_curve_;
  int numtopmatches;
  double threshold;
  bool thresh;
  bool topmatches;
};

//: A tool for inspecting nodes in the clustering
class dbctrk_foreground_background_tool : public dbctrk_clustering_tool
{
public:
  dbctrk_foreground_background_tool();
  virtual ~dbctrk_foreground_background_tool();

  //: Return the name of this tool
  virtual vcl_string name() const;

  //: Handle events
  virtual bool handle( const vgui_event & e, 
                       const bvis1_view_tableau_sptr& view );
  
protected:
  vgui_style_sptr neighbor_style_;
  
private:
  dbctrk_soview2D* object_;
  dbctrk_tracker_curve_sptr curr_curve_;
  double top_X;
  double bottom_X;
  double theta_R;

 
};

#endif // dbctrk_clustering_tools_h_
