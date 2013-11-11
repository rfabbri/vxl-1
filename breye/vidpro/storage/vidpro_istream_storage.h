// This is brl/vidpro/storage/vidpro_istream_storage.h
#ifndef vidpro_istream_storage_h_
#define vidpro_istream_storage_h_

//:
// \file
// \brief The vidpro storage class for an input stream
// \author Isabel Restrepo
// \date 7/15/03
//
//
// \verbatim
//  Modifications
//     2007-APR-16  Eduardo Almeida   Added path_, get_path() and set_path()
//     2007-APR-27  Eduardo Almeida   Added type_ and set_type()
// \endverbatim



#include <bpro/bpro_storage.h>
#include <vidpro/storage/vidpro_istream_storage_sptr.h>
#include <vidl/vidl_istream_sptr.h>

class vidpro_istream_storage : public bpro_storage {

public:

  vidpro_istream_storage();
  virtual ~vidpro_istream_storage();
  virtual vcl_string type() const { return "istream"; }
  
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
  virtual vcl_string is_a() const { return "vidpro_istream_storage"; }

  void set_istream( vidl_istream_sptr const &istream );
 
  vidl_istream_sptr get_istream();
  
  //: Get the istream directory and filename path
  vcl_string get_path() const;

  //: Set the istream path
  void set_path(const vcl_string& path);

  //: Set the istream type
  void set_type(const vcl_string& type);

protected:

private:
  
  vidl_istream_sptr istream_;
  vcl_string type_;
  vcl_string path_;

};

//: Create a smart-pointer to a vidpro_istream_storage.
struct vidpro_istream_storage_new : public vidpro_istream_storage_sptr
{
  typedef vidpro_istream_storage_sptr base;

  //: Constructor - creates a default vidpro_istream_storage_sptr.
  vidpro_istream_storage_new() : base(new vidpro_istream_storage()) { }
};

#endif // vidpro_istream_storage_h_
