// This is brcv/seg/dbcll/dbcll_parallel_k_means.txx
#ifndef dbcll_k_means_txx_
#define dbcll_k_means_txx_
//:
// \file
// \brief  Functions used for parallel K-Means Clustering (local contribution). 
// \author Isabel Restrepo mir@lems.brown.edu
// \date  25-Mar-2011.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "dbcll_parallel_k_means.h"
#include <vcl_cmath.h>
#include <vnl/vnl_random.h>

//: Compute distances between each pair of centroids
template <unsigned dim>
unsigned dbcll_parallel_k_means:init_pairwise_center_distance(const vcl_vector<vnl_vector_fixed<double,dim> >& centroids,
                                                         vcl_vector<vcl_vector<double> > &d_cc,
                                                         vcl_vector<double> &sc)
{
  const unsigned k = centroids.size();
  dcc.clear();
  dcc.resize(k, 0.0);
  
  // the distances between each pair of means
  vcl_vector<vcl_vector<double> > d_cc(k);
  // compute center distances
  for(unsigned i=0; i<k; ++i)
    for(unsigned j=0; j<i; ++j)
      d_cc[i].push_back(vcl_sqrt(vnl_vector_ssd(centroids[i],centroids[j])));
  
  // a vector of minimum distance between between each mean and other means
  // (divided by 2)
  vcl_vector<double>
  sc.clear();
  sc.resize(k,vcl_numeric_limits<double>::infinity());
  
  for(unsigned i=0; i<k; ++i){
    double& s = sc[i];
    for(unsigned j=0; j<k; ++j){
      if(i==j) continue;
      double dc = cluster_distance(i,j,d_cc);
      if(dc < 2*s)
        s = dc/2.0;
    }
  }
  
}


//: Compute distances between each pair of centroids
template <unsigned dim>
unsigned dbcll_parallel_k_means:update_pairwise_center_distance(const vcl_vector<vnl_vector_fixed<double,dim> >& centroids,
                                                              vcl_vector<vcl_vector<double> > &d_cc,
                                                              vcl_vector<double> &sc)
{
  const unsigned k = centroids.size();
  
  // the distances between each pair of means
  vcl_vector<vcl_vector<double> > d_cc(k);
  // compute center distances
  for(unsigned i=0; i<k; ++i)
    for(unsigned j=0; j<i; ++j)
      d_cc[i].push_back(vcl_sqrt(vnl_vector_ssd(centroids[i],centroids[j])));
  
  // a vector of minimum distance between between each mean and other means
  // (divided by 2)
  
  for(unsigned i=0; i<k; ++i){
    double& s = sc[i];
    for(unsigned j=0; j<k; ++j){
      if(i==j) continue;
      double dc = cluster_distance(i,j,d_cc);
      if(dc < 2*s)
        s = dc/2.0;
    }
  }
  
}


//: Assign n-points to k-means. Return cluster assigment and partial sum
//  This function is proccessed by the clients
template <unsigned dim>
unsigned dbcll_parallel_k_means::dbcll_init_k_means(   const vcl_vector<vnl_vector_fixed<double,dim> >& points,
                                                       const vcl_vector<vnl_vector_fixed<double,dim> >& centroids,
                                                       vcl_vector<vcl_vector<unsigned> >& clusters,
                                                       vcl_vector<vnl_vector_fixed<double,dim> >&partial_sums,
                                                       vcl_vector<double> &ub,
                                                       vcl_vector<vcl_vector<double> > lbs)
{
  typedef vnl_vector_fixed<double,dim> vector;

  if(means.empty())
    return 0;

  const unsigned k = centroids.size();
  clusters.clear();
  clusters.resize(k);

  // the minimum distances of each point to its cluster mean
  vcl_vector<double> min_dists(points.size(), vcl_numeric_limits<double>::infinity());

  // partial sum of each cluster
  partial_sums.clear();
  partial_sums.resize(k, vnl_vector_fixed<double,dim> >(0.0));
  
  //upper and lower bounds
  ub.clear();
  ub.resize(points.size(),vcl_numeric_limits<double>::infinity());
  lbs.clear();
  lbs.resize(points.size(),vcl_vector<double>(k,0.0));
  
   // initialize the clusters
  for(unsigned pi=0; pi<points.size(); ++pi){
    const vector& point = points[pi];
    unsigned best_mean = 0;
    double& best_dist = ub[pi];
    vcl_vector<double>& lb = lbs[pi];
    for(unsigned mi=0; mi<k; ++mi){
      if(cluster_distance(mi,best_mean,dcs) > 2*best_dist)
        continue;
      double dist = vcl_sqrt(vnl_vector_ssd(point,centroids[mi]));
      lb[mi] = dist;
      if(dist < best_dist){
        best_dist = dist;
        best_mean = mi;
      }
    }
    clusters[best_mean].push_back(pi);
    partial_sums[best_mean]+=point;
  }
}

