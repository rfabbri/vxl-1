// This is brcv/dbsk2d/vis/dbsk2d_rich_map_query_tool.cxx

//:
// \file

#include <vgui/vgui.h>
#include "dbsk2d_rich_map_query_tool.h"
#include <dbsk2d/dbsk2d_rich_map.h>

dbsk2d_rich_map_query_tool::dbsk2d_rich_map_query_tool()
{
  cur_geom_=0;
}

dbsk2d_rich_map_query_tool::~dbsk2d_rich_map_query_tool()
{
}

bool
dbsk2d_rich_map_query_tool::handle( const vgui_event & e, 
                              const bvis1_view_tableau_sptr& view )
{
  if( e.type == vgui_DRAW_OVERLAY ) {
    if (cur_geom_){
      if( cur_geom_->ex_pts().size() == 1 ) 
      {
        glColor3f( 1.0 , 0.0 , 0.0 );
        glPointSize( 5.0 );
        glBegin( GL_POINTS );
        glVertex2f(cur_geom_->ex_pts()[0].x() , cur_geom_->ex_pts()[0].y() ); 
        glEnd();

      } 
      else if ( cur_geom_->ex_pts().size() > 1 ) 
      {
        glColor3f( 0.0 , 1.0 , 0.0 );
        glLineWidth (1.0);
        glBegin( GL_LINE_STRIP );
        for( unsigned int i = 0 ; i < cur_geom_->ex_pts().size() ; i++ ) {
          glVertex2f( cur_geom_->ex_pts()[i].x() , cur_geom_->ex_pts()[i].y() );
        }
        glVertex2f( cur_geom_->ex_pts()[0].x() , cur_geom_->ex_pts()[0].y() );
        glEnd();
        glLineWidth (1.0);
      }
    }
  }

  if( e.type == vgui_MOTION ) 
  {
    float pointx, pointy;
    vgui_projection_inspector p_insp;
    p_insp.window_to_image_coordinates(e.wx, e.wy, pointx, pointy);
    int intx = (int)vcl_floor(pointx), inty = (int)vcl_floor(pointy);
    vgui::out << "[" << intx << " " << inty << "] : (" << pointx << " " << pointy << ") ";

    if (storage()->get_rich_map()) //if a rich map exists
    {
      //query the rich map at the current point for the nearest curve
      //dbsk2d_bnd_contour_sptr contour = storage()->get_rich_map()->closest_curve(intx, inty);
      float dist = storage()->get_rich_map()->distance(intx, inty);
      dbsk2d_shock_fragment_sptr fragment = storage()->get_rich_map()->shock_fragment(intx, inty);

      //maybe highlight the selected item here
      dbsk2d_base_gui_geometry* cur_geom = (dbsk2d_base_gui_geometry*)fragment.ptr();
      if (cur_geom_ != cur_geom){
        cur_geom_ = cur_geom;
        tableau()->post_overlay_redraw();
      }

      vgui::out << "d=" << dist << "\n";
    }
    else {
      vgui::out << "Rich Map does not exist! \n";
    }
  }

  return false;
}

vcl_string
dbsk2d_rich_map_query_tool::name() const
{
  return "Query Rich Map";
}

