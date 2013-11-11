//:
// \file


#include "dbgui_soview2D.h"
#include <vnl/vnl_math.h>


// -----------------------------------------------------------------------------
// dbgui_soview2D_circ_arc
// -----------------------------------------------------------------------------


//: Constructor
dbgui_soview2D_circ_arc::
dbgui_soview2D_circ_arc(float start_x, float start_y, float end_x, float end_y, float k)
{
  vgl_point_2d<double > start(start_x, start_y);
  vgl_point_2d<double > end(end_x, end_y);
  this->circ_arc_.set(start, end, k);
}




// -----------------------------------------------------------------------------
//: Render this node on the display.
void dbgui_soview2D_circ_arc::
draw() const
{
  dbgl_circ_arc arc = this->circ_arc();

  // decide the number of segments to draw based on the chord length of the arc
  // use at least 5 line segments
  int num_segments = (int) vnl_math_max(arc.length(), 5.0);
  num_segments = vnl_math_min(num_segments, 100);

  // list of points of the polyline to draw
  vcl_vector<vgl_point_2d<double > > pts;
  pts.reserve(num_segments + 1);
  for (int i=0; i<=num_segments; ++i)
  {
    pts.push_back(arc.point_at((1.0*i)/num_segments));
  }

    
  // draw the polyline with GL
  glBegin( GL_LINE_STRIP );
  for (int i=0; i<=num_segments; ++i)
  {
    vgl_point_2d<double > pt = arc.point_at((1.0*i)/num_segments);
    glVertex2f( pt.x(), pt.y() );
  }
  glEnd();
}


// -----------------------------------------------------------------------------
//: Translate this node by the given x and y distances.
void dbgui_soview2D_circ_arc::
translate(float x, float y)
{
  vgl_vector_2d<double > v(x, y);
  vgl_point_2d<double > new_start = this->circ_arc_.start() + v;
  vgl_point_2d<double > new_end = this->circ_arc_.end() + v;
  double k = this->circ_arc_.k();
  this->circ_arc_.set(new_start, new_end, k);
  return;
}


// -----------------------------------------------------------------------------
//: Returns the distance squared of this soview2D from the given position.
float dbgui_soview2D_circ_arc::
distance_squared(float x, float y) const
{
  vgl_point_2d<double > pt = this->circ_arc_.point_at(0.5);
  return (float)(x-pt.x())*(x-pt.x()) + (y-pt.y())*(y-pt.y());
}


// -----------------------------------------------------------------------------
//: Returns the centroid of this soview2D.
void dbgui_soview2D_circ_arc::
get_centroid(float* x, float* y) const
{
  vgl_point_2d<double > pt = this->circ_arc_.point_at(0.5);
  *x = float(pt.x());
  *y = float(pt.y());
}


// -----------------------------------------------------------------------------
//: Print details about this object to the given stream.
vcl_ostream& dbgui_soview2D_circ_arc::
print(vcl_ostream& os) const
{
  this->circ_arc_.print(os);
  return vgui_soview2D::print(os);
}


