//:
// \file
#include "dbctrk_soview2d.h"
#include <dbctrk/dbctrk_tracker_curve_sptr.h>
#include <dbctrk/dbctrk_tracker_curve.h>

#include <vcl_iostream.h>
#include <vcl_vector.h>

#include <vgui/vgui_gl.h>
#include <vgui/vgui_soview2D.h>


// Constructor
dbctrk_soview2D::dbctrk_soview2D( dbctrk_tracker_curve_sptr const & c)
 : dbctrk_sptr_(c)
{}


//: Render the dbctrk_curve on the display.
void
dbctrk_soview2D::draw() const
{
  //vcl_cout<<"\n drawing curve num : "<<dbctrk_sptr_->get_id();
  glBegin(GL_LINE_STRIP);
  for(int i=0;i<dbctrk_sptr_->desc->curve_->numPoints();i++)
    {
      glVertex2f(dbctrk_sptr_->desc->curve_->point(i).x(),dbctrk_sptr_->desc->curve_->point(i).y());
    }
  glEnd();
}


//: Print details about this dbctrk_tracker_curve to the given stream.
vcl_ostream&
dbctrk_soview2D::print(vcl_ostream&s) const
{
  return s << "[ a _dbctrk_ ]";
}



float dbctrk_soview2D::distance_squared(float x, float y) const
{
 return 0.0;
}

void dbctrk_soview2D::get_centroid(float* x, float* y) const 
{
 return ;
}

void dbctrk_soview2D::translate(float x, float y)  
{
 return ;
}
