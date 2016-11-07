#ifndef dbrec3d_pcl_codebook_utils_h
#define dbrec3d_pcl_codebook_utils_h

//:
// \file
// \brief 
// \author Isabel Restrepo mir@lems.brown.edu
// \date  11/10/11
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "dbrec3d_pcl_point_types.h"

#include <pcl/point_types.h>
#include <pcl/features/fpfh.h>

#include <vcl_vector.h>

#include <vnl/vnl_vector_fixed.h>

#include <dbcll/dbcll_k_means.h>
#include <dbcll/dbcll_euclidean_cluster_light.h>
#include <dbcll/dbcll_euclidean_cluster_util.h>

namespace dbrec3d_pcl_codebook_utils {


  //: Initialize k-means according to the algorithm in Bradley98
  //  Takes in 
  vcl_vector<vnl_vector_fixed<double,33> > init_codebook(unsigned K, unsigned long nsamples, unsigned J, unsigned max_it,
                                                        pcl::PointCloud<pcl::FPFHSignature33>::Ptr cloud_in);
   
  //: Learns the codebook
  void learn_codebook ( unsigned K, float init_fraction, unsigned J, unsigned max_it,
                  pcl::PointCloud<pcl::FPFHSignature33>::Ptr cloud_in,
                  vcl_vector<vnl_vector_fixed<double,33> > &means,
                  vcl_vector<dbcll_euclidean_cluster_light<33> > &all_clusters);
  
  //: Iterate throught the features and find the closest mean (in "means" vector) to the feature
  //  Return a point cloud with the center(x,y,z) if the feature, the id it belong to and the square distance to the mean
  void assign_features_to_cluster( pcl::PointCloud<pcl::FPFHSignature33>::Ptr features_cloud,
                                   pcl::PointCloud<pcl::PointNormal>::Ptr point_cloud,
                                   vcl_vector< vnl_vector_fixed<double, 33> > means,
                                   pcl::PointCloud<dbrec3d_pcl_point_types::PointClassId>::Ptr id_cloud);
  
  //: Iterate throught the features and find the closest mean (in "means" vector) to the feature
  //  For each  matched feature, record the relative position (with respect to the object center)
  //  Return a point cloud with the center(x,y,z) if the feature, the id it belong to and the square distance to the mean
  void assign_features_to_cluster_record_geometry( pcl::PointCloud<pcl::FPFHSignature33>::Ptr features_cloud,
                                  pcl::PointCloud<pcl::PointNormal>::Ptr point_cloud,
                                  vcl_vector< vnl_vector_fixed<double, 33> > means,
                                  pcl::PointCloud<dbrec3d_pcl_point_types::PointClassId>::Ptr id_cloud);
  
  //: Convert and id cloud to and rgb cloud for visualization purposes
  void convert_id_to_rgb(pcl::PointCloud<dbrec3d_pcl_point_types::PointClassId>::Ptr id_cloud,
                         unsigned nmeans, pcl::PointCloud<pcl::PointXYZRGB>::Ptr rgb_cloud);

}

#endif
