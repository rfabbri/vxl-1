// This is dbcll_euclidean_cluster_util.h
#ifndef dbcll_euclidean_cluster_util_h
#define dbcll_euclidean_cluster_util_h

//:
// \file
// \brief A class with utilities for euclidean clusters
// \author Isabel Restrepo mir@lems.brown.edu
// \date  14-Sep-2011.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vnl/vnl_vector_fixed.h>
#include <vcl_limits.h>
#include <vcl_vector.h>

template <unsigned dim>
class dbcll_euclidean_cluster_util{
  
public:
  
  
  dbcll_euclidean_cluster_util(vcl_vector<vnl_vector_fixed<double,dim> > const &means)
  {
    k_ = means.size();

    for(unsigned i =0; i < k_; i++)
    {
      clusters_.push_back(dbcll_euclidean_cluster_light<dim>(means[i]));
    }
    
    // the distances between each pair of means
    dist_clusters_.reserve(k_);
    dist_clusters_.resize(k_);
    
    // compute center distances
    for(unsigned i=0; i<k_; ++i)
      for(unsigned j=0; j<i; ++j)
        dist_clusters_[i].push_back(vcl_sqrt(vnl_vector_ssd(clusters_[i].mean(),clusters_[j].mean())));
  }

  
  
  dbcll_euclidean_cluster_util(vcl_vector<dbcll_euclidean_cluster_light<dim> > const &clusters)
  {
    k_ = clusters.size();
    clusters_=clusters;
    
    // the distances between each pair of means
    dist_clusters_.reserve(k_);
    dist_clusters_.resize(k_);
    
    // compute center distances
    for(unsigned i=0; i<k_; ++i)
      for(unsigned j=0; j<i; ++j)
        dist_clusters_[i].push_back(vcl_sqrt(vnl_vector_ssd(clusters_[i].mean(),clusters_[j].mean())));
    
    
  }
  
  //: Returns the closes mean to the given point
  unsigned closest_euclidean_cluster(const vnl_vector_fixed<double,dim>& point, double &best_dist);
  
  //: Returns the means, with a dist(point,mean) < s * std(cluster)
  void closest_euclidean_clusters(double s, const vnl_vector_fixed<double,dim>& point,
                                  vcl_vector<unsigned> &best_means);
  
protected:
  // the distances between each pair of means
  vcl_vector<vcl_vector<double> > dist_clusters_;
  // cluster centers
  vcl_vector<dbcll_euclidean_cluster_light<dim> > clusters_;
  // number of clusters
  unsigned k_;
};

//: Returns the closest mean to the given point
template <unsigned dim>
unsigned dbcll_euclidean_cluster_util<dim>::closest_euclidean_cluster(const vnl_vector_fixed<double,dim>& point,
                                                                      double &best_dist)
{
  
  
  unsigned best_mean = 0;
  best_dist = vcl_numeric_limits<double>::infinity();
  for(unsigned mi=0; mi<k_; ++mi){
    //triangle inequality to avoid redundant distance calculations
    if(cluster_distance(mi,best_mean,dist_clusters_) > 2*best_dist)
      continue;
    double dist = vcl_sqrt(vnl_vector_ssd(point,clusters_[mi].mean()));
    if(dist < best_dist){
      best_dist = dist;
      best_mean = mi;
    }
  }
  return best_mean;
}


//: Returns the means, with a dist(point,mean) < s * std(cluster)
template <unsigned dim>
void dbcll_euclidean_cluster_util<dim>::closest_euclidean_clusters(double s, const vnl_vector_fixed<double,dim>& point,
                                                                   vcl_vector<unsigned> &best_means)
{
  best_means.clear();
  
  for(unsigned mi=0; mi<k_; ++mi){
    double dist = vnl_vector_ssd(point,clusters_[mi].mean());
    if(dist < s * clusters_[mi].var()){
      best_means.push_back(mi);
    }
  }
}

#endif
