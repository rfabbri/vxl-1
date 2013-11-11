//  This is under contrib\Manifold_extraction\cmd

// Brief: An example for finding the box model projections based on Lie distances

#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_matrix_inverse.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_complexify.h>
#include <vnl/vnl_vector_fixed.h>
#include <vgl/vgl_box_2d.h>
#include <vcl_vector.h>
#include <vbl/vbl_array_3d.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_dialog.h>
#include <vehicle_model.h>
#include <vrml_print.h>
#include <Lie_group_operations.h>



int main(int argc,char **argv)
    {
vcl_string boxes_info = argv[1];
vcl_string output_file = argv[2];
vcl_string debug_info_file = argv[3];

vcl_ofstream ofstr(boxes_info.c_str());
vcl_ofstream out(output_file.c_str());
vcl_ofstream debug_info(debug_info_file.c_str());

int my_argc = 1;
    char ** my_argv = new char*[argc+1];
    for (int i = 0;i<argc;i++)
        my_argv[i] = argv[i];
    my_argv[argc] = "--mfc-use-gl";
    vgui::init(my_argc,my_argv);

    delete []my_argv;

print_vrml_header(out);
   
    
// defining the simple 3 box model 
vsol_point_2d_sptr p0_box1 = new vsol_point_2d (0.1,0.1);
vsol_point_2d_sptr p1_box1 = new vsol_point_2d (0.3,0.1);
vsol_point_2d_sptr p2_box1 = new vsol_point_2d (0.3,0.3);
vsol_point_2d_sptr p3_box1 = new vsol_point_2d (0.1,0.3);

vsol_rectangle_2d starting_box1(p0_box1,p1_box1,p2_box1,p3_box1);

vsol_point_2d_sptr p0_box2 = new vsol_point_2d (0.3,0.0);
vsol_point_2d_sptr p1_box2 = new vsol_point_2d (0.7,0.0);
vsol_point_2d_sptr p2_box2 = new vsol_point_2d (0.7,0.4);
vsol_point_2d_sptr p3_box2 = new vsol_point_2d (0.3,0.4);

vsol_rectangle_2d starting_box2(p0_box2,p1_box2,p2_box2,p3_box2);

vsol_point_2d_sptr p0_box3 = new vsol_point_2d (0.7,0.05);
vsol_point_2d_sptr p1_box3 = new vsol_point_2d (1,0.05);
vsol_point_2d_sptr p2_box3 = new vsol_point_2d (1,0.35);
vsol_point_2d_sptr p3_box3 = new vsol_point_2d (0.7,0.35);

vsol_rectangle_2d starting_box3(p0_box3,p1_box3,p2_box3,p3_box3);

//vgl_box_2d<double>starting_box1(0.1,0.3,0.1,0.3);
//vgl_box_2d<double>starting_box2(0.3,0.7,0,0.4);
//vgl_box_2d<double>starting_box3(0.7,1,0.05,0.35);
    
vehicle_model M(starting_box1,starting_box2,starting_box3);



vnl_matrix<double>m1(3,3,0.0);
vnl_matrix<double>m2(3,3,0.0);
vnl_matrix<double>m3(3,3,0.0);

//scaling the first box along x direction by 3,translating along x direction by 10
//scaling along y direction by 2,translating along y direction by 20

double s1x = 3,t1x = 10,s1y = 3,t1y = 10,s2x = 4,s2y = 4,s3x = 2,s3y = 2;

set_transformation_matrices(m1,m2,m3,M,s1x,t1x,s1y,t1y,s2x,s2y,s3x,s3y);


ofstr <<  M.engine() << vcl_endl;
ofstr <<  M.body() << vcl_endl;
ofstr <<  M.rear() << vcl_endl;

double color_coeff =0.25;
 print_vrml_model(out,M,color_coeff);

  vcl_vector<vehicle_model> vehic_set;
  vehic_set.push_back(M);

  int num_models = 10;

for (int i = 0;i<num_models;i++)
    {
    

    vehicle_model MT = transform_model(M,m1,m2,m3);

    vehic_set.push_back(MT);
   
ofstr <<  MT.engine() << vcl_endl;
ofstr <<  MT.body() << vcl_endl;
ofstr <<  MT.rear() << vcl_endl;    

set_transformation_matrices(m1,m2,m3,M,s1x+i,t1x+i,s1y+i,t1y+i,s2x+i,s2y+i,s3x+i,s3y+i);

    }

for (int j = 0;j<vehic_set.size();j = j+1)
    {
     color_coeff = 0.25;
   //  color_coeff = 0.25+ double(i)/15;
 print_vrml_model(out,vehic_set[j],color_coeff);
    }

double k = (num_models/2)+0.5 ;
set_transformation_matrices(m1,m2,m3,M,s1x+k,t1x+k,s1y+k,t1y+k,s2x+k,s2y+k,s3x+k,s3y+k);

 vehicle_model new_model = transform_model(M,m1,m2,m3);

 color_coeff =0.0;
 print_vrml_model(out,new_model,color_coeff);

ofstr << "printing the new model" << vcl_endl;
ofstr <<  new_model.engine() << vcl_endl;
ofstr <<  new_model.body() << vcl_endl;
ofstr <<  new_model.rear() << vcl_endl;   

int closest_model_index = 1e10;
double dist,min_dist = 1e10;

for (int i = 0;i<num_models;i++)
    {
   dist = calculate_Lie_distance(vehic_set[i],new_model,vehic_set[0]);
ofstr <<dist << vcl_endl;
if (dist < min_dist)
    {
    min_dist = dist;
closest_model_index = i;
    }
    }

ofstr << " closest model index " << closest_model_index << vcl_endl;
    ofstr.close();

     print_vrml_model(out,vehic_set[closest_model_index],0.0);

  return 0;
    }
