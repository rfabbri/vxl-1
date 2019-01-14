//  This is under contrib\Manifold_extraction\vis
// Brief: An example for doing pga on the three box models obtained
//from the Lidar data

#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_matrix_inverse.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_complexify.h>
#include <vnl/vnl_vector_fixed.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vector>
#include <vbl/vbl_array_3d.h>
#include <cassert>
#include <string>
#include <iostream>
#include <fstream>
#include <vehicle_model.h>
#include <vehicle_model_3d.h>
#include <Lie_group_operations.h>
#include <vsol/vsol_rectangle_2d_sptr.h>
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

        std::cout << "b_box_hood" <<b_box_hood << std::endl;
        std::cout << "b_box_cab" <<b_box_cab << std::endl;
        std::cout << "b_box_bed" <<b_box_bed << std::endl;

        if (!(b_box_hood.is_empty()) && !(b_box_cab.is_empty()) && !(b_box_hood.is_empty()))
            {
        vehicle_model_3d M(b_box_hood,b_box_cab,b_box_bed);
        vehic_mod_vec.push_back(M);
            }
        }
return vehic_mod_vec;
    }



 void print_models(std::vector<vehicle_model_3d> M,std::string bounding_boxes_info)
    {
    std::ofstream ofstr(bounding_boxes_info.c_str());

   for (unsigned int i = 0;i<M.size();i++)
       {
      vgl_box_3d<double> hood = M[i].hood() ;
      vgl_box_3d<double> cab = M[i].cab() ;
      vgl_box_3d<double> bed = M[i].bed() ;



      ofstr << hood.min_point() << std::endl;
      ofstr << hood.max_point() << std::endl;

       ofstr << cab.min_point() << std::endl;
      ofstr << cab.max_point() << std::endl;

       ofstr << bed.min_point() << std::endl;
      ofstr << bed.max_point() << std::endl;
       }
    }

vnl_matrix<double> get_transformation(const vgl_box_3d<double>& bb_1,const vgl_box_3d<double>& bb_2,std::ostream &s)
    {
    vnl_matrix<double> from_points(4,8,1);
    vnl_matrix<double> to_points(4,8,1);

    std::vector<double> p1_x,p1_y,p1_z;
    std::vector<double> p2_x,p2_y,p2_z;

    vgl_point_3d<double> min_pt_1 = bb_1.min_point();
    vgl_point_3d<double> max_pt_1 = bb_1.max_point();

    p1_x.push_back(min_pt_1.x());
    p1_y.push_back(min_pt_1.y());
    p1_z.push_back(min_pt_1.z());

     p1_x.push_back(min_pt_1.x());
    p1_y.push_back(min_pt_1.y());
    p1_z.push_back(max_pt_1.z());

     p1_x.push_back(min_pt_1.x());
    p1_y.push_back(max_pt_1.y());
    p1_z.push_back(max_pt_1.z());

     p1_x.push_back(min_pt_1.x());
    p1_y.push_back(max_pt_1.y());
    p1_z.push_back(min_pt_1.z());

     p1_x.push_back(max_pt_1.x());
    p1_y.push_back(min_pt_1.y());
    p1_z.push_back(min_pt_1.z());

     p1_x.push_back(max_pt_1.x());
    p1_y.push_back(min_pt_1.y());
    p1_z.push_back(max_pt_1.z());

     p1_x.push_back(max_pt_1.x());
    p1_y.push_back(max_pt_1.y());
    p1_z.push_back(max_pt_1.z());

     p1_x.push_back(max_pt_1.x());
    p1_y.push_back(max_pt_1.y());
    p1_z.push_back(min_pt_1.z());

    for (unsigned int i=0;i<7;i++)
        {
from_points.put(0,i,p1_x[i]);
from_points.put(1,i,p1_y[i]);
from_points.put(2,i,p1_z[i]);
from_points.put(3,i,1);
        }
     vgl_point_3d<double> min_pt_2 = bb_2.min_point();
    vgl_point_3d<double> max_pt_2 = bb_2.max_point();

    p2_x.push_back(min_pt_2.x());
    p2_y.push_back(min_pt_2.y());
    p2_z.push_back(min_pt_2.z());

     p2_x.push_back(min_pt_2.x());
    p2_y.push_back(min_pt_2.y());
    p2_z.push_back(max_pt_2.z());

     p2_x.push_back(min_pt_2.x());
    p2_y.push_back(max_pt_2.y());
    p2_z.push_back(max_pt_2.z());

     p2_x.push_back(min_pt_2.x());
    p2_y.push_back(max_pt_2.y());
    p2_z.push_back(min_pt_2.z());

    p2_x.push_back(max_pt_2.x());
    p2_y.push_back(min_pt_2.y());
    p2_z.push_back(min_pt_2.z());

     p2_x.push_back(max_pt_2.x());
    p2_y.push_back(min_pt_2.y());
    p2_z.push_back(max_pt_2.z());

     p2_x.push_back(max_pt_2.x());
    p2_y.push_back(max_pt_2.y());
    p2_z.push_back(max_pt_2.z());

     p2_x.push_back(max_pt_2.x());
    p2_y.push_back(max_pt_2.y());
    p2_z.push_back(min_pt_2.z());
    
  

    for (i=0;i<7;i++)
        {
to_points.put(0,i,p2_x[i]);
to_points.put(1,i,p2_y[i]);
to_points.put(2,i,p2_z[i]);
to_points.put(3,i,1);
        }

    /*s << "from_points" << from_points << std::endl;
    s << "to_points" << to_points << std::endl;*/

    // vnl_matrix<double> transformation_matrix = (to_points*from_points.transpose())* vnl_matrix_inverse<double>(from_points*from_points.transpose());

    vnl_matrix<double> transformation_matrix(4,4,0.0);
    transformation_matrix.put(0,0,bb_2.width()/bb_1.width());
    transformation_matrix.put(1,1,bb_2.height()/bb_1.height());
    transformation_matrix.put(2,2,bb_2.depth()/bb_1.depth());
    transformation_matrix.put(3,3,1);

    return transformation_matrix;

    }
