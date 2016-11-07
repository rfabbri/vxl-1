// This is brl/bbas/vidl/vidpro_object_stream.h
#ifndef vidpro_object_stream_h_
#define vidpro_object_stream_h_

//:
// \file
// \brief A base class for input video streams
//
// \author Matt Leotta
// \date 19 Dec 2005


#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_map.h>
#include <bpro/bpro_storage_sptr.h>
#include <vbl/vbl_ref_count.h>

//: A base class for input video streams
class vidpro_object_stream: public vbl_ref_count
{
 public:
  //: Constructor
  vidpro_object_stream();
    
  //: Constructor - opens a stream
  vidpro_object_stream(const vcl_string& directory,
                           const vcl_string& name_format = "%05d",
                           const unsigned int init_index = 0);

  //: Destructor
  ~vidpro_object_stream() { close(); }

  //: Open the stream
  virtual bool open(const vcl_string& directory,
                    const vcl_string& name_format = "%05d",
                    const unsigned int init_index = 0);
  
  //: Return true if the stream is open for reading
  bool is_open() const { return !dir_.empty();};

  //: Return true if the stream is in a valid state
  bool is_valid() const;

  //: Return true if the stream support seeking
  // virtual bool is_seekable() const = 0;

  //: Return the current frame number
  unsigned int frame_number();

  //: Close the stream
  void close();

  //: Advance to the next frame (but don't acquire)
  bool advance();

  //: Read the given frame from the stream 
  bpro_storage_sptr  read_frame(int frame);

  //: Write and image to the stream
  // \retval false if the image could not be written
  bool write_frame(const bpro_storage_sptr& sto, int frame);

  //: Reads the current frame in the stream
  bpro_storage_sptr current_frame();

  //: Seek to the given frame number
  // \returns true if successful
  bool seek_frame(unsigned int frame_number);

    //: Return the current frame number
  virtual unsigned int frame_number() const { return index_; }


 private:
  //: prevent deep copying a stream
  vidpro_object_stream(const vidpro_object_stream& other){}

  vcl_string next_file_name()const;

  vcl_map<int,vcl_string> paths_;

  //: The current index
  unsigned int index_;

  //: The directory to save images in
  vcl_string dir_;

  //: The printf-style format string for filenames
  vcl_string name_format_;
};


#endif // vidpro_object_stream_h_
