// This is brl/vidpro/storage/vidpro_istream_storage.cxx

//:
// \file

#include "vidpro_istream_storage.h"
#include <vil/io/vil_io_image_view.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vidl/vidl_image_list_istream.h>

#ifdef HAS_DSHOW
#include <vidl/vidl_dshow_file_istream.h>
#include <vidl/vidl_dshow_live_istream.h>
#endif

#ifdef HAS_FFMPEG
#include <vidl/vidl_ffmpeg_istream.h>
#include <vidl/vidl_ffmpeg_ostream.h>
#include <vidl/vidl_ffmpeg_ostream_params.h>
#endif

//: Constructor
vidpro_istream_storage::vidpro_istream_storage()
{
}


//: Destructor
vidpro_istream_storage::~vidpro_istream_storage()
{
}


//: Return IO version number;
short 
vidpro_istream_storage::version() const
{
  return 1;
}


//: Create a copy of the object on the heap.
// The caller is responsible for deletion
bpro_storage* 
vidpro_istream_storage::clone() const
{
  return new vidpro_istream_storage(*this);
}


//: Binary save self to stream.
void 
vidpro_istream_storage::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, this->path_);
  vsl_b_write(os, this->type_);
}


//: Binary load self from stream.
void 
vidpro_istream_storage::b_read(vsl_b_istream &is)
{
  if (!is) return;

  short ver;
  vsl_b_read(is, ver);
  switch(ver)
  {
  case 1:
    {
      vsl_b_read(is, this->path_);
      vsl_b_read(is, this->type_);

      if (!type_.compare("image_list"))
      {
        vidl_image_list_istream* vis = new vidl_image_list_istream(path_);
        if (!vis || !vis->is_open()) {
          vcl_cerr <<"\nI/O ERROR: vidpro_istream_storage::b_read(vsl_b_istream &is)\n"
            <<"        Failed to open the input stream.\nList \""<<path_<<"\" not found\n";
          delete vis;
          return;
        }
        set_istream(vis);
      }
#ifdef HAS_DSHOW
      else if (!type_.compare("dshow"))
      {
        vidl_dshow_file_istream* vis = new vidl_dshow_file_istream(path_);
        if (!vis || !vis->is_open()) {
          vcl_cerr <<"\nI/O ERROR: vidpro_istream_storage::b_read(vsl_b_istream &is)\n"
            <<"        Failed to open the input stream.\nList \""<<path_<<"\" not found\n";
          delete vis;
          return;
        }
        set_istream(vis);
      }
#endif

#ifdef HAS_FFMPEG
      else if (!type_.compare("ffmpeg"))
      {
        vidl_ffmpeg_istream* vis = new vidl_ffmpeg_istream(path_);
        if (!vis || !vis->is_open()) {
          vcl_cerr <<"\nI/O ERROR: vidpro_istream_storage::b_read(vsl_b_istream &is)\n"
            <<"        Failed to open the input stream.\nList \""<<path_<<"\" not found\n";
          delete vis;
          return;
        }
        set_istream(vis);
      }
#endif
      else
      {
        vcl_cerr << "I/O ERROR: vidpro_istream_storage::b_read(vsl_b_istream&)\n"
          << "           Unknown version istream type "<< ver << '\n';
      }
      break;
    }

  default:
    vcl_cerr << "I/O ERROR: vidpro_istream_storage::b_read(vsl_b_istream&)\n"
      << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}



//: Return a smart pointer to the image resource
vidl_istream_sptr
vidpro_istream_storage::get_istream()
{
  return istream_;
}


//: Store the image in the storage class
void
vidpro_istream_storage::set_istream(const vidl_istream_sptr  &istream )
{
  istream_ = istream;
}

//: Get the istream directory and filename path
vcl_string 
vidpro_istream_storage::get_path() const
{
  return path_;
}

//: Set the istream path
void
vidpro_istream_storage::set_path(const vcl_string& path)
{
  path_ = path;
}

//: Set the istream type
void 
vidpro_istream_storage::set_type(const vcl_string& type)
{
  type_ = type;
}
