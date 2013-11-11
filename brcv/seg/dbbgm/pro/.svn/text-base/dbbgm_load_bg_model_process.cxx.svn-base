// This is brl/brcv/trk/dbinfo/pro/dbbgm_load_bg_model_process.cxx

//:
// \file

#include "dbbgm_load_bg_model_process.h"
#include <vcl_iostream.h>
#include <bpro1/bpro1_storage.h>
#include <bpro1/bpro1_parameters.h>
#include <bvis1/bvis1_manager.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/vidpro1_repository.h>

//: Constructor
dbbgm_load_bg_model_process::dbbgm_load_bg_model_process() : bpro1_process(),  model_storage_(0)
{
  if( !parameters()->add( "Backgrounf Model file <filename...>" , "-bgmodel_filename", bpro1_filepath("","*") ))
    {
      vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
    }
}


//: Destructor
dbbgm_load_bg_model_process::~dbbgm_load_bg_model_process()
{
}


//: Clone the process
bpro1_process*
dbbgm_load_bg_model_process::clone() const
{
  return new dbbgm_load_bg_model_process(*this);
}


//: Return the name of the process
vcl_string dbbgm_load_bg_model_process::name()
{
  return "Load Background Model";
}




//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string >
dbbgm_load_bg_model_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
      to_return.clear();


  return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string >
dbbgm_load_bg_model_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;
      to_return.push_back( "dbbgm_distribution_image" );


  return to_return;
}


//: Returns the number of input frames to this process
int
dbbgm_load_bg_model_process::input_frames()
{
  return 0;
}


//: Returns the number of output frames from this process
int
dbbgm_load_bg_model_process::output_frames()
{
return 1;
}


//: Run the process on the current frame
bool
dbbgm_load_bg_model_process::execute()
{
  // read the tracks from the storage file  
  bpro1_filepath track_path;
  parameters()->get_value( "-bgmodel_filename" , track_path );
  vcl_string path = track_path.path;
  vsl_b_ifstream is(path);
  model_storage_ =  dbbgm_distribution_image_storage_new();
  model_storage_->b_read(is);
  model_storage_->set_frame(-2);//for global storage
  this->finish();//Not called by the execute menu macro.
  return true;
}

//: Finish
bool
dbbgm_load_bg_model_process::finish() 
{
if(!model_storage_)
return false;
vidpro1_repository_sptr res = bvis1_manager::instance()->repository();
if(!res)
return false;
res->initialize_global();
// frame index of -2 should indicate global storage
if(!res->store_data_at(model_storage_, -2))
    {
    vcl_cout << "In dbbgm_load_bg_model_process "
        << "store to repository failed\n";
    return false;
    }
return true;
}



