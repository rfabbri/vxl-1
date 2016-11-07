// This is brl/vidpro1/process/dbbgm_save_bg_model_process.cxx

//:
// \file

#include "dbbgm_save_bg_model_process.h"
#include <bpro1/bpro1_parameters.h>
#include <vidpro1/vidpro1_repository.h>
#include <bvis1/bvis1_manager.h>

//: Constructor
dbbgm_save_bg_model_process::dbbgm_save_bg_model_process() 
 : bpro1_process()
{
  if( !parameters()->add( "Background Model file <filename...>" , "-bgmodel_filename", bpro1_filepath("","*") ) )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbbgm_save_bg_model_process::~dbbgm_save_bg_model_process()
{
}


//: Clone the process
bpro1_process*
dbbgm_save_bg_model_process::clone() const
{
  return new dbbgm_save_bg_model_process(*this);
}


//: Return the name of the process
vcl_string dbbgm_save_bg_model_process::name()
{
  return "Save Background Model";
}


//: Call the parent function and reset num_frames_
void
dbbgm_save_bg_model_process::clear_output()
{
  bpro1_process::clear_output();
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string >
dbbgm_save_bg_model_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  // image type required
    to_return.push_back( "dbbgm_distribution_image" );

  return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string >
dbbgm_save_bg_model_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.clear();

  return to_return;
}


//: Returns the number of input frames to this process
int
dbbgm_save_bg_model_process::input_frames()
{
  return 1;
}


//: Returns the number of output frames from this process
int
dbbgm_save_bg_model_process::output_frames()
{
  return 0;
}


//: Run the process on the current frame
bool
dbbgm_save_bg_model_process::execute()
{
 if ( input_data_.size() != 1 ){
    vcl_cout << "In dbbgm_save_bgmodel_process::execute() - "
             << "no distribution image storage to save \n";
    return false;
  }
  dbbgm_distribution_image_storage_sptr image_storage;
  image_storage.vertical_cast(input_data_[0][0]);
  bpro1_filepath file;
  parameters()->get_value( "-bgmodel_filename" , file );
  vcl_string path = file.path;
  vsl_b_ofstream os(path);
  image_storage->b_write(os);
  return true;
}


//: Finish
bool
dbbgm_save_bg_model_process::finish() 
{
    
 
  return true;
}


