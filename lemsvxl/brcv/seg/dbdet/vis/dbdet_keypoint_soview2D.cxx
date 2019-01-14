// This is brcv/seg/dbdet/vis/dbdet_keypoint_soview2D.cxx

//:
// \file
#include "dbdet_keypoint_soview2D.h"

#include <iostream>
#include <cmath>

#include <vgui/vgui_style.h>
#include <vgui/vgui_soview2D.h>

#include <dbdet/dbdet_lowe_keypoint.h>


//: Constructor - creates a default view
dbdet_keypoint_soview2D::dbdet_keypoint_soview2D( dbdet_keypoint_sptr const & kp, bool box)
 : sptr(kp), draw_box_(box), point_style_(0)
{
}


//: Draw the keypoint
void
dbdet_keypoint_soview2D::draw() const
{
  float x = sptr->x();
  float y = sptr->y();

  // FIX ME : use dynamic cast
  float orient = sptr->orientation();
  float scale = 8.0*sptr->scale();

  //style->apply_all();
  glBegin(GL_LINES);
    glVertex2f(x,y);
    glVertex2f(x+scale*std::cos(orient), y+scale*std::sin(orient));
  glEnd();

  if( draw_box_ ) {
    glBegin(GL_LINE_LOOP);
    for( int i=0; i<4; ++i ){
      glVertex2f(x+scale*(1.41421f)*std::cos(orient+(i*0.5+0.25)*3.14159f),
                 y+scale*(1.41421f)*std::sin(orient+(i*0.5+0.25)*3.14159f) );
    }
    glEnd();
  }

  if( point_style_ ) point_style_->apply_all();
  glBegin(GL_POINTS);
    glVertex2f(x,y);
  glEnd();
}


std::ostream& dbdet_keypoint_soview2D::print(std::ostream& s) const
{
  s << "[dbdet_keypoint_soview2D (" << sptr->x() << ','
                                    << sptr->y() << ") ";
  return vgui_soview2D::print(s) << ']';
}


float dbdet_keypoint_soview2D::distance_squared(float x, float y) const
{
  float dx = sptr->x() - x;
  float dy = sptr->y() - y;
  return dx*dx + dy*dy;
}

void dbdet_keypoint_soview2D::get_centroid(float* x, float* y) const
{
  *x = sptr->x();
  *y = sptr->y();
}

void dbdet_keypoint_soview2D::translate(float tx, float ty)
{
  sptr->x() = ( sptr->x() + tx );
  sptr->x() = ( sptr->x() + ty );
}


