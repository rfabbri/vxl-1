#ifndef bvis_edge_selection_tools_header
#define bvis_edge_selection_tools_header

//:
// \file
// \brief A tool that prunes out the unselected curves from active vsol2D storage
//        first select the curves then SHIFT + left click to prune unselected ones
// \author Ozge C Ozcanli, (ozge@lems.brown.edu)
// \date 05/14/2004
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

class dbvis_edge_selection_tool : public bvis_tool
{
public:
  dbvis_edge_selection_tool();
  virtual ~dbvis_edge_selection_tool();

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

  vgui_event_condition gesture_prune_;

};



#endif

