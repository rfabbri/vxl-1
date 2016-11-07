// This is dbsksp/vis/dbsksp_soview_shock.cxx

//:
// \file


#include "dbsksp_soview_shock.h"

#include <vgl/vgl_distance.h>
#include <dbgl/algo/dbgl_closest_point.h>
#include <vnl/vnl_math.h>
#include <vgui/vgui_gl.h>
#include <vgui/vgui_style.h>
#include <dbsksp/dbsksp_shock_node.h>
#include <dbsksp/dbsksp_shock_node_descriptor.h>
#include <dbsksp/dbsksp_shock_edge.h>
#include <dbsksp/dbsksp_shock_fragment.h>


//--------------------------------------------------------------------------
// dbsksp_soview_shock_node
//-------------------------------------------------------------------------

//: Render this shock node the display.
void dbsksp_soview_shock_node::
draw() const
{
  glBegin(GL_POINTS);
  glVertex2f(this->node()->pt().x(), this->node()->pt().y());
  glEnd();
}


//: Returns the distance squared of this soview2D from the given position.
float dbsksp_soview_shock_node::
distance_squared(float x, float y) const
{
  float dx = this->node()->pt().x()-x;
  float dy = this->node()->pt().y()-y;
  return dx*dx + dy*dy;
}

//: Returns the centroid of this soview2D.
void dbsksp_soview_shock_node::
get_centroid(float* x, float* y) const
{
  *x = this->node()->pt().x();
  *y = this->node()->pt().y();
}


//: Print details about this vsol object to the given stream.
vcl_ostream& dbsksp_soview_shock_node::
print(vcl_ostream& os) const
{
  this->node()->print(os);
  return os;
}






//--------------------------------------------------------------------------
// dbsksp_soview_shock_edge_chord
//-------------------------------------------------------------------------

//: Render this node on the display.
void dbsksp_soview_shock_edge_chord::
draw() const
{
  vgl_point_2d<double > start = this->edge()->source()->pt();
  vgl_point_2d<double > end = this->edge()->target()->pt();

  // special treament for degenerate edges (A-infinity)
  if (this->edge()->source()->degree()==1 || this->edge()->target()->degree()==1)
  {
    dbsksp_shock_node_sptr degree_one_node = (this->edge()->source()->degree()==1) ? 
      this->edge()->source() : this->edge()->target();
    end = start - this->edge()->chord_dir(degree_one_node)*degree_one_node->radius();
  }
  
  glBegin(GL_LINES);
  glVertex2f(start.x(), start.y());
  glVertex2f(end.x(), end.y());
  glEnd();
}

//: Returns the distance squared of this soview2D from the given position.
float dbsksp_soview_shock_edge_chord::
distance_squared(float x, float y) const
{
  vgl_point_2d<double > start = this->edge()->source()->pt();
  vgl_point_2d<double > end = this->edge()->target()->pt();

  // special treatment for degenerate edges (A-infinity)
  //if (this->edge()->is_degenerate())
  if (this->edge()->source()->degree()==1 || this->edge()->target()->degree()==1)
  {
    end = start + this->edge()->chord_dir()* this->edge()->source()->radius();
  }

  // only use the middle 4/5 of the chord to find closest point so that the
  // nodes can be picked
  vgl_point_2d<double > p0 = midpoint(start, end, 0.1);
  vgl_point_2d<double > p2 = midpoint(start, end, 0.9);
  return 
    vgl_distance2_to_linesegment(p0.x(), p0.y(), p2.x(), p2.y(), 
    double(x), double(y));
}

//: Returns the centroid of this soview2D.
void dbsksp_soview_shock_edge_chord::
get_centroid(float* x, float* y) const
{
  vgl_point_2d<double > start = this->edge()->source()->pt();
  vgl_point_2d<double > end = this->edge()->target()->pt();
  *x = (start.x() + end.y())/2;
  *y = (start.y() + end.y()) / 2;
  return;
}


//: Print details about this vsol object to the given stream.
vcl_ostream& dbsksp_soview_shock_edge_chord::
print(vcl_ostream& os) const
{
  this->edge()->print(os);
  return os;
}





//--------------------------------------------------------------------------
// dbsksp_soview_shock_geom
//-------------------------------------------------------------------------




// ----------------------------------------------------------------------------
//: Render this node on the display.
void dbsksp_soview_shock_geom::
draw() const
{
  if (!this->edge()->fragment())
    this->edge()->form_fragment();

  dbgl_conic_arc conic = this->edge()->fragment()->shock_geom();

  // decide the number of segments to draw based on the chord length of the conic
  int num_segments = (int) vnl_math::max(this->edge()->chord_length(), 5.0);
  num_segments = vnl_math::min(num_segments, 100);
  glBegin( GL_LINE_STRIP );
  for (int i=0; i<=num_segments; ++i)
  {
    vgl_point_2d<double > pt = conic.point_at((1.0*i)/num_segments);
    glVertex2f( pt.x(), pt.y() );
  }
  glEnd();

  return;
}


