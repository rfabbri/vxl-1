// This is brl/vidpro/process/vidpro_grey_image_process.cxx

//:
// \file

#include "vidpro_grey_image_process.h"
#include <bpro/bpro_parameters.h>
#include <vidpro/storage/vidpro_image_storage.h>
#include <vidpro/storage/vidpro_image_storage_sptr.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>


//: Constructor
vidpro_grey_image_process::vidpro_grey_image_process()
{
  if( !parameters()->add( "Red Weight" ,   "-gs_red" ,   0.2125f ) ||
      !parameters()->add( "Green Weight" , "-gs_green" , 0.7154f ) ||
      !parameters()->add( "Blue Weight" ,  "-gs_blue" ,  0.0721f ) ) {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
vidpro_grey_image_process::~vidpro_grey_image_process()
{
}


//: Clone the process
bpro_process* 
vidpro_grey_image_process::clone() const
{
  return new vidpro_grey_image_process(*this);
}


//: Return the name of this process
vcl_string
vidpro_grey_image_process::name()
{
  return "Convert to Grey";
}


//: Return the number of input frame for this process
vcl_vector< vcl_string > vidpro_grey_image_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  return to_return;
}


//: Return the name of this process
vcl_vector< vcl_string > vidpro_grey_image_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  return to_return;
}


//: Return the number of input frames for this process
int
vidpro_grey_image_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
vidpro_grey_image_process::output_frames()
{
  return 1;
}


//: Execute the process
bool
vidpro_grey_image_process::execute()
{
  if ( input_data_.size() != 1 )
  {
      vcl_cout << "In vidpro_grey_image_process::execute() - not exactly one"
               << " input image \n";
      return false;
  }
  clear_output();

  float rw=0, gw=0, bw=0;

  parameters()->get_value( "-gs_red" , rw );
  parameters()->get_value( "-gs_green" , gw );
  parameters()->get_value( "-gs_blue" , bw );

  vidpro_image_storage_sptr frame_image;
  frame_image.vertical_cast(input_data_[0][0]);

  vil_image_resource_sptr image_sptr = frame_image->get_image();

  vil_image_view_base_sptr grey_view_sptr;
  grey_view_sptr = vil_convert_to_grey_using_rgb_weighting(rw,gw,bw,image_sptr->get_view());
  vil_image_resource_sptr output_sptr = vil_new_image_resource_of_view(*grey_view_sptr);


  //update the image
  frame_image->set_image(output_sptr);

  // create the output storage class
  vidpro_image_storage_sptr output_storage = vidpro_image_storage_new();
  output_storage->set_image(output_sptr);

  output_data_[0].push_back(output_storage);
  return true;  
}


bool
vidpro_grey_image_process::finish()
{
  return true;
}


