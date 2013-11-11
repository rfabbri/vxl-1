//:
// \file


#include "dbsksp_soview_xshock.h"

#include <vgl/vgl_distance.h>
#include <vnl/vnl_math.h>
#include <vgui/vgui_gl.h>
#include <vgui/vgui_style.h>

//#include <dbgl/algo/dbgl_closest_point.h>

#include <dbgl/algo/dbgl_biarc.h>
#include <dbsksp/dbsksp_xshock_edge.h>
#include <dbsksp/dbsksp_xshock_edge_sptr.h>
#include <dbsksp/dbsksp_xshock_node.h>
#include <dbsksp/dbsksp_xshock_node_sptr.h>
#include <dbsksp/dbsksp_xshock_fragment.h>





// =============================================================================
// dbsksp_soview_xshock_node
// =============================================================================

//: Constructor
dbsksp_soview_xshock_node::
dbsksp_soview_xshock_node(const dbsksp_xshock_node_sptr& xnode) :
  vgui_soview2D(), xnode_(xnode)
{
  this->compute_geometry();
}

//: Render this object on the display.
void dbsksp_soview_xshock_node::
draw() const
{
  glBegin(GL_POINTS);
  glVertex2f(this->pt_.x(), this->pt_.y());
  glEnd();
  return;
}


//: Returns the distance squared of this soview2D from the given position.
float dbsksp_soview_xshock_node::
distance_squared(float x, float y) const
{
  float dx = this->pt_.x()-x;
  float dy = this->pt_.y()-y;
  return dx*dx + dy*dy;
}

 
//: Returns the centroid of this soview2D.
void dbsksp_soview_xshock_node::
get_centroid(float* x, float* y) const
{
  *x = this->pt_.x();
  *y = this->pt_.y();
  return;
}


//: Print details about this object to the given stream.
vcl_ostream& dbsksp_soview_xshock_node::
print(vcl_ostream& os) const
{
  return os;
}


//: Compute extrinsic geometry
void dbsksp_soview_xshock_node::
compute_geometry()
{
  // extract point coordinate from the extrinsic node
  if (this->xnode())
  {
    this->pt_.set(this->xnode()->pt().x(), this->xnode()->pt().y());
  }
  else
  {
    this->pt_.set(0, 0);
  }
}


// =============================================================================
// dbsksp_soview_xshock_chord
// =============================================================================

// -----------------------------------------------------------------------------
//: Constructor
dbsksp_soview_xshock_chord::
dbsksp_soview_xshock_chord(const dbsksp_xshock_edge_sptr& xedge): 
  vgui_soview2D(), xedge_(xedge) 
{
  this->compute_geometry();
};


// -----------------------------------------------------------------------------
//: Render this node on the display.
void dbsksp_soview_xshock_chord::
draw() const
{
  glBegin(GL_LINES);
  glVertex2f(this->start_.x(), this->start_.y());
  glVertex2f(this->end_.x(), this->end_.y());
  glEnd();
  return;
}


// -----------------------------------------------------------------------------
//: Returns the distance squared of this soview2D from the given position.
float dbsksp_soview_xshock_chord::
distance_squared(float x, float y) const
{
  // only use the middle 3/5 of the chord to find closest point so that the
  // nodes can be picked
  vgl_point_2d<float > p0 = midpoint(this->start_, this->end_, 0.2f);
  vgl_point_2d<float > p2 = midpoint(this->start_, this->end_, 0.8f);
  return 
    vgl_distance2_to_linesegment(p0.x(), p0.y(), p2.x(), p2.y(), x, y);
}

// -----------------------------------------------------------------------------
//: Returns the centroid of this soview2D.
void dbsksp_soview_xshock_chord::
get_centroid(float* x, float* y) const
{
  *x = (this->start_.x() + this->end_.x())/2;
  *y = (this->start_.y() + this->end_.y())/2;
  return;
}


// -----------------------------------------------------------------------------
//: Print details about this object to the given stream.
vcl_ostream& dbsksp_soview_xshock_chord::
print(vcl_ostream& os) const
{
  return os;
}


