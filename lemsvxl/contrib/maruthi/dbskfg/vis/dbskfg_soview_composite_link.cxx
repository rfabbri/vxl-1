// This is dbsksp/vis/dbsksp_soview_shock.cxx

//:
// \file


#include <dbskfg/vis/dbskfg_soview_composite_link.h>
#include <dbskfg/dbskfg_composite_link.h>
#include <dbskfg/dbskfg_composite_node.h>
#include <dbskfg/dbskfg_shock_link.h>
#include <dbskfg/dbskfg_contour_link.h>

#include <vgl/vgl_distance.h>
#include <vgl/vgl_point_2d.h>


//--------------------------------------------------------------------------
// dbskfg_soview_contour_link
//-------------------------------------------------------------------------


//: Constructor
dbskfg_soview_contour_link::
dbskfg_soview_contour_link(dbskfg_contour_link* link)
    :vgui_soview2D(),
     link_(link)
{

    start_=(*link_->source()).pt();
    end_  =(*link_->target()).pt();

}

//: Destructor
dbskfg_soview_contour_link::~dbskfg_soview_contour_link()
{

}

//: Render this contour link the display.
void dbskfg_soview_contour_link::
draw() const
{
  glBegin(GL_LINES);
  glVertex2f(start_.x(), start_.y());
  glVertex2f(end_.x(), end_.y());
  glEnd();
  return;

}


//: Returns the distance squared of this soview2D from the given position.
float dbskfg_soview_contour_link::
distance_squared(float x, float y) const
{
    // only use the middle 3/5 of the chord to find closest point so that the
    // nodes can be picked
    vgl_point_2d<double > p0 = midpoint(start_, end_, 0.2);
    vgl_point_2d<double > p2 = midpoint(start_, end_, 0.8);

    return 
        vgl_distance2_to_linesegment(p0.x(), p0.y(), p2.x(), p2.y(), 
                                     static_cast<double>(x), 
                                     static_cast<double>(y));

}

//: Returns the centroid of this soview2D.
void dbskfg_soview_contour_link::
get_centroid(float* x, float* y) const
{
  *x = (start_.x() + end_.x())/2;
  *y = (start_.y() + end_.y())/2;
  return;
}


//: Print details about this vsol object to the given stream.
vcl_ostream& dbskfg_soview_contour_link::
print(vcl_ostream& os) const
{

    link_->print(os);
    return os;
}



//--------------------------------------------------------------------------
// dbskfg_soview_shock_link
//-------------------------------------------------------------------------

//: Constructor
dbskfg_soview_shock_link::
dbskfg_soview_shock_link(dbskfg_shock_link* link)
    :vgui_soview2D(),
     link_(link)
{

    start_=(*link_->source()).pt();
    end_  =(*link_->target()).pt();

}

//: Destructor
dbskfg_soview_shock_link::~dbskfg_soview_shock_link()
{

}

//: Render this shock link the display.
void dbskfg_soview_shock_link::
draw() const
{

    glBegin( GL_LINE_STRIP );
    for( unsigned int i = 0 ; i < link_->ex_pts().size() ; i++ ) {
      glVertex2f( link_->ex_pts()[i].x() , link_->ex_pts()[i].y() );
    }
    glEnd();

}


//: Returns the distance squared of this soview2D from the given position.
float dbskfg_soview_shock_link::
distance_squared(float x, float y) const
{

    // only use the middle 3/5 of the chord to find closest point so that the
    // nodes can be picked
    vgl_point_2d<double > p0 = midpoint(start_, end_, 0.2);
    vgl_point_2d<double > p2 = midpoint(start_, end_, 0.8);

    return 
        vgl_distance2_to_linesegment(p0.x(), p0.y(), p2.x(), p2.y(), 
                                     static_cast<double>(x), 
                                     static_cast<double>(y));

}

//: Returns the centroid of this soview2D.
void dbskfg_soview_shock_link::
get_centroid(float* x, float* y) const
{
  *x = (start_.x() + end_.x())/2;
  *y = (start_.y() + end_.y())/2;
  return;
}


//: Print details about this vsol object to the given stream.
vcl_ostream& dbskfg_soview_shock_link::
print(vcl_ostream& os) const
{

    link_->print(os);
    return os;
}


//--------------------------------------------------------------------------
// dbskfg_soview_shock_ray
//-------------------------------------------------------------------------

//: Constructor
dbskfg_soview_shock_ray::
dbskfg_soview_shock_ray(dbskfg_shock_link* link)
    :vgui_soview2D(),
     link_(link)
{

    start_=(*link_->source()).pt();
    end_  =(*link_->target()).pt();

}

//: Destructor
dbskfg_soview_shock_ray::~dbskfg_soview_shock_ray()
{

}

//: Render this shock ray the display.
void dbskfg_soview_shock_ray::
draw() const
{
    glBegin(GL_LINES);
    glVertex2f(start_.x(), start_.y());
    glVertex2f(end_.x(), end_.y());
    glEnd();

}


//: Returns the distance squared of this soview2D from the given position.
float dbskfg_soview_shock_ray::
distance_squared(float x, float y) const
{

    // only use the middle 3/5 of the chord to find closest point so that the
    // nodes can be picked
    vgl_point_2d<double > p0 = midpoint(start_, end_, 0.2);
    vgl_point_2d<double > p2 = midpoint(start_, end_, 0.8);

    return 
        vgl_distance2_to_linesegment(p0.x(), p0.y(), p2.x(), p2.y(), 
                                     static_cast<double>(x), 
                                     static_cast<double>(y));

}

//: Returns the centroid of this soview2D.
void dbskfg_soview_shock_ray::
get_centroid(float* x, float* y) const
{
  *x = (start_.x() + end_.x())/2;
  *y = (start_.y() + end_.y())/2;
  return;
}


//: Print details about this vsol object to the given stream.
vcl_ostream& dbskfg_soview_shock_ray::
print(vcl_ostream& os) const
{
    
    link_->print(os);
    return os;
}
