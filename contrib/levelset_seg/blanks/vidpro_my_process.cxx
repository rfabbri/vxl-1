// This is contrib/levelset_seg/vidpro1_my_process.cxx

//:
// \file

#include "vidpro1_my_process.h"
#include <vidpro1/vidpro1_parameters.h>

// include storage classes needed
// such as ...
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

// other includes needed



//: Constructor
vidpro1_my_process::vidpro1_my_process()
{
  // Set up the parameters for this process
  if( !parameters_sptr_->add_parameter( "Parameter Name 1" , "-p1" , vidpro1_parameters::FLOAT , 10.0f ) ||
      !parameters_sptr_->add_parameter( "Parameter Name 2" , "-p2" , vidpro1_parameters::BOOL , true ) ) {
    vcl_cerr << "ERROR: Adding parameters in vidpro1_my_process::vidpro1_my_process()" << vcl_endl;
  }
}


//: Destructor
vidpro1_my_process::~vidpro1_my_process()
{
}


//: Return the name of this process
vcl_string
vidpro1_my_process::name()
{
  return "My Process";
}


//: Return the number of input frame for this process
int
vidpro1_my_process::input_frames()
{
  // input from this frame and the previous frame
  return 2;
}


//: Return the number of output frames for this process
int
vidpro1_my_process::output_frames()
{
  // output to this frame only
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > 
vidpro1_my_process::get_input_type()
{
  // this process looks for an image and vsol2D storage class
  // at each input frame
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  to_return.push_back( "vsol2D" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > 
vidpro1_my_process::get_output_type()
{  
  // this process produces a vsol2D storage class
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "vsol2D" );
  return to_return;
}


//: Execute the process
bool
vidpro1_my_process::execute()
{
  // verify that the number of input frames is correct
  if ( input_data_.size() != 2 ){
    vcl_cout << "In vidpro1_my_process::execute() - not exactly two"
             << " input frames" << vcl_endl;
    return false;
  }
  clear_output();

  // get images from the storage classes
  vidpro1_image_storage_sptr curr_frame_image, prev_frame_image;
  curr_frame_image.vertical_cast(input_data_[0][0]);
  prev_frame_image.vertical_cast(input_data_[1][0]);
  vil_image_resource_sptr curr_image_sptr = curr_frame_image->get_image();
  vil_image_resource_sptr prev_image_sptr = prev_frame_image->get_image();

  // get vsol2D from the storage classes
  vidpro1_vsol2D_storage_sptr curr_frame_vsol2D;
  curr_frame_vsol2D.vertical_cast(input_data_[0][1]);

  vcl_vector< vsol_spatial_object_2d_sptr > vsol2D_data = curr_frame_vsol2D->get_vsol_list();

 
  //get parameters
  float param1;
  bool param2;
  parameters_sptr_->get_parameter( "-p1" , param1 );
  parameters_sptr_->get_parameter( "-p2" , param2 );


  //=========================================
  // Do something here with all of this data
  //=========================================


  // create the output storage class
  vidpro1_vsol2D_storage_sptr output_vsol2D = vidpro1_vsol2D_storage_new();
  output_vsol2D->add_vsol_sptr(/*add output geometry here*/);
  output_data_[0].push_back(output_vsol2D);
  
  return true;
}


//: Finish
bool
vidpro1_my_process::finish()
{
  // I'm not really sure what this is for, ask Amir
  // - mleotta
  return true;
}