// -----------------------------------------------------------------------------
//: Compute geometry (start and end point) of the chord
void dbsksp_soview_xshock_chord::
compute_geometry()
{
  vgl_point_2d<double > start = this->xedge()->source()->pt();
  vgl_point_2d<double > end = this->xedge()->target()->pt();

  // special treament for degenerate edges (A-infinity)
  if (this->xedge()->is_terminal_edge())
  {
    dbsksp_xshock_node_sptr degree_one_node = (this->xedge()->source()->degree()==1) ? 
      this->xedge()->source() : this->xedge()->target();
    vgl_vector_2d<double > t = degree_one_node->descriptor(this->xedge())->shock_tangent();
    end = start - degree_one_node->radius() * t;
  }
  this->start_.set(start.x(), start.y());
  this->end_.set(end.x(), end.y());
}












//==============================================================================
// dbsksp_soview_xshock_curve
//==============================================================================

//------------------------------------------------------------------------------
//: Constructor
dbsksp_soview_xshock_curve::
dbsksp_soview_xshock_curve(const dbsksp_xshock_edge_sptr& xedge):
  vgui_soview2D(), xedge_(xedge) 
{
  this->compute_geometry();
};


//------------------------------------------------------------------------------
//: Render this object on the display.
void dbsksp_soview_xshock_curve::
draw() const
{
  glBegin( GL_LINE_STRIP );
  for (unsigned i =0; i < this->pts_.size(); ++i)
  {
    glVertex2f( this->pts_[i].x(), this->pts_[i].y());
  }
  glEnd();
  return;
}



//------------------------------------------------------------------------------
//: Returns the distance squared of this soview2D from the given position.
float dbsksp_soview_xshock_curve::
distance_squared(float x, float y) const
{
  // \todo this is not accurate, but fast. Change this if accuracy becomes important.
  return vnl_math_sqr(x-this->centroid_x_) + vnl_math_sqr(y - this->centroid_y_);
}



//------------------------------------------------------------------------------
//: Returns the centroid of this soview2D.
void dbsksp_soview_xshock_curve::
get_centroid(float* x, float* y) const
{
  *x = this->centroid_x_;
  *y = this->centroid_y_;
}


//------------------------------------------------------------------------------
//: Print details about this object to the given stream.
vcl_ostream& dbsksp_soview_xshock_curve::
print(vcl_ostream& os) const
{
  return os;
}


//------------------------------------------------------------------------------
//: Compute geometry (start and end point) of the chord
void dbsksp_soview_xshock_curve::
compute_geometry()
{
  this->pts_.clear();
  this->centroid_x_ = -1e10;
  this->centroid_y_ = -1e10;

  dbsksp_xshock_edge_sptr xe = this->xedge();
  dbsksp_xshock_node_descriptor start = *(xe->source()->descriptor(xe));
  dbsksp_xshock_node_descriptor end = xe->target()->descriptor(xe)->opposite_xnode();
  
  // approximate the xshock curve using biarc
  dbgl_biarc shock_curve;
  bool success = shock_curve.compute_biarc_params(start.pt(), start.shock_tangent(),
                                                  end.pt(), end.shock_tangent()) &&
                 shock_curve.is_consistent();
  if (success)
  {
    double len = shock_curve.len();
    int num_segments = vnl_math_rnd(vnl_math_max(len, 5.0));
    num_segments = vnl_math_min(num_segments, (int)50);

    // compute the extrinsic points and store to a vector
    this->pts_.reserve(num_segments+1);
    for (int i=0; i<=num_segments; ++i)
    {
      vgl_point_2d<double > pt = shock_curve.point_at((len*i)/num_segments);
      this->pts_.push_back(vgl_point_2d<float >(pt.x(), pt.y()));
    }

    // compute centroid
    vgl_point_2d<double > centroid = shock_curve.point_at(0.5*len);
    this->centroid_x_ = float(centroid.x());
    this->centroid_y_ = float(centroid.y());
  }
  return;
}









// =============================================================================
// dbsksp_soview_xshock_contact
// =============================================================================

// -----------------------------------------------------------------------------
//: Constructor
dbsksp_soview_xshock_contact::
dbsksp_soview_xshock_contact(const dbsksp_xshock_node_sptr& xnode):
  vgui_soview2D(), xnode_(xnode)
{
  this->compute_geometry();
  return;
};


