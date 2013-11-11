//this is /contrib/bm/dvrml/dvrml_shape.h
#ifndef dvrml_transformation_h_
#define dvrml_transformation_h_
//:
// \file
// \date August 9, 2010
// \author Brandon A. Mayer
//
// Base class for vrml shapes
//
// \verbatim
//  Modifications
// \endverbatim

#include"dvrml_appearance_sptr.h"

#include<vbl/vbl_ref_count.h>

#include<vcl_cassert.h>
#include<vcl_ostream.h>
#include<vcl_string.h>

#include<vgl/vgl_point_3d.h>

#include<vnl/vnl_vector_fixed.h>

class dvrml_shape:public vbl_ref_count
{
public:
    dvrml_shape( vgl_point_3d<double> c = vgl_point_3d<double>(0.0,0.0,0.0),
                 dvrml_appearance_sptr ap_sptr = new dvrml_appearance,
                 vnl_vector_fixed<double,4> xyz_rot = vnl_vector_fixed<double,4>(1.0,0.0,0.0,0.0),
                 vnl_vector_fixed<double,3> xyz_sca = vnl_vector_fixed<double,3>(1.0,1.0,1.0),
                 vnl_vector_fixed<double,3> xyz_rat = vnl_vector_fixed<double,3>(1.0,1.0,1.0) ):
                 center(c), appearance_sptr(ap_sptr), xyz_rotation(xyz_rot), xyz_scale(xyz_sca), xyz_ratios(xyz_rat){}

    ~dvrml_shape(){}

    virtual void draw( vcl_ostream& os ) = 0;

    virtual vcl_string shape() = 0;

    vgl_point_3d<double> center;
    dvrml_appearance_sptr appearance_sptr;
    vnl_vector_fixed<double,4> xyz_rotation;
    vnl_vector_fixed<double,3> xyz_scale;
    vnl_vector_fixed<double,3> xyz_ratios;
};

#endif //dvrml_transformation_h_