//this is /contrib/bm/dvrml/dvrml_point.h
#ifndef dvrml_point_h_
#define dvrml_point_h_
//:
// \file
// \date August 9, 2010
// \author Brandon A. Mayer
//
// A vrml point. Knows how to write itself to vrml stream.
//
// \verbatim
//  Modifications
// \endverbatim

#include"dvrml_shape.h"

class dvrml_point: public dvrml_shape
{
public:
    //dvrml_point(){}

    dvrml_point( vgl_point_3d<double> const& pt_center = vgl_point_3d<double>(0.0,0.0,0.0),
                 dvrml_appearance_sptr ap_sptr = new dvrml_appearance,
                 double const& pt_radius = 1.0):radius(pt_radius){ center = pt_center; appearance_sptr = ap_sptr; }

    ~dvrml_point(){}

    virtual void draw( vcl_ostream& os );

    vcl_string shape(){ return "point"; }

    double radius;
    
};

#endif //dvrml_point_h_