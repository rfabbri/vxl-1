#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_matrix_inverse.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_complexify.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_least_squares_function.h>
#include <vcl_vector.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include "vehicle_model.h"

struct geodesic_vnl_least_squares_function:public vnl_least_squares_function
    {
geodesic_vnl_least_squares_function(vcl_vector<vnl_matrix<double> > group_elements,vnl_vector<double> params_freeze,
                                    vnl_vector<double> fixed_param_values,int num_params);
                                                                                                                
    //since we are loooking for the projection onto 1-D sub manifold,there is only 1 parameter and the number of residuals 
    //equals the total number of elements in the matrix which for the 3x3 matrix is 9.


 void f(vnl_vector<double> const& params, vnl_vector<double>& residuals);

vcl_vector<vnl_matrix<double> >group_elements_;
vnl_vector<double> params_freeze_;
vnl_vector<double> fixed_param_values_;
int num_params_;
    };

struct geodesic_vnl_least_squares_function_9x9:public vnl_least_squares_function
    {
geodesic_vnl_least_squares_function_9x9(vcl_vector<vnl_matrix<double> > group_elements,vnl_vector<double> params_freeze,
                                    vnl_vector<double> fixed_param_values,int num_params);
                                                                                                                
    //since we are loooking for the projection onto 1-D sub manifold,there is only 1 parameter and the number of residuals 
    //equals the total number of elements in the matrix which for the 3x3 matrix is 9.


 void f(vnl_vector<double> const& params, vnl_vector<double>& residuals);

vcl_vector<vnl_matrix<double> >group_elements_;
vnl_vector<double> params_freeze_;
vnl_vector<double> fixed_param_values_;
int num_params_;
    };

struct projection_vnl_least_squares_function:public vnl_least_squares_function
    {
projection_vnl_least_squares_function(vnl_matrix<double> G1,vnl_matrix<double>sub_manifold_gen);
                                      
                                    

    //since we are loooking for the projection onto 1-D sub manifold,there is only 1 parameter and the number of residuals 
    //equals the total number of elements in the matrix which for the 3x3 matrix is 9.


 void f(vnl_vector<double> const& params, vnl_vector<double>& residuals);

vnl_matrix<double>  sub_manifold_generator_;
vnl_matrix<double>  group_element_;
    };


void set_transformation_matrices(vnl_matrix<double>& m1,vnl_matrix<double>& m2,vnl_matrix<double>& m3,vehicle_model M,
                            double s1x,double t1x,double s1y,double t1y,double s2x,double s2y,double s3x,double s3y,
                            double t3x,double t3y);
   
void transform_box(vnl_matrix<double> m,vsol_rectangle_2d box);
    

// only scales and translates the model 
//model should be aligned wrt x and y axes
vehicle_model transform_model(vehicle_model M,vnl_matrix<double> m1,vnl_matrix<double> m2,vnl_matrix<double> m3);
    

void rotate_box(vnl_matrix<double> m,vsol_rectangle_2d box);

vehicle_model rotate_model(vehicle_model M,double angle);
    
vnl_matrix<double> get_transformation_matrix(vsol_rectangle_2d from_box,vsol_rectangle_2d to_box);
   
double matrix_log_dist(vnl_matrix<double> M1,vnl_matrix<double> M2);
    
double calculate_Lie_distance_between_vectors
(vcl_vector<vsol_rectangle_2d >model1,vcl_vector<vsol_rectangle_2d  >model2,vcl_vector<vsol_rectangle_2d  >ref_model);
   
double calculate_Lie_distance(vehicle_model M1,vehicle_model M2,vehicle_model RM);


void get_transformation_matrices(vehicle_model M1,vehicle_model M2,vnl_matrix<double> &G1,
                                 vnl_matrix<double> &G2,vnl_matrix<double> &G3); 

//finds the Lie algebra element of a given group element..
//works for scaling,translation and rotation put together 
vnl_matrix<double> get_Lie_algebra(vnl_matrix<double> G);

//finds the Lie algebra element of a given group element of size 9x9
//this assumes that the 9x9 matrices whose lie algebra element is sought are 
//of the form G1 0 0
//            0 G2 0
//            0 0 G3
// where G1,G2 and G3 are 3x3 matrices 
vnl_matrix<double> get_Lie_algebra_9x9(vnl_matrix<double> G);

//finds the Lie group element of a given algebra element..
//works for scaling,translation and rotation put together 
vnl_matrix<double> get_Lie_group(vnl_matrix<double> g);

//finds the Lie group element of a given algebra element of size 9x9
//this assumes that the 9x9 matrices whose lie group element is sought are 
//of the form g1 0 0
//            0 g2 0
//            0 0 g3
// where g1,g2 and g3 are 3x3 matrices 
vnl_matrix<double> get_Lie_group_9x9(vnl_matrix<double> g);

void calculate_coupled_intrinsic_mean(vcl_vector<vnl_matrix<double> > box_one_elements,
                                            vcl_vector<vnl_matrix<double> > box_two_elements,
                                            vcl_vector<vnl_matrix<double> > box_three_elements,
                                            double M1_x2,double M1_x4,
                                              vnl_matrix<double> &In_mean_B1,
                                              vnl_matrix<double> &In_mean_B2,
                                              vnl_matrix<double> &In_mean_B3);


vnl_matrix<double> calculate_intrinsic_mean(vcl_vector<vnl_matrix<double> > group_elements);

double get_variance(vcl_vector<vnl_matrix<double> > group_elements,vnl_matrix<double> intrinsic_mean);

//get the optimal 1 d geodesic by minimizing the variation of projection of each point
//onto this goedesic

//      the number of unknowns would be as follows:

//      there would be one unknown scalar associated with each generator and there can be atmost 9 different
//      generator coefficients for a 3x3 algebra element.so this gives 9 unknowns.
//      for projecting each of the elements,there would be one unknown scalar associated.
//      so this gives 'n' unknowns where 'n' is the number of elements
//      The total number of unknowns is n+9 

vnl_matrix<double> get_geodesic(vcl_vector<vnl_matrix<double> > group_elements,vnl_vector<double> initial_params,
                                vnl_vector<double> params_freeze,vnl_vector<double> &params);

vnl_matrix<double> get_geodesic_9x9(vcl_vector<vnl_matrix<double> > group_elements,vnl_vector<double> initial_params,
                                vnl_vector<double> params_freeze,vnl_vector<double> &params);
                                
vnl_matrix<double> get_projection(vnl_matrix<double> G1,vnl_matrix<double> sub_manifold_gen);



