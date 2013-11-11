// This is contrib/sadali/vidpro1_new_vsol2d_tablo_process.cxx

//:
// \file

#include "vidpro1_new_vsol2d_tablo_process.h"
#include <bpro1/bpro1_parameters.h>

// include storage classes needed
// such as ...
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vtol_storage.h>
#include <vidpro1/storage/vidpro1_vtol_storage_sptr.h>

// other includes needed


#include <vsol/vsol_spatial_object_2d_sptr.h>



//: Constructor
vidpro1_new_vsol2d_tablo_process::vidpro1_new_vsol2d_tablo_process()
{
  
}


//: Destructor
vidpro1_new_vsol2d_tablo_process::~vidpro1_new_vsol2d_tablo_process()
{
}


bpro1_process* vidpro1_new_vsol2d_tablo_process:: clone() const
{
    return new vidpro1_new_vsol2d_tablo_process(*this);
}

//: Return the name of this process
vcl_string
vidpro1_new_vsol2d_tablo_process::name()
{
  return "Drawing Tablo";
}


//: Return the number of input frame for this process
int
vidpro1_new_vsol2d_tablo_process::input_frames()
{
  // input from this frame and the previous frame
  return 0;
}


//: Return the number of output frames for this process
int
vidpro1_new_vsol2d_tablo_process::output_frames()
{
  // output to this frame only
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > 
vidpro1_new_vsol2d_tablo_process::get_input_type()
{
  // this process looks for an image and vsol2D storage class
  // at each input frame
  vcl_vector< vcl_string > to_return;
 // to_return.push_back( "vtol" );
 // to_return.push_back( "vtol" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > 
vidpro1_new_vsol2d_tablo_process::get_output_type()
{  
  // this process produces a vsol2D storage class
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "vsol2D" );
  return to_return;
}


//: Execute the process
bool
vidpro1_new_vsol2d_tablo_process::execute()
{
  // verify that the number of input frames is correct
  if ( input_data_.size() != 0 ){
    vcl_cout << "In vidpro1_new_vsol2d_tablo_process::execute() - not exactly"
             << " two input frames" << vcl_endl;
    return false;
  }
  clear_output();

  
 
  //get parameters
 
  //=========================================
  // Do something here with all of this data
  //=========================================

 
 
  
   // create the output storage class
  vidpro1_vsol2D_storage_sptr output_vsol2D = vidpro1_vsol2D_storage_new();
  //vcl_vector<vsol_spatial_object_2d_sptr> point=NULL;
  //output_vsol2D->add_vsol_sptr(point);
  output_data_[0].push_back(output_vsol2D);
  
  return true;
}


//: Finish
bool
vidpro1_new_vsol2d_tablo_process::finish()
{
  // I'm not really sure what this is for, ask Amir
  // - mleotta
  return true;
}
