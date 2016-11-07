// This is brcv/dbsk2d/vis/dbsk2d_ishock_highlight_tool.cxx

//:
// \file

#include "dbsk2d_ishock_highlight_tool.h"

dbsk2d_ishock_highlight_tool::dbsk2d_ishock_highlight_tool()
{
  current_ = 0;
}

dbsk2d_ishock_highlight_tool::~dbsk2d_ishock_highlight_tool()
{
}

void
dbsk2d_ishock_highlight_tool::draw_base_gui_geometry(dbsk2d_base_gui_geometry *geo, float r, float g, float b, float size) {
  
  if( geo->ex_pts().size() == 1 ) 
  {
    glColor3f( r , g , b );
    glPointSize( size );
    glBegin( GL_POINTS );
    glVertex2f(geo->ex_pts()[0].x() , geo->ex_pts()[0].y() ); 
    glEnd();

  } 
  else if ( geo->ex_pts().size() > 1 ) 
  {
    glColor3f( r , g , b );
    glLineWidth (size);
    glBegin( GL_LINE_STRIP );
    for( unsigned int i = 0 ; i < geo->ex_pts().size() ; i++ ) {
      glVertex2f( geo->ex_pts()[i].x() , geo->ex_pts()[i].y() );
    }
    glEnd();
    glLineWidth (1.0);
  }
}

bool
dbsk2d_ishock_highlight_tool::handle( const vgui_event & e, 
                              const bvis1_view_tableau_sptr& view )
{
  if( e.type == vgui_DRAW_OVERLAY ) {
    if( current_ != NULL ) {
      draw_base_gui_geometry(current_, 1.0f, 0.0f, 0.0f, 5.0f);
    }
  }

  if( e.type == vgui_MOTION ) 
  {
    if( tableau()->get_ishock_graph().ptr() != NULL ||
        tableau()->get_shock_graph() ) 
    {
      dbsk2d_base_gui_geometry * temp;
      temp = tableau()->select( e.wx , e.wy );

      if( temp != current_ ) {
        current_ = temp;
        tableau()->post_overlay_redraw();
      }
    }
  }
  

  return false;
}

vcl_string
dbsk2d_ishock_highlight_tool::name() const
{
  return "Highlight Shocks";
}

