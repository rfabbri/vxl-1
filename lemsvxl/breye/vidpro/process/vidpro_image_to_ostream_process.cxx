// This is brl/vidpro/process/vidpro_image_to_ostream_process.cxx

//:
// \file

#include <vidpro/process/vidpro_image_to_ostream_process.h>
#include <vcl_iostream.h>

#include <bpro/bpro_parameters.h>
#include <vidpro/storage/vidpro_image_storage.h>
#include <vidpro/storage/vidpro_image_storage_sptr.h>

#include <vil/vil_load.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_new.h>
#include <vil/vil_flip.h>

#include <vidl/vidl_istream.h>
#include <vidl/vidl_istream_sptr.h>
#include <vidl/vidl_frame.h>
#include <vidl/vidl_frame_sptr.h>
#include <vidl/vidl_dshow_file_istream.h>
#include <vidl/vidl_convert.h>

//: Constructor
vidpro_image_to_ostream_process::vidpro_image_to_ostream_process() : bpro_process()
{
 
}

//: Constructor
vidpro_image_to_ostream_process::vidpro_image_to_ostream_process(vidl_ostream_sptr ostream) : bpro_process()
{
 ostream_ = ostream;
}

//: Destructor
vidpro_image_to_ostream_process::~vidpro_image_to_ostream_process()
{
}


//: Clone the process
bpro_process*
vidpro_image_to_ostream_process::clone() const
{
  return new vidpro_image_to_ostream_process(*this);
}


//: Return the name of the process
vcl_string vidpro_image_to_ostream_process::name()
{
  return "Image to output stream";
}


bool 
vidpro_image_to_ostream_process::set_ostream(vidl_ostream_sptr ostream)  
{
    ostream_ = ostream;
    return true;
}

//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string > vidpro_image_to_ostream_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;

  to_return.push_back("image");

  return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string > vidpro_image_to_ostream_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;

  // output type
  to_return.clear();

  return to_return;
}


//: Returns the number of input frames to this process
int
vidpro_image_to_ostream_process::input_frames()
{
  return 1;
}


//: Returns the number of output frames from this process
int
vidpro_image_to_ostream_process::output_frames()
{
  return 0;
}


//: Run the process on the current frame
bool
vidpro_image_to_ostream_process::execute()
{

  if ( input_data_.size() != 1 ){
    vcl_cout << "In vidpro_image_to_ostream_process::execute() - "
             << "not exactly one input stream \n";
    return false;
  }
  
  clear_output();

  // get image view from the storage class
  vidpro_image_storage_sptr image_storage;
  image_storage.vertical_cast(input_data_[0][0]);
 
  if(!image_storage)
      return false;
  
 // vil_image_view_base view = ); 
  vidl_memory_chunk_frame *output_frame = new vidl_memory_chunk_frame(*(image_storage->get_image()->get_view()));

  //need to get active ostream from global data
 // ostream_ = bvis_manager::instance()->repository()->get_data_by_name("ostream0")

  if (!ostream_->is_open()) {
          vcl_cout << "Failed to open the output stream\n";
          return false;
        }
   
  ostream_->write_frame(output_frame);
 
  return true;

}


//: Finish
bool vidpro_image_to_ostream_process::finish() 
{
  return true;
}




