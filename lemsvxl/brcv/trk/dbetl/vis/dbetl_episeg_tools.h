// This is brcv/trk/dbetl/vis/dbetl_episeg_tools.h
#ifndef dbetl_episeg_tools_h_
#define dbetl_episeg_tools_h_
//:
// \file
// \brief Tools that work on dbecl_episegs
// \author Matt Leotta, (mleotta@lems.brown.edu)
// \date 11/10/04
//
// \verbatim
//  Modifications
// \endverbatim

#include <bvis1/bvis1_tool.h>
#include <dbecl/vis/dbecl_episeg_tools.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>



//: A tool for inspecting intensity profiles across episegs
class dbetl_epiprofile_tool : public dbecl_episeg_tool
{
public:
  dbetl_epiprofile_tool();
  virtual ~dbetl_epiprofile_tool();

  //: Return the name of this tool
  virtual vcl_string name() const;

  //: This is called when the tool is activated
  virtual void activate();

  //: Handle events
  virtual bool handle( const vgui_event & e, 
                       const bvis1_view_tableau_sptr& view );

  //: Add popup menu items
  virtual void get_popup(const vgui_popup_params& params, vgui_menu &menu);
  
protected:

  dbecl_epipole_sptr epipole_;
  vidpro1_image_storage_sptr image_storage_;

  bool draw_samples_;
  bool draw_color_;
  bool draw_points_;
  
private:
  dbecl_episeg_soview2D* object_;
  float last_x_, last_y_;

};

#endif // dbetl_episeg_tools_h_
