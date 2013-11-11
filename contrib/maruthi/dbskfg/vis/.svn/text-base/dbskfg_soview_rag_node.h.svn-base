// This is brcv/shp/dbskfg/vis/dbskfg_soview_rag_node.h
#ifndef dbskfg_soview_rag_node_h_
#define dbskfg_soview_rag_node_h_
//------------------------------------------------------------------------------
//:
// \file
// \brief soview for rag node objects
// \author Maruthi Narayanan
// \date July 07, 2010
//
// \verbatim
//  Modifications:
// \endverbatim
//------------------------------------------------------------------------------


#include <vgui/vgui_soview2D.h>
#include <dbskfg/dbskfg_rag_node_sptr.h>
#include <dbskfg/dbskfg_rag_node.h>
#include <vgl/vgl_polygon.h>
#include <vil/vil_image_resource_sptr.h>
#include <dbskfg/dbskfg_utilities.h>

class dbskfg_soview_rag_node : public vgui_soview2D
{
 public:
  
    //: Constructor
    dbskfg_soview_rag_node(const dbskfg_rag_node_sptr& node);

    //: Destructor
    /* virtual */ ~dbskfg_soview_rag_node();

    //: Returns the distance squared of this soview2D from the given position.
    /* virtual */
    float distance_squared(float x, float y) const;
 
    //: Returns the centroid of this soview2D.
    /* virtual */
    void get_centroid(float* x, float* y) const;

    //: Translate this node by the given x and y distances.
    /* virtual */
    void translate(float x, float y) {}

    //: Render this node on the display.
    /* virtual */ 
    void draw() const;

    //: Returns the type of this class
    /* virtual */ 
    vcl_string type_name() const {return "dbskfg_soview_rag_node"; }
 
    //: Print details about this object to the given stream.
    /* virtual */ 
    vcl_ostream& print(vcl_ostream& os) const;
 
    //: Get id of rag node
    unsigned int rag_node_id(){return node_->id();}

    //: Save image
    void save_image(vil_image_resource_sptr image)
    {dbskfg_utilities::save_image_poly(poly_,image,"fragment.png");}
    
protected:

    // Region Adjancency Node
    dbskfg_rag_node_sptr node_;

    // Polygon representing node
    vgl_polygon<double> poly_;

};


#endif // dbskfg_soview_rag_node_h_
