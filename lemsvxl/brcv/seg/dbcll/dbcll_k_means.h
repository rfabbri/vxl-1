// This is brcv/seg/dbcll/dbcll_k_means.h
#ifndef dbcll_k_means_h_
#define dbcll_k_means_h_
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
// The D^2 initialization is based on the algorithm described in:
//   D. Arthur, S. Vassilvitskii
//   k-means++ The Advantages of Careful Seeding
//   SODA 2007
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_vector.h>
#include <vcl_limits.h>
#include <vnl/vnl_vector_fixed.h>



//: Apply K-Means clustering to the points
// means should be initialized with k points
template <unsigned dim>
unsigned dbcll_k_means(const vcl_vector<vnl_vector_fixed<double,dim> >& points,
                       vcl_vector<vcl_vector<unsigned> >& clusters,
                       vcl_vector<vnl_vector_fixed<double,dim> >& means,
                       const unsigned max_num_itr = vcl_numeric_limits<unsigned>::max());


//: Apply Accelerated K-Means clustering to the points
// means should be initialized with k points
template <unsigned dim>
unsigned dbcll_fast_k_means(const vcl_vector<vnl_vector_fixed<double,dim> >& points,
                            vcl_vector<vcl_vector<unsigned> >& clusters,
                            vcl_vector<vnl_vector_fixed<double,dim> >& means,
                            const unsigned max_num_itr = vcl_numeric_limits<unsigned>::max());


//: Pick a new mean index using the K-Means++ D^2 weighing scheme
unsigned dbcll_random_sample_d2(const vcl_vector<double >& min_dists);


//: Pick a new mean index using the D weighing scheme
// equivalent to D^2 if min_dists is squared distance
unsigned dbcll_random_sample_d(const vcl_vector<double >& min_dists);


//: Randomly choose k points from the set to initialize K-Means
template <unsigned dim>
vcl_vector<vnl_vector_fixed<double,dim> >
dbcll_init_k_means_rand(const vcl_vector<vnl_vector_fixed<double,dim> >& points, unsigned k);


//: Randomly choose k points from the set to initialize K-Means
//  using the d2 distribution (K-Means++)
template <unsigned dim>
vcl_vector<vnl_vector_fixed<double,dim> >
dbcll_init_k_means_d2(const vcl_vector<vnl_vector_fixed<double,dim> >& points, unsigned k);


#endif // dbcll_k_means_h_
