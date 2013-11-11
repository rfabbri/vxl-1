//this is /contrib/bm/dvrml/dvrml_cylinder.h
#ifndef dvrml_cylinder_h_
#define dvrml_cylinder_h_

#include"dvrml_shape.h"

class dvrml_cylinder:public dvrml_shape
{
public:
    dvrml_cylinder( vgl_point_3d<double> const& cylinder_center = vgl_point_3d<double>(0.0,0.0,0.0),
                    double const& cylinder_radius = 1.0,
                    double const& cylinder_height = 1.0,
                    dvrml_appearance_sptr ap_sptr = new dvrml_appearance,
                    vnl_vector_fixed<double,4> const& cylinder_xyz_rotation = vnl_vector_fixed<double,4>(1.0,0.0,0.0,0.0),
                    vnl_vector_fixed<double,3> const& cylinder_xyz_scale = vnl_vector_fixed<double,3>(1.0,1.0,1.0),
                    vnl_vector_fixed<double,3> const& cylinder_xyz_ratios = vnl_vector_fixed<double,3>(1.0,1.0,1.0) ):
                    radius(cylinder_radius), height(cylinder_height)
    { center = cylinder_center; appearance_sptr = ap_sptr; xyz_rotation = cylinder_xyz_rotation; xyz_scale = cylinder_xyz_scale; xyz_ratios = cylinder_xyz_ratios; }


    ~dvrml_cylinder(){}

    virtual void draw( vcl_ostream& os );

    virtual vcl_string shape(){ return "cylinder"; }

    double radius;
    double height;

};

#endif //dvrml_cylinder_h_