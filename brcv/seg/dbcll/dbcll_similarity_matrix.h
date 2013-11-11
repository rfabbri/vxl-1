// This is brcv/seg/dbcll/dbcll_similarity_matrix.h
#ifndef dbcll_similarity_matrix_h_
#define dbcll_similarity_matrix_h_
//:
// \file
// \brief A similarity matrix and cluster that uses it
// \author Matthew Leotta, (mleotta@lems.brown.edu)
// \date 3/27/08
//
//
// \verbatim
//  Modifications
// \endverbatim

#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include "dbcll_cluster.h"
#include "dbcll_cluster_sptr.h"
#include <vnl/vnl_sym_matrix.h>


class dbcll_similarity_matrix: public vbl_ref_count
{
 public:
  //: Constructor
  dbcll_similarity_matrix(unsigned n);

  //: Data Access
  double operator () (unsigned i, unsigned j) const
  {
    if (i==j) return 0.0;
    return (i > j) ? data_.fast(i-1,j) : data_.fast(j-1,i);
  }

  double& operator () (unsigned i, unsigned j)
  {
    assert(i!=j);
    return (i > j) ? data_.fast(i-1,j) : data_.fast(j-1,i);
  }

  //: Fast Data Access -- assumes i>j
  double fast(unsigned i, unsigned j) const
  {
    return data_.fast(i-1,j);
  }

  double& fast(unsigned i, unsigned j)
  {
    return data_.fast(i-1,j);
  }

  //: return the number of valid indices left in the table
  unsigned valid_size() const { return valid_idx_.size(); }

  //: reduce the matrix by merging rows i1 and i2 into the smaller of the two
  // return the index that contains the merged results
  // \param s1 and \param s2 are the size cluster sizes for weighted averaging
  unsigned reduce(unsigned i1, unsigned i2, unsigned s1, unsigned s2);

 private:
  vnl_sym_matrix<double> data_;
  vcl_vector<unsigned> valid_idx_;
};


typedef vbl_smart_ptr<dbcll_similarity_matrix> dbcll_similarity_matrix_sptr;


//: A cluster with precomputed similarity measures stored in a shared matrix
// Merging clusters uses group averaging to combine similarities
class dbcll_precompute_cluster: public dbcll_cluster
{
 public:
  //: Constructor - single index
  dbcll_precompute_cluster(unsigned idx,
                           const dbcll_similarity_matrix_sptr& M,
                           unsigned m_idx)
  : dbcll_cluster(idx), sim_matrix_(M), m_idx_(m_idx) {}

  //: Constructor - multiple indices
  dbcll_precompute_cluster(const vcl_vector<unsigned>& idxs,
                           const dbcll_similarity_matrix_sptr& M,
                           unsigned m_idx)
  : dbcll_cluster(idxs), sim_matrix_(M), m_idx_(m_idx) {}

  //: Destructor
  virtual ~dbcll_precompute_cluster() {}

  //: compute similarity with another cluster
  virtual double similarity(const dbcll_cluster& other) const;

  //: merge the other cluster into this one
  // subclasses should call this base class function to merge indices
  virtual void merge(const dbcll_cluster& other);

  //: make a clone of this cluster
  virtual dbcll_cluster* clone() const
  {
    // not safe to clone this type
    assert(!"Not safe to clone a precomputed cluster");
    return NULL;
  }

  //: return a measure of cluster variance
  virtual double var() const { return 0.0; }


 protected:
  //: pointer to shared similarity matrix
  dbcll_similarity_matrix_sptr sim_matrix_;
  //: index into similarity matrix
  unsigned m_idx_;

};


//: Compute a set of precomputed clusters from an existing set of clusters
vcl_vector<dbcll_cluster_sptr>
dbcll_precompute_similarity(const vcl_vector<dbcll_cluster_sptr>& clusters);




#endif // dbcll_similarity_matrix_h_
