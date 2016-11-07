// This is brcv/shp/dbsk2d/vis/dbsk2d_bpoint_view.cxx

//:
// \file

#include "dbsk2d_bpoint_view.h"
#include <dbsk2d/dbsk2d_ishock_bpoint.h>
#include <dbsk2d/dbsk2d_ishock_edge.h>

void dbsk2d_bpoint_view::draw_filled_arc_sector(double x, double y, double r, double t0, double t1) const
{
  //don't draw these for now
  if (t0==-1.0 && t1==-1.0)
    return;

  //draw the invalid regions in lighter blue
  glColor3f( 0.73f , 0.73f , 0.95f );

  //for nodes just draw lines
  if (t0==t1){
    glBegin( GL_LINE_STRIP );
    glVertex2f(x, y);
    glVertex2f(x+r*vcl_cos(t0), y+r*vcl_sin(t0));
    glEnd();
    return;
  }

  //normal arc sector
  int n_line_segs = int(100*fabs(t1-t0)/(2*3.14159));
  if(n_line_segs < 4) n_line_segs = 4;

  glBegin(GL_POLYGON);
  glVertex2f(x, y); //center
  for(int i = 0; i < n_line_segs; ++i) {
    double t = t0 + (t1-t0)*i/double(n_line_segs-1);
    glVertex2f(x+r*vcl_cos(t), y+r*vcl_sin(t));
  }
  glEnd();
}

void dbsk2d_bpoint_view::draw(bool selected, bool draw_burnt_region) const
{
  if (draw_burnt_region){
    //// first draw the invalid region
    //bnd_ishock_map_iter curS = _bp->shock_map().begin();
    //for (; curS!=_bp->shock_map().end(); ++curS){
    //  double eta1 = curS->first.eta1;
    //  double eta2 = curS->second->eEta(curS->first.type);

    //  double v1, v2;
    //  if (curS->first.type == dbsk2d_ishock_bnd_key::LEFT){
    //    v1 = angle0To2Pi(_bp->vref() - eta1);
    //    v2 = angle0To2Pi(_bp->vref() - eta2);
    //  }
    //  else {
    //    v2 = angle0To2Pi(_bp->vref() - eta1);
    //    v1 = angle0To2Pi(_bp->vref() - eta2);
    //  }

    //  if (v1 > v2)
    //    draw_filled_arc_sector(_bp->pt().x() , _bp->pt().y(), 5, v1, v2+2*vnl_math::pi);
    //  else
    //    draw_filled_arc_sector(_bp->pt().x() , _bp->pt().y(), 5, v1, v2);
    //}
  }

  //then draw the point
  if (selected)
    glColor3f( 1.0 , 0.0 , 0.0 );
  else
    glColor3f( 0.0 , 0.0 , 1.0 );

  glPointSize( 3.0 );
  glBegin( GL_POINTS );
  glVertex2f(_bp->pt().x() , _bp->pt().y() ); 
  glEnd();
}
