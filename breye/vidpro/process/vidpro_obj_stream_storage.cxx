// This is brl/vidpro/storage/vidpro_obj_stream_storage.cxx

//:
// \file

#include "vidpro_obj_stream_storage.h"


//: Constructor
vidpro_obj_stream_storage::vidpro_obj_stream_storage()
{
}


//: Destructor
vidpro_obj_stream_storage::~vidpro_obj_stream_storage()
{
}


//: Return IO version number;
short 
vidpro_obj_stream_storage::version() const
{
  return 1;
}


//: Create a copy of the object on the heap.
// The caller is responsible for deletion
bpro_storage* 
vidpro_obj_stream_storage::clone() const
{
  return new vidpro_obj_stream_storage(*this);
}


//: Binary save self to stream.
void 
vidpro_obj_stream_storage::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, this->path_);
}


//: Binary load self from stream.
void 
vidpro_obj_stream_storage::b_read(vsl_b_istream &is)
{
  if (!is) return;

  short ver;
  vsl_b_read(is, ver);
  switch(ver)
  {
  case 1:
    {
      vsl_b_read(is, this->path_);
      vidpro_object_stream_sptr obj_is = new vidpro_object_stream(path_ + "/*");
      if (!obj_is || !obj_is->is_open()) {
        vcl_cerr <<"\nI/O ERROR: vidpro_obj_stream_storage::b_read(vsl_b_istream &is)\n"
          <<"        Failed to open the input stream.\nList \""<<path_<<"\" not found\n";
         return;
      }
      set_stream(obj_is);
      break;
    }

  default:
    vcl_cerr << "I/O ERROR: vidpro_obj_stream_storage::b_read(vsl_b_istream&)\n"
      << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}



//: Return a smart pointer to the image resource
 vidpro_object_stream_sptr
vidpro_obj_stream_storage::get_stream()
{
  return stream_;
}


//: Store the image in the storage class
void
vidpro_obj_stream_storage::set_stream(const  vidpro_object_stream_sptr  &stream )
{
  stream_ = stream;
}

//: Get the istream directory and filename path
vcl_string 
vidpro_obj_stream_storage::get_path() const
{
  return path_;
}

//: Set the istream path
void
vidpro_obj_stream_storage::set_path(const vcl_string& path)
{
  path_ = path;
}

