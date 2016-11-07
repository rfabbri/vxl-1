// This is brcv/seg/dbbgm/dbbgm_distribution_image.h
#ifndef dbbgm_distribution_image_h_
#define dbbgm_distribution_image_h_

//:
// \file
// \brief An image of distributions
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 07/12/05
//
// \verbatim
//  Modifications
// \endverbatim


#include <dbsta/dbsta_distribution.h>
#include <vbl/vbl_array_2d.h>
#include <vbl/vbl_ref_count.h>
#include <vsl/vsl_binary_loader.h>
#include <vbl/vbl_smart_ptr.h>

//: A base class to abstract the distribution data type
class dbbgm_dist_image_base : public vbl_ref_count
{
 public:
  virtual ~dbbgm_dist_image_base(){}

  //: Binary save self to stream.
  virtual void b_write(vsl_b_ostream &os) const=0;

  //: Binary load self from stream.
  virtual void b_read(vsl_b_istream &is)=0;

  virtual vcl_string is_a() const=0;
  
  virtual dbbgm_dist_image_base* clone() const = 0;

};


//: An image of distributions 
template <class T>
class dbbgm_distribution_image : public dbbgm_dist_image_base
{
 public:
  //: Constructor
     dbbgm_distribution_image<T>(){};
  dbbgm_distribution_image<T>(unsigned int ni, unsigned int nj, 
                              const dbsta_distribution<T >& model);

  //: Destructor
  ~dbbgm_distribution_image<T>();

  //: Return the width of the image
  unsigned int ni() const { return data_.rows(); }

  //: Return the height
  unsigned int nj() const { return data_.cols(); }

  //: Read only access to the distributions
  const dbsta_distribution<T >& 
  operator() (unsigned int i, unsigned int j) const
  { return *data_(i,j); }

  //: Access to the distributions
  dbsta_distribution<T >& 
  operator() (unsigned int i, unsigned int j)
  { return *data_(i,j); }

  //: Set the distribution at (i,j) to a copy of d
  void set(unsigned int i, unsigned int j, const dbsta_distribution<T >& d)
  { data_(i,j) = d.clone(); }

  virtual vcl_string is_a() const ;

  virtual dbbgm_dist_image_base* clone() const;

  //: Return IO version number;
  short version() const;

  //: Binary save self to stream.
  virtual void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  virtual void b_read(vsl_b_istream &is);

  class iterator
  {
    public:
      iterator(vbl_smart_ptr<dbsta_distribution<T > >* ptr) : ptr_(ptr) {}
      iterator(const iterator& other) : ptr_(other.ptr_) {}
      void operator++() { ++ptr_; }
      dbsta_distribution<T >& operator*(){ return **ptr_; }
      bool operator==(const iterator& other) { return ptr_ == other.ptr_; }
      bool operator!=(const iterator& other) { return ptr_ != other.ptr_; }

    private:
      vbl_smart_ptr<dbsta_distribution<T > >* ptr_;
  };

  //: Return an iterator to the first element
  iterator begin() { return iterator(data_[0]); }
  //: Return an iterator to one past the last element
  iterator end() { return iterator(data_[0]+data_.size()); }

 private:
  //: the data
  vbl_array_2d<vbl_smart_ptr<dbsta_distribution<T > > > data_;

};

void vsl_add_to_binary_loader(dbbgm_dist_image_base const& b);

//: Binary save dbbgm_distribution_image to stream.
template <class T>
void
vsl_b_write(vsl_b_ostream &os, const dbbgm_distribution_image<T>* p);

//: Binary load dbsta_gaussian_indep from stream.
template <class T>
void
vsl_b_read(vsl_b_istream &is, dbbgm_distribution_image<T>* &p);


#endif // dbbgm_distribution_image_h_