// -----------------------------------------------------------------------------
//: Render this object on the display.
void dbsksp_soview_xshock_contact::
draw() const
{
  glBegin(GL_LINES);
  for (unsigned i =0; i < this->bnd_pts_.size(); ++i)
  {
    glVertex2f(this->center_.x(), this->center_.y());
    glVertex2f(this->bnd_pts_[i].x(), this->bnd_pts_[i].y());
  }
  glEnd();
  return;
}


// -----------------------------------------------------------------------------
//: Returns the distance squared of this soview2D from the given position.
float dbsksp_soview_xshock_contact::
distance_squared(float x, float y) const
{
  return (this->center_.x()-x)*(this->center_.x()-x) + 
    (this->center_.y()-y) * (this->center_.y()-y);
}


// -----------------------------------------------------------------------------
//: Returns the centroid of this soview2D.
void dbsksp_soview_xshock_contact::
get_centroid(float* x, float* y) const
{

  *x = this->center_.x();
  *y = this->center_.y();
  return;
}


// -----------------------------------------------------------------------------
//: Print details about this object to the given stream.
vcl_ostream& dbsksp_soview_xshock_contact::
print(vcl_ostream& os) const
{
  return os;
}


// -----------------------------------------------------------------------------
//: Compute geometry (center + boundary points)
void dbsksp_soview_xshock_contact::
compute_geometry()
{
  this->center_.set(this->xnode()->pt().x(), this->xnode()->pt().y());
  this->bnd_pts_.clear();
  for (dbsksp_xshock_node::edge_iterator eit = this->xnode()->edges_begin(); 
    eit != this->xnode()->edges_end(); ++eit)
  {
    dbsksp_xshock_edge_sptr xe = *eit;
    dbsksp_xshock_node_descriptor* xdesc = this->xnode()->descriptor(xe);
    vgl_point_2d<double > pt = xdesc->bnd_pt_left();
    this->bnd_pts_.push_back(vgl_point_2d<float >(pt.x(), pt.y()));
  }
  return;
}



// =============================================================================
// dbsksp_soview_xshock_bnd
// =============================================================================


// -----------------------------------------------------------------------------
//: Constructor
dbsksp_soview_xshock_bnd::
dbsksp_soview_xshock_bnd(const dbsksp_xshock_edge_sptr& xedge, bool is_left_bnd):
  vgui_soview2D(), xedge_(xedge), is_left_bnd_(is_left_bnd)
{
  this->compute_geometry();
  return;
}


// -----------------------------------------------------------------------------
//: Render this object on the display.
void dbsksp_soview_xshock_bnd::
draw() const
{
  glBegin( GL_LINE_STRIP );
  for (unsigned i =0; i < this->bnd_pts_.size(); ++i)
  {
    glVertex2f( this->bnd_pts_[i].x(), this->bnd_pts_[i].y());
  }
  glEnd();
  return;
}


// -----------------------------------------------------------------------------
//: Returns the distance squared of this soview2D from the given position.
float dbsksp_soview_xshock_bnd::
distance_squared(float x, float y) const
{
  int mid_index = (this->bnd_pts_.size()+1) / 2;
  vgl_point_2d<float > pt = this->bnd_pts_[mid_index];
  return (pt.x()-x)*(pt.x()-x) + (pt.y()-y)*(pt.y()-y);
}


// -----------------------------------------------------------------------------
//: Returns the centroid of this soview2D.
void dbsksp_soview_xshock_bnd::
get_centroid(float* x, float* y) const
{
  int mid_index = (this->bnd_pts_.size()+1) / 2;
  vgl_point_2d<float > pt = this->bnd_pts_[mid_index];
  *x = pt.x();
  *y = pt.y();
  return;
}

// -----------------------------------------------------------------------------
//: Print details about this object to the given stream.
vcl_ostream& dbsksp_soview_xshock_bnd::
print(vcl_ostream& os) const
{
  return os;
}


