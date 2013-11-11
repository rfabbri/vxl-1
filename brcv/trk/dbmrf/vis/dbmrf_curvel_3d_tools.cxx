// This is brcv/trk/dbmrf/vis/dbmrf_curvel_3d_tool.cxx
//:
// \file

#include "dbmrf_curvel_3d_tools.h"

#include <bmrf/bmrf_curve_3d.h>
#include <bgui3d/bgui3d_tableau.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_soview2D.h>
#include <bmrf/bmrf_curve_3d.h>
#include <bmrf/bmrf_curvel_3d.h>
#include <bvis1/bvis1_manager.h>
#include <bvis1/bvis1_view_tableau.h>

#include "SoCurvel3D.h"
#include <Inventor/SoPickedPoint.h>
#include <Inventor/SoPath.h>



//: Constructor
dbmrf_curvel_3d_inspect_tool::dbmrf_curvel_3d_inspect_tool(const vgui_event_condition& pick)
: gesture_pick_(pick), curr_curve_(NULL)
{
}

//: Destructor
dbmrf_curvel_3d_inspect_tool::~dbmrf_curvel_3d_inspect_tool()
{
}


//: Handle events
bool 
dbmrf_curvel_3d_inspect_tool::handle( const vgui_event & e, 
                                      const bvis1_view_tableau_sptr& view )
{
  bool is_mouse_over = ( bvis1_manager::instance()->active_tableau()
                         == view->selector()->active_tableau() );

  double frame = view->frame();

  // Draw neighbors as overlays
  if( e.type == vgui_DRAW_OVERLAY  && !is_mouse_over){
    if (!curr_curve_)
      return false;
      
    vgui_style_sptr line_style = vgui_style::new_style(1.0, 0.0, 0.0, 3.0, 1.0);
    line_style->apply_all();
    vcl_vector<float> pos_x, pos_y;
    for ( bmrf_curve_3d::iterator itr = curr_curve_->begin();
          itr != curr_curve_->end();  ++itr )
    {
      vnl_double_2 point;
      if( (*itr)->pos_in_frame((unsigned int)frame, point) ){
        pos_x.push_back((float)point[0]);
        pos_y.push_back((float)point[1]);
        vgui_soview2D_point((float)point[0], (float)point[1]).draw(); 
      }
      else if(!pos_x.empty()){
        vgui_soview2D_linestrip(pos_x.size(), &*pos_x.begin(), &*pos_y.begin()).draw();
        pos_x.clear();
        pos_y.clear();
      }
    }

    vgui_soview2D_linestrip(pos_x.size(), &*pos_x.begin(), &*pos_y.begin()).draw();   
    return true;
  }


  if(e.type == vgui_MOTION ){
    if(SoPickedPoint* pp = this->pick(e.wx, e.wy)){
      SoNode * node = pp->getPath()->getTail();
      delete pp;

      if(node->getTypeId() == SoCurvel3D::getClassTypeId()){
        SoCurvel3D * curve_view = (SoCurvel3D*) node;
        curve_view->set_highlight(true);
        tableau_->post_redraw();
        view->post_overlay_redraw();
        return false;
      }
    }
  }

  if( gesture_pick_(e)){

    if(SoPickedPoint* pp = this->pick(e.wx, e.wy)){
      SoNode * node = pp->getPath()->getTail();
      delete pp;

      if(node->getTypeId() == SoCurvel3D::getClassTypeId()){
        SoCurvel3D * curve_view = (SoCurvel3D*) node;
        curve_view->set_highlight(true);
        curr_curve_ = curve_view->curve();
        vcl_cout << "Curve size: " << curr_curve_->size() << vcl_endl;
        view->post_overlay_redraw();
        return true;
      }
    }
  }

  return false;
}
 
  
