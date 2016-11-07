// This is dbcll_parallel_k_means.h
#ifndef dbcll_parallel_k_means_h
#define dbcll_parallel_k_means_h

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

//: Parallel K means 
//  This class is meant to used by parallel processes that share no memory among eachother nor whith the server.
//  Thus all functions are static and no class variables are kept in memory
class dbcll_parallel_k_means 
{
public:
  
  //: Assign n-points to k-means. Return cluster assigment and partial sum
  template <unsigned dim>
  static unsigned dbcll_assign_k_means( const vcl_vector<vnl_vector_fixed<double,dim> >& points,
                                        const vcl_vector<vnl_vector_fixed<double,dim> >& centroids,
                                        vcl_vector<vcl_vector<unsigned> >& clusters,
                                       vcl_vector<vnl_vector_fixed<double,dim> >&partial_sums);


};
#endif