//: Assign n-points to k-means. Return cluster assigment and partial sum
//  This function is proccessed by the clients
template <unsigned dim>
unsigned dbcll_parallel_k_means::dbcll_update_k_means(  const vcl_vector<vnl_vector_fixed<double,dim> >& points,
                                                        const vcl_vector<vnl_vector_fixed<double,dim> >& means,
                                                        vcl_vector<vcl_vector<unsigned> >& clusters,
                                                        vcl_vector<vnl_vector_fixed<double,dim> >&partial_sums,
                                                        vcl_vector<double> &ub,
                                                        vcl_vector<vcl_vector<double> > lbs
                                                        bool &change)
{
  typedef vnl_vector_fixed<double,dim> vector;
  
  if(means.empty())
    return 0;
  
  const unsigned k = means.size();
  
  change = false;
  vcl_vector<vcl_vector<unsigned> > new_clusters(k);
  for(unsigned i=0; i<k; ++i)
    new_clusters[i].clear();
  
  //iterate through old means
  for(unsigned old_mi=0; old_mi<k; ++old_mi){
    vcl_vector<unsigned>& cluster = clusters[old_mi];
    
    // minimum distance between between this mean and other means (divided by 2)
    const double& s = sc[old_mi];
    
    //iterate through all point in this cluster
    for(unsigned pi=0; pi<cluster.size(); ++pi){
      
      unsigned& point_idx = cluster[pi];
      
      double&  best_dist = ub[point_idx];
      
      //Step 2: Identify all points that u(x) <= s(c(x)) - points get assigned to the same cluster
      if(best_dist <= s){  
        new_clusters[old_mi].push_back(point_idx);
        continue;
      }
      
      const vector& point = points[point_idx];
      unsigned best_mean = old_mi;
      vcl_vector<double>& lb_c= lb_cx[point_idx];
      bool valid_best_dist = false;
      //Step 3:
      for(unsigned new_mi=0; new_mi<k; ++new_mi){
        
        // skip all points s.t c!=c(x), u(x) > l(x,c) and u(x) > (1/2)d(c(x),c)
        if(new_mi == best_mean || best_dist <= lb_c[new_mi] ||
           2*best_dist <= cluster_distance(new_mi,best_mean,dcs))
          continue;
        
        if(!valid_best_dist){
          best_dist = vcl_sqrt(vnl_vector_ssd(point,means[best_mean]));
          lb[best_mean] = best_dist;
          valid_best_dist = true;
          if(best_dist <= lb[new_mi] || 2*best_dist <= cluster_distance(new_mi,best_mean,dcs))
            continue;
        }
        double dist = vcl_sqrt(vnl_vector_ssd(point,means[new_mi]));
        lb_c[new_mi] = dist;
        if(dist < best_dist){
          best_dist = dist;
          best_mean = new_mi;
        }
      }
      new_clusters[best_mean].push_back(point_idx);
      if(best_mean != old_mi)
        change = true;
      else {
        new_partial_sum[best_mean]+=point;
        
      }

    }
  }
  clusters.swap(new_clusters);
}




//: Assign n-points to k-means. Return cluster assigment and partial sum
//  This function is proccessed by the clients
//  Partial sums are only modified for points that hanges membership
//  Cluster membership is computed using triangle inequality
template <unsigned dim>
void dbcll_parallel_k_means::add_pairwise_partial_sums( const vcl_vector<vnl_vector_fixed<double,dim> >&partial_sums1, const vcl_vector<unsigned long> n1,
                                                            const vcl_vector<vnl_vector_fixed<double,dim> >&partial_sums2, const vcl_vector<unsigned long> n2,
                                                            vcl_vector<vnl_vector_fixed<double,dim> >&partial_sums_out, vcl_vector<unsigned long> n_out)
{
  
  const unsigned k = partial_sums1.size();

  partial_sums.clear();
  partial_sums.resize(k, vnl_vector_fixed<double,dim>(0.0));
  
  for (unsigned mi=0; mi<k; ++mi) {
    partial_sums_out[mi] = partial_sums1[mi] + partial_sums2[mi];
    n_out[mi] = n1[mi]+n2[mi];
  }
  
  return;
}



//: Assign n-points to k-means. Return cluster assigment and partial sum
//  This function is proccessed by the clients
//  Partial sums are only modified for points that hanges membership
//  Cluster membership is computed using triangle inequality
template <unsigned dim>
void dbcll_parallel_k_means::recompute_means( const vcl_vector<vnl_vector_fixed<double,dim> >&total_sum, const vcl_vector<unsigned long> cluster_size,
                                              vcl_vector<vnl_vector_fixed<double,dim> >& means,
                                              vcl_vector<vnl_vector_fixed<double,dim> >& new_means,)
{
  const unsigned k = means.size();

  for (unsigned mi=0; mi<k; ++mi) {
    vector& mean = means[mi];
    vector old_mean = mean;
    
    // check for empty cluster
    if(cluster_size[mi] = 0){
      //mean = points[dbcll_random_sample_d2(ub)];
    }
    else {
      mean = total_sum[mi]/cluster_size[mi];
    }
    mean_shifts[mi] = vcl_sqrt(vnl_vector_ssd(mean,old_mean));
  }
  
  return;
}

