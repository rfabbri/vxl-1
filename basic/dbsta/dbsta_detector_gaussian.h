// This is brcv/seg/dbsta/algo/dbsta_detector_gaussian.h
#ifndef dbsta_detector_gaussian_h_
#define dbsta_detector_gaussian_h_

//:
// \file
// \brief Detectors applying to Gaussians  
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 07/20/05
//
// \verbatim
//  Modifications
// \endverbatim

#include <dbsta/dbsta_gaussian.h>
#include <dbsta/dbsta_detector.h>



//: A simple Mahalanobis distance detector for a Gaussian
// detects samples that lie within some Mahalanobis distance
template <class T>
class dbsta_g_mdist_detector : public dbsta_detector<T>
{
 public:
  //: Constructor
  dbsta_g_mdist_detector(const T& thresh=T(2.5)) : threshold(thresh) {}
  
  //: The main function
  // \retval true if the Mahalanobis distance is less than the threshold
  virtual bool operator() (const dbsta_distribution<T>& d, 
                           const vnl_vector<T>& sample) const
  {
    assert(d.dim() == sample.size());
    assert(dynamic_cast<const dbsta_gaussian<T>* >(&d));
    const dbsta_gaussian<T>& g = static_cast<const dbsta_gaussian<T>& >(d);
    return g.mahalanobis_dist(sample) < threshold; 
  }
   
  //: the threshold on Mahalanobis distance                       
  T threshold;
};




#endif // dbsta_detector_gaussian_h_