// ----------------------------------------------------------------------------
//: Returns the distance squared of this soview2D from the given position.
// TODO : make this accurate
float dbsksp_soview_shock_geom::
distance_squared(float x, float y) const
{
  if (!this->edge()->fragment())
    this->edge()->form_fragment();
  dbgl_conic_arc conic = this->edge()->fragment()->shock_geom();
  vgl_point_2d<double > mid_pt = conic.point_at(0.5);
  
  return (float) vnl_math::hypot(x-mid_pt.x(), y-mid_pt.y());
}


// ----------------------------------------------------------------------------
//: Returns the centroid of this soview2D.
void dbsksp_soview_shock_geom::
get_centroid(float* x, float* y) const
{
  vgl_point_2d<double > midpoint = centre(this->edge()->source()->pt(), 
    this->edge()->target()->pt());
  *x = midpoint.x();
  *y = midpoint.y();
  return;
}


// ----------------------------------------------------------------------------
//: Print details about this vsol object to the given stream.
vcl_ostream& dbsksp_soview_shock_geom::
print(vcl_ostream& os) const
{
  this->edge()->print(os);
  return os;
}

 





//-------------------------------------------------------------------------
// dbsksp_soview_contact_shock
//-------------------------------------------------------------------------


//: Render this node on the display.
void dbsksp_soview_contact_shock::
draw() const
{
  vgl_point_2d<double > start = this->node()->pt();

  // iterate thru the edges to draw the contact shocks
  for (dbsksp_shock_node::edge_iterator eit = this->node()->edges_begin();
    eit != this->node()->edges_end(); ++eit)
  {
    // get the foot of the contact shock
    dbsksp_shock_edge_sptr e = (*eit);
    dbsksp_shock_node_descriptor_sptr de = this->node()->descriptor(e);
    vgl_vector_2d<double > contact_dir = rotated(e->chord_dir(this->node()),
      de->alpha + de->phi);

    vgl_point_2d<double > end = start + contact_dir * this->node()->radius();

    // draw the contact shock
    glBegin(GL_LINES);
    glVertex2f(start.x(), start.y());
    glVertex2f(end.x(), end.y());
    glEnd();
  }
}

//: Returns the distance squared of this soview2D from the given position.
float dbsksp_soview_contact_shock::
distance_squared(float x, float y) const
{
  float dx = this->node()->pt().x()-x;
  float dy = this->node()->pt().y()-y;
  return dx*dx + dy*dy;
}


//: Returns the centroid of this soview2D.
void dbsksp_soview_contact_shock::
get_centroid(float* x, float* y) const
{
  *x = this->node()->pt().x();
  *y = this->node()->pt().y();
  return ;
}




//: Print details about this vsol object to the given stream.
vcl_ostream& dbsksp_soview_contact_shock::
print(vcl_ostream& os) const
{
  this->node()->print(os);
  return os;
}



//-------------------------------------------------------------------------
// dbsksp_soview_bnd_arc
//-------------------------------------------------------------------------


// draw a polyline with a whilte stripe around it
void draw_polyline_w_white_boundary(const vgui_soview* so, 
                                      const vcl_vector<vgl_point_2d<double > >& pts)
{
  vgui_style::new_style(1.0f, 1.0f, 1.0f, 5, 5)->apply_all();

  glBegin( GL_LINE_STRIP );
  for (unsigned i=0; i<pts.size(); ++i)
  {
    vgl_point_2d<double > pt = pts[i];
    glVertex2f( pt.x(), pt.y() );
  }
  glEnd();

  so->get_style()->apply_all();
  glBegin( GL_LINE_STRIP );
  for (unsigned i=0; i<pts.size(); ++i)
  {
    vgl_point_2d<double > pt = pts[i];
    glVertex2f( pt.x(), pt.y() );
  }
  glEnd();
}

//: Render this node on the display.
void dbsksp_soview_bnd_arc::
draw() const
{
  if (!this->bnd_arc()) return;

  dbgl_circ_arc arc = this->bnd_arc()->arc();

  // decide the number of segments to draw based on the chord length of the arc
  // use at least 5 line segments
  int num_segments = (int) vnl_math::max(arc.length(), 5.0);
  num_segments = vnl_math::min(num_segments, 100);

  // list of points of the polyline to draw
  vcl_vector<vgl_point_2d<double > > pts;
  pts.reserve(num_segments + 1);
  for (int i=0; i<=num_segments; ++i)
  {
    pts.push_back(arc.point_at((1.0*i)/num_segments));
  }

  draw_polyline_w_white_boundary(this, pts);
  
  
  //// draw the polyline with GL
  //glBegin( GL_LINE_STRIP );
  //for (int i=0; i<=num_segments; ++i)
  //{
  //  vgl_point_2d<double > pt = arc.point_at((1.0*i)/num_segments);
  //  glVertex2f( pt.x(), pt.y() );
  //}
  //glEnd();

  return;
}


