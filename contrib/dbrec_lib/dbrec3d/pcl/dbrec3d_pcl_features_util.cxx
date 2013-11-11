//:
// \file
// \author Isabel Restrepo
// \date 11/3/11

#include "dbrec3d_pcl_features_util.h"

#include <pcl/kdtree/kdtree_flann.h>

#include <vul/vul_timer.h>

#include <vcl_iostream.h>


void dbrec3d_compute_pcl_fpfh_features(pcl::PointCloud<pcl::PointNormal>::Ptr cloud_in, 
                                       pcl::PointCloud<pcl::FPFHSignature33>::Ptr cloud_out,
                                       double cell_length, int region_unit_radius)
{

  // Create the FPFH estimation class, and pass the input dataset+normals to it
  pcl::FPFHEstimation<pcl::PointNormal, pcl::PointNormal, pcl::FPFHSignature33> fpfh;
  fpfh.setInputCloud (cloud_in);
  fpfh.setInputNormals (cloud_in);
  
  // Create an empty kdtree representation, and pass it to the FPFH estimation object.
  // Its content will be filled inside the object, based on the given input dataset (as no other search surface is given).
  pcl::search::KdTree<pcl::PointNormal>::Ptr tree (new pcl::search::KdTree<pcl::PointNormal> ());
  fpfh.setSearchMethod (tree);
  
  // Use all neighbors in a sphere of radius 5cm
  // IMPORTANT: the radius used here has to be larger than the radius used to estimate the surface normals!!!
  fpfh.setRadiusSearch (cell_length*(double)(region_unit_radius));
  
  // Compute the features
  vul_timer timer;
  timer.mark();
  vcl_cout << "Computing FPFH: " << vcl_endl;
  fpfh.compute (*cloud_out); 
  vcl_cout << "Done \n";
  timer.print(vcl_cout);
  

}

void dbrec3d_compute_pcl_fpfh_features(pcl::PointCloud<pcl::PointNormal>::Ptr cloud_in, 
                                       pcl::PointCloud<pcl::FPFHSignature33>::Ptr cloud_out,
                                       double radius)
{
  // Create the FPFH estimation class, and pass the input dataset+normals to it
  pcl::FPFHEstimation<pcl::PointNormal, pcl::PointNormal, pcl::FPFHSignature33> fpfh;
  fpfh.setInputCloud (cloud_in);
  fpfh.setInputNormals (cloud_in);

  // Create an empty kdtree representation, and pass it to the FPFH estimation object.
  // Its content will be filled inside the object, based on the given input dataset (as no other search surface is given).
  pcl::search::KdTree<pcl::PointNormal>::Ptr tree (new pcl::search::KdTree<pcl::PointNormal> ());
  fpfh.setSearchMethod (tree);

  // Use all neighbors in a sphere of radius 5cm
  // IMPORTANT: the radius used here has to be larger than the radius used to estimate the surface normals!!!
  fpfh.setRadiusSearch (radius);

  // Compute the features
  vul_timer timer;
  timer.mark();
  vcl_cout << "Computing FPFH: " << vcl_endl;
  fpfh.compute (*cloud_out); 
  vcl_cout << "Done \n";
  timer.print(vcl_cout);
  
}

//: Put the data inside the point cloud into a stl vector.
//  This only cares about the data and not the 3-d location
void dbrec3d_pcd_to_vnl_vector(pcl::PointCloud<pcl::FPFHSignature33>::Ptr cloud,
                               vcl_vector<vnl_vector_fixed<double, 33 > > &features)
{
  for (size_t i = 0; i < cloud->points.size (); ++i) {
    vnl_vector_fixed<double, 33> hist(0.0);
    for (unsigned bin = 0; bin<33; bin++) {
      hist[bin]=(double)cloud->points[i].histogram[bin];
    }
    features.push_back(hist);
  }
}


