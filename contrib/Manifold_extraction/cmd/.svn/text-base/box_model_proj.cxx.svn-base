//  This is under contrib\Manifold_extraction\vis
// Brief: An example for finding the best vehicle model position based on Lie distances


#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_matrix_inverse.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_complexify.h>
#include <vnl/vnl_vector_fixed.h>
#include <vgl/vgl_box_2d.h>
#include <vcl_vector.h>
#include <vbl/vbl_array_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_dialog.h>
#include <vehicle_model.h>
#include <vrml_print.h>


void set_transformation_matrices(vnl_matrix<double>& m1,vnl_matrix<double>& m2,vnl_matrix<double>& m3,vehicle_model M,
                            double s1x,double t1x,double s1y,double t1y,double s2x,double s2y,double s3x,double s3y)
    {
  double  xmin1 = M.engine().min_x();
  double  xmax1 = M.engine().max_x();

 

   double  xmin2 = M.body().min_x();
   double  xmax2 = M.body().max_x();

 
   double   xmin3 = M.rear().min_x();
   double  xmax3 = M.rear().max_x();
 assert(xmax1 == xmin2);
 assert(xmax2 == xmin3); 

m1.put(0,0,s1x);
m1.put(0,2,t1x);
m1.put(1,1,s1y);
m1.put(1,2,t1y);
m1.put(2,2,1);

m2.put(0,0,s2x);
m2.put(0,2,t1x+(s1x-s2x)*xmin2);
m2.put(1,1,s2y);
m2.put(1,2,t1y);
m2.put(2,2,1);

m3.put(0,0,s3x);
m3.put(0,2,s2x*xmax2+t1x+(s1x-s2x)*xmin2-s3x*xmin3);
m3.put(1,1,s3y);
m3.put(1,2,t1y);
m3.put(2,2,1);
    }
// only for m of size 3x3
vgl_box_2d<double> transform_box(vnl_matrix<double> m,vgl_box_2d<double> box)
    {

    vgl_point_2d<double>v1(box.min_x(),box.min_y());
    vgl_point_2d<double>v2(box.min_x(),box.max_y());
    vgl_point_2d<double>v3(box.max_x(),box.max_y());
    vgl_point_2d<double>v4(box.max_x(),box.min_y());

    // since only scaling and translation are done,the order of vertices would be preserved
vgl_point_2d<double>tv1(m.get(0,0)*v1.x()+m.get(0,2),m.get(1,1)*v1.y()+m.get(1,2));
vgl_point_2d<double>tv2(m.get(0,0)*v2.x()+m.get(0,2),m.get(1,1)*v2.y()+m.get(1,2));
vgl_point_2d<double>tv3(m.get(0,0)*v3.x()+m.get(0,2),m.get(1,1)*v3.y()+m.get(1,2));
vgl_point_2d<double>tv4(m.get(0,0)*v4.x()+m.get(0,2),m.get(1,1)*v4.y()+m.get(1,2));

vgl_box_2d<double>transformed_box(tv1,tv3);
return transformed_box;
    }

vehicle_model transform_model(vehicle_model M,vnl_matrix<double>const& m1,vnl_matrix<double>const& m2,vnl_matrix<double>const& m3)
    {
    double ty1 = M.engine().centroid_y();
    double ty2 = M.body().centroid_y();
    double ty3 = M.rear().centroid_y();

    vnl_matrix<double>T1(3,3,0.0); 
    vnl_matrix<double>T2(3,3,0.0); 
    vnl_matrix<double>T3(3,3,0.0); 

    T1.put(0,0,1);
    T1.put(1,1,1);
    T1.put(1,2,-ty1);

    T2.put(0,0,1);
    T2.put(1,1,1);
    T2.put(1,2,-ty2);

    T3.put(0,0,1);
    T3.put(1,1,1);
    T3.put(1,2,-ty3);

vgl_box_2d<double>te = transform_box(T1,M.engine());
vgl_box_2d<double>tr_e = transform_box(m1,te);
T1.put(1,2,ty1);
vgl_box_2d<double>tr_engine = transform_box(T1,tr_e);

vgl_box_2d<double>tb = transform_box(T2,M.body());
vgl_box_2d<double>tr_b = transform_box(m2,tb);
T1.put(1,2,ty2);
vgl_box_2d<double>tr_body = transform_box(T2,tr_b);

vgl_box_2d<double>trear = transform_box(T3,M.rear());
vgl_box_2d<double>tr_re = transform_box(m3,trear);
T3.put(1,2,ty3);
vgl_box_2d<double>tr_rear = transform_box(T3,tr_re);

//vgl_box_2d<double>tr_body = transform_box(m2,M.body());
//vgl_box_2d<double>tr_rear = transform_box(m3,M.rear());

vehicle_model tr_model(tr_engine,tr_body,tr_rear);

return tr_model;
    }


