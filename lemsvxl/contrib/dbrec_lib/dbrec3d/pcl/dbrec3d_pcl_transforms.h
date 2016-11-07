#ifndef dbrec3d_pcl_transforms_h
#define dbrec3d_pcl_transforms_h

//:
// \file
// \brief Set of functions to transform (scale, rotate point cloud data)
// \author Isabel Restrepo mir@lems.brown.edu
// \date  11/22/11
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <pcl/common/centroid.h>
#include <pcl/common/transforms.h>
#include <pcl/common/eigen.h>


namespace dbrec3d_pcl_transforms {
  
  //: Substracts centroid from point cloud and rotates the coordinate system such that x axis is aligned with
  //  the eigen vector associated with the largest eigen value of the XY covariance matrix
  template <class T_point>
  void xy_canonical(const pcl::PointCloud<T_point> &cloud_in, 
                    pcl::PointCloud<T_point> &cloud_out);
}



//: Substracts centroid from point cloud and rotates the coordinate system such that x axis is aligned with
//  the eigen vector associated with the largest eigen value of the XY covariance matrix
template <class T_point>
void dbrec3d_pcl_transforms::xy_canonical(const pcl::PointCloud<T_point> &cloud_in, 
                                                pcl::PointCloud<T_point> &cloud_out)
{
  //a temporary cloud
  pcl::PointCloud<T_point> temp_cloud;

  
  //substract centroid
  Eigen::Vector4f centroid;
  pcl::compute3DCentroid(cloud_in, centroid);
  pcl::demeanPointCloud(cloud_in, centroid, temp_cloud);
  
  //compute covariance 
  centroid =- centroid;
  Eigen::Matrix3f covariance_matrix;
  pcl::computeCovarianceMatrix(temp_cloud, centroid, covariance_matrix);
  
  //eigen value decomposition on XY covariance
  Eigen::Matrix2f eigenvectors;
  Eigen::Vector2f eigenvalues;
  Eigen::Matrix2f XY_covariance;
  
  XY_covariance(0,0) = covariance_matrix (0,0);
  XY_covariance(0,1) = covariance_matrix (0,1);
  XY_covariance(1,0) = covariance_matrix (1,0);
  XY_covariance(1,1) = covariance_matrix (1,1);
  
  
  Eigen::SelfAdjointEigenSolver<Eigen::Matrix2f> ei_symm (XY_covariance);
  eigenvalues  = ei_symm.eigenvalues ();
  eigenvectors = ei_symm.eigenvectors ();
  
  //if (ei_symm.info() != Success) vcl_abort();
  vcl_cout << "The eigenvalues are:\n" << ei_symm.eigenvalues() << vcl_endl;
  vcl_cout << "The eigenvectors are:\n"
  << ei_symm.eigenvectors() << vcl_endl;
  //pcl::eigen33(XY_covariance, eigenvectors, eigenvalues);
  
  Eigen::Vector3f x_axis(eigenvectors(0,0), eigenvectors(1,0), 0.0f);
  Eigen::Vector3f y_axis(eigenvectors(0,1), eigenvectors(1,1), 0.0f);
  Eigen::Affine3f transformation;
  
  pcl::getTransFromUnitVectorsXY(x_axis, y_axis, transformation);
  pcl::transformPointCloudWithNormals(temp_cloud, cloud_out, transformation);
    
}


#endif
