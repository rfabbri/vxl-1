// This is brcv/seg/dbsta/dbsta_distribution.h
#ifndef dbsta_distribution_h_
#define dbsta_distribution_h_

//:
// \file
// \brief A base class for probability distributions
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 07/14/05
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_cassert.h>
#include <vcl_string.h>
#include <vsl/vsl_binary_loader.h>
#include <vbl/vbl_ref_count.h>

//: forward declare vnl_vector
template <class T> class vnl_vector;


//: A base class for probability distributions
template <class T>
class dbsta_distribution:public vbl_ref_count
{
 public:
  //: Inherit from this class to store additional data with a distribution
  struct user_data
  {
    //: clone the user's data
    virtual user_data* clone() const = 0;
    virtual ~user_data(){}
  };

  //: Constructor
  dbsta_distribution<T>() : data_(0) {}
  
  //: Constructor - with user data
  dbsta_distribution<T>(const user_data& data) : data_(data.clone()) {}
  
  //: Copy Constructor
  dbsta_distribution<T>(const dbsta_distribution<T>& other) 
   : data_((other.data_)?other.data_->clone():0) {}
      
  //: Destructor
  virtual ~dbsta_distribution() { delete data_; } 
  
  //: Clone
  // allocate a new copy of this object.
  // the caller is responsible for deletion 
  virtual dbsta_distribution<T>* clone() const = 0;
  
  //: Return the number of dimensions in the space
  virtual unsigned int dim() const = 0;

  //: Compute the probablity of this point
  virtual T probability(const vnl_vector<T>& pt) const = 0;
  
  //: Return true if user data is stored with this distribution
  bool has_data() const { return data_ != 0; }
  
  //: Return a const reference to the user data
  // \note do not call this if no data is stored!
  const user_data& data() const { assert(data_); return *data_; }
  
  //: Return a reference to the user data
  // \note do not call this if no data is stored!
  user_data& data() { assert(data_); return *data_; }
  
  //: Store a copy of the data object here
  void set_data(const user_data& data) { data_ = data.clone(); }

  //: Binary save self to stream.
  virtual void b_write(vsl_b_ostream &os) const =0;

  //: Binary load self from stream.
  virtual void b_read(vsl_b_istream &is)=0;

  virtual vcl_string is_a() const=0;


  
 private:
  //: Optionally store a pointer to additional data
  // This should not be used by derived classes to hold the distribution.
  // Rather, it is a place for algorithms to store additional state information
  user_data *data_;
  
};

template <class T>
void vsl_add_to_binary_loader(dbsta_distribution<T> const& b){
      vsl_binary_loader<dbsta_distribution<T> >::instance().add(b);
    
}


#endif // dbsta_distribution_h_
