//  This is under contrib\Manifold_extraction\vis


#include <vnl/vnl_matrix.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_complexify.h>
#include <vnl/vnl_vector_fixed.h>
#include <vcl_vector.h>
#include <vbl/vbl_array_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_dialog.h>
#include <vehicle_model.h>
#include <Lie_group_operations.h>
#include <vsol/vsol_rectangle_2d_sptr.h>
#include <bgui3d/bgui3d_file_io.h>
#include <bgui3d/bgui3d_examiner_tableau.h>
#include <bgui3d/bgui3d.h>



int main(int argc,char **argv)
    {
    vcl_string trucks_info = argv[1];
    vcl_string geodesics_info = argv[2];
    vcl_string vrml_file = argv[3];

    //usage: 

    //trucks_info is the file containing (x,y,z) values which are nothing but sx/sy values for the 
    //hood,cab and the bed of the three box models fitted to trucks in the images.

    //geodesics_info_read is the file holding the (x,y,z) values which are nothing but sx/sy values 
    //for the hood,cab and bed of three box models lying along the geodesics.

    //vrml_file is the output vrml file which has the trucks plotted as spheres in 3d and the
    //curves in 3d representing the principal geodesics
    
    vcl_ifstream trucks_info_read(trucks_info.c_str());
   vcl_ifstream geodesics_info_read(geodesics_info.c_str());

    int i,it,num_models,num_samples;
    trucks_info_read >> num_models;
    geodesics_info_read >> num_samples;

    vnl_vector<double>x(num_models),y(num_models),z(num_models);
    vnl_vector<double>x_g(3*num_samples),y_g(3*num_samples),z_g(3*num_samples);

    for (i = 0;i<num_models;i++)
        {
        trucks_info_read >>  x[i] >> y[i] >> z[i] ;
        }


for (i = 0;i<3*num_samples;i++)
        {
       geodesics_info_read >>  x_g[i] >> y_g[i] >> z_g[i] ;
        }
    

    vcl_ofstream out(vrml_file.c_str());
    

    out <<      "#VRML V2.0 utf8\n";
    out <<      "Background { \n";
    out <<      "    skyColor [ 1 1 1 ]\n";
    out <<      "    groundColor [ 1 1 1 ]\n";
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
    
//write down all the spheres corresponding to the trucks

for (i = 0;i<num_models;i++)
    {
    out <<      " Transform { \n";
out <<      "translation" << " " <<x[i] << " " << y[i] << " " << z[i] << "\n";
out << "children [ \n";
out << "Shape { \n";
out << " appearance Appearance{ \n";
out << "   material Material \n";
out << "    { \n";
out << "      diffuseColor 1 " <<double(num_models-i)/double(num_models)<<" "<<double(i)/double(num_models) <<" \n";
out << "      transparency 0 \n";
out << "    } \n";
out << "  } \n";
out << " geometry Sphere \n";
out << "{ \n";
out << "  radius 0.02 \n";
out << "   } \n";
out << "  } \n";
out << " ] \n";
out << "} \n";

    }

for (it = 0;it<3;it++)
    {
out << "Transform { \n";
out << "translation 0 0  0 \n";
out << " children [ \n";
out << "Shape { \n";
out << " appearance Appearance{ \n";
out << " material Material \n";
out << "    { \n";
out << "      diffuseColor 0.25 0.25 0.25 \n";
out << "      emissiveColor 0.25 0.25 0.25 \n";
out << "    } \n";
out << "  } \n";
out << " geometry IndexedLineSet \n";
out << "{ \n";
out << "      coord Coordinate{ \n";
out << "       point[ \n";

double s_b1,s_b2,s_b3,k,s = 1;
//write down the principal geodesics 

for (k=0;k<num_samples;k++)
    {   

out <<" " << x_g[it*num_samples+k] <<" " << y_g[it*num_samples+k] << " " <<z_g[it*num_samples+k] << vcl_endl;
    }

out <<"    ] \n";
out <<"  }   coordIndex [ \n";

for (i = 0;i<num_samples;i++)
    {
out << i << ",";
    }
out << "-1  ] \n";
out << "  } \n";
out <<"} ] \n";
out <<"} \n";

}
    
out.close();

return 0;

    }


