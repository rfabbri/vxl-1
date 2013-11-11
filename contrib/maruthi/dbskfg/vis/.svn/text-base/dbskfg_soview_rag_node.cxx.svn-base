// This is dbsksp/vis/dbsksp_soview_shock.cxx

//:
// \file

// dbskfg headers
#include <dbskfg/vis/dbskfg_soview_rag_node.h>
#include <dbskfg/dbskfg_rag_node.h>
// vgl headers
#include <vgl/vgl_distance.h>
#include <vgl/vgl_area.h>

//--------------------------------------------------------------------------
// dbskfg_soview_rag_node
//-------------------------------------------------------------------------


//: Constructor
dbskfg_soview_rag_node::
dbskfg_soview_rag_node(const dbskfg_rag_node_sptr& node)
    :vgui_soview2D(),
     node_(node),
     poly_()
{
    node_->fragment_boundary(poly_);
 
}

//: Destructor
dbskfg_soview_rag_node::~dbskfg_soview_rag_node()
{

}

//: Render this rag node the display.
void dbskfg_soview_rag_node::
draw() const
{

    for (unsigned int s = 0; s < poly_.num_sheets(); ++s)
    { 
        glBegin(GL_LINE_LOOP);
        for (unsigned int p = 0; p < poly_[s].size(); ++p)
        {
          
            glVertex2f(poly_[s][p].x(), poly_[s][p].y());
  
        }
        glEnd();
    } 
 
}


//: Returns the distance squared of this soview2D from the given position.
float dbskfg_soview_rag_node::
distance_squared(float x, float y) const
{
    vgl_point_2d<double> point(x,y);
    return vgl_distance(poly_,point);

}

//: Returns the centroid of this soview2D.
void dbskfg_soview_rag_node::
get_centroid(float* x, float* y) const
{
    vgl_point_2d<double> center = vgl_centroid(poly_);
    *x = center.x();
    *y = center.y();
}


//: Print details about this vsol object to the given stream.
vcl_ostream& dbskfg_soview_rag_node::
print(vcl_ostream& os) const
{
    node_->print(os);
    return os;
}

