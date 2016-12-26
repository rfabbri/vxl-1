// This is brl/vidpro/process/vidpro_frame_average_process.cxx

//:
// \file

#include "vidpro_frame_average_process.h"
#include <brip/brip_vil_float_ops.h>
#include <bpro/bpro_parameters.h>
#include <vidpro/storage/vidpro_image_storage.h>
#include <vidpro/storage/vidpro_image_storage_sptr.h>

#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_math.h>
#include <vil/vil_convert.h>

//: Constructor
vidpro_frame_average_process::vidpro_frame_average_process()
{
  if( !parameters()->add( "Average All" , "-avgall" , bool(true)) ||
      !parameters()->add( "No Frames to Average" , "-nframes_avg" , unsigned(10)))
    {
      vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
    }
  first_frame_ = true;
  frame_count_ = 0;
}



//: Destructor
vidpro_frame_average_process::~vidpro_frame_average_process()
{
}


//: Clone the process
bpro_process* 
vidpro_frame_average_process::clone() const
{
  return new vidpro_frame_average_process(*this);
}


//: Return the name of the process
vcl_string
vidpro_frame_average_process::name()
{
  return "Frame Average";
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string > vidpro_frame_average_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string > vidpro_frame_average_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  return to_return;
}


//: Returns the number of input frames to this process
int
vidpro_frame_average_process::input_frames()
{
  return 1;
}


//: Returns the number of output frames from this process
int
vidpro_frame_average_process::output_frames()
{
  return 1;
}

void vidpro_frame_average_process::
store_output(vil_image_view<float> const& result)
{
  vil_image_view<float> temp;
  temp.deep_copy(result);
  vil_image_view<vxl_byte> output;
  vil_convert_cast(result, output);

  // create the output storage class
  vidpro_image_storage_sptr output_storage = vidpro_image_storage_new();
  output_storage->set_image(vil_new_image_resource_of_view(output));
  output_data_[0].push_back(output_storage);
}

//: Run the process on the current frame
bool
vidpro_frame_average_process::execute()
{
  if ( input_data_.size() != 1 )
    {
      vcl_cout << "In vidpro_frame_average_process::execute() - not exactly one"
               << " input image \n";
      return false;
    }
  clear_output();

  bool average_all=false; 
  parameters()->get_value( "-avgall" , average_all);
  unsigned n_frames_avg = 0;
  parameters()->get_value( "-nframes_avg" , n_frames_avg);
  
  vidpro_image_storage_sptr frame_image;
  frame_image.vertical_cast(input_data_[0][0]);
  if(!frame_image)
    return false;

  vil_image_resource_sptr image_sptr = frame_image->get_image();
  
  vil_image_view< float > fimage = 
    brip_vil_float_ops::convert_to_float(*image_sptr);

  if(first_frame_)
    {
      average_.deep_copy(fimage);
      frame_count_ = 1;
      store_output(average_);
      first_frame_ = false;
      return true;
    }

  //Here we compute a recursive average
  if(average_all)
    ++ frame_count_;
  else if ( frame_count_< n_frames_avg)
    ++ frame_count_;

  // N = frame_count_
  // If we are averaging N frames, each new frame is weighted as:
  // average = (N fimage + N-1 average)/N
  double scale = frame_count_;
  vil_math_scale_values(fimage, 1.0/scale);
  vil_image_view< float > temp;
  temp.deep_copy(average_);
  vil_math_scale_values(temp, (scale-1.0)/scale);
  vil_math_image_sum(temp, fimage, average_);
  store_output(average_);
  return true;
}


//: Finish
bool
vidpro_frame_average_process::finish()
{
  first_frame_ = true;
  return true;
}


