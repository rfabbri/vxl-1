
// This is brcv/shp/dbsk2d/vis/dbsk2d_bline_view.cxx

//:
// \file

#include "dbsk2d_bline_view.h"
#include <dbsk2d/dbsk2d_ishock_bline.h>
#include <vgl/vgl_point_2d.h>

void dbsk2d_bline_view::draw_filled_rectangle(vgl_point_2d<double> spt, double u, double t0, double t1) const
{
  //don't draw these for now
  if (t0==-1.0 && t1==-1.0)
    return;

  //draw the invalid regions in lighter blue
  glColor3f( 0.73f , 0.73f , 0.95f );

  //for nodes just draw lines
  /*if (t0==t1){
    glBegin( GL_LINE_STRIP );
    glVertex2f(x, y);
    glVertex2f(x+r*vcl_cos(t0), y+r*vcl_sin(t0));
    glEnd();
    return;
  }*/

  glBegin(GL_POLYGON);
  glVertex2f(spt.x()+t0*cos(u), spt.y()+t0*sin(u));
  glVertex2f(spt.x()+t1*cos(u), spt.y()+t1*sin(u));
  glVertex2f(spt.x()+t1*cos(u)+5*cos(u+vnl_math::pi/2), spt.y()+t1*sin(u)+5*sin(u+vnl_math::pi/2));
  glVertex2f(spt.x()+t0*cos(u)+5*cos(u+vnl_math::pi/2), spt.y()+t0*sin(u)+5*sin(u+vnl_math::pi/2));
  glEnd();
}

void dbsk2d_bline_view::draw(bool selected, bool draw_burnt_region) const
{
  if (draw_burnt_region){
    //// first draw the invalid region
    //bnd_ishock_map_iter curS = _bl->shock_map().begin();
    //for (; curS!=_bl->shock_map().end(); ++curS)
    //  draw_filled_rectangle(_bl->start(), _bl->u(), curS->first.s_eta, curS->first.eta2);
  }
  
  //then draw the line
  if (selected)
    glColor3f( 1.0 , 0.0 , 0.0 );
  else
    glColor3f( 0.0 , 0.0 , 1.0 );

  glLineWidth (1.0);
  glBegin( GL_LINE_STRIP );
  glVertex2f(_bl->start().x(), _bl->start().y() );
  glVertex2f(_bl->end().x(), _bl->end().y() );
  glEnd();
}
