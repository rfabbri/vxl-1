
// This is brcv/rec/dber/vis/dber_instance_tools.h
#ifndef dbru_osl_tools_h_
#define dbru_osl_tools_h_
//:
// \file
// \brief Tools that work on an video instances library
// \author O. C. Ozcanli adapted from J. L. Mundy's dbinfo_osl_tools
// \date Nov 08, 2006
//
//
// \verbatim
//  Modifications
// \endverbatim
#include <vil/vil_image_resource_sptr.h>
#include <bvis1/bvis1_tool.h>
#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_grid_tableau.h>

#include <dber/pro/dber_instance_storage_sptr.h>
#include <dbru/dbru_label_sptr.h>
#include <vcl_utility.h>
#include <vidl1/vidl1_movie_sptr.h>


//:------------------- OSL LABEK INSTANCES TOOL ------------------------
//: A tool for editing an osl
class dber_instance_label_tool : public bvis1_tool {
public:
  //: Constructor
  dber_instance_label_tool();


  //: Destructor
  virtual ~dber_instance_label_tool();

  //: Returns the string name of this tool
  virtual vcl_string name() const;

  //: Set the tableau to work with
  virtual bool set_tableau( const vgui_tableau_sptr& tableau );

  //: Allow the tool to add to the popup menu as a tableau would
  //virtual void get_popup(const vgui_popup_params& params, vgui_menu &menu);

  //: handle events
  bool handle( const vgui_event & e, 
               const bvis1_view_tableau_sptr& view );

  //: This is called when the tool is activated

  virtual void activate();
  //:Tool utilities

protected:
  vgui_event_condition gesture_label_;
  vgui_event_condition prepare_label_, gesture_display_;
  dber_instance_storage_sptr ins_storage_;

  bool label_all_selected(void);
  
private:
  bool active_;
  vgui_grid_tableau_sptr tableau_;
  dbru_label_sptr current_label_;
};

#endif // dber_instance_tools_h_
