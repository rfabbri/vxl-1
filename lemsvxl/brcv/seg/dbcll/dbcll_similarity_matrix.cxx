// This is brcv/seg/dbcll/dbcll_similarity_matrix.cxx
// \file
// \brief A similarity matrix and cluster that uses it
// \author Matthew Leotta, (mleotta@lems.brown.edu)
// \date 3/27/08
//


#include "dbcll_similarity_matrix.h"
#include <vcl_vector.h>


//: Constructor
dbcll_similarity_matrix::dbcll_similarity_matrix(unsigned n)
 : data_(n-1), valid_idx_(n)
{
  for(unsigned i=0; i<n; ++i)
    valid_idx_[i] = i;
}

//: reduce the matrix by merging rows i1 and i2 into the smaller of the two
// return the index that contains the merged results
// \param s1 and \param s2 are the size cluster sizes for weighted averaging
unsigned
dbcll_similarity_matrix::reduce(unsigned i1, unsigned i2, unsigned s1, unsigned s2)
{
  if(i1 > i2){
    vcl_swap(i1,i2);
    vcl_swap(s1,s2);
  }
  double st = s1+s2;
  vcl_vector<unsigned>::iterator to_remove = valid_idx_.end();
  for(vcl_vector<unsigned>::iterator i=valid_idx_.begin(); i!=valid_idx_.end(); ++i)
  {
    unsigned i3 = *i;
    if(i3 == i2){
      to_remove = i;
      continue;
    }
    if(i3 == i1)
      continue;

    double& v1 = (*this)(i1,i3);
    double& v2 = (*this)(i2,i3);
    v1 = (s1*v1 + s2*v2)/st;
  }
  valid_idx_.erase(to_remove);
  return i1;
}


//: compute similarity with another cluster
double dbcll_precompute_cluster::similarity(const dbcll_cluster& other) const
{
  // Cast to the appropriate type
  assert(dynamic_cast<const dbcll_precompute_cluster*>(&other));
  const dbcll_precompute_cluster& c2 =
    static_cast<const dbcll_precompute_cluster&>(other);

  const dbcll_similarity_matrix& M = *this->sim_matrix_;

  return M(this->m_idx_,c2.m_idx_);
}

//: merge the other cluster into this one
// subclasses should call this base class function to merge indices
void dbcll_precompute_cluster::merge(const dbcll_cluster& other)
{
  // Cast to the appropriate type
  assert(dynamic_cast<const dbcll_precompute_cluster*>(&other));
  const dbcll_precompute_cluster& c2 =
    static_cast<const dbcll_precompute_cluster&>(other);


  this->m_idx_ = sim_matrix_->reduce(this->m_idx_, c2.m_idx_,
                                     this->size(), c2.size());

  dbcll_cluster::merge(other);
}



vcl_vector<dbcll_cluster_sptr>
dbcll_precompute_similarity(const vcl_vector<dbcll_cluster_sptr>& clusters)
{
  const unsigned num = clusters.size();
  dbcll_similarity_matrix_sptr sim_matrix(new dbcll_similarity_matrix(num));

  // populate the matrix
  for(unsigned i=0; i<num; ++i){
    for(unsigned j=0; j<i; ++j){
      sim_matrix->fast(i,j) = clusters[i]->similarity(*clusters[j]);
    }
  }

  vcl_vector<dbcll_cluster_sptr> new_clusters;
  for(unsigned i=0; i<num; ++i){
    new_clusters.push_back(new dbcll_precompute_cluster(clusters[i]->key_index(),
                                                        sim_matrix, i));
  }

  return new_clusters;
}



