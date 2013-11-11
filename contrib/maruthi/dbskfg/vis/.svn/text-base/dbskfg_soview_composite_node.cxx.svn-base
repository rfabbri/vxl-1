// This is dbsksp/vis/dbsksp_soview_shock.cxx

//:
// \file


#include <dbskfg/vis/dbskfg_soview_composite_node.h>
#include <dbskfg/dbskfg_composite_node.h>
#include <dbskfg/dbskfg_contour_node.h>
#include <dbskfg/dbskfg_shock_node.h>

//--------------------------------------------------------------------------
// dbskfg_soview_composite_node
//-------------------------------------------------------------------------


//: Constructor
dbskfg_soview_composite_node::
dbskfg_soview_composite_node(const dbskfg_composite_node_sptr& node)
    :vgui_soview2D(),
     node_(node)
{

}

//: Destructor
dbskfg_soview_composite_node::~dbskfg_soview_composite_node()
{

}

//: Render this composite node the display.
void dbskfg_soview_composite_node::
draw() const
{
  glBegin(GL_POINTS);
  glVertex2f(node_->pt().x(), node_->pt().y());
  glEnd();
}


//: Returns the distance squared of this soview2D from the given position.
float dbskfg_soview_composite_node::
distance_squared(float x, float y) const
{
  float dx = node_->pt().x()-x;
  float dy = node_->pt().y()-y;
  return dx*dx + dy*dy;
}

//: Returns the centroid of this soview2D.
void dbskfg_soview_composite_node::
get_centroid(float* x, float* y) const
{
  *x = node_->pt().x();
  *y = node_->pt().y();
}


//: Print details about this vsol object to the given stream.
vcl_ostream& dbskfg_soview_composite_node::
print(vcl_ostream& os) const
{
    node_->print(os);
    return os;
}
