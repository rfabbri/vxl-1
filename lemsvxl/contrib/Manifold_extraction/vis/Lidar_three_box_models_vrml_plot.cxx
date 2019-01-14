//  This is under contrib\Manifold_extraction\vis


#include <vnl/vnl_matrix.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_complexify.h>
#include <vnl/vnl_vector_fixed.h>
#include <vector>
#include <vbl/vbl_array_3d.h>
#include <vgl/vgl_point_3d.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_dialog.h>

#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_point_3d.h>

#include <vehicle_model.h>
#include <vehicle_model_3d.h>
#include <Lie_group_operations.h>
#include <vsol/vsol_rectangle_2d_sptr.h>
#include <bgui3d/bgui3d_file_io.h>
#include <bgui3d/bgui3d_examiner_tableau.h>
#include <bgui3d/bgui3d.h>
#include <vul/vul_file_iterator.h>


void read_box(std::istream &s,vgl_box_3d<double>& b_box)
{
std::string str;
char ch;
double min_x,min_y,min_z,max_x,max_y,max_z;

 s >> str; 
 s >> min_x >> ch >> min_y >> ch >> min_z >> str
           >> max_x >> ch >> max_y >> ch >> max_z >> ch;

 b_box.set_min_x(min_x);
 b_box.set_min_y(min_y);
 b_box.set_min_z(min_z);

 b_box.set_max_x(max_x);
 b_box.set_max_y(max_y);
 b_box.set_max_z(max_z);
}


std::vector<vehicle_model_3d> read_3d_models(std::string bounding_boxes_info,std::string file_ext)
    {
    std::string b_box_type_part1,b_box_type_part2,b_box_type_part3,f_name;
    std::vector<vehicle_model_3d> vehic_mod_vec;

    vgl_box_3d<double> b_box,b_box_hood,b_box_cab, b_box_bed;
   
    for (vul_file_iterator fn= bounding_boxes_info + file_ext; fn; ++fn) {
       f_name =  fn();
        std::ifstream ifstr(f_name.c_str());

        for (unsigned int i = 0;i<3;i++)
            {
         ifstr >> b_box_type_part1 >> b_box_type_part2 >> b_box_type_part3;
      /* read_box(ifstr,b_box);

        std::cout << b_box << std::endl; */
      

        if (strcmp(b_box_type_part3.c_str(),"hood:") == 0)
            read_box(ifstr,b_box_hood);


        if (strcmp(b_box_type_part3.c_str(),"cab:") == 0)
            read_box(ifstr,b_box_cab);

        if (strcmp(b_box_type_part3.c_str(),"bed:") == 0)
            read_box(ifstr,b_box_bed);
            }

    /*    std::cout << "b_box_hood" <<b_box_hood << std::endl;
        std::cout << "b_box_cab" <<b_box_cab << std::endl;
        std::cout << "b_box_bed" <<b_box_bed << std::endl;*/

        if (!(b_box_hood.is_empty()) && !(b_box_cab.is_empty()) && !(b_box_hood.is_empty()))
            {
        vehicle_model_3d M(b_box_hood,b_box_cab,b_box_bed);
        vehic_mod_vec.push_back(M);
            }
        }
return vehic_mod_vec;
    }

int main(int argc,char **argv)
    {
    std::string vehicle_model_info = argv[1];
    std::string intrinsic_mean_info = argv[2];
    std::string vrml_file = argv[3];

  
    std::string part;
    double diag_hood_mean,diag_cab_mean,diag_bed_mean;
    double diag_hood,diag_cab,diag_bed;

    std::vector<vehicle_model_3d> model_vec = read_3d_models(vehicle_model_info,"\\*.txt");
    std::ifstream ifstr(intrinsic_mean_info.c_str());

    vnl_matrix<double > mean_hood_tr(4,4,0.0),mean_cab_tr(4,4,0.0),mean_bed_tr(4,4,0.0);
    
    ifstr >> part;
    ifstr >> mean_hood_tr;
     ifstr >> part;
    ifstr >> mean_cab_tr;
     ifstr >> part;
    ifstr >> mean_bed_tr;

    std::cout << "mean_hood_tr" << mean_hood_tr << std::endl;
    std::cout << "mean_cab_tr" << mean_cab_tr << std::endl;
    std::cout << "mean_bed_tr" << mean_bed_tr << std::endl;

    ifstr.close();

    //set the reference model as the intrinsic mean model
 
     vehicle_model_3d mean_model(model_vec[0].hood(),model_vec[0].cab(),model_vec[0].bed());
     mean_model.transform_model(mean_hood_tr,mean_cab_tr,mean_bed_tr);

    diag_hood_mean = std::pow((mean_model.hood().height()),2) + std::pow((mean_model.hood().width()),2) + std::pow((mean_model.hood().depth()),2);
    diag_cab_mean = std::pow((mean_model.cab().height()),2) + std::pow((mean_model.cab().width()),2) + std::pow((mean_model.cab().depth()),2);
    diag_bed_mean = std::pow((mean_model.bed().height()),2) + std::pow((mean_model.bed().width()),2) + std::pow((mean_model.bed().depth()),2);

      int i,num_models = model_vec.size();

    std::vector<double> s1,s2,s3;

    for (i = 0;i<num_models;i++)
        {
        diag_hood = std::pow((model_vec[i].hood().height()),2) + std::pow((model_vec[i].hood().width()),2)+
            std::pow((model_vec[i].hood().depth()),2);

        diag_cab = std::pow((model_vec[i].cab().height()),2) + std::pow((model_vec[i].cab().width()),2)+
        std::pow((model_vec[i].cab().depth()),2);

        diag_bed = std::pow((model_vec[i].bed().height()),2) + std::pow((model_vec[i].bed().width()),2)+
            std::pow((model_vec[i].bed().depth()),2);

        s1.push_back(diag_hood/diag_hood_mean);
        s2.push_back(diag_cab/diag_cab_mean);
        s3.push_back(diag_bed/diag_bed_mean);
        }

    std::ofstream out(vrml_file.c_str());

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
out <<      "translation" << " " <<s1[i] << " " << s2[i] << " " << s3[i] << "\n";
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

    
out.close();

return 0;

    }


