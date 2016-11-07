// This is pro/vidpro1_homog_2d_storage.h
#ifndef vidpro1_homog_2d_storage_h_
#define vidpro1_homog_2d_storage_h_

//:
// \file
// \brief vidpro1 storage class for 2D homography
// \author Nhon Trinh (ntrinh@lems.brown.edu
// \date 5/7/2005
//
//
// \verbatim
//  Modifications
// \endverbatim

#include "vidpro1_homog_2d_storage_sptr.h"
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <bpro1/bpro1_storage.h>

//: Storage class for vimt_tranform_2d
class vidpro1_homog_2d_storage : public bpro1_storage {
public:

  //: Constructor
  vidpro1_homog_2d_storage(){} 

  //: Destructor
  virtual ~vidpro1_homog_2d_storage(){}

  //: Returns the type string "homog_2d"
  virtual vcl_string type() const { return "homog_2d"; }

  //: Return IO version number;
  short version() const;
  
  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);

  //: Create a copy of the object on the heap.
  // The caller is responsible for deletion
  virtual bpro1_storage* clone() const;
  
  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const { return "vidpro1_homog_2d_storage"; }

  //: Return homography matrix 
  vgl_h_matrix_2d< double > H(){ return H_; }
  //: Set homography matrix
  void set_H(const vgl_h_matrix_2d< double >& H ){ H_ = H;}
  
protected:
  // the 2D-homography matrix
  vgl_h_matrix_2d< double > H_;

};


//: Create a smart-pointer to a vidpro1_homog_2d_storage.
struct vidpro1_homog_2d_storage_new : public vidpro1_homog_2d_storage_sptr
{
  typedef vidpro1_homog_2d_storage_sptr base;

  //: Constructor - creates a vidpro1_homog_2d_storage_sptr.
  vidpro1_homog_2d_storage_new() 
    : base(new vidpro1_homog_2d_storage()) { }
};


#endif //vidpro1_homog_2d_storage_h_
