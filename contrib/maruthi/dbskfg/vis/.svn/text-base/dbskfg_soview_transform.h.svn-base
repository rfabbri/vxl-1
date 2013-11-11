// This is brcv/shp/dbskfg/vis/dbskfg_soview_transform.h
#ifndef dbskfg_soview_transform_h_
#define dbskfg_soview_transform_h_
//------------------------------------------------------------------------------
//:
// \file
// \brief soview for transform objects
// \author Maruthi Narayanan
// \date July 28, 2010
//
// \verbatim
//  Modifications:
// \endverbatim
//------------------------------------------------------------------------------


#include <vgui/vgui_soview2D.h>
#include <dbskfg/algo/dbskfg_transform_descriptor_sptr.h>
#include <vgl/vgl_polygon.h>

class dbskfg_soview_transform : public vgui_soview2D
{
 public:
  
    //: Constructor
    dbskfg_soview_transform(const dbskfg_transform_descriptor_sptr& transform);

    //: Destructor
    /* virtual */ ~dbskfg_soview_transform();

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
    vcl_string type_name() const {return "dbskfg_soview_transform"; }

    //: Returns the transform associated with this class
    dbskfg_transform_descriptor_sptr& transform(){return transform_;}
 
    //: Print details about this object to the given stream.
    /* virtual */ 
    vcl_ostream& print(vcl_ostream& os) const;
    

protected:
 
    // Holds the transform object
    dbskfg_transform_descriptor_sptr transform_;


};


#endif // dbskfg_soview_transform_h_
