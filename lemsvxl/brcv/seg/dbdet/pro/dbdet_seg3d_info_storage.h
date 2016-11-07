// This is brl/vidpro1/storage/dbdet_seg3d_info_storage.h
#ifndef dbdet_seg3d_info_storage_h_
#define dbdet_seg3d_info_storage_h_

//:
// \file
// \brief The storage class for a dbdet_seg3d_info
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date June 3, 2006
//
//
// \verbatim
//  Modifications
// \endverbatim



#include <bpro1/bpro1_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

#include "dbdet_seg3d_info_storage_sptr.h"
#include <dbdet/dbdet_seg3d_info.h>
#include <dbdet/dbdet_seg3d_info_sptr.h>


class dbdet_seg3d_info_storage : public bpro1_storage {

public:

  dbdet_seg3d_info_storage();
  virtual ~dbdet_seg3d_info_storage();
  virtual vcl_string type() const { return "seg3d_info"; }
  
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
  virtual vcl_string is_a() const { return "dbdet_seg3d_info_storage"; }

  //: Return a smart pointer to seg3d_info
  dbdet_seg3d_info_sptr seg3d_info() const { return this->seg3d_info_; }

  //: Set the seg3d_info smart pointer
  void set_seg3d_info(const dbdet_seg3d_info_sptr& s)
  {this->seg3d_info_ = s; }


  //: Return the storage vector
  vcl_vector <vidpro1_vsol2D_storage_sptr > vsol_storage_list() const
  { return this->vsol_storage_list_; }


  //: Return vsol2D storage at a frame  
  vidpro1_vsol2D_storage_sptr vsol_storage_at_frame(int i)
  { return this->vsol_storage_list_[i]; }

  //: set the storage list
  void set_vsol_storage_list(
    const vcl_vector<vidpro1_vsol2D_storage_sptr >& storage_list)
  { this->vsol_storage_list_ = storage_list; }
  
protected:
  dbdet_seg3d_info_sptr seg3d_info_;

  // list of vsol2d storage for the contour, one storage per frame
  vcl_vector <vidpro1_vsol2D_storage_sptr > vsol_storage_list_;
};

//: Create a smart-pointer to a dbdet_seg3d_info_storage.
struct dbdet_seg3d_info_storage_new : public dbdet_seg3d_info_storage_sptr
{
  typedef dbdet_seg3d_info_storage_sptr base;

  //: Constructor - creates a default dbdet_seg3d_info_storage_sptr.
  dbdet_seg3d_info_storage_new() : base(new dbdet_seg3d_info_storage()) { }
};

#endif // dbdet_seg3d_info_storage_h_
