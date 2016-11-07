// This is brcv/seg/dbsta/dbsta_discrete.h
#ifndef dbsta_discrete_h_
#define dbsta_discrete_h_

//:
// \file
// \brief A discrete distribution (also see dbsta_histogram) 
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 08/17/05
//
// \verbatim
//  Modifications
// \endverbatim


#include "dbsta_distribution.h"
#include <vcl_vector.h>


//: A discrete distribution 
// This distribution maps the continuous space into a bounded
// discrete array of probabilities
template <class T>
class dbsta_discrete_base : public dbsta_distribution<T>
{
 public:
  
  //: Destructor
  virtual ~dbsta_discrete_base<T>(){}
  
  //: Return the number of dimensions in the space
  virtual unsigned int dim() const { return dim_; }
    
  //: The probability of this sample
  virtual T probability(const vnl_vector<T>& pt) const
  { 
    int idx = index(pt); 
    if(idx == -1) return T(0);
    return prob_idx(idx);
  }
   
 protected:
  //: Constructor - each dimension has the same range and number of bins
  dbsta_discrete_base<T>(unsigned int dim, T min, T max, 
                         unsigned int bins);
  
  //: Constructor - each dimension has different parameters
  dbsta_discrete_base<T>(unsigned int dim, const vcl_vector<T>& min, 
                         const vcl_vector<T>& max, 
                         const vcl_vector<unsigned int>& bins);
                    
  //: return the probability for a given index
  virtual T prob_idx(unsigned int index) const = 0;
  
  //: Map a vector into an index for a discrete bin
  // \returns -1 if the vector is out of bounds
  int index(const vnl_vector<T>& pt) const;

  
  unsigned int dim_;
  vcl_vector<T> min_vals_;
  vcl_vector<T> max_vals_;
  vcl_vector<unsigned int> num_bins_;
 
};




#endif // dbsta_discrete_h_
