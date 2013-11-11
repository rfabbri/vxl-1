// This is brcv/seg/dbcll/dbcll_k_means.txx
#ifndef dbcll_k_means_txx_
#define dbcll_k_means_txx_
//:
// \file
// \brief K-Means Clustering
// \author Matthew Leotta, (mleotta@lems.brown.edu)
// \date 2/29/08
//
// The fast version of the code is base on the algorithm described in:
//   C. Elkan
//   Using the Triangle Inequality to Accelerate KMeans
//   ICML 2003
//
// \verbatim
//  Modifications
// \endverbatim

#include "dbcll_k_means.h"
#include <vcl_cmath.h>
#include <vnl/vnl_random.h>


//: Apply K-Means clustering to the points
// means should be initialized with k points
template <unsigned dim>
unsigned dbcll_k_means(const vcl_vector<vnl_vector_fixed<double,dim> >& points,
                       vcl_vector<vcl_vector<unsigned> >& clusters,
                       vcl_vector<vnl_vector_fixed<double,dim> >& means,
                       const unsigned max_num_itr)
{
  typedef vnl_vector_fixed<double,dim> vector;

  if(means.empty())
    return 0;

  const unsigned k = means.size();
  clusters.clear();
  clusters.resize(k);

  // the minimum distances of each point to its cluster mean
  vcl_vector<double> min_dists(points.size(), vcl_numeric_limits<double>::infinity());


  // initialize the clusters
  for(unsigned pi=0; pi<points.size(); ++pi){
    const vector& point = points[pi];
    unsigned best_mean = 0;
    double& best_dist = min_dists[pi];
    best_dist = vcl_numeric_limits<double>::infinity();
    for(unsigned mi=0; mi<k; ++mi){
      double dist = vcl_sqrt(vnl_vector_ssd(point,means[mi]));
      if(dist < best_dist){
        best_dist = dist;
        best_mean = mi;
      }
    }
    clusters[best_mean].push_back(pi);
  }

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


//: Apply Accelerated K-Means clustering to the points
// means should be initialized with k points
template <unsigned dim>
unsigned dbcll_fast_k_means(const vcl_vector<vnl_vector_fixed<double,dim> >& points,
                            vcl_vector<vcl_vector<unsigned> >& clusters,
                            vcl_vector<vnl_vector_fixed<double,dim> >& means,
                            const unsigned max_num_itr)
{
  typedef vnl_vector_fixed<double,dim> vector;

  if(means.empty())
    return 0;

  const unsigned k = means.size();
  clusters.clear();
  clusters.resize(k);

  vcl_vector<vcl_vector<unsigned> > new_clusters(k);
  vcl_vector<double> mean_shifts(k,0.0);

  vcl_vector<double> ub(points.size(),vcl_numeric_limits<double>::infinity());
  vcl_vector<vcl_vector<double> > lbs(points.size(),vcl_vector<double>(k,0.0));

  // the distances between each pair of means
  vcl_vector<vcl_vector<double> > dcs(k);
  // compute center distances
  for(unsigned i=0; i<k; ++i)
    for(unsigned j=0; j<i; ++j)
      dcs[i].push_back(vcl_sqrt(vnl_vector_ssd(means[i],means[j])));

  // a vector of minimum distance between between each mean and other means
  // (divided by 2)
  vcl_vector<double> sc(k,vcl_numeric_limits<double>::infinity());


  // initialize the clusters
  for(unsigned pi=0; pi<points.size(); ++pi){
    const vector& point = points[pi];
    unsigned best_mean = 0;
    double&  best_dist = ub[pi];
    vcl_vector<double>& lb = lbs[pi];
    for(unsigned mi=0; mi<k; ++mi){
      if(cluster_distance(mi,best_mean,dcs) > 2*best_dist)
        continue;
      double dist = vcl_sqrt(vnl_vector_ssd(point,means[mi]));
      lb[mi] = dist;
      if(dist < best_dist){
        best_dist = dist;
        best_mean = mi;
      }
    }
    clusters[best_mean].push_back(pi);
  }

  bool change = true;
  unsigned n=0;
  for(; change && n<max_num_itr; ++n){

    // recompute the means
    for(unsigned mi=0; mi<k; ++mi){
      vector& mean = means[mi];
      vector old_mean = mean;
      vcl_vector<unsigned>& cluster = clusters[mi];
      // check for empty cluster
      if(cluster.empty()){
        mean = points[dbcll_random_sample_d2(ub)];
      }
      else{
        mean.fill(0.0);
        for(unsigned pi=0; pi<cluster.size(); ++pi){
          mean += points[cluster[pi]];
        }
        mean /= cluster.size();
      }
      mean_shifts[mi] = vcl_sqrt(vnl_vector_ssd(mean,old_mean));
    }

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

    // update the distances between each pair of means
    for(unsigned i=0; i<k; ++i)
      for(unsigned j=0; j<i; ++j)
        dcs[i][j] = vcl_sqrt(vnl_vector_ssd(means[i],means[j]));


    // update a vector of minimum distance between between each mean and other means
    // (divided by 2)
    for(unsigned i=0; i<k; ++i){
      double& s = sc[i];
      s = vcl_numeric_limits<double>::infinity();
      for(unsigned j=0; j<k; ++j){
        if(i==j) continue;
        double dc = cluster_distance(i,j,dcs);
        if(dc < 2*s)
          s = dc/2.0;
      }
    }

    change = false;
    for(unsigned i=0; i<k; ++i)
      new_clusters[i].clear();
    for(unsigned omi=0; omi<k; ++omi){
      vcl_vector<unsigned>& cluster = clusters[omi];
      const double& s = sc[omi];
      for(unsigned pi=0; pi<cluster.size(); ++pi){
        unsigned& ci = cluster[pi];
        double&  best_dist = ub[ci];
        if(best_dist <= s){
          new_clusters[omi].push_back(ci);
          continue;
        }
        const vector& point = points[ci];
        unsigned best_mean = omi;
        vcl_vector<double>& lb = lbs[ci];
        bool valid_best_dist = false;
        for(unsigned nmi=0; nmi<k; ++nmi){
          if(nmi == best_mean || best_dist <= lb[nmi] ||
             2*best_dist <= cluster_distance(nmi,best_mean,dcs))
            continue;
          if(!valid_best_dist){
            best_dist = vcl_sqrt(vnl_vector_ssd(point,means[best_mean]));
            lb[best_mean] = best_dist;
            valid_best_dist = true;
            if(best_dist <= lb[nmi] || 2*best_dist <= cluster_distance(nmi,best_mean,dcs))
              continue;
          }
          double dist = vcl_sqrt(vnl_vector_ssd(point,means[nmi]));
          lb[nmi] = dist;
          if(dist < best_dist){
            best_dist = dist;
            best_mean = nmi;
          }
        }
        new_clusters[best_mean].push_back(ci);
        if(best_mean != omi)
          change = true;
      }
    }
    clusters.swap(new_clusters);


  }

  return n;
}



//: Randomly choose k points from the set to initialize K-Means
template <unsigned dim>
vcl_vector<vnl_vector_fixed<double,dim> >
dbcll_init_k_means_rand(const vcl_vector<vnl_vector_fixed<double,dim> >& points, const unsigned k)
{
  vcl_vector<vnl_vector_fixed<double,dim> > means;
  vnl_random rand;

  for(unsigned i=0; i<k; ++i)
    means.push_back(points[rand.lrand32(points.size()-1)]);

  return means;
}


//: Randomly choose k points from the set to initialize K-Means
//  using the d2 distribution (K-Means++)
template <unsigned dim>
vcl_vector<vnl_vector_fixed<double,dim> >
dbcll_init_k_means_d2(const vcl_vector<vnl_vector_fixed<double,dim> >& points, unsigned k)
{
  vcl_vector<vnl_vector_fixed<double,dim> > means;
  vcl_vector<double > min_dist2(points.size(),vcl_numeric_limits<double>::infinity());
  vcl_vector<unsigned > best_mean(points.size(),0);
  vnl_random rand;

  //: Choose the first point at random
  means.push_back(points[rand.lrand32(points.size()-1)]);

  for(unsigned i=1; i<k; ++i)
  {
    const vnl_vector_fixed<double,dim>& last = means.back();
    vcl_vector<double > mean_dist2(i,0.0);
    for(unsigned j=0; j<i-1; ++j){
      mean_dist2[j] = vnl_vector_ssd(last,means[j]);
    }
    for(unsigned j=0; j<points.size(); ++j)
    {
      if(mean_dist2[best_mean[j]] > 4*min_dist2[j])
        continue;
      double d2 = vnl_vector_ssd(last, points[j]);
      if(d2 < min_dist2[j]){
        min_dist2[j] = d2;
        best_mean[j] = i-1;
      }
    }
    means.push_back(points[dbcll_random_sample_d(min_dist2)]);
  }
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