//: Returns the distance squared of this soview2D from the given position.
float dbsksp_soview_bnd_arc::
distance_squared(float x, float y) const
{
  dbgl_circ_arc arc = this->bnd_arc()->arc();
  double ret_ratio = 0;
  return vnl_math::sqr( dbgl_closest_point::point_to_circular_arc(vgl_point_2d<double >(x, y), 
    arc.point_at(0.1), arc.point_at(0.9), arc.k(), ret_ratio));
}

//: Returns the centroid of this soview2D.
// Just return the midpoint of the circular arc for now
// The true answer is different
void dbsksp_soview_bnd_arc::
get_centroid(float* x, float* y) const
{
  vgl_point_2d<double > pt = this->bnd_arc()->arc().point_at(0.5);
  *x = pt.x();
  *y = pt.y();
  return;
}

//: Print details about this object to the given stream.
vcl_ostream& dbsksp_soview_bnd_arc::
print(vcl_ostream& os) const
{
  return os;
}






//-------------------------------------------------------------------------
// dbsksp_soview_shapelet
//-------------------------------------------------------------------------

//: Constructor
dbsksp_soview_shapelet::
dbsksp_soview_shapelet(const dbsksp_shapelet_sptr& shapelet, 
     const vgui_style_sptr& shock_chord_style,
     const vgui_style_sptr& shock_geom_style, 
     const vgui_style_sptr& contact_shock_style, 
     const vgui_style_sptr& bnd_style) :
   shapelet_(shapelet),
     shock_chord_style_(shock_chord_style),
     shock_geom_style_(shock_geom_style), 
     contact_shock_style_(contact_shock_style), 
     bnd_style_(bnd_style)
{
     
  vgui_style_sptr default_style = vgui_style::new_style(1.0f, 0.0f, 0.0f, 3.0f, 3.0f);
  if (!shock_chord_style) shock_chord_style_ = default_style;
  if (!shock_geom_style) shock_geom_style_ = default_style;
  if (!contact_shock_style) contact_shock_style_ = default_style;
  if (!bnd_style) bnd_style_ = default_style;
}



       



//: Render this node on the display.
void dbsksp_soview_shapelet::
draw() const
{
  //// draw the shock chord
  //this->shock_chord_style_->apply_all();
  //vgl_point_2d<double > start = this->shapelet()->start();
  //vgl_point_2d<double > end = this->shapelet()->end();
  //
  //glBegin(GL_LINES);
  //glVertex2f(start.x(), start.y());
  //glVertex2f(end.x(), end.y());
  //glEnd();


  // draw the shock curve
  this->shock_geom_style_->apply_all();
  dbgl_conic_arc conic = this->shapelet()->shock_geom();
  // decide the number of segments to draw based on the chord length of the conic
  int num_segments = (int) vnl_math::max(this->shapelet()->chord_length(), 5.0);
  num_segments = vnl_math::min(num_segments, 100);
  glBegin( GL_LINE_STRIP );
  for (int i=0; i<=num_segments; ++i)
  {
    vgl_point_2d<double > pt = conic.point_at((1.0*i)/num_segments);
    glVertex2f( pt.x(), pt.y() );
  }
  glEnd();

  // draw boundary and its contact shocks
  for (int k=0; k<2; ++k)
  {
    // the boundary
    this->bnd_style_->apply_all();
    dbgl_circ_arc arc = this->shapelet()->bnd_arc(k);
    // decide the number of segments to draw based on the chord length of the arc
    // use at least 5 line segments and maximum 100
    int num_segments = (int) vnl_math::max(arc.length(), 5.0);
    num_segments = (int) vnl_math::min(num_segments, 100);
    
    glBegin( GL_LINE_STRIP );
    for (int i=0; i<=num_segments; ++i)
    {
      vgl_point_2d<double > pt = arc.point_at((1.0*i)/num_segments);
      glVertex2f( pt.x(), pt.y() );
    }
    glEnd();

    // contact shocks
    this->contact_shock_style_->apply_all();
    
    // start
    glBegin(GL_LINES);
    glVertex2f(this->shapelet()->start().x(), this->shapelet()->start().y());
    glVertex2f(arc.point1().x(), arc.point1().y());
    glEnd();
    
    // end
    glBegin(GL_LINES);
    glVertex2f(this->shapelet()->end().x(), this->shapelet()->end().y());
    glVertex2f(arc.point2().x(), arc.point2().y());
    glEnd();
  }

  
  return;
}



//: Returns the distance squared of this soview2D from the given position.
float dbsksp_soview_shapelet::
distance_squared(float x, float y) const
{
  vgl_point_2d<double > pt = this->shapelet()->shock_geom().point_at(0.5);
  return vnl_math::sqr(x-pt.x()) + vnl_math::sqr(y-pt.y());
}

//: Returns the centroid of this soview2D.
void dbsksp_soview_shapelet::
get_centroid(float* x, float* y) const
{
  vgl_point_2d<double > pt = this->shapelet()->shock_geom().point_at(0.5);
  *x = pt.x();
  *y = pt.y();
  return;
}




//: Print details about this vsol object to the given stream.
vcl_ostream& dbsksp_soview_shapelet::
print(vcl_ostream& os) const
{
    return os;
}




  






