// This is brl/vidpro/process/vidpro_load_image_process.cxx

//:
// \file

#include <vidpro/process/vidpro_load_image_process.h>
#include <vcl_iostream.h>

#include <bpro/bpro_parameters.h>
#include <vidpro/storage/vidpro_image_storage.h>
#include <vidpro/storage/vidpro_image_storage_sptr.h>

#include <vil/vil_load.h>
#include <vil/vil_image_resource_sptr.h>


//: Constructor
vidpro_load_image_process::vidpro_load_image_process() : bpro_process()
{
  if( !parameters()->add( "Image file <filename...>" , "-image_filename" , bpro_filepath("","*") ))
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
vidpro_load_image_process::~vidpro_load_image_process()
{
}


//: Clone the process
bpro_process*
vidpro_load_image_process::clone() const
{
  return new vidpro_load_image_process(*this);
}


//: Return the name of the process
vcl_string vidpro_load_image_process::name()
{
  return "Load Image";
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string > vidpro_load_image_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;

  // no input type required
  to_return.clear();

  return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string > vidpro_load_image_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;

  // output type
  to_return.push_back( "image" );

  return to_return;
}


//: Run the process on the current frame
bool
vidpro_load_image_process::execute()
{
  bpro_filepath image_path;
  parameters()->get_value( "-image_filename" , image_path );
  vcl_string image_filename = image_path.path;

  //no input storage class for this process
  clear_output();

  vil_image_resource_sptr loaded_image = vil_load_image_resource( image_filename.c_str() );
  if( !loaded_image ) {
    vcl_cerr << "Failed to load image file" << image_filename << vcl_endl;
    return false;
  }

  // create the storage data structure
  vidpro_image_storage_sptr image_storage = vidpro_image_storage_new();
  image_storage->set_image( loaded_image );

  output_data_[0].push_back(image_storage);

  return true;
}


//: Finish
bool vidpro_load_image_process::finish() 
{
  return true;
}


//: Returns the number of input frames to this process
int
vidpro_load_image_process::input_frames()
{
  return 0;
}


//: Returns the number of output frames from this process
int
vidpro_load_image_process::output_frames()
{
  return 1;
}