// -----------------------------------------------------------------------------
//: Compute geometry (center + boundary points)
void dbsksp_soview_xshock_bnd::
compute_geometry()
{
  this->bnd_pts_.clear();

  dbsksp_xshock_edge_sptr xe = this->xedge();
  dbsksp_xshock_node_descriptor* xdesc_source = xe->source()->descriptor(xe);
  dbsksp_xshock_node_descriptor xdesc_target = xe->target()->descriptor(xe)->opposite_xnode();
  
  // draw left boundary
  if (this->is_left_bnd_)
  {
    dbgl_biarc left_biarc;
    if (left_biarc.compute_biarc_params(xdesc_source->bnd_pt_left(), xdesc_source->bnd_tangent_left(),
      xdesc_target.bnd_pt_left(), xdesc_target.bnd_tangent_left()) &&
      left_biarc.is_consistent())
    {
      double len = left_biarc.len();
      int num_segments = (int) vnl_math_max(len, 5.0);
      num_segments = (int) vnl_math_min(num_segments, 50);
      
      // compute the extrinsic points and store to a vector
      this->bnd_pts_.reserve(num_segments+1);
      for (int i=0; i<=num_segments; ++i)
      {
        vgl_point_2d<double > pt = left_biarc.point_at((len*i)/num_segments);
        this->bnd_pts_.push_back(vgl_point_2d<float >(pt.x(), pt.y()));
      }
    }
  }
  // right boundary
  else
  {
    // draw right boundary
    dbgl_biarc right_biarc;
    if (right_biarc.compute_biarc_params(xdesc_source->bnd_pt_right(), xdesc_source->bnd_tangent_right(),
      xdesc_target.bnd_pt_right(), xdesc_target.bnd_tangent_right()))
    {
      double len = right_biarc.len();
      int num_segments = (int) vnl_math_max(len, 5.0);
      num_segments = (int) vnl_math_min(num_segments, 50);

      // compute the extrinsic points
      this->bnd_pts_.clear();
      for (int i=0; i<=num_segments; ++i)
      {
        vgl_point_2d<double > pt = right_biarc.point_at((len*i)/num_segments);
        this->bnd_pts_.push_back(vgl_point_2d<float >(pt.x(), pt.y()));
      }
    }
  }
  return;
}


// =============================================================================
// dbsksp_soview_xfrag
// =============================================================================

// -----------------------------------------------------------------------------
//: Constructor
dbsksp_soview_xfrag::
dbsksp_soview_xfrag(const dbsksp_xshock_fragment_sptr& xfrag) : xfrag_(xfrag)
{
  assert(xfrag);


  vgui_style_sptr default_style = vgui_style::new_style(1.0f, 0.0f, 0.0f, 3.0f, 3.0f);
  if (!this->bnd_style_) this->bnd_style_ = default_style;
  if (!this->contact_shock_style_) this->contact_shock_style_ = default_style;
  if (!this->shock_chord_style_) this->shock_chord_style_ = default_style;
  return;
}

//: Render this node on the display.
void dbsksp_soview_xfrag::
draw() const
{
  dbsksp_xshock_node_descriptor xdesc_source = this->xfrag()->start();
  dbsksp_xshock_node_descriptor xdesc_target = this->xfrag()->end();
  
  // draw left boundary
  this->bnd_style_->apply_all();
  dbgl_biarc left_biarc = this->xfrag()->bnd_left_as_biarc();
  if (left_biarc.is_consistent())
  {
    double len = left_biarc.len();
    int num_segments = (int) vnl_math_max(len, 5.0);
    num_segments = (int) vnl_math_min(num_segments, 50);

    glBegin( GL_LINE_STRIP );
    for (int i=0; i<=num_segments; ++i)
    {
      vgl_point_2d<double > pt = left_biarc.point_at((len*i)/num_segments);
      glVertex2f( pt.x(), pt.y() );
    }
    glEnd();
  }
  
  // draw right boundary
  this->bnd_style_->apply_all();
  dbgl_biarc right_biarc = this->xfrag()->bnd_right_as_biarc();
  if (right_biarc.is_consistent())
  {
    double len = right_biarc.len();
    int num_segments = (int) vnl_math_max(len, 5.0);
    num_segments = (int) vnl_math_min(num_segments, 50);

    glBegin( GL_LINE_STRIP );
    for (int i=0; i<=num_segments; ++i)
    {
      vgl_point_2d<double > pt = right_biarc.point_at((len*i)/num_segments);
      glVertex2f( pt.x(), pt.y() );
    }
    glEnd();
  }
    
  // contact shocks
  this->contact_shock_style_->apply_all();

  // front contact shock
  glBegin(GL_LINE_STRIP);
  glVertex2f(xdesc_source.bnd_pt_left().x(), xdesc_source.bnd_pt_left().y());
  glVertex2f(xdesc_source.pt().x(), xdesc_source.pt().y());
  glVertex2f(xdesc_source.bnd_pt_right().x(), xdesc_source.bnd_pt_right().y());
  glEnd();

  // rear contact shock
  glBegin(GL_LINE_STRIP);
  glVertex2f(xdesc_target.bnd_pt_left().x(), xdesc_target.bnd_pt_left().y());
  glVertex2f(xdesc_target.pt().x(), xdesc_target.pt().y());
  glVertex2f(xdesc_target.bnd_pt_right().x(), xdesc_target.bnd_pt_right().y());
  glEnd();

  // draw the shock chord
  this->shock_chord_style_->apply_all();
  
  glBegin(GL_LINES);
  glVertex2f(xdesc_source.pt().x(), xdesc_source.pt().y());
  glVertex2f(xdesc_target.pt().x(), xdesc_target.pt().y());
  glEnd();

  return;
}

