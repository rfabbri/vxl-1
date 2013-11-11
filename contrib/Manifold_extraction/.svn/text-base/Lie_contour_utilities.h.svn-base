/*************************************************************************
 *    NAME: pradeep
 *    FILE: Lie_contour_utilities.h
 *    DATE: 06 Aug 2007
 // brief:  The basic representation for shape is as follows
 //Shape is represented by the samples along the contour 


 //This file contains some utility functions for the above mentioned shape representation
 *************************************************************************/

#include <vsol/vsol_spatial_object_2d.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>



//compute the lie cost for deforming curve 1 into curve 2
double compute_lie_cost(vcl_vector<vsol_point_2d_sptr> curve1_samples,vcl_vector<vsol_point_2d_sptr> curve2_samples );

//compute the angles and scales at each joint of articulated structure
void angles_scales(vcl_vector<vsol_point_2d_sptr> curve1,vcl_vector<vsol_point_2d_sptr> curve2,
                   vcl_vector<double> &angles,vcl_vector<double> &scales);

//transform the articulated structure with the specified angles and scales at different joints
vcl_vector<vsol_point_2d_sptr> transform_shape(vcl_vector<vsol_point_2d_sptr> curve1,
                                               vcl_vector<double> angles,vcl_vector<double> scales);

//generates the samples along a geodesic.'t' is a scalar which determines which point along the geodesic joining identity and the given 
//point is being output
void generate_values_along_geodesic(vcl_vector<double> angles,vcl_vector<double> scales,vcl_vector<double> &sample_angles,
                                    vcl_vector<double> &sample_scales,double t);

void generate_spoke_values_along_geodesic(vcl_vector<double> angles,vcl_vector<double> scales,vcl_vector<double> &sample_angles,
                                                                          vcl_vector<double> &sample_scales,double t);          

//transforms the input structure so that it becomes closed
vcl_vector<vsol_point_2d_sptr> closed_articulated_structure(vcl_vector<vsol_point_2d_sptr> final_points);

//saves the articulated structure as a contour file
void save_shape(vcl_vector<vsol_point_2d_sptr> new_shape,vcl_string new_shapes_path,unsigned int i);

//saves the articulated structure as an image
void save_shape_as_image(vcl_vector<vsol_point_2d_sptr> new_shape,vcl_string new_shapes_path,
                         unsigned int i);

//reads the angles and scales representing a particular geodesic direction
void read_geodesic_info(vcl_string geodesics_info,vcl_vector<double> &angles,vcl_vector<double> &scales);

void read_spoke_geodesic_info(vcl_string geodesics_info,vcl_vector<double> &angles,vcl_vector<double> &scales);

//computes the frobenious norm for a pair of points 
double compute_frob_norm(vcl_vector<double> sample_angles,vcl_vector<double> sample_scales,
                          vcl_vector<double>instance_angles,vcl_vector<double> instance_scales);

//finds the projection of a given point onto the geodesic direction
void find_projection_on_geodesic(vcl_vector<double> geodesic_angles,vcl_vector<double> geodesic_scales,
                                 vcl_vector<double> instance_angles,vcl_vector<double> instance_scales,
                                 vcl_vector<double> &proj_angles,vcl_vector<double> &proj_scales,double & proj_cost,int &proj_coeff);

void find_spoke_projection_on_geodesic(vcl_vector<double> geodesic_angles,vcl_vector<double> geodesic_scales,
                                 vcl_vector<double> instance_angles,vcl_vector<double> instance_scales,
                                 vcl_vector<double> &proj_angles,vcl_vector<double> &proj_scales,double & proj_cost,int &proj_coeff);

