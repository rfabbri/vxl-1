
#include "vidpro_object_stream.h"
#include <vcl_cassert.h>
#include <vcl_utility.h>
#include <vcl_iostream.h>
#include <vcl_sstream.h>

#include <vul/vul_file_iterator.h>
#include <vul/vul_file.h>
#include <vul/vul_sprintf.h>

#include <vsl/vsl_binary_io.h>
#include <vbl/io/vbl_io_smart_ptr.txx>

#include <bpro/bpro_storage.h>


//: Constructor
vidpro_object_stream::
vidpro_object_stream()
: index_(0),
dir_(),
name_format_()
{
}


//: Constructor - opens a stream
vidpro_object_stream::
vidpro_object_stream(const vcl_string& directory,
                      const vcl_string& name_format,
                      const unsigned int init_index)
{
  open(directory, name_format, init_index);
}

//: Open the stream
bool
vidpro_object_stream::
open(const vcl_string& directory,
     const vcl_string& name_format,
     const unsigned int init_index)
{
  if (!vul_file::is_directory(directory)) {
    close();
    vcl_cerr << __FILE__ ": Directory does not exist\n   "<<directory<<vcl_endl;
    return false;
  }

  dir_ = directory;
  name_format_ = name_format;
  index_ = init_index;
  return true;
}

//: Close the stream
void
vidpro_object_stream::close()
{
}

////: Advance to the next frame (but don't acquire an image)
bool 
vidpro_object_stream::advance()
{
  ++index_;
  return true;
}

//: Read the next frame from the stream (advance and acquire)
bpro_storage_sptr 
vidpro_object_stream::read_frame(int frame)
{
  bpro_storage_sptr obj_storage;

  for ( vcl_map<int, vcl_string>::iterator mit =paths_.begin(); mit!= paths_.end(); mit++)
  {
    if (!(mit->first == frame))
      continue;

    vsl_b_ifstream in_stream(mit->second);

    if (!in_stream){
      vcl_cerr<<"Failed to open " << mit->second << " for binary IO input." << vcl_endl;
    }
    vcl_cout << "Opened binary IO file " << mit->second << " successfully." << vcl_endl;

    vsl_b_read(in_stream,obj_storage);
    in_stream.close();
  }
  return obj_storage;
}

//: Write and image to the stream
// \retval false if the image could not be written
bool 
vidpro_object_stream::write_frame(const bpro_storage_sptr& obj_storage, int frame)
{
  vcl_string file_name = next_file_name();
  ++index_;

  vsl_b_ofstream out_stream(file_name.c_str());
  if(!out_stream)
  {
    return false;
  }
  //write the object
  vsl_b_write(out_stream, obj_storage);
  paths_.insert(vcl_pair<int, vcl_string>(frame, file_name));
  return true;
}


//: Return the current frame in the stream
bpro_storage_sptr 
vidpro_object_stream::current_frame()
{
  return read_frame(index_);
}

//: Seek to the given frame number
// \returns true if successful
bool 
vidpro_object_stream::seek_frame(unsigned int frame_number)
{
  if(!read_frame(frame_number))
    return false;

  return true;

}


//: Return the next file name to be written to
vcl_string
vidpro_object_stream::
next_file_name() const
{
  return dir_ + '/' +
    vul_sprintf(name_format_.c_str(),index_);

}