//: Returns the distance squared of this soview2D from the given position.
float dbsksp_soview_xfrag::
distance_squared(float x, float y) const
{
  float cx, cy;
  this->get_centroid(&cx, &cy);
  return ((x-cx)*(x-cx) + (y-cy)*(y-cy));
}

//: Returns the centroid of this soview2D.
void dbsksp_soview_xfrag::
get_centroid(float* x, float* y) const
{
  vgl_point_2d<double > source_pt = this->xfrag()->start().pt();
  vgl_point_2d<double > target_pt = this->xfrag()->end().pt();
  vgl_point_2d<double > mid_pt = centre(source_pt, target_pt);
  *x = float(mid_pt.x());
  *y = float(mid_pt.y());
  return;
}

//: Print details about this object to the given stream.
vcl_ostream& dbsksp_soview_xfrag::
print(vcl_ostream& os) const
{
  return os;
}


//: Set the styles to draw the extrinsic fragment
void dbsksp_soview_xfrag::
set_xfrag_styles(const vgui_style_sptr& shock_chord_style, const vgui_style_sptr& 
                 contact_shock_style, const vgui_style_sptr& bnd_style)
{
  this->shock_chord_style_ = shock_chord_style;
  this->contact_shock_style_ = contact_shock_style;
  this->bnd_style_ = bnd_style;
}



  



// =============================================================================
// dbsksp_soview_xsample
// =============================================================================

//------------------------------------------------------------------------------
//: Constructor
dbsksp_soview_xsample::
dbsksp_soview_xsample(const dbsksp_xshock_node_descriptor& xsample):
  vgui_soview2D(), xsample_(xsample)
{
  this->compute_geometry();
  return;
};



//------------------------------------------------------------------------------
//: Render this node on the display.
void dbsksp_soview_xsample::
draw() const
{
  glBegin(GL_LINES);
  for (unsigned i =0; i < 2; ++i)
  {
    glVertex2f(this->shock_pt_.x(), this->shock_pt_.y());
    glVertex2f(this->bnd_pts_[i].x(), this->bnd_pts_[i].y());
  }
  glEnd();
  return;
}


//------------------------------------------------------------------------------
//: Returns the distance squared of this soview2D from the given position.
float dbsksp_soview_xsample::
distance_squared(float x, float y) const
{
  return (vnl_math_sqr(this->shock_pt_.x()-x) + 
          vnl_math_sqr(this->shock_pt_.y()-y));
}
 
//------------------------------------------------------------------------------
//: Returns the centroid of this soview2D.
void dbsksp_soview_xsample::
get_centroid(float* x, float* y) const
{
  *x = this->shock_pt_.x();
  *y = this->shock_pt_.y();
}


//------------------------------------------------------------------------------
  //: Print details about this object to the given stream.
vcl_ostream& dbsksp_soview_xsample::
print(vcl_ostream& os) const
{
  return os;
}



//------------------------------------------------------------------------------
//: Compute geometry (center + boundary points)
void dbsksp_soview_xsample::
compute_geometry()
{
  vgl_point_2d<double > center = this->xsample_.pt();
  vgl_point_2d<double > left   = this->xsample_.bnd_pt_left();
  vgl_point_2d<double > right  = this->xsample_.bnd_pt_right();

  this->shock_pt_.set(center.x(), center.y());
  this->bnd_pts_[0].set(left.x(), left.y()); 
  this->bnd_pts_[1].set(right.x(), right.y());
  return;
}





