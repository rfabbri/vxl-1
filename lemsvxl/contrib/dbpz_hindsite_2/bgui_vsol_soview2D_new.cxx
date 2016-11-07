//:
// \file
#include "bgui_vsol_soview2D_new.h"

#include <vcl_iostream.h>

#include <vgui/vgui_gl.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/internals/vgui_draw_line.h>

#include <vgl/vgl_distance.h>
#include <vgui/vgui_soview2D.h>

#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_edgel.h>

#include <vsol/vsol_spatial_object_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polygon_2d.h>

//--------------------------------------------------------------------------
//: vsol_spatial_object_2d view
//-------------------------------------------------------------------------
bgui_vsol_soview2D_new::bgui_vsol_soview2D_new( vsol_spatial_object_2d_sptr const & pt)
   :sptr_(pt)
{
}

vcl_ostream& bgui_vsol_soview2D_new::print(vcl_ostream& s) const
{
  this->sptr_->describe(s);
  return vgui_soview2D::print(s);
}


//--------------------------------------------------------------------------
//: vsol_point_2d view
//--------------------------------------------------------------------------

bgui_vsol_soview2D_new_point::bgui_vsol_soview2D_new_point( vsol_point_2d_sptr const & pt)
 : bgui_vsol_soview2D_new(pt.ptr())
{
}

vsol_point_2d_sptr bgui_vsol_soview2D_new_point::sptr() const
{
  return  (vsol_point_2d*)(sptr_.ptr());
}

void bgui_vsol_soview2D_new_point::draw() const
{
  style->apply_point_size();
  glBegin(GL_POINTS);
  glVertex2f(sptr()->x(),sptr()->y());
  glEnd();
}

float bgui_vsol_soview2D_new_point::distance_squared(float x, float y) const
{
  float dx = sptr()->x() - x;
  float dy = sptr()->y() - y;
  return dx*dx + dy*dy;
}

void bgui_vsol_soview2D_new_point::get_centroid(float* x, float* y) const
{
  *x = sptr()->x();
  *y = sptr()->y();
}

void bgui_vsol_soview2D_new_point::translate(float tx, float ty)
{
  sptr()->set_x( sptr()->x() + tx );
  sptr()->set_y( sptr()->y() + ty );
}


//--------------------------------------------------------------------------
//: vsol_line_2d view
//--------------------------------------------------------------------------

bgui_vsol_soview2D_new_line_seg::bgui_vsol_soview2D_new_line_seg( vsol_line_2d_sptr const & pt)
: bgui_vsol_soview2D_new(pt.ptr())
{
}

vsol_line_2d_sptr bgui_vsol_soview2D_new_line_seg::sptr() const
{
  return  (vsol_line_2d*)(sptr_.ptr());
}

void bgui_vsol_soview2D_new_line_seg::draw() const
{
  //vcl_cerr << "line id : " << id << vcl_endl;

  //glLineWidth(style->line_width);
  glBegin(GL_LINES);
  glVertex2f(sptr()->p0()->x(),sptr()->p0()->y());
  glVertex2f(sptr()->p1()->x(),sptr()->p1()->y());
  glEnd();
}

float bgui_vsol_soview2D_new_line_seg::distance_squared(float x, float y) const
{
  return vgl_distance2_to_linesegment(sptr()->p0()->x(), sptr()->p0()->y(),
                                      sptr()->p1()->x(), sptr()->p1()->y(),
                                      double(x), double(y));
}

void bgui_vsol_soview2D_new_line_seg::get_centroid(float* x, float* y) const
{
  *x = (sptr()->p0()->x() + sptr()->p1()->x()) / 2;
  *y = (sptr()->p0()->y() + sptr()->p1()->y()) / 2;
}

void bgui_vsol_soview2D_new_line_seg::translate(float tx, float ty)
{
  sptr()->p0()->set_x( sptr()->p0()->x() + tx );
  sptr()->p0()->set_y( sptr()->p0()->y() + ty );
  sptr()->p1()->set_x( sptr()->p1()->x() + tx );
  sptr()->p1()->set_y( sptr()->p1()->y() + ty );
}


//--------------------------------------------------------------------------
//: vsol_polyline_2d view
//--------------------------------------------------------------------------

