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
double compute_lie_cost(std::vector<vsol_point_2d_sptr> curve1_samples,std::vector<vsol_point_2d_sptr> curve2_samples );

//compute the angles and scales at each joint of articulated structure
void angles_scales(std::vector<vsol_point_2d_sptr> curve1,std::vector<vsol_point_2d_sptr> curve2,
                   std::vector<double> &angles,std::vector<double> &scales);

//transform the articulated structure with the specified angles and scales at different joints
std::vector<vsol_point_2d_sptr> transform_shape(std::vector<vsol_point_2d_sptr> curve1,
                                               std::vector<double> angles,std::vector<double> scales);

//generates the samples along a geodesic.'t' is a scalar which determines which point along the geodesic joining identity and the given 
//point is being output
void generate_values_along_geodesic(std::vector<double> angles,std::vector<double> scales,std::vector<double> &sample_angles,
                                    std::vector<double> &sample_scales,double t);

void generate_spoke_values_along_geodesic(std::vector<double> angles,std::vector<double> scales,std::vector<double> &sample_angles,
                                                                          std::vector<double> &sample_scales,double t);          

//transforms the input structure so that it becomes closed
std::vector<vsol_point_2d_sptr> closed_articulated_structure(std::vector<vsol_point_2d_sptr> final_points);

//saves the articulated structure as a contour file
void save_shape(std::vector<vsol_point_2d_sptr> new_shape,std::string new_shapes_path,unsigned int i);

//saves the articulated structure as an image
void save_shape_as_image(std::vector<vsol_point_2d_sptr> new_shape,std::string new_shapes_path,
                         unsigned int i);

//reads the angles and scales representing a particular geodesic direction
void read_geodesic_info(std::string geodesics_info,std::vector<double> &angles,std::vector<double> &scales);

void read_spoke_geodesic_info(std::string geodesics_info,std::vector<double> &angles,std::vector<double> &scales);

//computes the frobenious norm for a pair of points 
double compute_frob_norm(std::vector<double> sample_angles,std::vector<double> sample_scales,
                          std::vector<double>instance_angles,std::vector<double> instance_scales);

//finds the projection of a given point onto the geodesic direction
void find_projection_on_geodesic(std::vector<double> geodesic_angles,std::vector<double> geodesic_scales,
                                 std::vector<double> instance_angles,std::vector<double> instance_scales,
                                 std::vector<double> &proj_angles,std::vector<double> &proj_scales,double & proj_cost,int &proj_coeff);

void find_spoke_projection_on_geodesic(std::vector<double> geodesic_angles,std::vector<double> geodesic_scales,
                                 std::vector<double> instance_angles,std::vector<double> instance_scales,
                                 std::vector<double> &proj_angles,std::vector<double> &proj_scales,double & proj_cost,int &proj_coeff);

