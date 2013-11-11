#ifndef vrml_print_h_
#define vrml_print_h_

#include <vnl/vnl_math.h>
#include <vsol/vsol_rectangle_2d.h>
#include <vcl_vector.h>

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vehicle_model.h>

void vrml_print_box(vsol_rectangle_2d box,vcl_ostream& out)
    {
out << box.p0()->x() << " " << box.p0()->y() << " " << 1 << vcl_endl;
out << box.p1()->x() << " " << box.p1()->y() << " " << 1 << vcl_endl;
out << box.p2()->x() << " " << box.p2()->y() << " " << 1 << vcl_endl;
out << box.p3()->x() << " " << box.p3()->y() << " " << 1 << vcl_endl;

    }

void print_vrml_header(vcl_ostream& out)
    {
 out <<      "#VRML V2.0 utf8\n";
    out <<      "Background { \n";
    out <<      "    skyColor [ 0 0 0 ]\n";
    out <<      "    groundColor [ 0 0 0 ]\n";
    out <<      "}\n";
    out <<      "PointLight {\n";
    out <<      "    on FALSE\n";
    out <<      "    intensity 1 \n";
    out <<      "ambientIntensity 0 \n";
    out <<      "color 1 1 1 \n";
    out <<      "location 0 0 0 \n";
    out <<      "attenuation 1 0 0 \n";
    out <<      "radius 100  \n";
    out <<      "}\n";
    }

void print_vrml_model(vcl_ostream& out,vehicle_model M1,double color_coeff)
    {
     out << " Transform {\n";
     out << "translation 0 0  0 \n";
    out << "  children [ \n";
    out <<      "Shape {\n";
    out <<      "    appearance Appearance {\n";
    out <<      "       material Material { diffuseColor 0.25 "<<color_coeff<<" 0.25 \n";
    out <<      "                           emissiveColor 0.25 "<<color_coeff<<" 0.25}\n";
    out <<      "   } \n";
    out <<      "   geometry IndexedFaceSet {\n";
    out <<      "      coord Coordinate{\n";
    out <<      "         point[\n";
    // out << M1 << vcl_endl;
      M1.vrml_print(out);
    out <<      "         ]\n";
   out <<      " }   coordIndex [";
   
   for(int k = 0;k<9;k = k+4)
       {
       for (int i = k;i<k+4;i++)
           {
           /*if (i == k+3)
               out <<i<<","<<i-3<<","<<-1<<",";
           else
           out <<i<<","<<i+1<<","<<-1<<",";*/
           if (i == k+3)
               out <<i<<","<<i-3<<",";
           else
           out <<i<<",";
           }
       out << "-1,";
       }


    out  <<"]\n" ;
   out <<      "   }\n";
    out <<      "}\n";
    }
#endif

