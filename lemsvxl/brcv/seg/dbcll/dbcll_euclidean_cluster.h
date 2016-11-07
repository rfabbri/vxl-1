// This is brcv/seg/dbcll/dbcll_euclidean_cluster.h
#ifndef dbcll_euclidean_cluster_h_
#define dbcll_euclidean_cluster_h_
//:
// \file
// \brief A cluster in Euclidean space
// \author Matthew Leotta, (mleotta@lems.brown.edu)
// \date 1/28/08
//
//
// \verbatim
//  Modifications
// \endverbatim


#include "dbcll_cluster.h"
#include <vcl_cassert.h>
#include <vnl/vnl_vector_fixed.h>

#include "dbcll_cluster_sptr.h"


//: A cluster in Euclidean space
template <unsigned dim>
class dbcll_euclidean_cluster : public dbcll_cluster
{
 public:
  //: Constructor - Default
  dbcll_euclidean_cluster()
  : mean_(0.0), var_(0.0) {} 
  
  //: Constructor - from a single point
  dbcll_euclidean_cluster(const vnl_vector_fixed<double,dim>& pt, 
                          unsigned idx)
    : dbcll_cluster(idx), mean_(pt), var_(0.0) {}

  //: Constructor
  dbcll_euclidean_cluster(const vnl_vector_fixed<double,dim>& m, 
                          double v, const vcl_vector<unsigned>& idxs)
    : dbcll_cluster(idxs), mean_(m), var_(v) {}

  //: Destructor
  ~dbcll_euclidean_cluster() {}

  //: make a clone of this cluster
  dbcll_cluster* clone() const
  {
    return new dbcll_euclidean_cluster<dim>(*this);
  }

  //: return a measure of cluster variance
  virtual double var() const { return var_; }

  //: return the mean
  const vnl_vector_fixed<double,dim>& mean() const { return mean_; }

  //: compute similarity with another cluster
  double similarity(const dbcll_cluster& other) const
  {
    // Cast to the appropriate type 
    assert(dynamic_cast<const dbcll_euclidean_cluster<dim>*>(&other));
    const dbcll_euclidean_cluster<dim>& c2 =
      static_cast<const dbcll_euclidean_cluster<dim>&>(other);
    
    return -(this->var_ + c2.var_ + vnl_vector_ssd(this->mean_,c2.mean_));
  }
  
  //: compute similarity with a point
  double similarity(const vnl_vector_fixed<double,dim>& pt) const
  {
    return -(this->var_ + vnl_vector_ssd(this->mean_, pt));
  }
  
  //: merge the other cluster into this one
  void merge(const dbcll_cluster& other)
  {
    // Cast to the appropriate type 
    assert(dynamic_cast<const dbcll_euclidean_cluster<dim>*>(&other));
    const dbcll_euclidean_cluster<dim>& c2 =
    static_cast<const dbcll_euclidean_cluster<dim>&>(other);

    double n = static_cast<double>(this->size());
    double m = static_cast<double>(c2.size());
    double t = n+m;

    this->var_ = (n*this->var_ + m*c2.var_ 
                 + n*m/t * vnl_vector_ssd(this->mean_, c2.mean_))/t;

    this->mean_ = (n*this->mean_ + m*c2.mean_)/t;

    dbcll_cluster::merge(other);
  }

 private:
  vnl_vector_fixed<double,dim> mean_;
  double var_;

};


//: Generate a vector of single element Euclidean clusters from vnl points
template <unsigned d>
vcl_vector<dbcll_cluster_sptr> 
dbcll_init_euclidean_clusters(const vcl_vector<vnl_vector_fixed<double,d> >& pts)
{
  vcl_vector<dbcll_cluster_sptr> results;
  for(unsigned i=0; i<pts.size(); ++i)
    results.push_back(new dbcll_euclidean_cluster<d>(pts[i],i));
  return results;
}



#endif // dbcll_euclidean_cluster_h_
