// This is brl/vidpro/storage/vidpro_obj_stream_storage.h
#ifndef vidpro_obj_stream_storage_h_
#define vidpro_obj_stream_storage_h_

//:
// \file
// \brief The vidpro storage class for an input stream
// \author Isabel Restrepo
// \date 7/15/03
//
//
// \verbatim
//  Modifications
//     
// \endverbatim



#include <bpro/bpro_storage.h>
#include <vidpro/storage/vidpro_obj_stream_storage_sptr.h>
#include <vidpro/storage/vidpro_object_stream.h>
#include <vidpro/storage/vidpro_object_stream_sptr.h>


class vidpro_obj_stream_storage : public bpro_storage {

public:

  vidpro_obj_stream_storage();
  virtual ~vidpro_obj_stream_storage();
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
  virtual vcl_string is_a() const { return "vidpro_obj_stream_storage"; }

  void set_stream( vidpro_object_stream_sptr const &stream );

  vidpro_object_stream_sptr get_stream();

  //: Get the istream directory and filename path
  vcl_string get_path() const;

  //: Set the istream path
  void set_path(const vcl_string& path);

protected:

private:

  vidpro_object_stream_sptr stream_;

  vcl_string path_;

};

//: Create a smart-pointer to a vidpro_obj_stream_storage.
struct vidpro_obj_stream_storage_new : public vidpro_obj_stream_storage_sptr
{
  typedef vidpro_obj_stream_storage_sptr base;

  //: Constructor - creates a default vidpro_obj_stream_storage_sptr.
  vidpro_obj_stream_storage_new() : base(new vidpro_obj_stream_storage()) { }
};

#endif // vidpro_obj_stream_storage_h_
