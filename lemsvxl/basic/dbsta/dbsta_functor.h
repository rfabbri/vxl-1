// This is brcv/seg/dbsta/dbsta_functor.h
#ifndef dbsta_functor_h_
#define dbsta_functor_h_

//:
// \file
// \brief Base classes for general functors operating on distrubutions  
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 07/13/05
//
// 
//
// \verbatim
//  Modifications
// \endverbatim

#include "dbsta_distribution.h"
#include <vbl/vbl_ref_count.h>

//: A base class for general functors operating on distributions
template <class T>
class dbsta_functor : public vbl_ref_count
{
 public:
  //: Destructor
  virtual ~dbsta_functor(){}
  
  //: The size of the vector returned by the functor
  //  \retval 0 assume the return size is the same as the number of dimensions
  virtual unsigned int return_size() const { return 0; }
  
  //: The main function
  virtual vnl_vector<T> operator() ( const dbsta_distribution<T>& d ) const = 0;
  
 protected:
  //: Constructor
  dbsta_functor(){}
};


//: A base class for general functors operating on a distribution and a vector
template <class T>
class dbsta_data_functor : public vbl_ref_count
{
 public:
  //: Destructor
  virtual ~dbsta_data_functor(){}
  
  //: The size of the vector returned by the functor
  //  \retval 0 assume the return size is the same as the sample size
  virtual unsigned int return_size() const { return 0; }
  
  //: The main function
  virtual vnl_vector<T> operator() ( const dbsta_distribution<T>& d, 
                                     const vnl_vector<T>& sample ) const = 0;
  
 protected:
  //: Constructor
  dbsta_data_functor(){}
};


#endif // dbsta_functor_h_
