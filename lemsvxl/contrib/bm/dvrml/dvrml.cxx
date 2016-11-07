//this is /contrib/bm/dvrml/dvrml.cxx
#include"dvrml.h"

//input a single value between 0 and 1 and output an rgb vector
vnl_vector_fixed<double,3> dvrml::color_map(double const& input)
{
    assert( input >= double(0.0) );
    assert( input <= double(1.0) );
    vnl_vector_fixed<double,3> rgb;

    double red,green,blue;

    red = input;
    if(input <= .5)
        green = 2*input;
    else
        green = -2*input + 2;

    blue = -1*input + 1;
    
    rgb[0] = red;
    rgb[1] = green;
    rgb[2] = blue;

    return rgb;
}//end vgl_point_3d<double> dvrml::color_map

//
//#define SETUP if (of_stream_ptr_ == 0) { vcl_cerr << "dvrml -- vcl_ostream not set!\n"; return; } vcl_ofstream& os = *of_stream_ptr_
//
//void dvrml::preamble()
//{
//    SETUP; 
//    os << "#VRML V2.0 utf8\n\n"; 
//}

//void dvrml::draw_point( vgl_point_3d<double> const& pt, dvrml_appearance_sptr appearance_sptr, double const& radius )
//{
//    SETUP;
//
//    os << "#point\n"
//       << "Transform{\n"
//       << "  translation " << pt.x() << ' ' << pt.y() << ' ' << pt.z() << '\n'
//       << "  children [\n"
//       << "     Shape{\n"
//       << "         appearance Appearance {\n"
//       << "             material Material {\n"
//       << "                 diffuseColor " << appearance_sptr->diffuseColor().get(0) << ' ' << appearance_sptr->diffuseColor().get(1) << ' ' << appearance_sptr->diffuseColor().get(2) << '\n'
//       << "                 ambientIntensity " << appearance_sptr->ambientIntensity() << '\n'
//       << "                 emissiveColor " << appearance_sptr->emissiveColor().get(0) << ' ' << appearance_sptr->emissiveColor().get(1) << ' ' << appearance_sptr->emissiveColor().get(2) << '\n'
//       << "                 specularColor " << appearance_sptr->specularColor().get(0) << ' ' << appearance_sptr->specularColor().get(1) << ' ' << appearance_sptr->specularColor().get(2) << '\n'
//       << "                 shininess " << appearance_sptr->shininess() << '\n'
//       << "                 transparency " << appearance_sptr->transparency() << '\n'
//       << "             }# end Material\n"
//       << "         }# end Appearance\n"
//       << "         geometry Sphere{\n"
//       << "             radius " << radius << '\n'
//       << "         } #end geometry\n"
//       << "         } #end Shape\n"
//       << "   ] #end children\n"
//       << "} #end transform\n\n";
//
//}//end dvrml::draw_point( vgl_point_2d<double>, dvrml_appearance_sptr ) main 3d point fnc
//
//void dvrml::draw_point( vgl_point_2d<double> const& pt, dvrml_appearance_sptr appearance_sptr, double const& radius )
//{
//    vgl_point_3d<double> pt3d(pt.x(),pt.y(),double(0.0));
//
//    this->draw_point(pt3d,appearance_sptr,radius);
//}//end dvrml::draw_point 2d point
//
//void dvrml::draw_point( vgl_point_2d<unsigned> const& pt, dvrml_appearance_sptr appearance_sptr, double const& radius )
//{
//    vgl_point_3d<double> pt3d(double(pt.x()),double(pt.y()),double(0.0));
//    this->draw_point(pt3d,appearance_sptr,radius);
//}//end dvrml::draw_point 2d unsigned input
//
//void dvrml::draw_box( vgl_point_3d<double> const& center, 
//                      dvrml_appearance_sptr appearance_sptr,          
//                      vnl_vector_fixed<double,3> const& xyz_ratios,
//                      vnl_vector_fixed<double,4> const& xyz_rotation,
//                      vnl_vector_fixed<double,3> const& xyz_scale)
//{
//    SETUP;
//        os << "#box\n"
//       << "Transform{\n"
//       << "  translation " << center.x() << ' ' << center.y() << ' ' << center.z() << '\n'
//       << "  rotation " << xyz_rotation.get(0) << ' ' << xyz_rotation.get(1) << ' ' << xyz_rotation.get(2) << ' ' << xyz_rotation.get(3) << '\n'
//       << "  scale " << xyz_scale.get(0) << ' ' << xyz_scale.get(1) << ' ' << xyz_scale.get(2) << '\n'
//       << "  children [\n"
//       << "     Shape{\n"
//       << "         appearance Appearance {\n"
//       << "             material Material {\n"
//       << "                 diffuseColor " << appearance_sptr->diffuseColor().get(0) << ' ' << appearance_sptr->diffuseColor().get(1) << ' ' << appearance_sptr->diffuseColor().get(2) << '\n'
//       << "                 ambientIntensity " << appearance_sptr->ambientIntensity() << '\n'
//       << "                 emissiveColor " << appearance_sptr->emissiveColor().get(0) << ' ' << appearance_sptr->emissiveColor().get(1) << ' ' << appearance_sptr->emissiveColor().get(2) << '\n'
//       << "                 specularColor " << appearance_sptr->specularColor().get(0) << ' ' << appearance_sptr->specularColor().get(1) << ' ' << appearance_sptr->specularColor().get(2) << '\n'
//       << "                 shininess " << appearance_sptr->shininess() << '\n'
//       << "                 transparency " << appearance_sptr->transparency() << '\n'
//       << "             }# end Material\n"
//       << "         }# end Appearance\n"
//       << "         geometry Box{\n"
//       << "             size " << xyz_ratios.get(0) << ' ' << xyz_ratios.get(1) << ' ' << xyz_ratios.get(2) << '\n'
//       << "         } #end geometry\n"
//       << "         } #end Shape\n"
//       << "   ] #end children\n"
//       << "} #end transform\n\n";
//
//}//end dvrml::draw_box 3d double center
//
//void dvrml::draw_box( vgl_point_2d<double> const& center, 
//                      dvrml_appearance_sptr appearance_sptr, 
//                      vnl_vector_fixed<double,3> const& xyz_ratios,
//                      vnl_vector_fixed<double,4> const& xyz_rotation, 
//                      vnl_vector_fixed<double,3> const& xyz_scale )
//{
//    vgl_point_3d<double> pt3d(center.x(),center.y(),double(0.0));
//    this->draw_box(pt3d,appearance_sptr,xyz_ratios,xyz_rotation);
//}//end dvrml::draw_box 2d unsigned center
//
//void dvrml::draw_box( vgl_point_2d<unsigned> const& center, 
//                      dvrml_appearance_sptr appearance_sptr, 
//                      vnl_vector_fixed<double,3> const& xyz_ratios,
//                      vnl_vector_fixed<double,4> const& xyz_rotation,
//                      vnl_vector_fixed<double,3> const& xyz_scale )
//{
//    vgl_point_3d<double> pt3d(double(center.x()),double(center.y()),double(0.0));
//    this->draw_box(pt3d,appearance_sptr,xyz_ratios,xyz_rotation,xyz_scale);
//}//end dvrml::draw_box 2d unsigned center
//
//void dvrml::draw_cylinder( vgl_point_3d<double> const& center, 
//                            dvrml_appearance_sptr appearance_sptr, 
//                            vnl_vector_fixed<double,2> const& radius_height,
//                            vnl_vector_fixed<double,4> const& xyz_rotation,
//                            vnl_vector_fixed<double,3> const& xyz_scale )
//{
//       SETUP;
//       os << "#cylinder\n"
//       << "Transform{\n"
//       << "  translation " << center.x() << ' ' << center.y() << ' ' << center.z() << '\n'
//       << "  rotation " << xyz_rotation.get(0) << ' ' << xyz_rotation.get(1) << ' ' << xyz_rotation.get(2) << ' ' << xyz_rotation.get(3) << '\n'
//       << "  scale " << xyz_scale.get(0) << ' ' << xyz_scale.get(1) << ' ' << xyz_scale.get(2) << '\n'
//       << "  children [\n"
//       << "     Shape{\n"
//       << "         appearance Appearance {\n"
//       << "             material Material {\n"
//       << "                 diffuseColor " << appearance_sptr->diffuseColor().get(0) << ' ' << appearance_sptr->diffuseColor().get(1) << ' ' << appearance_sptr->diffuseColor().get(2) << '\n'
//       << "                 ambientIntensity " << appearance_sptr->ambientIntensity() << '\n'
//       << "                 emissiveColor " << appearance_sptr->emissiveColor().get(0) << ' ' << appearance_sptr->emissiveColor().get(1) << ' ' << appearance_sptr->emissiveColor().get(2) << '\n'
//       << "                 specularColor " << appearance_sptr->specularColor().get(0) << ' ' << appearance_sptr->specularColor().get(1) << ' ' << appearance_sptr->specularColor().get(2) << '\n'
//       << "                 shininess " << appearance_sptr->shininess() << '\n'
//       << "                 transparency " << appearance_sptr->transparency() << '\n'
//       << "             }# end Material\n"
//       << "         }# end Appearance\n"
//       << "         geometry Cylinder{\n"
//       << "             radius " << radius_height.get(0) << '\n' 
//       << "             height " << radius_height.get(1) << '\n' 
//       << "         } #end geometry\n"
//       << "         } #end Shape\n"
//       << "   ] #end children\n"
//       << "} #end transform\n\n";
//}//end dvrml::draw_cylinder
//
//void dvrml::draw_cone( vgl_point_3d<double> const& bottom_center, 
//                       dvrml_appearance_sptr appearance_sptr, 
//                       vnl_vector_fixed<double,2> const& bottomRadius_height,
//                       vnl_vector_fixed<double,4> const& xyz_rotation,
//                       vnl_vector_fixed<double,3> const& xyz_scale )
//{
//    SETUP;
//    os << "#cone\n"
//       << "Transform{\n"
//       << "  translation " << bottom_center.x() << ' ' << bottom_center.y() << ' ' << bottom_center.z() << '\n'
//       << "  rotation " << xyz_rotation.get(0) << ' ' << xyz_rotation.get(1) << ' ' << xyz_rotation.get(2) << ' ' << xyz_rotation.get(3) << '\n'
//       << "  scale " << xyz_scale.get(0) << ' ' << xyz_scale.get(1) << ' ' << xyz_scale.get(2) << '\n'
//       << "  children [\n"
//       << "     Shape{\n"
//       << "         appearance Appearance {\n"
//       << "             material Material {\n"
//       << "                 diffuseColor " << appearance_sptr->diffuseColor().get(0) << ' ' << appearance_sptr->diffuseColor().get(1) << ' ' << appearance_sptr->diffuseColor().get(2) << '\n'
//       << "                 ambientIntensity " << appearance_sptr->ambientIntensity() << '\n'
//       << "                 emissiveColor " << appearance_sptr->emissiveColor().get(0) << ' ' << appearance_sptr->emissiveColor().get(1) << ' ' << appearance_sptr->emissiveColor().get(2) << '\n'
//       << "                 specularColor " << appearance_sptr->specularColor().get(0) << ' ' << appearance_sptr->specularColor().get(1) << ' ' << appearance_sptr->specularColor().get(2) << '\n'
//       << "                 shininess " << appearance_sptr->shininess() << '\n'
//       << "                 transparency " << appearance_sptr->transparency() << '\n'
//       << "             }# end Material\n"
//       << "         }# end Appearance\n"
//       << "         geometry Cone{\n"
//       << "             bottomRadius " << bottomRadius_height.get(0) << '\n' 
//       << "             height "       << bottomRadius_height.get(1) << '\n' 
//       << "         } #end geometry\n"
//       << "         } #end Shape\n"
//       << "   ] #end children\n"
//       << "} #end transform\n\n";
//
//}//end dvrml::draw_cone
//
//void dvrml::draw_points( vcl_vector<vgl_point_3d<double> > const& pts, 
//                         dvrml_appearance_sptr appearance_sptr, 
//                         double const& radius )
//{
//    vcl_vector<vgl_point_3d<double> >::const_iterator point_set_itr;
//    vcl_vector<vgl_point_3d<double> >::const_iterator point_set_end = pts.end();
//    SETUP;
//    os << "#indexed point set\n\n"
//       << "Shape {\n"
//       << " appearance Appearance{\n"
//       << "             material Material {\n"
//       << "                 diffuseColor " << appearance_sptr->diffuseColor().get(0) << ' ' << appearance_sptr->diffuseColor().get(1) << ' ' << appearance_sptr->diffuseColor().get(2) << '\n'
//       << "                 ambientIntensity " << appearance_sptr->ambientIntensity() << '\n'
//       << "                 emissiveColor " << appearance_sptr->emissiveColor().get(0) << ' ' << appearance_sptr->emissiveColor().get(1) << ' ' << appearance_sptr->emissiveColor().get(2) << '\n'
//       << "                 specularColor " << appearance_sptr->specularColor().get(0) << ' ' << appearance_sptr->specularColor().get(1) << ' ' << appearance_sptr->specularColor().get(2) << '\n'
//       << "                 shininess " << appearance_sptr->shininess() << '\n'
//       << "                 transparency " << appearance_sptr->transparency() << '\n'
//       << "             }# end Material\n"
//       << "         }# end Appearance\n"
//       << " geometry PointSet{\n"
//       << "     coord Coordinate {\n"
//       << "         point [\n";
//for( point_set_itr = pts.begin(); point_set_itr != point_set_end; ++point_set_itr )
//    os << "        " << point_set_itr->x() << ' ' << point_set_itr->y() << ' ' << point_set_itr->z() << ",\n";
//
//    os << "               ]# end point\n"
//       << "         }# end geometry\n"
//       << "}# end Shape\n";
//
//}//end dvrml::draw_points
//
//void dvrml::draw_plane( vgl_point_3d<double> const& center, 
//            dvrml_appearance_sptr appearance_sptr,
//            vnl_vector_fixed<double,2> const& xy_ratios, 
//            vnl_vector_fixed<double,4> const& xyz_rotation,
//            vnl_vector_fixed<double,3> const& xyz_scale )
//{
//    vnl_vector_fixed<double,3> xyz_ratios(xy_ratios.get(0),xy_ratios.get(1),0.2);
//    this->draw_box(center, appearance_sptr, xyz_ratios, xyz_rotation, xyz_scale );
//}//end dvrml::draw_plane
//
//
//void dvrml::draw_text( vcl_string const& text,
//                       vgl_point_3d<double> const& center,
//                       dvrml_appearance_sptr appearance_sptr,
//                       double const& length,
//                       vnl_vector_fixed<double,4> const& xyz_rotation,
//                       vnl_vector_fixed<double,3> const& xyz_scale )
//{
//    SETUP;
//    os << "#text\n"
//       << "Transform{\n"
//       << "  translation " << center.x() << ' ' << center.y() << ' ' << center.z() << '\n'
//       << "  rotation " << xyz_rotation.get(0) << ' ' << xyz_rotation.get(1) << ' ' << xyz_rotation.get(2) << ' ' << xyz_rotation.get(3) << '\n'
//       << "  scale " << xyz_scale.get(0) << ' ' << xyz_scale.get(1) << ' ' << xyz_scale.get(2) << '\n'
//       << "  children [\n"
//       << "     Shape{\n"
//       << "         appearance Appearance {\n"
//       << "             material Material {\n"
//       << "                 diffuseColor " << appearance_sptr->diffuseColor().get(0) << ' ' << appearance_sptr->diffuseColor().get(1) << ' ' << appearance_sptr->diffuseColor().get(2) << '\n'
//       << "                 ambientIntensity " << appearance_sptr->ambientIntensity() << '\n'
//       << "                 emissiveColor " << appearance_sptr->emissiveColor().get(0) << ' ' << appearance_sptr->emissiveColor().get(1) << ' ' << appearance_sptr->emissiveColor().get(2) << '\n'
//       << "                 specularColor " << appearance_sptr->specularColor().get(0) << ' ' << appearance_sptr->specularColor().get(1) << ' ' << appearance_sptr->specularColor().get(2) << '\n'
//       << "                 shininess " << appearance_sptr->shininess() << '\n'
//       << "                 transparency " << appearance_sptr->transparency() << '\n'
//       << "             }# end Material\n"
//       << "         }# end Appearance\n"
//       << "         geometry Text{\n"
//       << "             string \"" << text << "\"\n" 
//       << "             length " << length << '\n'
//       << "         } #end geometry\n"
//       << "         } #end Shape\n"
//       << "   ] #end children\n"
//       << "} #end transform\n\n";
//
//}//end draw_textd
//
//void dvrml::draw_axis()
//{
//
//    SETUP;
//
//    vgl_point_3d<double> origin(0.0,0.0,0.0);
//
//    //radius and height of the cylinder for the axis
//    vnl_vector_fixed<double,2> radius_height(0.1,5.0);
//
//    //so cylinders are aligned with axis
//    vnl_vector_fixed<double,4> xaxis_xyz_rotation(0,0,1,1.57);
//    //vnl_vector_fixed<double,4> yaxis_xyz_rotation(0,0,1,1.57);
//    vnl_vector_fixed<double,4> zaxis_xyz_rotation(1,0,0,1.57);
//
//    //colors
//    vnl_vector_fixed<double,3> green(0,1,0);
//    vnl_vector_fixed<double,3> blue(0,0,1);
//
//    dvrml_appearance_sptr x_appearance = new dvrml_appearance;
//    dvrml_appearance_sptr y_appearance = new dvrml_appearance(green);
//    dvrml_appearance_sptr z_appearance = new dvrml_appearance(blue);
//
//    //x-axis
//    this->draw_cylinder(origin,x_appearance,radius_height,xaxis_xyz_rotation);
//    //y-axis
//    this->draw_cylinder(origin,y_appearance,radius_height);
//    //z-axis
//    this->draw_cylinder(origin,z_appearance,radius_height,zaxis_xyz_rotation);
//
//
//    //Arrows (cones)
//    vgl_point_3d<double> x_cone_center(3.0,0.0,0.0);
//    vgl_point_3d<double> y_cone_center(0.0,3.0,0.0);
//    vgl_point_3d<double> z_cone_center(0.0,0.0,3.0);
//
//    vnl_vector_fixed<double,4> x_cone_xyz_rotation(0,0,1,-1.57);
//    vnl_vector_fixed<double,4> z_cone_xyz_rotation(1,0,0,1.57);
//
//    
//
//    vnl_vector_fixed<double,2> cone_radius_height(.1,1);
//    this->draw_cone(x_cone_center,x_appearance,cone_radius_height,x_cone_xyz_rotation);
//    this->draw_cone(y_cone_center,y_appearance,cone_radius_height);
//    this->draw_cone(z_cone_center,z_appearance,cone_radius_height,z_cone_xyz_rotation);
//
//    vgl_point_3d<double> x_text_center(3.0,0.0,0.0);
//    vgl_point_3d<double> y_text_center(0.3,3.0,0.0);
//    vgl_point_3d<double> z_text_center(0.0,0.0,3.0);
//    this->draw_text("x",x_text_center,x_appearance);
//    this->draw_text("y",y_text_center,y_appearance);
//    this->draw_text("z",z_text_center,z_appearance);
//
//    //vgl_point_3d<double> xpt(2.5,0.0,0.0);
//
//    //this->draw_point(xpt,x_appearance,.3);
//
//}//end dvrml::draw_axis