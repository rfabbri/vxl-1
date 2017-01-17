#ifndef bvis_edge_transform_tools_header
#define bvis_edge_transform_tools_header

//:
// \file
// \brief A tool that transforms a selected pair of curves from vsol2D storage.
//        select first curve and then SHIFT+left click to register as the first curve
//        then select the second curve and SHIFT+left click to register it.
//        You might select a third curve and SHIFT+left click and its registered as well.
//        Then pressing 't' key finds the transformation between the first and second curves,
//        and displays the transformed version of curve1.
//        if a third curve is selected as well then the same transformation is applied to it
//        and displayed.
//        If two frames are added, first curve can be selected from first frame
//        and second curve can be selected from the second frame.
//        if 't' is pressed while viewing the second frame 
// \author Ozge C Ozcanli, (ozge@lems.brown.edu)
// \date 05/20/2004
//
// \verbatim
//  Modifications
// \endverbatim

#include <bvis/bvis_tool.h>
#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_soview2D.h>
#include <bgui/bgui_vsol_soview2D.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polygon_2d.h>

#include <vidpro/storage/vidpro_vsol2D_storage.h>
#include <vidpro/storage/vidpro_vsol2D_storage_sptr.h>

#include <bgui/bgui_vsol2D_tableau_sptr.h>
#include <bgui/bgui_vsol2D_tableau.h>

class dbvis_edge_transform_tool : public bvis_tool
{
public:
  dbvis_edge_transform_tool();
  virtual ~dbvis_edge_transform_tool();

  virtual bool set_tableau ( const vgui_tableau_sptr& tableau );
  virtual bool set_storage ( const bpro_storage_sptr& storage);

  virtual vcl_string name() const;

  //: handle events
  bool handle( const vgui_event & e, 
               const bvis_view_tableau_sptr& view );

protected:

  bgui_vsol2D_tableau_sptr tableau();
  vidpro_vsol2D_storage_sptr storage();

  bgui_vsol2D_tableau_sptr tableau_;
  vidpro_vsol2D_storage_sptr storage_;

  vgui_event_condition gesture_register_;
  vgui_event_condition gesture_transform_;
  vcl_vector < vsol_polyline_2d_sptr > curves_;

};



#endif

