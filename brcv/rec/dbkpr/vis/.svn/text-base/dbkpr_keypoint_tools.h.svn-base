// This is brcv/rec/dbkpr/vis/dbkpr_keypoint_tools.h
#ifndef dbkpr_tools_h_
#define dbkpr_tools_h_
//:
// \file
// \brief Tools that work on dbdet_keypoints
// \author Matt Leotta, (mleotta@lems.brown.edu)
// \date 3/31/05
//
// \verbatim
//  Modifications
// \endverbatim

#include <bvis1/bvis1_tool.h>
#include <bpro1/bpro1_storage_sptr.h>
#include <dbdet/pro/dbdet_keypoint_storage_sptr.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <dbdet/vis/dbdet_keypoint_soview2D.h>

//: An abstract base class for tools in this file
class dbkpr_keypoint_tool : public bvis1_tool
{
public:
  //: Set the tableau to work with
  virtual bool set_tableau ( const vgui_tableau_sptr& tableau );

  //: Set the storage class for the active tableau
  virtual bool set_storage ( const bpro1_storage_sptr& storage);
  
  virtual ~dbkpr_keypoint_tool(){}
  
protected:
  dbkpr_keypoint_tool();
  
  vgui_easy2D_tableau_sptr tableau_;
  dbdet_keypoint_storage_sptr storage_;
};

  
//: A tool for inspecting nodes in the network
class dbkpr_keypoint_inspector_tool : public dbkpr_keypoint_tool
{
public:
  dbkpr_keypoint_inspector_tool();
  virtual ~dbkpr_keypoint_inspector_tool();

  //: Return the name of this tool
  virtual vcl_string name() const;

  //: Handle events
  virtual bool handle( const vgui_event & e, 
                       const bvis1_view_tableau_sptr& view );

  //: Add popup menu items
  virtual void get_popup(const vgui_popup_params& params, vgui_menu &menu);
  
  
private:
  dbdet_keypoint_soview2D* object_;
  dbdet_keypoint_sptr curr_keypt_;
  int match_index_;

};


 

#endif // dbkpr_keypoint_tools_h_
