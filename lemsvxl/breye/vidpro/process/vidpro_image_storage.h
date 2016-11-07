// This is brl/vidpro/storage/vidpro_image_storage.h
#ifndef vidpro_image_storage_h_
#define vidpro_image_storage_h_

//:
// \file
// \brief The vidpro storage class for an image
// \author Mark Johnson (mrj@lems.brown.edu)
// \date 7/15/03
//
//
// \verbatim
//  Modifications
// \endverbatim



#include <bpro/bpro_storage.h>
#include <vidpro/storage/vidpro_image_storage_sptr.h>
#include <vil/vil_image_resource_sptr.h>


class vidpro_image_storage : public bpro_storage {

public:

  vidpro_image_storage();
  virtual ~vidpro_image_storage();
  virtual vcl_string type() const { return "image"; }
  
  //: Return IO version number;
  short version() const;
  
  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);

  //: Create a copy of the object on the heap.
  // The caller is responsible for deletion
  virtual bpro_storage* clone() const;
  
  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const { return "vidpro_image_storage"; }

  

  void set_image( const vil_image_resource_sptr& img );
  vil_image_resource_sptr get_image();
  
protected:

private:
  
  vil_image_resource_sptr image_;
  
};

//: Create a smart-pointer to a vidpro_image_storage.
struct vidpro_image_storage_new : public vidpro_image_storage_sptr
{
  typedef vidpro_image_storage_sptr base;

  //: Constructor - creates a default vidpro_image_storage_sptr.
  vidpro_image_storage_new() : base(new vidpro_image_storage()) { }
};

#endif // vidpro_image_storage_h_