bgui_vsol_soview2D_new_polyline::bgui_vsol_soview2D_new_polyline(vsol_polyline_2d_sptr const& pline)
: bgui_vsol_soview2D_new(pline.ptr())
{
}

vsol_polyline_2d_sptr bgui_vsol_soview2D_new_polyline::sptr() const
{
  return ( vsol_polyline_2d*)(sptr_.ptr());
}

void bgui_vsol_soview2D_new_polyline::draw() const
{
  unsigned int n = sptr()->size();

  glBegin( GL_LINE_STRIP );
  for (unsigned int i=0; i<n;i++)
  {
    glVertex2f( sptr()->vertex(i)->x() , sptr()->vertex(i)->y() );
  }
  glEnd();
}

float bgui_vsol_soview2D_new_polyline::distance_squared(float x, float y) const
{
  unsigned int n = sptr()->size();

  float* xptr = new float[n];
  float* yptr = new float[n];
  for (unsigned int i=0; i<n;i++)
  {
    xptr[i] = sptr()->vertex(i)->x();
    yptr[i] = sptr()->vertex(i)->y();
  }

  double tmp = vgl_distance_to_non_closed_polygon( xptr , yptr , n , x , y );

  delete [] xptr;
  delete [] yptr;

  return tmp * tmp;
}

void bgui_vsol_soview2D_new_polyline::get_centroid(float* x, float* y) const
{
  unsigned int n = sptr()->size();

  *x = 0;
  *y = 0;

  for (unsigned int i=0; i<n;i++)
  {
    *x += sptr()->vertex(i)->x();
    *y += sptr()->vertex(i)->y();
  }
  float s = 1.0f / float( n );
  *x *= s;
  *y *= s;
}

void bgui_vsol_soview2D_new_polyline::translate(float tx, float ty)
{
  unsigned int n = sptr()->size();

  for (unsigned int i=0; i<n;i++)
  {
    sptr()->vertex(i)->set_x( sptr()->vertex(i)->x() + tx );
    sptr()->vertex(i)->set_y( sptr()->vertex(i)->y() + ty );
  }
}


//--------------------------------------------------------------------------
//: vdgl_digital_curve regular view
//--------------------------------------------------------------------------

vcl_ostream& bgui_vsol_soview2D_new_digital_curve::print(vcl_ostream& s) const
{
  return s;
}

void bgui_vsol_soview2D_new_digital_curve::draw() const
{
  //get the edgel chain
  vdgl_interpolator_sptr itrp = this->digital_curve()->get_interpolator();
  vdgl_edgel_chain_sptr ech = itrp->get_edgel_chain();

  unsigned int n = ech->size();

  glBegin( GL_LINE_STRIP );
  for (unsigned int i=0; i<n;i++)
  {
    vdgl_edgel ed = (*ech)[i];
    glVertex2f( ed.get_x() , ed.get_y() );
  }
  glEnd();

  if (draw_dotted_)
  {
    glBegin(GL_POINTS);
    for (unsigned int i=0; i<n;i++)
    {
      vdgl_edgel ed = (*ech)[i];
      glVertex2f( ed.get_x() , ed.get_y() );
    }
    glEnd();
  }
}

float bgui_vsol_soview2D_new_digital_curve::distance_squared( float x , float y ) const
{
  //get the edgel chain
  vdgl_interpolator_sptr itrp = this->digital_curve()->get_interpolator();
  vdgl_edgel_chain_sptr ech = itrp->get_edgel_chain();

  unsigned int n = ech->size();

  float* xptr = new float[n];
  float* yptr = new float[n];
  for (unsigned int i=0; i<n;i++)
  {
    vdgl_edgel ed = (*ech)[i];
    xptr[i]=ed.get_x();
    yptr[i]=ed.get_y();
  }

  double tmp = vgl_distance_to_non_closed_polygon( xptr , yptr , n , x , y );

  delete [] xptr;
  delete [] yptr;

  return tmp * tmp;
}

