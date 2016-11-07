// This is contrib/fine/gui/dbseg_seg_tableau.h
#ifndef dbseg_seg_tableau_h_
#define dbseg_seg_tableau_h_
//-----------------------------------------------------------------------------
//:
// \file
// \brief A child tableau of vgui_easy2D_tableau that knows how to display vsol objects.
// \author
//   (8/1/08) Eli Fine
//
//   Default styles are defined for each geometry object soview.
//   Users can change the default style by using the set_*_style commands,
//
// \verbatim
//  Modifications:
//   
// \endverbatim
//-----------------------------------------------------------------------------
#include <vcl_vector.h>

/*#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_conic_2d_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_digital_curve_2d_sptr.h>

#include <vdgl/vdgl_digital_curve_sptr.h>
*/
#include <vgui/vgui_tableau_sptr.h>
#include <vgui/vgui_image_tableau_sptr.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_style_sptr.h>

#include <vgui/vgui_projection_inspector.h>
#include <vgl/vgl_polygon.h>
#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_composite_tableau.h>


#include "dbseg_seg_tableau_sptr.h"
#include <structure/dbseg_seg_object.h>
#include <structure/dbseg_seg_object_base.h>
#include "dbseg_seg_drawing_tableau.h"

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_closest_point.h>

#include <fstream>


class bgui_vsol_soview2D_point;
class bgui_vsol_soview2D_line_seg;
class bgui_vsol_soview2D_conic_seg;
class bgui_vsol_soview2D_polyline;
class bgui_vsol_soview2D_polygon;
class bgui_vsol_soview2D_digital_curve;
class bgui_vsol_soview2D_edgel_curve;
struct drawing_tableau_sptr;

class dbseg_seg_tableau : public vgui_composite_tableau// changed : public vgui_easy2D_tableau 
{
 public:
  dbseg_seg_tableau(dbseg_seg_object<vxl_byte>* object);

   ~dbseg_seg_tableau();

  //: Generate the popup menu for this tableau
  void get_popup(const vgui_popup_params& params, vgui_menu &menu);



    //displays the segments at desired depth
  void display_image(int dep);
  //displays the outlines at desired depth
  void display_outlines(int dep);

  bool handle(const vgui_event &e);

  const int get_depth();
  void set_depth(int d);
  int get_editing();
  //what region is being edited
  void set_editing(int e);

    dbseg_seg_object<vxl_byte>* get_object();

  //: Returns the type of this tableau ('bgui_vsol2D_tableau').
  vcl_string type_name() const { return "dbseg_seg_tableau";}

    //returns the last region to break an is_continuous call
  int get_discontinuous();

  void clear_all_outlines(int d = 0);

  void toggle_display_background();

  void toggle_display_disk_pro();

  bool get_display_disk_pro();

  bool get_display_background();
 
  list<int> get_outlined_regions();

  void set_discontinuous_coords(double x1=0, double y1=0, double x2=0, double y2=0);

  void get_discontinuous_coords(double&, double&, double&, double&);

  //updates the status of object o on the list of objects that are outlined
  //void update_outlined(int o);

 protected:
 
     //the current depth being viewed
     int depth;
  //: Default styles
  vgui_style_sptr point_style_;
  vgui_style_sptr line_style_;
  vgui_style_sptr conic_style_;
  vgui_style_sptr polyline_style_;
  vgui_style_sptr digital_curve_style_;
  vgui_style_sptr dotted_digital_curve_style_;
  vgui_style_sptr edgel_curve_style_;
  vgui_style_sptr dotted_edgel_curve_style_;
    
  dbseg_seg_object<vxl_byte>* object_;
  float line_width_;

    int editing;

    vgui_image_tableau_sptr img_tab_;    
    dbseg_seg_drawing_tableau_sptr drawing_tab_;

    vgui_event_condition left_click_;
  vgui_event_condition shift_left_click_;
  vgui_event_condition page_up_;
  vgui_event_condition page_down_;

  //list<int> outlinedRegions;

    int highlightedSection;    

    static bool display_background, display_disk_pro;

    int discontinuous;

    //for displaying line to discontinuous region
    double dx1, dx2, dy1, dy2;


  void init();
};

//this stuff is needed to establish inheritance between tableau  smart pointers
//cloned from xcv_image_tableau
struct dbseg_seg_tableau_new : public dbseg_seg_tableau_sptr
{
  typedef dbseg_seg_tableau_sptr base;

  dbseg_seg_tableau_new(dbseg_seg_object<vxl_byte>* object) :
    base(new dbseg_seg_tableau(object)) { }

/*    dbseg_seg_tableau_new(vgui_image_tableau_sptr const& it,
                                 const char* n="unnamed") :
    base(new dbseg_seg_tableau(it,n)) { }

  dbseg_seg_tableau_new(vgui_tableau_sptr const& t, const char* n="unnamed")
    :  base(new dbseg_seg_tableau(t, n)) { }
*/
  operator vgui_easy2D_tableau_sptr () const { vgui_easy2D_tableau_sptr tt; tt.vertical_cast(*this); return tt; }
};

#endif // bgui_vsol2D_tableau_h_


