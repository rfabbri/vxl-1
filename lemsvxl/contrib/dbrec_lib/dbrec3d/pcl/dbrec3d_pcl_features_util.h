#ifndef dbrec3d_pcl_features_util_h
#define dbrec3d_pcl_features_util_h

//:
// \file
// \brief 
// \author Isabel Restrepo mir@lems.brown.edu
// \date  11/3/11
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <pcl/point_types.h>
#include <pcl/features/fpfh.h>

#include <vcl_vector.h>

#include <vnl/vnl_vector_fixed.h>


//: Compute Fast Point Feature Histogram Features 
void dbrec3d_compute_pcl_fpfh_features(pcl::PointCloud<pcl::PointNormal>::Ptr cloud_in, 
                                       pcl::PointCloud<pcl::FPFHSignature33>::Ptr cloud_out,
                                       double cell_length, int region_unit_radius);

//: Compute Fast Point Feature Histogram Features 
void dbrec3d_compute_pcl_fpfh_features(pcl::PointCloud<pcl::PointNormal>::Ptr cloud_in, 
                                       pcl::PointCloud<pcl::FPFHSignature33>::Ptr cloud_out,
                                       double radius);

//: Put the data inside the point cloud into a stl vector.
//  This only cares about the data and not the 3-d location
void dbrec3d_pcd_to_vnl_vector(pcl::PointCloud<pcl::FPFHSignature33>::Ptr cloud,
                               vcl_vector<vnl_vector_fixed<double, 33> > &features);




#endif
