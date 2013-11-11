// This is brcv/seg/dbsta/dbsta_updater.h
#ifndef dbsta_updater_h_
#define dbsta_updater_h_

//:
// \file
// \brief A base class for functors that update a distribution 
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 07/13/05
//
// An updater takes a new sample and uses it to update an
// existing distribution
//
// \verbatim
//  Modifications
// \endverbatim

#include "dbsta_distribution.h"
#include <vbl/vbl_ref_count.h>


//: A base class for functors that update distributions
template <class T>
class dbsta_updater : public vbl_ref_count
{
 public:
  
  //: Destructor
  virtual ~dbsta_updater(){}
  
  //: The main function
  // \note this function is const because the updater should not maintain state
  //       The same updater instance may act on multiple distributions
  virtual void operator() ( dbsta_distribution<T>& d ) const = 0;
                            
 protected:
  //: Constructor
  dbsta_updater(){}

};


//: A base class for functors that update distributions given data
template <class T>
class dbsta_data_updater : public vbl_ref_count
{
 public:
  
  //: Destructor
  virtual ~dbsta_data_updater(){}
  
  //: The main function
  // \note this function is const because the updater should not maintain state
  //       The same updater instance may act on multiple distributions
  virtual void operator() ( dbsta_distribution<T>& d, 
                            const vnl_vector<T>& sample ) const = 0;
                            
 protected:
  //: Constructor
  dbsta_data_updater(){}

};


#endif // dbsta_updater_h_
