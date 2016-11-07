// This is brcv/seg/dbdet/pro/dbdet_keypoint_storage.h
#ifndef dbdet_keypoint_storage_h_
#define dbdet_keypoint_storage_h_

//:
// \file
// \brief The vidpro1 storage class for keypoints.
// \author Matthew Leotta, (mleotta@lems.brown.edu)
// \date Sat Nov 16 2003
//
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_storage.h>
#include "dbdet_keypoint_storage_sptr.h"
#include <vcl_vector.h>
#include <dbdet/dbdet_keypoint_sptr.h>

//: Storage class for bapl keypoints
//  Currently only handles Lowe keypoints
class dbdet_keypoint_storage : public bpro1_storage {
public:

  //: Constructor
  dbdet_keypoint_storage(){}
  //: Constructor
  dbdet_keypoint_storage( const vcl_vector< dbdet_keypoint_sptr >& keypoints, int ni, int nj);
  //: Destructor
  virtual ~dbdet_keypoint_storage(){}

  //: Returns the type string "keypoint"
  virtual vcl_string type() const { return "keypoints"; }

    //: Register vsol_spatial_object_2d types for I/O
  virtual void register_binary_io() const;

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
  virtual vcl_string is_a() const { return "dbdet_keypoint_storage"; }

  //: Set the keypoint storage vector
  void set_keypoints( const vcl_vector< dbdet_keypoint_sptr >& keypoints);

  //: Retrieve the keypoint storage vector
  const vcl_vector< dbdet_keypoint_sptr >& keypoints() const { return keypoints_; }

  //: Return the width
  int ni() const { return ni_; }

  //: Return the height
  int nj() const { return nj_; }
   
private:
  //: The vector of lowe keypoint smart pointers
  vcl_vector< dbdet_keypoint_sptr > keypoints_;

  //: The width of the image that contains the keypoints
  int ni_;
  //: The height of the image that contains the keypoints
  int nj_;

  //: Is the binary I/O registered
  static bool registered_; 
};


//: Create a smart-pointer to a dbdet_keypoint_storage.
struct dbdet_keypoint_storage_new : public dbdet_keypoint_storage_sptr
{
  typedef dbdet_keypoint_storage_sptr base;

  //: Constructor - creates a default dbdet_keypoint_storage_sptr.
  dbdet_keypoint_storage_new() : base(new dbdet_keypoint_storage()) { }

  //: Constructor - creates a dbdet_keypoint_storage_sptr with keypoints.
  dbdet_keypoint_storage_new(const vcl_vector< dbdet_keypoint_sptr >& keypoints, int ni, int nj)
   : base(new dbdet_keypoint_storage( keypoints, ni, nj )) { }
};


#endif //dbdet_keypoint_storage_h_
