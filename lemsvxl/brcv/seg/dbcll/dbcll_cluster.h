// This is brcv/seg/dbcll/dbcll_cluster.h
#ifndef dbcll_cluster_h_
#define dbcll_cluster_h_
//:
// \file
// \brief A base class for clusters
// \author Matthew Leotta, (mleotta@lems.brown.edu)
// \date 1/28/08
//
//
// \verbatim
//  Modifications
// \endverbatim

#include <vbl/vbl_ref_count.h>
#include <vcl_vector.h>
#include <vcl_iterator.h>
#include <vcl_algorithm.h>


//: A Cluster (base class)
class dbcll_cluster: public vbl_ref_count
{
 public:
  //: Default Constructor
  dbcll_cluster() {}
  //: Constructor - single index
  dbcll_cluster(unsigned idx) : members_(1,idx) {}
  //: Constructor - multiple indices
  dbcll_cluster(const vcl_vector<unsigned>& idxs) : members_(idxs) {}
  //: Destructor
  virtual ~dbcll_cluster() {}

  //: compute similarity with another cluster
  virtual double similarity(const dbcll_cluster& other) const = 0;

  //: merge the other cluster into this one
  // subclasses should call this base class function to merge indices
  virtual void merge(const dbcll_cluster& other)
  {
    vcl_vector<unsigned> tmp;
    vcl_merge(this->members_.begin(), this->members_.end(),
              other.members_.begin(), other.members_.end(),
              vcl_back_inserter(tmp));

    this->members_.swap(tmp);
  }

  //: make a clone of this cluster
  virtual dbcll_cluster* clone() const = 0;

  //: return a measure of cluster variance
  virtual double var() const = 0;

  //: The first index in the set is the key index
  unsigned key_index() const { return members_.front(); }

  //: Access the set of member indices
  vcl_vector<unsigned> members() const { return members_; }

  //: The size of a cluster is the number of members it has
  unsigned size() const { return members_.size(); }

 protected:
  vcl_vector<unsigned> members_;

};




#endif // dbcll_cluster_h_
