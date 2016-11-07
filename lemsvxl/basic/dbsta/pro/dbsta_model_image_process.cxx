// This is brcv/seg/dbsta/pro/dbsta_model_image_process.cxx

//:
// \file

#include "dbsta_model_image_process.h"
#include <bpro1/bpro1_parameters.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_plane.h>
#include <vil/vil_new.h>
#include <vil/algo/vil_gauss_filter.h>
#include <dbsta/algo/dbsta_image_stats.h>
#include <dbsta/dbsta_histogram.h>
#include <vnl/vnl_vector_fixed.h>



//: Constructor
dbsta_model_image_process::dbsta_model_image_process()
{
  if( !parameters()->add( "number of bins" ,   "-bins" ,   (unsigned int)32 ) ){
    vcl_cerr << "ERROR: Adding parameters in " __FILE__<< vcl_endl;
  }
}


//: Destructor
dbsta_model_image_process::~dbsta_model_image_process()
{
}


//: Clone the process
bpro1_process* 
dbsta_model_image_process::clone() const
{
  return new dbsta_model_image_process(*this);
}


//: Return the name of this process
vcl_string
dbsta_model_image_process::name()
{
  return "Image Stats";
}


//: Return the number of input frame for this process
int
dbsta_model_image_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
dbsta_model_image_process::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbsta_model_image_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  to_return.push_back( "image" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbsta_model_image_process::get_output_type()
{  
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "image" );
  return to_return;
}


//: Execute the process
bool
dbsta_model_image_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dbsta_model_image_process::execute() - "
             << "not exactly one input frame \n";
    return false;
  }
  
  // get the process parameters 
  unsigned int num_bins=0;
  parameters()->get_value( "-bins" , num_bins );

  // get image from the storage class
  vidpro1_image_storage_sptr frame_image;
  frame_image.vertical_cast(input_data_[0][0]);

  // convert the image to float and smooth it
  vil_image_resource_sptr image_rsc = frame_image->get_image();
  vil_image_view<float> img = vil_convert_cast(float(), image_rsc->get_view());
  float min_val, max_val;
  vil_math_value_range(img, min_val, max_val);
  vcl_cout << min_val << " - "<<max_val<<vcl_endl;
  

  frame_image.vertical_cast(input_data_[0][1]);

  // convert the image to float and smooth it
  image_rsc = frame_image->get_image();
  vil_image_view<float> img2 = vil_convert_cast(float(), image_rsc->get_view());
  vil_math_value_range(img2, min_val, max_val);
  vcl_cout << min_val << " - "<<max_val<<vcl_endl;
  
  vil_image_view<float> joint_img(img.ni(), img.nj(), 2);
  vil_plane(joint_img,0).deep_copy(img);
  vil_plane(joint_img,1).deep_copy(img2);
  
  dbsta_histogram<float> hist(joint_img.nplanes(),0.0f,256.0f,num_bins);
  dbsta_image_statistics<float>(joint_img,hist,dbsta_histogram_updater<float>());

  vil_image_view<float> out_img(num_bins,num_bins,1);
  for(unsigned int i=0; i<num_bins; ++i){
    for(unsigned int j=0; j<num_bins; ++j){
      vnl_vector_fixed<float,2> p(i*256.0f/num_bins,j*256.0f/num_bins);
      out_img(i,j) = hist.probability(p);
    }
  }
  float sum = 0.0;
  vil_math_sum(sum,out_img,0);
  vcl_cout << "total prob " << sum <<vcl_endl;
  vil_math_value_range(out_img, min_val, max_val);
  vcl_cout << "max prob "<<max_val << vcl_endl;
  vil_math_scale_values(out_img,1.0/max_val);

  vidpro1_image_storage_sptr output_storage = vidpro1_image_storage_new();
  output_storage->set_image(vil_new_image_resource_of_view( out_img ));
  output_data_[0].push_back(output_storage);

  return true;  
}
    


bool
dbsta_model_image_process::finish()
{
  return true;
}