void bgui_vsol_soview2D_new_digital_curve::get_centroid( float* x, float* y ) const
{
  //get the edgel chain
  vdgl_interpolator_sptr itrp = this->digital_curve()->get_interpolator();
  vdgl_edgel_chain_sptr ech = itrp->get_edgel_chain();

  unsigned int n = ech->size();

  *x = 0;
  *y = 0;

  for (unsigned int i=0; i<n;i++)
  {
    vdgl_edgel ed = (*ech)[i];
    *x += ed.get_x();
    *y += ed.get_y();
  }
  float s = 1.0f / float( n );
  *x *= s;
  *y *= s;
}

void bgui_vsol_soview2D_new_digital_curve::translate( float x , float y )
{
  //get the edgel chain
  vdgl_interpolator_sptr itrp = this->digital_curve()->get_interpolator();
  vdgl_edgel_chain_sptr ech = itrp->get_edgel_chain();

  unsigned int n = ech->size();

  for (unsigned int i=0; i<n;i++)
  {
    vdgl_edgel ed = (*ech)[i];
    ed.set_x( ed.get_x() + x );
    ed.set_y( ed.get_y() + y );
  }
}


//--------------------------------------------------------------------------
//: vsol_polygon_2d view
//--------------------------------------------------------------------------


bgui_vsol_soview2D_new_polygon::bgui_vsol_soview2D_new_polygon(vsol_polygon_2d_sptr const& e)
: bgui_vsol_soview2D_new(e.ptr())
{
}

vsol_polygon_2d_sptr bgui_vsol_soview2D_new_polygon::sptr() const
{
  return (vsol_polygon_2d*)(sptr_.ptr());
}

//XXX
// This is why i have moved these files, so i could create my own draw_select for the polygon
// such that selection will work when INSIDE the polygon (this, BTW is how it should work
// in all cases, makes no sense to do it the same as a polyLINE when this is a POLYGON (not line!)
void bgui_vsol_soview2D_new_polygon::draw_select() const {
    //vcl_cout << "inside polygon draw select!" << vcl_endl;
  unsigned int n = sptr()->size();

  //yup, all i did was change from GL_LINE_STRIP to GL_POLYGON
  glBegin( GL_POLYGON );
  for (unsigned int i=0; i<n;i++)
  {
    glVertex2f( sptr()->vertex(i)->x() , sptr()->vertex(i)->y() );
  }
  glEnd();
}
//XXX

void bgui_vsol_soview2D_new_polygon::draw() const
{
    //DEBUG
    //vcl_cout << "inside polygon and drawing!" << vcl_endl;
    

  unsigned int n = sptr()->size();

  glBegin( GL_LINE_STRIP );
  for (unsigned int i=0; i<n;i++)
  {
      //vcl_cout << "drawing: (" << sptr()->vertex(i)->x() << ", " << sptr()->vertex(i)->y() << ")" << vcl_endl;
    glVertex2f( sptr()->vertex(i)->x() , sptr()->vertex(i)->y() );
  }
  glEnd();
}

float bgui_vsol_soview2D_new_polygon::distance_squared( float x , float y ) const
{
  unsigned int n = sptr()->size();

  float* xptr = new float[n];
  float* yptr = new float[n];
  for (unsigned int i=0; i<n;i++)
  {
    xptr[i]=sptr()->vertex(i)->x();
    yptr[i]=sptr()->vertex(i)->y();
  }

  double tmp = vgl_distance_to_closed_polygon( xptr , yptr , n , x , y );

  delete [] xptr;
  delete [] yptr;

  return tmp * tmp;
}

void bgui_vsol_soview2D_new_polygon::get_centroid( float* x, float* y ) const
{
  unsigned int n = sptr()->size();

  *x = 0;
  *y = 0;

  for (unsigned int i=0; i<n;i++)
  {
    *x += sptr()->vertex(i)->x();
    *y += sptr()->vertex(i)->y();
  }
  float s = 1.0f / float( n );
  *x *= s;
  *y *= s;
}

void bgui_vsol_soview2D_new_polygon::translate( float x , float y )
{
  unsigned int n = sptr()->size();

  for (unsigned int i=0; i<n;i++)
  {
    sptr()->vertex(i)->set_x( sptr()->vertex(i)->x() + x );
    sptr()->vertex(i)->set_y( sptr()->vertex(i)->y() + y );
  }
}

