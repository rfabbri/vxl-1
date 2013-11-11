// This is brcv/shp/dbskfg/vis/dbskfg_soview_composite_link.h
#ifndef dbskfg_soview_composite_link_h_
#define dbskfg_soview_composite_link_h_
//------------------------------------------------------------------------------
//:
// \file
// \brief soview for composite link objects shocks and contours
// \author Maruthi Narayanan
// \date July 07, 2010
//
// \verbatim
//  Modifications:
// \endverbatim
//------------------------------------------------------------------------------

#include <vgui/vgui_soview2D.h>
#include <vgl/vgl_point_2d.h>

class dbskfg_contour_link;
class dbskfg_shock_link;

class dbskfg_soview_contour_link : public vgui_soview2D
{
 public:
  
    //: Constructor
    dbskfg_soview_contour_link(dbskfg_contour_link* link);

    //: Destructor
    /* virtual */ ~dbskfg_soview_contour_link();

    //: Returns the distance squared of this soview2D from the given position.
    /* virtual */
    float distance_squared(float x, float y) const;
 
    //: Returns the centroid of this soview2D.
    /* virtual */
    void get_centroid(float* x, float* y) const;

    //: Translate this link by the given x and y distances.
    /* virtual */
    void translate(float x, float y) {}

    //: Render this link on the display.
    /* virtual */ 
    void draw() const;

    //: Returns the type of this class
    /* virtual */ 
    vcl_string type_name() const {return "dbskfg_soview_contour_link"; }
 
    //: Print details about this object to the given stream.
    /* virtual */ 
    vcl_ostream& print(vcl_ostream& os) const;
  
protected:

    // Keep track of link
    dbskfg_contour_link* link_;

    //Get start and end
    vgl_point_2d<double> start_;

    // Get end
    vgl_point_2d<double> end_;

};


class dbskfg_soview_shock_link : public vgui_soview2D
{
 public:
  
    //: Constructor
    dbskfg_soview_shock_link(dbskfg_shock_link* link);

    //: Destructor
    /* virtual */ ~dbskfg_soview_shock_link();

    //: Returns the distance squared of this soview2D from the given position.
    /* virtual */
    float distance_squared(float x, float y) const;
 
    //: Returns the centroid of this soview2D.
    /* virtual */
    void get_centroid(float* x, float* y) const;

    //: Translate this link by the given x and y distances.
    /* virtual */
    void translate(float x, float y) {}

    //: Render this link on the display.
    /* virtual */ 
    void draw() const;

    //: Returns the type of this class
    /* virtual */ 
    vcl_string type_name() const {return "dbskfg_soview_shock_link"; }
 
    //: Print details about this object to the given stream.
    /* virtual */ 
    vcl_ostream& print(vcl_ostream& os) const;
  
protected:

    // Keep track of link
    dbskfg_shock_link* link_;

    //Get start and end
    vgl_point_2d<double> start_;

    // Get end
    vgl_point_2d<double> end_;

};



class dbskfg_soview_shock_ray : public vgui_soview2D
{
 public:
  
    //: Constructor
    dbskfg_soview_shock_ray(dbskfg_shock_link* link);

    //: Destructor
    /* virtual */ ~dbskfg_soview_shock_ray();

    //: Returns the distance squared of this soview2D from the given position.
    /* virtual */
    float distance_squared(float x, float y) const;
 
    //: Returns the centroid of this soview2D.
    /* virtual */
    void get_centroid(float* x, float* y) const;

    //: Translate this link by the given x and y distances.
    /* virtual */
    void translate(float x, float y) {}

    //: Render this link on the display.
    /* virtual */ 
    void draw() const;

    //: Returns the type of this class
    /* virtual */ 
    vcl_string type_name() const {return "dbskfg_soview_shock_ray"; }
 
    //: Print details about this object to the given stream.
    /* virtual */ 
    vcl_ostream& print(vcl_ostream& os) const;
  
protected:

    // Keep track of link
    dbskfg_shock_link* link_;

    //Get start and end
    vgl_point_2d<double> start_;

    // Get end
    vgl_point_2d<double> end_;

};


#endif // dbskfg_soview_composite_link_h_
