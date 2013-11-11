// This is contrib/mleotta/pro/mleotta_video_slice_process.cxx

//:
// \file

#include "mleotta_video_slice_process.h"
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <bpro1/bpro1_parameters.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_math.h>


//: Constructor
mleotta_video_slice_process::mleotta_video_slice_process()
{  
  if( !parameters()->add( "Slice Row" ,      "-row" ,  (unsigned int)0   ) ){
    vcl_cerr << "ERROR: Adding parameters in " __FILE__<< vcl_endl;
  }
}


//: Destructor
mleotta_video_slice_process::~mleotta_video_slice_process()
{
}


//: Return the name of this process
vcl_string
mleotta_video_slice_process::name()
{
  return "Video Slice";
}


//: Return the number of input frame for this process
int
mleotta_video_slice_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
mleotta_video_slice_process::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > mleotta_video_slice_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > mleotta_video_slice_process::get_output_type()
{  
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "image" );
  return to_return;
}


//: Clone the process
bpro1_process*
mleotta_video_slice_process::clone() const
{
  return new mleotta_video_slice_process(*this);
}


//: Execute the process
bool
mleotta_video_slice_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In mleotta_video_slice_process::execute() - "
             << "not exactly one input frame\n";
    return false;
  }

  // get image from the storage class
  vidpro1_image_storage_sptr frame_image;
  frame_image.vertical_cast(input_data_[0][0]);

  frames_.push_back(frame_image->get_image());
 
  return true;  
}
    


bool
mleotta_video_slice_process::finish()
{
  if(frames_.empty())
    return false;
    
  unsigned int row=0;
  parameters()->get_value( "-row" ,  row );
  
  vil_image_view<vxl_byte> slice(frames_[0]->ni(), frames_.size(), frames_[0]->nplanes());
  for(unsigned int j=0; j<frames_.size(); ++j){
    vil_image_view<vxl_byte> frame = frames_[j]->get_view();
    for(unsigned int i=0; i<slice.ni(); ++i){
      for(unsigned int p=0; p<slice.nplanes(); ++p){
        slice(i,j,p) = frame(i,row,p);
      }
    }
  }
  
  // create the output storage class
  vidpro1_image_storage_sptr output_storage = vidpro1_image_storage_new();
  output_storage->set_image(vil_new_image_resource_of_view(slice));
  output_storage->mark_global();
  output_data_[0].push_back(output_storage);
  
  frames_.clear();
  return true;
}




