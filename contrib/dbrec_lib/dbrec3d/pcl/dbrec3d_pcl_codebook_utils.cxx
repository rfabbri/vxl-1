//:
// \file
// \author Isabel Restrepo
// \date 11/10/11

#include "dbrec3d_pcl_codebook_utils.h"
#include "dbrec3d_pcl_features_util.h"

#include <pcl/filters/random_sample.h>

#include <vcl_ctime.h>
#include <vcl_iostream.h>

#include <vil/algo/vil_colour_space.h>


//: Initialize k-means according to the algorithm in Bradley98
vcl_vector<vnl_vector_fixed<double,33> > 
dbrec3d_pcl_codebook_utils::init_codebook(unsigned K, unsigned long nsamples, unsigned J, unsigned max_it,
                                         pcl::PointCloud<pcl::FPFHSignature33>::Ptr cloud)
{ 
  
  vcl_vector<vnl_vector_fixed<double,33> > rnd_means;
  vcl_vector<vnl_vector_fixed<double,33> > CM;
  vcl_vector<vcl_vector<vnl_vector_fixed<double,33> > > FM(J);
  vcl_vector<vcl_vector<vnl_vector_fixed<double,33> > > initial_means(J);
  
  
  //create the pcl random sample filter
  pcl::RandomSample<pcl::FPFHSignature33> random_filter;
  random_filter.setInputCloud(cloud);
  random_filter.setSample(K);
  pcl::PointCloud<pcl::FPFHSignature33>::Ptr temp_cloud(new pcl::PointCloud<pcl::FPFHSignature33>);
  random_filter.filter(*temp_cloud);
  
  //makes a deep copy of the cloud into a vector of vnl vectors
  dbrec3d_pcd_to_vnl_vector(temp_cloud, rnd_means);  
  temp_cloud->clear();
  
  for (unsigned j=0; j<J; j++) {
    
    initial_means[j] = rnd_means;
    
    vcl_vector<vnl_vector_fixed<double,33> > subsamples;
    
    //sample a random subsample from the cloud
    random_filter.setSample(nsamples);
    random_filter.setSeed(vcl_time(NULL));
    random_filter.filter(*temp_cloud);
    
    //makes a deep copy of the cloud into a vector of vnl vectors
    dbrec3d_pcd_to_vnl_vector(temp_cloud, subsamples);  
    temp_cloud->clear();
    
    //perform k-means for subsumples
    vcl_vector<vcl_vector<unsigned> > clusters;
    vcl_cout << "Size subsamples: " << subsamples.size() << "\n";
       
    unsigned n_iterations = dbcll_fast_k_means<33>(subsamples, clusters, initial_means[j], max_it);
    vcl_cout << "Number of means: " << initial_means[j].size() << "\n";
    subsamples.clear();
    vcl_cout <<" Number of iterations for fast-k means is: " << n_iterations << vcl_endl;  
    CM.insert(CM.end(), initial_means[j].begin(), initial_means[j].end()); 
    FM[j] = initial_means[j];
  }
  
  int min_j = -1;
  double min_sse = vcl_numeric_limits<double>::infinity();
  
  //choose the initial set of means that minimizes sum of square differences between points and means
  for (unsigned j=0; j<J; j++) {
    vcl_vector<vcl_vector<unsigned> > clusters;
    unsigned n_iterations = dbcll_fast_k_means(CM, clusters, FM[j], max_it);
    vcl_cout <<" Number of iterationsfor fast-k means is: " << n_iterations << vcl_endl;
    vcl_vector<dbcll_euclidean_cluster_light<33> > all_clusters;
    dbcll_init_euclidean_clusters(CM, clusters, FM[j], all_clusters);
    double total_sse = 0.0;
    for(unsigned ci = 0; ci<all_clusters.size(); ci++){
      dbcll_euclidean_cluster_light<33> cluster = all_clusters[ci];
      total_sse+=cluster.var()*(double)cluster.size();
    }
    if(total_sse < min_sse){
      min_sse = total_sse;
      min_j =j;
    }
  }
  
  return FM[min_j];
  
}

