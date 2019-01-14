/*************************************************************************
 *    NAME: pradeep
 *    FILE: Lie_spoke_utilities.h
 *    DATE: 06 Aug 2007
 // brief:  The basic representation for shape is as follows
 //Given the samples along the contour,the shape is represented by spokes
 //joining the samples to the centroid of the sample points

 //This file contains some utility functions for the spoke representation
 *************************************************************************/

#include <vsol/vsol_spatial_object_2d.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>


//value in (-Pi,Pi]
double spoke_curve_fixAngleMPiPi (double a);

//computes the centroid given the sample points along a contour
  vsol_point_2d_sptr compute_centroid(std::vector<vsol_point_2d_sptr> contour);

//computes the scales and angles of a shape wrt a reference shape 
  void compute_spoke_scales_angles(std::vector<vsol_point_2d_sptr> contour,std::vector<vsol_point_2d_sptr> ref_contour,
                                                       std::vector<double> &scales,std::vector<double> &angles);

 //transforms the spoke configuration 
 std::vector<vsol_point_2d_sptr> transform_spoke_shape(std::vector<vsol_point_2d_sptr> contour,std::vector<double> scales,
                                                      std::vector<double> angles);

 //computes the Lie cost for deforming curve 1 into curve 2  
 double compute_lie_spoke_cost(std::vector<vsol_point_2d_sptr> curve1,std::vector<vsol_point_2d_sptr> curve2);

 void loadCON(std::string fileName, std::vector<vsol_point_2d_sptr> &points);

 void writeCON(std::string fileName,std::vector<vsol_point_2d_sptr> points);

 //use vul file iterator to get all the con files residing in a directory
 std::vector<std::string> get_all_files(std::string file_path);

