// This is brcv/seg/dbcll/dbcll_sim3d_cluster.h
#ifndef dbcll_sim3d_cluster_h_
#define dbcll_sim3d_cluster_h_
//:
// \file
// \brief A cluster on 3D similarity Lie group manifold
// \author Matthew Leotta, (mleotta@lems.brown.edu)
// \date 3/25/08
//
//
// \verbatim
//  Modifications
// \endverbatim


#include "dbcll_cluster.h"
#include <vcl_cassert.h>
#include <dbgl/algo/dbgl_similarity_3d.h>

#include "dbcll_cluster_sptr.h"


//: A cluster on the similarity manifold
class dbcll_sim3d_cluster : public dbcll_cluster
{
 public:
  //: Default Constructor 
  dbcll_sim3d_cluster()
    : xforms_(NULL), mean_(), var_(0.0) {}

  //: Constructor - from a single point
  dbcll_sim3d_cluster(const vcl_vector<dbgl_similarity_3d<double> >& xforms,
                      unsigned idx)
    : dbcll_cluster(idx), xforms_(&xforms), mean_(xforms[idx]), var_(0.0) {}

  //: Constructor
  dbcll_sim3d_cluster(const vcl_vector<dbgl_similarity_3d<double> >& xforms,
                      const vcl_vector<unsigned>& idxs)
    : dbcll_cluster(idxs), xforms_(&xforms), mean_(xforms[idxs[0]]), var_(0.0)
  {
    compute_stats();
  }

  //: Destructor
  ~dbcll_sim3d_cluster() {}

  //: make a clone of this cluster
  dbcll_cluster* clone() const
  {
    return new dbcll_sim3d_cluster(*this);
  }

  //: return a measure of cluster variance
  virtual double var() const { return var_; }

  //: return the mean
  const dbgl_similarity_3d<double>& mean() const { return mean_; }

  //: compute similarity with another cluster
  double similarity(const dbcll_cluster& other) const;

  //: compute similarity with a point
  double similarity(const dbgl_similarity_3d<double>& pt) const;

  //: merge the other cluster into this one
  void merge(const dbcll_cluster& other);

 protected:
  //: compute the mean and variance
  void compute_stats();

 private:
  const vcl_vector<dbgl_similarity_3d<double> >* xforms_;
  dbgl_similarity_3d<double> mean_;
  double var_;

};


//: Generate a vector of single element clusters
vcl_vector<dbcll_cluster_sptr> 
dbcll_init_sim3d_clusters(const vcl_vector<dbgl_similarity_3d<double> >& pts);



#endif // dbcll_sim3d_cluster_h_
