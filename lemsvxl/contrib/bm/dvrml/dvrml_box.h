//this is /contrib/bm/dvrml/dvrml_box.h
#ifndef dvrml_box_h_
#define dvrml_box_h_

#include"dvrml_shape.h"

class dvrml_box: public dvrml_shape
{
public:
    dvrml_box( vgl_point_3d<double> const& box_center = vgl_point_3d<double>(0.0,0.0,0.0),
               dvrml_appearance_sptr ap_sptr = new dvrml_appearance,
               vnl_vector_fixed<double,4> const& box_xyz_rotation = vnl_vector_fixed<double,4>(1.0,0.0,0.0,0.0),
               vnl_vector_fixed<double,3> const& box_xyz_scale = vnl_vector_fixed<double,3>(1.0,1.0,1.0),
               vnl_vector_fixed<double,3> const& box_xyz_ratios = vnl_vector_fixed<double,3>(1.0,1.0,1.0) )
    { center = box_center; appearance_sptr = ap_sptr; xyz_rotation = box_xyz_rotation; xyz_scale = box_xyz_scale; xyz_ratios = box_xyz_ratios; }

    ~dvrml_box(){}

    virtual void draw( vcl_ostream& os );

    virtual vcl_string shape(){ return "box"; }
                
};

#endif //dvrml_box_h_