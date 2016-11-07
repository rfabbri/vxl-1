//this is /contrib/bm/dvrml/dvrml_cone.h
#ifndef dvrml_cone_h_
#define dvrml_cone_h_

#include"dvrml_shape.h"

class dvrml_cone: public dvrml_shape
{
public:
    dvrml_cone( vgl_point_3d<double> const& cone_center = vgl_point_3d<double>(0.0,0.0,0.0), 
                double const& cone_bottom_radius = 1.0, 
                double const& cone_height = 1.0,
                dvrml_appearance_sptr ap_sptr = new dvrml_appearance,
                vnl_vector_fixed<double,4> const& cone_xyz_rotation = vnl_vector_fixed<double,4>(1.0,0.0,0.0,0.0),
                vnl_vector_fixed<double,3> const& cone_xyz_scale = vnl_vector_fixed<double,3>(1.0,1.0,1.0),
                vnl_vector_fixed<double,3> const& cone_xyz_ratios = vnl_vector_fixed<double,3>(1.0,1.0,1.0) ):
                bottom_radius(cone_bottom_radius),height(cone_height)
     { center = cone_center; xyz_rotation = cone_xyz_rotation; xyz_scale = cone_xyz_scale; xyz_ratios = cone_xyz_ratios; }

    ~dvrml_cone(){}

    virtual void draw( vcl_ostream& os );

    virtual vcl_string shape(){ return "cone"; }

    double bottom_radius;
    double height;
};

#endif //dvrml_cone_h_