vnl_matrix<double> get_transformation_matrix(vgl_box_2d<double> from_box,vgl_box_2d<double> to_box)
    {
vnl_matrix<double> from_points(3,4,1);
vnl_matrix<double> to_points(3,4,1);

from_points.put(0,0,from_box.min_x());
from_points.put(1,0,from_box.min_y());

from_points.put(0,1,from_box.min_x());
from_points.put(1,1,from_box.max_y());

from_points.put(0,2,from_box.max_x());
from_points.put(1,2,from_box.max_y());

from_points.put(0,3,from_box.max_x());
from_points.put(1,3,from_box.min_y());

to_points.put(0,0,to_box.min_x());
to_points.put(1,0,to_box.min_y());

to_points.put(0,1,to_box.min_x());
to_points.put(1,1,to_box.max_y());

to_points.put(0,2,to_box.max_x());
to_points.put(1,2,to_box.max_y());

to_points.put(0,3,to_box.max_x());
to_points.put(1,3,to_box.min_y());

vnl_matrix<double> transformation_matrix = (to_points*from_points.transpose())* vnl_matrix_inverse<double>(from_points*from_points.transpose());
return transformation_matrix;

    }

double matrix_log_dist(vnl_matrix<double> M1,vnl_matrix<double> M2)
    {
vnl_matrix<double>resultant = M1*vnl_matrix_inverse<double>(M2);

 
double sx = vcl_log(resultant.get(0,0));
double sy = vcl_log(resultant.get(1,1));
double tx = resultant.get(0,2);
double ty = resultant.get(1,2);
double distance = 1e10;

// in order to take the log of the scaling factors,they have to be non negative
sx = vcl_abs(sx);
sy = vcl_abs(sy);

if (~(sx == 1)&&~(sy == 1))
distance = vcl_sqrt(vcl_pow((sx),2) + vcl_pow((sy),2) +
                  vcl_pow((tx*vcl_log(sx))/(sx-1),2) + vcl_pow((ty*vcl_log(sy))/(sy-1),2));


return distance;
    }

double calculate_Lie_distance_between_vectors
(vcl_vector<vgl_box_2d<double> >model1,vcl_vector<vgl_box_2d<double> >model2,vcl_vector<vgl_box_2d<double> >ref_model)
    {
vnl_matrix<double> T11 = get_transformation_matrix(model1[0],ref_model[0]);
vnl_matrix<double> T21 = get_transformation_matrix(model1[1],ref_model[1]);
vnl_matrix<double> T31 = get_transformation_matrix(model1[2],ref_model[2]);

vnl_matrix<double> T12 = get_transformation_matrix(model2[0],ref_model[0]);
vnl_matrix<double> T22 = get_transformation_matrix(model2[1],ref_model[1]);
vnl_matrix<double> T32 = get_transformation_matrix(model2[2],ref_model[2]);

// vcl_cout << T11 << vcl_endl;

double d = matrix_log_dist(T11,T12) + matrix_log_dist(T21,T22) + matrix_log_dist(T31,T32);
  //  double d = 0;
return d;
    }

double calculate_Lie_distance(vehicle_model M1,vehicle_model M2,vehicle_model RM)
    {
vnl_matrix<double> T11 = get_transformation_matrix(M1.engine(),RM.engine());
vnl_matrix<double> T21 = get_transformation_matrix(M1.body(),RM.body());
vnl_matrix<double> T31 = get_transformation_matrix(M1.rear(),RM.rear());

vnl_matrix<double> T12 = get_transformation_matrix(M2.engine(),RM.engine());
vnl_matrix<double> T22 = get_transformation_matrix(M2.body(),RM.body());
vnl_matrix<double> T32 = get_transformation_matrix(M2.rear(),RM.rear());

// vcl_cout << T11 << vcl_endl;

double d = matrix_log_dist(T11,T12) + matrix_log_dist(T21,T22) + matrix_log_dist(T31,T32);
  //  double d = 0;
return d;
    }


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
   
    
//vcl_vector< vgl_box_2d<double> >boxes1;
//vcl_vector< vgl_box_2d<double> >boxes2;
//vcl_vector< vgl_box_2d<double> >boxes3;
//vgl_box_2d<double>transformed_box1,transformed_box2,transformed_box3;


// defining the simple 3 box model 
vgl_box_2d<double>starting_box1(0.1,0.3,0.1,0.3);
vgl_box_2d<double>starting_box2(0.3,0.7,0,0.4);
vgl_box_2d<double>starting_box3(0.7,1,0.05,0.35);

