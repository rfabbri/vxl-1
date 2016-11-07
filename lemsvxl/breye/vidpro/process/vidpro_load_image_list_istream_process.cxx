// This is brl/vidpro/process/vidpro_load_image_list_istream_process.cxx

//:
// \file

#include <vidpro/process/vidpro_load_image_list_istream_process.h>
#include <vcl_iostream.h>

#include <bpro/bpro_parameters.h>
#include <vidpro/storage/vidpro_istream_storage.h>
#include <vidpro/storage/vidpro_istream_storage_sptr.h>
#include <vidl/vidl_image_list_istream.h>


//: Constructor
vidpro_load_image_list_istream_process::vidpro_load_image_list_istream_process() : bpro_process()
{
 if( !parameters()->add( "Video stream file <filename...>" , "-istream_filename" , bpro_filepath("","*") ))
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
vidpro_load_image_list_istream_process::~vidpro_load_image_list_istream_process()
{
}


//: Clone the process
bpro_process*
vidpro_load_image_list_istream_process::clone() const
{
  return new vidpro_load_image_list_istream_process(*this);
}


//: Return the name of the process
vcl_string vidpro_load_image_list_istream_process::name()
{
  return "Load Input Stream";
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string > vidpro_load_image_list_istream_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;

  // no input type required
  to_return.clear();

  return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string > vidpro_load_image_list_istream_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;

  // output type
  to_return.push_back( "istream" );

  return to_return;
}


//: Run the process on the current frame
bool
vidpro_load_image_list_istream_process::execute()
{
  bpro_filepath istream_path;
  
  parameters()->get_value( "-istream_filename" , istream_path );
  
  //needs to be adapted to different types of istream
  vidl_image_list_istream *istream= new vidl_image_list_istream(istream_path.path);
   
  //no input storage class for this process
  clear_output();

  // create the storage data structure
  vidpro_istream_storage_sptr istream_storage = vidpro_istream_storage_new();
  istream_storage->set_istream(istream);  //need inheritance between sptr to pass sptr as parameter

  output_data_[0].push_back(istream_storage);

  return true;
}


//: Finish
bool vidpro_load_image_list_istream_process::finish() 
{
  return true;
}


//: Returns the number of input frames to this process
int
vidpro_load_image_list_istream_process::input_frames()
{
  return 0;
}


//: Returns the number of output frames from this process
int
vidpro_load_image_list_istream_process::output_frames()
{
  return 1;
}