void get_transformation_matrices(const vehicle_model_3d & M1,const vehicle_model_3d & M2,vnl_matrix<double> &G1,vnl_matrix<double> &G2,
                                 vnl_matrix<double> &G3,std::ostream &s)
    {

   G1 = get_transformation(M1.hood(),M2.hood(),s);
   G2 = get_transformation(M1.cab(),M2.cab(),s);
   G3 = get_transformation(M1.bed(),M2.bed(),s);
    }

int main(int argc,char **argv)
    {
    std::string vehicle_model_info = argv[1];
    std::string debug_info_file = argv[2];
    std::string mean_info_file = argv[3];
    
    std::ofstream ofstr(debug_info_file.c_str());
     std::ofstream ofstr_mean(mean_info_file.c_str());

    std::vector<vehicle_model_3d> model_vec = read_3d_models(vehicle_model_info,"\\*.txt");

    vnl_matrix<double>G1,G2,G3,I_mean_hood,I_mean_cab,I_mean_bed;

    std::vector<vnl_matrix<double> > G1_vec,G2_vec,G3_vec;

    for (unsigned int i = 1;i<model_vec.size();i++)
        {
        std::cout << model_vec[i].hood() << std::endl;
        std::cout << model_vec[i].cab() << std::endl;
        std::cout << model_vec[i].bed() << std::endl;

         get_transformation_matrices(model_vec[0],model_vec[i],G1,G2,G3,ofstr);
         // ofstr << "transformation: " << i << std::endl;
         ofstr << G1 << std::endl;
         ofstr << G2 << std::endl;
         ofstr << G3 << std::endl;
         G1_vec.push_back(G1);
         G2_vec.push_back(G2);
         G3_vec.push_back(G3);
        }
    
    I_mean_hood = calculate_intrinsic_mean_3d(G1_vec);
    I_mean_cab = calculate_intrinsic_mean_3d(G2_vec);
    I_mean_bed = calculate_intrinsic_mean_3d(G3_vec);

    ofstr_mean << "hood:" << std::endl;
    ofstr_mean << I_mean_hood;
    ofstr_mean << "cab:" << std::endl;
    ofstr_mean << I_mean_cab;
    ofstr_mean << "bed:" << std::endl;
    ofstr_mean << I_mean_bed;



    return 0;
    }

