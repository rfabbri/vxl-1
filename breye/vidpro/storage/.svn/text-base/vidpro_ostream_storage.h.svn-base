// This is brl/vidpro/storage/vidpro_ostream_storage.h
#ifndef vidpro_ostream_storage_h_
#define vidpro_ostream_storage_h_

//:
// \file
// \brief The vidpro storage class for an output stream
// \author Isabel Restrepo
// \date 3/28/07
//
//
// \verbatim
//  Modifications
// \endverbatim



#include <bpro/bpro_storage.h>
#include <vidpro/storage/vidpro_ostream_storage_sptr.h>
#include <vidl/vidl_ostream_sptr.h>


class vidpro_ostream_storage : public bpro_storage {

public:

  vidpro_ostream_storage();
  virtual ~vidpro_ostream_storage();
  virtual vcl_string type() const { return "ostream"; }
  
  //: Return IO version number;
  short version() const;
  
  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_ostream &is);

  //: Create a copy of the object on the heap.
  // The caller is responsible for deletion
  virtual bpro_storage* clone() const;
  
  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const { return "vidpro_ostream_storage"; }

  void set_ostream( const vidl_ostream_sptr &ostream );
 
  vidl_ostream_sptr get_ostream();
  
protected:

private:
  
  vidl_ostream_sptr ostream_;
  
};

//: Create a smart-pointer to a vidpro_ostream_storage.
struct vidpro_ostream_storage_new : public vidpro_ostream_storage_sptr
{
  typedef vidpro_ostream_storage_sptr base;

  //: Constructor - creates a default vidpro_ostream_storage_sptr.
  vidpro_ostream_storage_new() : base(new vidpro_ostream_storage()) { }
};

#endif // vidpro_ostream_storage_h_
