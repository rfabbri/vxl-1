//this is /contrib/bm/dvrml/dvrml.h
#ifndef dvrml_h_
#define dvrml_h_
//:
// \file
// \date August 18, 2010
// \author Brandon A. Mayer
//
// Simple api to draw shapes in VRML 2.0. Some concepts taken from oxl vrml_out class
//
// \verbatim
//  Modifications
// \endverbatim

#include"dvrml_shape_sptr.h"
#include"dvrml_box_sptr.h"
#include"dvrml_cone_sptr.h"
#include"dvrml_cylinder_sptr.h"
#include"dvrml_point_sptr.h"
#include"dvrml_indexed_lineset_sptr.h"

#include<vcl_cassert.h> //the macro is called assert(expression)
#include<vcl_iostream.h>


// Note that a rotation is defined in vrml as a 4-tuple whereas the first three filds indicate which axis to define the rotation around.
// These values are arbitrary: 200 is just as good as 1 for any of these fields and will only indicate that we are rotating about the given axis.
// We use the x,y,z ordering convention. The 4th value is the angle of rotation in RADIANS!!!
//
// Some Helpful radian values:
// Fraction         Degrees         Radians(pi)         Decimal
//    1/24            15              pi/12              0.262
//    1/12            30              pi/6               0.524
//    1/8             45              pi/4               0.785
//    1/6             60              pi/3               1.05
//    1/4             90              pi/2               1.57
//    1/2            180              pi                 3.14

#define DVRML_SETUP if (of_stream_ptr_ == 0){ vcl_cerr << "dvrml -- vcl_ostream not set!\n"; return; } vcl_ofstream& os = *of_stream_ptr_

class dvrml
{
public: 
    //dvrml():of_stream_ptr_(NULL){}
    //dvrml( vcl_ofstream& os ):of_stream_ptr_(&os){}

    dvrml( vcl_ofstream& os ):of_stream_ptr_(&os){ preamble(); }

    ~dvrml(){}

    void draw( dvrml_shape_sptr shape ){ DVRML_SETUP; shape->draw(os); }

    //input a single number between 1 and 0 and output an rgb value
    static vnl_vector_fixed<double,3> color_map(double const& input);
    
    static vnl_vector_fixed<double,3> color_map(unsigned const& input){ return dvrml::color_map(double(input)); }
    
    //void draw_point( vgl_point_2d<unsigned> const& pt, dvrml_appearance_sptr appearance_sptr = new dvrml_appearance, double const& radius = 1 );

    //void draw_point( vgl_point_2d<double> const& pt, dvrml_appearance_sptr appearance_sptr = new dvrml_appearance, double const& radius = 1 );

    //void draw_point( vgl_point_3d<double> const& pt, dvrml_appearance_sptr appearance_sptr = new dvrml_appearance, double const& radius = 1 );

    //void draw_points( vcl_vector<vgl_point_3d<double> > const& pts, dvrml_appearance_sptr appearance_sptr = new dvrml_appearance, double const& radius = 1 );

    //void draw_box( vgl_point_3d<double> const& center, 
    //               dvrml_appearance_sptr appearance_sptr = new dvrml_appearance, 
    //               vnl_vector_fixed<double,3> const& xyz_ratios = vnl_vector_fixed<double,3>(1.0,1.0,1.0),
    //               vnl_vector_fixed<double,4> const& xyz_rotation = vnl_vector_fixed<double,4>(1.0,0.0,0.0,0.0),
    //               vnl_vector_fixed<double,3> const& xyz_scale = vnl_vector_fixed<double,3>(1.0,1.0,1.0) );
    //                        

    //void draw_box( vgl_point_2d<double> const& center, 
    //               dvrml_appearance_sptr appearance_sptr = new dvrml_appearance, 
    //               vnl_vector_fixed<double,3> const& xyz_ratios = vnl_vector_fixed<double,3>(1.0,1.0,1.0),
    //               vnl_vector_fixed<double,4> const& xyz_rotation = vnl_vector_fixed<double,4>(1.0,0.0,0.0,0.0),
    //               vnl_vector_fixed<double,3> const& xyz_scale = vnl_vector_fixed<double,3>(1.0,1.0,1.0) );

    //void draw_box( vgl_point_2d<unsigned> const& center, 
    //               dvrml_appearance_sptr appearance_sptr, 
    //               vnl_vector_fixed<double,3> const& xyz_ratios = vnl_vector_fixed<double,3>(1.0,1.0,1.0), 
    //               vnl_vector_fixed<double,4> const& xyz_rotation = vnl_vector_fixed<double,4>(1.0,0.0,0.0,0.0),
    //               vnl_vector_fixed<double,3> const& xyz_scale = vnl_vector_fixed<double,3>(1.0,1.0,1.0) );

    //void draw_cylinder( vgl_point_3d<double> const& center, 
    //                    dvrml_appearance_sptr appearance_sptr, 
    //                    vnl_vector_fixed<double,2> const& radius_height = vnl_vector_fixed<double,2>(1.0,1.0),
    //                    vnl_vector_fixed<double,4> const& xyz_rotation = vnl_vector_fixed<double,4>(1.0,0.0,0.0,0.0),
    //                    vnl_vector_fixed<double,3> const& xyz_scale = vnl_vector_fixed<double,3>(1.0,1.0,1.0) );

    //void draw_cone( vgl_point_3d<double> const& bottom_center, 
    //                dvrml_appearance_sptr appearance_sptr, 
    //                vnl_vector_fixed<double,2> const& bottomRadius_height = vnl_vector_fixed<double,2>(1.0,1.0),
    //                vnl_vector_fixed<double,4> const& xyz_rotation = vnl_vector_fixed<double,4>(1.0,0.0,0.0,0.0),
    //                vnl_vector_fixed<double,3> const& xyz_scale = vnl_vector_fixed<double,3>(1.0,1.0,1.0) );

    ////just a thin cube could use indexed face set
    //void draw_plane( vgl_point_3d<double> const& center, 
    //                 dvrml_appearance_sptr appearance_sptr = new dvrml_appearance,
    //                 vnl_vector_fixed<double,2> const& xy_ratios = vnl_vector_fixed<double,2>(1.0,1.0), 
    //                 vnl_vector_fixed<double,4> const& xyz_rotation = vnl_vector_fixed<double,4>(1.0,0.0,0.0,0.0),
    //                 vnl_vector_fixed<double,3> const& xyz_scale = vnl_vector_fixed<double,3>(5.0,5.0,5.0) );

    //void draw_text( vcl_string const& text,
    //                vgl_point_3d<double> const& center = vgl_point_3d<double>(0.0,0.0,0.0),
    //                dvrml_appearance_sptr appearance_sptr = new dvrml_appearance,
    //                double const& length = 1,
    //                vnl_vector_fixed<double,4> const& xyz_rotation = vnl_vector_fixed<double,4>(1.0,0.0,0.0,0.0),
    //                vnl_vector_fixed<double,3> const& xyz_scale = vnl_vector_fixed<double,3>(1.0,1.0,1.0) );
                    

    /*void draw_axis();*/

protected:

    dvrml():of_stream_ptr_(NULL){}
    void preamble(){ DVRML_SETUP; os << "#VRML V2.0 utf8\n\n";}
    vcl_ofstream* of_stream_ptr_;
};

#endif //dvrml_h_