//boxes1.push_back(starting_box1);
//boxes2.push_back(starting_box2);
//boxes3.push_back(starting_box3);
     
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










//debug_info << "transformation matrix 2:" << vcl_endl;
//    debug_info << m2 << vcl_endl;
//     debug_info << vcl_endl;
//
//    debug_info << "M body:" << vcl_endl;
//    debug_info << M.body() << vcl_endl;
//     debug_info << vcl_endl;
//
//    debug_info << "MT body:" << vcl_endl;
//    debug_info << MT.body() << vcl_endl;
//     debug_info << vcl_endl;
//
//      debug_info << vcl_endl;
//    debug_info << "transformation matrix 3:" << vcl_endl;
//    debug_info << m3 << vcl_endl;
//    
//    debug_info << "M rear:" << vcl_endl;
//    debug_info << M.rear() << vcl_endl;
//     debug_info << vcl_endl;
//
//    debug_info << "MT rear:" << vcl_endl;
//    debug_info << MT.rear() << vcl_endl;
//     debug_info << vcl_endl;
/*
for (i = 0;i<10;i++)
    {
    
    vehicle_model MT = transform_model(M,m1,m2,m3);
    
    vehic_set.push_back(MT);
    print_vrml_model(out,MT);
 

ofstr <<  MT.engine() << vcl_endl;
ofstr <<  MT.body() << vcl_endl;
ofstr <<  MT.rear() << vcl_endl;    

//debug_info << vcl_endl;
//debug_info << "printing the model " << vcl_endl;
//debug_info <<  MT.engine() << vcl_endl;
//debug_info <<  MT.body() << vcl_endl;
//debug_info <<  MT.rear() << vcl_endl;  
//debug_info << vcl_endl;

 /*transformed_box1 = transform_box(m1,starting_box1);
 transformed_box2 = transform_box(m2,starting_box2);
 transformed_box3 = transform_box(m3,starting_box3);*/

 /*debug_info << "printing the vectors " << vcl_endl;
 debug_info <<  transformed_box1 << vcl_endl;
debug_info <<  transformed_box2 << vcl_endl;
debug_info <<  transformed_box3 << vcl_endl;*/

 /*vehicle_model Mod(transformed_box1,transformed_box2,transformed_box3);

 print_vrml_model(out,Mod);

//boxes1.push_back(transformed_box1);
//boxes2.push_back(transformed_box2);
//boxes3.push_back(transformed_box3);
//
//ofstr <<  transformed_box1 << vcl_endl;
//ofstr <<  transformed_box2 << vcl_endl;
//ofstr <<  transformed_box3 << vcl_endl;

m1.put(0,0,s1x+i);
m1.put(0,2,t1x+i);
m1.put(1,1,s1y+i);
m1.put(1,2,t1y+i);
m1.put(2,2,1);

m2.put(0,0,s2x+i);
m2.put(0,2,t1x+i+((s1x+i-1)*l1x/2)+((s2x+i-1)*l2x/2));
m2.put(1,1,s2y+i);
m2.put(1,3,t1y+i);
m2.put(2,2,1);

m3.put(0,0,s3x+i);
m3.put(0,2,t1x+i+((s1x+i-1)*l1x/2)+((s2x+i-1)*l2x/2)+((s3x+i-1)*l3x/2));
m3.put(1,1,s3y+i);
m3.put(1,3,t1y+i);
m3.put(2,2,1);
    }
*/

//vcl_vector<vgl_box_2d<double> >ref_model;
//
//ref_model.push_back(boxes1[0]);
//ref_model.push_back(boxes2[0]);
//ref_model.push_back(boxes3[0]);
//
//for (i = 0;i<9;i++)
//    {
//vcl_vector<vgl_box_2d<double> >model1,model2;
//
//model1.push_back(boxes1[i]);
//model1.push_back(boxes2[i]);
//model1.push_back(boxes3[i]);
//
//model2.push_back(boxes1[i+1]);
//model2.push_back(boxes2[i+1]);
//model2.push_back(boxes3[i+1]);
//
//debug_info << vcl_endl;
//debug_info << "printing the model " << vcl_endl;
//debug_info <<  vehic_set[i].engine() << vcl_endl;
//debug_info <<  vehic_set[i].body() << vcl_endl;
//debug_info <<  vehic_set[i].rear()  << vcl_endl;  
//debug_info << vcl_endl;
//
//debug_info << "printing the vectors " << vcl_endl;
// debug_info <<  boxes1[i] << vcl_endl;
//debug_info <<  boxes2[i] << vcl_endl;
//debug_info <<  boxes3[i] << vcl_endl;
//
//ofstr << calculate_Lie_distance_between_vectors(model1,model2,ref_model)<< vcl_endl;
//
//
//    }




