// This is brcv/seg/dbsta/dbsta_detector.h
#ifndef dbsta_detector_h_
#define dbsta_detector_h_

//:
// \file
// \brief A base class for functors that detect in distributions 
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 07/13/05
//
// Detectors take a distribution and a sample (of the same
// dimension) and make a boolean decision.  Thus they detect
// the presence of some characteristic of the sample given
// the distribution.
//
// If you want to make continuous valued decisions use
// dbsta_functor
//
// \verbatim
//  Modifications
// \endverbatim

#include "dbsta_distribution.h"
#include <vbl/vbl_ref_count.h>


//: A base class for functors that detect in distributions
template <class T>
class dbsta_detector : public vbl_ref_count
{
 public:
  //: Destructor
  virtual ~dbsta_detector(){}
  
  //: The main function
  // \note this function is const because the updater should not maintain state
  //       The same detector instance may act on multiple distributions
  virtual bool operator() (const dbsta_distribution<T>& d, 
                           const vnl_vector<T>& sample) const = 0;
                           
 protected:
  //: Constructor
  dbsta_detector(){}

};


//: A simple probability thresholding detector
template <class T>
class dbsta_probability_detector : public dbsta_detector<T>
{
 public:
  //: Constructor
  dbsta_probability_detector(const T& thresh=T(0.1)) : threshold(thresh) {}
  
  //: The main function
  // \retval true if the probability is above the threshold
  virtual bool operator() (const dbsta_distribution<T>& d, 
                           const vnl_vector<T>& sample) const
  {
    assert(d.dim() == sample.size());
    return d.probability(sample) > threshold; 
  }
                          
  T threshold;
};

#endif // dbsta_detector_h_
