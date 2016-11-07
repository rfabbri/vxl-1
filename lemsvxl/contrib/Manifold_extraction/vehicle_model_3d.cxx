#include "vehicle_model_3d.h"
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vgl/vgl_homg_point_2d.h>

//helper function to transform the model
//only translation and scaling

void transform_component(vnl_matrix<double>const & m,vgl_box_3d<double>& box)
    {
    vnl_matrix<double> points(4,2,1.0),tr_points(4,2,1.0);
    double min_x,min_y,min_z,max_x,max_y ,max_z;

   // vcl_cout << " box: " << box << vcl_endl;

    min_x = box.min_x();
    min_y = box.min_y();
    min_z = box.min_z();

    max_x = box.max_x();
    max_y = box.max_y();
    max_z = box.max_z();

    points.put(0,0,min_x);
    points.put(1,0,min_y);
    points.put(2,0,min_z);
    points.put(0,1,max_x);
    points.put(1,1,max_y);
    points.put(2,1,max_z);

    tr_points = m*points;

    box.set_min_x(tr_points.get(0,0));
    box.set_min_y(tr_points.get(1,0));
    box.set_min_z(tr_points.get(2,0));

    box.set_max_x(tr_points.get(0,1));
    box.set_max_y(tr_points.get(1,1));
    box.set_max_z(tr_points.get(2,1));
    }

//void vehicle_model_3d::print_vrml(vcl_ofstream &ofstr)
//    {
//       ofstr <<      "#VRML V2.0 utf8\n";
//        ofstr <<      "Background { \n";
//        ofstr <<      "    skyColor [ 0 0 0 ]\n";
//        ofstr <<      "    groundColor [ 0 0 0 ]\n";
//        ofstr <<      "}\n";
//        ofstr <<      "PointLight {\n";
//        ofstr <<      "    on FALSE\n";
//        ofstr <<      "    intensity 1 \n";
//        ofstr <<      "ambientIntensity 0 \n";
//        ofstr <<      "color 1 1 1 \n";
//        ofstr <<      "location 0 0 0 \n";
//        ofstr <<      "attenuation 1 0 0 \n";
//        ofstr <<      "radius 100  \n";
//        ofstr <<      "}\n";
//
//            ofstr <<      " Transform { \n";
//            ofstr <<      "translation" << " " <<centroids[i].x() << " " << centroids[i].y() << " " << centroids[i].z() << "\n";
//            ofstr << "children [ \n";
//            ofstr << "Shape { \n";
//            ofstr << " appearance Appearance{ \n";
//            ofstr << "   material Material \n";
//            ofstr << "    { \n";
//            ofstr << "      diffuseColor 1 0 0 \n";
//            ofstr << "      transparency 0 \n";
//            ofstr << "    } \n";
//            ofstr << "  } \n";
//            ofstr << " geometry Box \n";
//            ofstr << "{ \n";
//            ofstr << "  size " << width() <<" "<< b_boxes[i].height()<<" " << b_boxes[i].depth() <<" \n";
//            ofstr << "   } \n";
//            ofstr << "  } \n";
//            ofstr << " ] \n";
//            ofstr << "} \n";
//    }

vehicle_model_3d::vehicle_model_3d(vgl_box_3d<double> hood,vgl_box_3d <double> cab,vgl_box_3d <double> bed):
    hood_(hood),cab_(cab),bed_(bed)
        {

        }

void vehicle_model_3d:: transform_model(vnl_matrix<double>const& m1,vnl_matrix<double>const& m2,vnl_matrix<double>const& m3)
    {
    transform_component(m1,hood_);
    transform_component(m2,cab_);
    transform_component(m3,bed_);

    }