//: Learns the codebook
void 
dbrec3d_pcl_codebook_utils::learn_codebook ( unsigned K, float init_fraction,unsigned J, unsigned max_it,
                                            pcl::PointCloud<pcl::FPFHSignature33>::Ptr cloud_in,
                                            vcl_vector<vnl_vector_fixed<double,33> > &means,
                                            vcl_vector<dbcll_euclidean_cluster_light<33> > &all_clusters)
{
  unsigned long nsamples = (unsigned long)((float)cloud_in->points.size() * init_fraction);

  means = init_codebook(K, nsamples, J, max_it, cloud_in);
  //makes a deep copy of the cloud into a vector of vnl vectors
  
  vcl_vector<vnl_vector_fixed<double,33> > features;

  //Fix this, performs a deep copy, it should just operate on the cloud data directly
  dbrec3d_pcd_to_vnl_vector(cloud_in, features);  
  
  //Perform fast-kmeans on all samples
  vcl_vector<vcl_vector<unsigned> > clusters;
  unsigned n_iterations = dbcll_fast_k_means(features, clusters, means, max_it);
  vcl_cout <<" Number of iterationsfor fast-k means is: " << n_iterations << vcl_endl;  

  dbcll_init_euclidean_clusters(features, clusters, means, all_clusters);
    
}

//: Iterate throught the features and find the closest mean (in "means" vector) to the feature
//  Return a point cloud with the center(x,y,z) if the feature, the id it belong to and the square distance to the mean
void 
dbrec3d_pcl_codebook_utils::assign_features_to_cluster(pcl::PointCloud<pcl::FPFHSignature33>::Ptr fpfh_cloud,
                                                      pcl::PointCloud<pcl::PointNormal>::Ptr point_cloud,
                                                      vcl_vector< vnl_vector_fixed<double, 33> > means,
                                                      pcl::PointCloud<dbrec3d_pcl_point_types::PointClassId>::Ptr id_cloud)
{

  //Initialize cluster utils
  dbcll_euclidean_cluster_util<33> cluster_util(means);
  
  if(id_cloud->size() > 0)
    id_cloud->clear();
  
  id_cloud->reserve(fpfh_cloud->size());
  
  pcl::PointCloud<pcl::FPFHSignature33>::const_iterator fpfh_it = fpfh_cloud->begin();
  pcl::PointCloud<pcl::PointNormal>::const_iterator point_it = point_cloud->begin();
  
  //iterate through the features
  for (; fpfh_it!=fpfh_cloud->end() && point_it != point_cloud->end(); fpfh_it++, point_it++) {
    double dist;
    vnl_vector_fixed<double, 33> hist(0.0);
    for (unsigned bin =0; bin < 33; bin++) {
      hist[bin] = (double)fpfh_it->histogram[bin];
    }
    unsigned id  = cluster_util.closest_euclidean_cluster(hist, dist);
    dbrec3d_pcl_point_types::PointClassId id_point;
    id_point.x = point_it->x; id_point.y = point_it->y; id_point.z = point_it->z; id_point.class_id = (int)id; id_point.weight = (float) dist;
    id_cloud->push_back(id_point);
    
  }

  
}

//: Convert and id cloud to and rgb cloud for visualization purposes
void dbrec3d_pcl_codebook_utils::convert_id_to_rgb(pcl::PointCloud<dbrec3d_pcl_point_types::PointClassId>::Ptr id_cloud,
                                                   unsigned nmeans, pcl::PointCloud<pcl::PointXYZRGB>::Ptr rgb_cloud)
{
  pcl::PointCloud<dbrec3d_pcl_point_types::PointClassId>::const_iterator id_it = id_cloud->begin();
  
  
  float hue_interval = 360.0f/(float)nmeans;
  
  //iterate through the features assigning color
  for (; id_it!=id_cloud->end(); id_it++) {
    
    if(id_it->weight > 2.0)
      continue;
    
    pcl::PointXYZRGB point_rgb;
    float r, g, b;
    vil_colour_space_HSV_to_RGB<float>(((float)(id_it->class_id)) * hue_interval ,1.0f,255.0f,&r, &g, &b);
    
    uint32_t rgb = ((uint32_t)r << 16 | (uint32_t)g << 8 | (uint32_t)b);
    point_rgb.rgb = *reinterpret_cast<float*>(&rgb);

    point_rgb.x = id_it->x; point_rgb.y = id_it->y; point_rgb.z = id_it->z;
    rgb_cloud->push_back(point_rgb);
  }
  
  vcl_cout<< vcl_endl;
  
  for (unsigned id =0 ; id < nmeans; id++) {
    float r, g, b;
    vil_colour_space_HSV_to_RGB<float>(((float)(id)) * hue_interval ,1.0f,255.0f,&r, &g, &b);
     uint32_t rgb = ((uint32_t)r << 16 | (uint32_t)g << 8 | (uint32_t)b);
    vcl_cout << "Id :    " << id << " r: " << r << " g: " << g << " b: " << b << "rgb: " << rgb << "float: " << *reinterpret_cast<float*>(&rgb) << vcl_endl;

  }

}