// This is brcv/shp/dbskfg/vis/dbskfg_soview_composite_node.h
#ifndef dbskfg_soview_composite_node_h_
#define dbskfg_soview_composite_node_h_
//------------------------------------------------------------------------------
//:
// \file
// \brief soview for composite node objects
// \author Maruthi Narayanan
// \date July 07, 2010
//
// \verbatim
//  Modifications:
// \endverbatim
//------------------------------------------------------------------------------


#include <vgui/vgui_soview2D.h>
#include <dbskfg/dbskfg_composite_node_sptr.h>

class dbskfg_soview_composite_node : public vgui_soview2D
{
 public:
  
    //: Constructor
    dbskfg_soview_composite_node(const dbskfg_composite_node_sptr& node);

    //: Destructor
    /* virtual */ ~dbskfg_soview_composite_node();

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
    vcl_string type_name() const {return "dbskfg_soview_composite_node"; }
 
    //: Print details about this object to the given stream.
    /* virtual */ 
    vcl_ostream& print(vcl_ostream& os) const;
 
 
protected:
 
    dbskfg_composite_node_sptr node_;

};


#endif // dbskfg_soview_composite_node_h_
