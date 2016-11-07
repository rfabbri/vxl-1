// This is breye1/dvidpro1/storage/vidpro1_shockshape_storage.h
#ifndef vidpro1_shockshape_storage_h_
#define vidpro1_shockshape_storage_h_

//:
// \file
// \brief The vidpro1 storage class for shockshape.
// \author Nhon Trinh, (ntrinh@lems.brown.edu)
// \date 10/30/04
//
//
// \verbatim
//  Modifications
// \endverbatim

#include "vidpro1_shockshape_storage_sptr.h"
#include <bpro1/bpro1_storage.h>
#include <shockshape/shockshape_sptr.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>
// #include <vsol/vsol_polyline_2d_sptr.h>


//: Storage class for shockshape
class vidpro1_shockshape_storage : public bpro1_storage {
public:

  //: Constructor
  vidpro1_shockshape_storage();
  //: Constructor
  vidpro1_shockshape_storage( const shockshape_sptr& shockshape);

  //: Destructor
  virtual ~vidpro1_shockshape_storage(){}

  //: Returns the type string "shockshape"
  virtual vcl_string type() const { return "shockshape"; }

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
  virtual vcl_string is_a() const { return "vidpro1_shockshape_storage"; }

  //: Return smart pointer to the shockshape object
  shockshape_sptr get_shockshape(){return shockshape_;}
  //: set the shockshape object
  void set_shockshape(const shockshape_sptr& shockshape_sp){shockshape_ = shockshape_sp;};
  
  // vcl_vector< vsol_spatial_object_2d_sptr > shock_line;
  
protected:
  shockshape_sptr shockshape_;
  
};


//: Create a smart-pointer to a vidpro1_shockshape_storage.
struct vidpro1_shockshape_storage_new : public vidpro1_shockshape_storage_sptr
{
  typedef vidpro1_shockshape_storage_sptr base;

  //: Constructor - creates a vidpro1_shockshape_storage_sptr.
  vidpro1_shockshape_storage_new() 
    : base(new vidpro1_shockshape_storage()) { }
};


#endif //vidpro1_shockshape_storage_h_
