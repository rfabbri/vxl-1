// This is brcv/trk/dbinfo/vis/dbinfo_track_tools.h
#ifndef dbinfo_track_tools_h_
#define dbinfo_track_tools_h_
//:
// \file
// \brief Tools that work dbinfo tracks
// \author J. L. Mundy
// \date 10/23/05
//
// This file contains tools for editing dbinfo_track(s)
//
// \verbatim
//  Modifications
// \endverbatim

#include <bvis1/bvis1_tool.h>
#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_soview2D.h>
#include <vgui/vgui_displaylist2D_tableau.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <dbinfo/pro/dbinfo_track_storage_sptr.h>
#include <dbinfo/dbinfo_track_sptr.h>
#include <dbinfo/dbinfo_observation_sptr.h>
//: A tool for describing the selected dbinfo track
class dbinfo_track_describe_tool : public bvis1_tool {
public:
  //: Constructor
  dbinfo_track_describe_tool( const vgui_event_condition& lift = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true),
                       const vgui_event_condition& drop = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true) );
  //: Destructor
  virtual ~dbinfo_track_describe_tool();

  //: Returns the string name of this tool
  virtual vcl_string name() const;

  //: Set the tableau to work with
  virtual bool set_tableau( const vgui_tableau_sptr& tableau );

  //: handle events
  bool handle( const vgui_event & e, 
               const bvis1_view_tableau_sptr& view );

  //: This is called when the tool is activated
  virtual void activate();

protected:
  vgui_event_condition gesture_lift_;
  vgui_event_condition gesture_drop_;
  float diff_x, diff_y;
  float last_x, last_y;
  dbinfo_track_storage_sptr track_storage_;

private:
  bool active_;
  vgui_soview2D * object_;
  vgui_displaylist2D_tableau_sptr tableau_;
  vgui_displaylist2D_tableau_sptr cached_tableau_;
};


//: A tool for editing a dbinfo track
class dbinfo_track_edit_tool : public bvis1_tool {
public:
  //: Constructor
  dbinfo_track_edit_tool( const vgui_event_condition& lift = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true));
  //: Destructor
  virtual ~dbinfo_track_edit_tool();

  //: Returns the string name of this tool
  virtual vcl_string name() const;

  //: Set the tableau to work with
  virtual bool set_tableau( const vgui_tableau_sptr& tableau );

  //: handle events
  bool handle( const vgui_event & e,
               const bvis1_view_tableau_sptr& view );

  //: This is called when the tool is activated
  virtual void activate();
  
  //: Allow the tool to add to the popup menu as a tableau would
  virtual void get_popup(const vgui_popup_params& params, vgui_menu &menu);

  //: The currently selected track
  dbinfo_track_sptr selected_track(){return selected_track_;}

  //: The track storage
  dbinfo_track_storage_sptr track_storage(){return track_storage_;}

  //: remove the current soview object from the display
  void remove_object()
  {
    vgui_soview * obj(object_);
    tableau_->remove(obj);
    if(!obj) object_ = NULL;
  }

  //: redraw
  void redraw(){tableau_->post_redraw();}
protected:
  vgui_event_condition gesture_lift_;
  dbinfo_track_storage_sptr track_storage_;
  dbinfo_track_sptr selected_track_;
private:
  vgui_soview2D * object_;
  vgui_displaylist2D_tableau_sptr tableau_;
};

//: A tool for moving a polgonal region and resulting  minfo computation
class dbinfo_region_minfo_tool : public bvis1_tool {
public:
  //: Constructor
  dbinfo_region_minfo_tool( const vgui_event_condition& lift =
                            vgui_event_condition(vgui_LEFT,
                                                 vgui_MODIFIER_NULL,
                                                 true),
                            const vgui_event_condition& drop =
                            vgui_event_condition(vgui_LEFT,
                                                 vgui_MODIFIER_NULL,
                                                 true),
                            const vgui_event_condition& pick =
                            vgui_event_condition(vgui_LEFT,
                                                 vgui_SHIFT,
                                                 true));
  //: Destructor
  virtual ~dbinfo_region_minfo_tool();

  //: Returns the string name of this tool
  virtual vcl_string name() const;

  //: Set the tableau to work with
  virtual bool set_tableau( const vgui_tableau_sptr& tableau );

  //: handle events
  bool handle( const vgui_event & e, 
               const bvis1_view_tableau_sptr& view );

  //: Allow the tool to add to the popup menu as a tableau would
  virtual void get_popup(const vgui_popup_params& params, vgui_menu &menu);

  //: This is called when the tool is activated but we don't care right now
  //  virtual void activate();

  //: Sets the initial observation from a picked polygon
  void set_observation();

  //: Gets the picked polygon
  vsol_polygon_2d_sptr picked_poly(){return picked_poly_;}

  //: Scan a region and produce a set of encoded sample points
  bool scan_region();

  //: Render the mutual information region display
  void render();

  //: explore a region about the initial observation and print minfo
  bool explore();

  //: public parameters
  bool intensity_info_, gradient_info_, color_info_;
  double gamma_;
  double mina_, maxa_;
  float radius_, step_;
protected:
  //:used in ::handle to print minfo when region is released
  void print_minfo(vgui_displaylist2D_tableau_sptr const& tab,
                       vgui_soview2D*& object);
  vgui_event_condition gesture_lift_;
  vgui_event_condition gesture_drop_;
  vgui_event_condition gesture_pick_;
  bool pick_mode_;
  float diff_x, diff_y;
  float last_x, last_y;
  float x0, y0;
  float tx, ty;
  vsol_polygon_2d_sptr picked_poly_;
  vsol_polygon_2d_sptr poly0_;
  vsol_polygon_2d_sptr poly_;
  dbinfo_observation_sptr obs_;
  vidpro1_vsol2D_storage_sptr vvs_;
private:
  bool active_;
  vgui_soview2D * object_;
  vgui_displaylist2D_tableau_sptr tableau_;
  vgui_displaylist2D_tableau_sptr cached_tableau_;
};


#endif // dbinfo_track_tools_h_
