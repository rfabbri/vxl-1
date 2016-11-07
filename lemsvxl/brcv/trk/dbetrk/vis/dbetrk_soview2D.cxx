//:
// \file
#include <dbetrk/vis/dbetrk_soview2D.h>
#include <dbetrk/dbetrk_edge_sptr.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vgui/vgui_gl.h>
#include <vgui/vgui_soview2D.h>
#include<dbetrk/dbetrk_edge.h>


// Constructor
dbetrk_soview2D::dbetrk_soview2D( dbetrk_edge_sptr const & c)
 : dbetrk_edge_sptr_(c)
{}


//: Render the ctrk_curve on the display.
void
dbetrk_soview2D::draw() const
{
    //glPointSize( 3.0 );
    glBegin( GL_POINTS );
    glVertex2f(dbetrk_edge_sptr_->point_.x(), dbetrk_edge_sptr_->point_.y());
    glEnd();
}


//: Print details about this ctrk_tracker_curve to the given stream.
vcl_ostream&
dbetrk_soview2D::print(vcl_ostream&s) const
{
  return s << "[ a _edgetrk_ ]";
}



float dbetrk_soview2D::distance_squared(float x, float y) const
{
 return 0.0;
}

void dbetrk_soview2D::get_centroid(float* x, float* y) const 
{
 return ;
}

void dbetrk_soview2D::translate(float x, float y)  
{
 return ;
}