// To be computed by clients after mean-shifts have been calculated
template <unsigned dim>
void dbcll_parallel_k_means::update_bounds( const vcl_vector<vnl_vector_fixed<double,dim> >& points,
                                            const vcl_vector<double >&mean_shifts, 
                                            vcl_vector<vcl_vector<unsigned> >& clusters,
                                            vcl_vector<vnl_vector_fixed<double,dim> >& means,
                                           vcl_vector<double> &ub,
                                           vcl_vector<vcl_vector<double> > &lbs)
{
  // update the lower bounds
  for(unsigned pi=0; pi<points.size(); ++pi){
    vcl_vector<double>& lb = lbs[pi];
    for(unsigned mi=0; mi<k; ++mi){
      double& l = lb[mi];
      l -= mean_shifts[mi];
      if(l < 0.0) l = 0.0;
    }
  }
  
  // update the upper bounds
  for(unsigned mi=0; mi<k; ++mi){
    vcl_vector<unsigned>& cluster = clusters[mi];
    for(unsigned pi=0; pi<cluster.size(); ++pi){
      unsigned& ci = cluster[pi];
      ub[ci] += mean_shifts[mi];
    }
  }
  
}


//: Recompute centers
template <unsigned dim>
unsigned dbcll_recompute_centers( vcl_vector<vcl_vector<unsigned> >& clusters,
                                  vcl_vector<vnl_vector_fixed<double,dim> >& means)
{
  bool change = true;
  unsigned n=0;
  for(; change && n<max_num_itr; ++n){
    // recompute the means
    for(unsigned mi=0; mi<k; ++mi){
      vector& mean = means[mi];
      vcl_vector<unsigned>& cluster = clusters[mi];
      // check for empty cluster
      if(cluster.empty()){
        mean = points[dbcll_random_sample_d2(min_dists)];
      }
      else{
        mean.fill(0.0);
        for(unsigned pi=0; pi<cluster.size(); ++pi){
          mean += points[cluster[pi]];
        }
        mean /= cluster.size();
      }
    }

    // recompute the clusters
    vcl_vector<vcl_vector<unsigned> > new_clusters(k);
    change = false;
    for(unsigned omi=0; omi<k; ++omi){
      vcl_vector<unsigned>& cluster = clusters[omi];
      for(unsigned pi=0; pi<cluster.size(); ++pi){
        const vector& point = points[cluster[pi]];
        unsigned best_mean = 0;
        double& best_dist = min_dists[cluster[pi]];
        best_dist = vcl_numeric_limits<double>::infinity();
        for(unsigned nmi=0; nmi<k; ++nmi){
          double dist = vcl_sqrt(vnl_vector_ssd(point,means[nmi]));
          if(dist < best_dist){
            best_dist = dist;
            best_mean = nmi;
          }
        }
        new_clusters[best_mean].push_back(cluster[pi]);
        if(best_mean != omi)
          change = true;
      }
    }
    clusters.swap(new_clusters);
  }

  return n;
}


namespace {

  inline double cluster_distance(unsigned i, unsigned j,
                                 const vcl_vector<vcl_vector<double> >& dcs)
  {
    if(i==j) return 0.0;
    if(i>j)  return dcs[i][j];
    return dcs[j][i];
  }
};



//: Randomly choose k points from the set to initialize K-Means suing Bradley98 refinment algotrithm
template <unsigned dim>
vcl_vector<vnl_vector_fixed<double,dim> >
dbcll_parallel_k_means::init_k_means_refining(const vcl_vector<vnl_vector_fixed<double,dim> >& points, const unsigned k)
{
  vcl_vector<vnl_vector_fixed<double,dim> > means;
  vnl_random rand;
  
  for(unsigned i=0; i<k; ++i)
    means.push_back(points[rand.lrand32(points.size()-1)]);
  
  return means;
}



#define DBCLL_K_MEANS_INSTANTIATE(T) \
template \
unsigned dbcll_k_means(const vcl_vector<vnl_vector_fixed<double,T > >& points,\
                       vcl_vector<vcl_vector<unsigned> >& clusters,\
                       vcl_vector<vnl_vector_fixed<double,T > >& means,\
                       const unsigned max_num_itr);\
template \
unsigned dbcll_fast_k_means(const vcl_vector<vnl_vector_fixed<double,T > >& points,\
                            vcl_vector<vcl_vector<unsigned> >& clusters,\
                            vcl_vector<vnl_vector_fixed<double,T > >& means,\
                            const unsigned max_num_itr);\
template \
vcl_vector<vnl_vector_fixed<double,T > > \
dbcll_init_k_means_rand(const vcl_vector<vnl_vector_fixed<double,T > >& points, const unsigned k);\
template \
vcl_vector<vnl_vector_fixed<double,T > > \
dbcll_init_k_means_d2(const vcl_vector<vnl_vector_fixed<double,T > >& points, unsigned k);



#endif // dbcll_k_means_txx_


