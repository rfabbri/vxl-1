// This is brcv/seg/dbbgm/pro/dbbgm_shadow_detect_process.cxx

//:
// \file

#include "dbbgm_shadow_detect_process.h"
#include "dbbgm_distribution_image_storage.h"
#include <bpro1/bpro1_parameters.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_convert.h>
#include <vil/vil_new.h>
#include <vil/algo/vil_gauss_filter.h>
#include <dbbgm/dbbgm_distribution_image.h>
#include <dbbgm/dbbgm_detect.h>
#include <dbbgm/dbbgm_apply.h>
#include <dbsta/dbsta_gaussian_full.h>
#include <dbsta/dbsta_gaussian_sphere.h>
#include <dbsta/dbsta_detector_gaussian.h>
#include <dbsta/dbsta_detector_mixture.h>
#include <dbsta/dbsta_basic_functors.h>
#include <dbsta/dbsta_functor_sptr.h>
#include <dbsta/algo/dbsta_gaussian_stats.h>
#include <dbsta/algo/dbsta_bayes_functor.h>
#include <vil/algo/vil_colour_space.h>


//: Constructor
dbbgm_shadow_detect_process::dbbgm_shadow_detect_process()
{
  if( !parameters()->add( "Smoothing Sigma" ,      "-sigma" ,   1.0f        ) ||
      !parameters()->add( "Distance Theshold" ,    "-dist" ,    2.5f        ) ||
      !parameters()->add( "YUV Color Space" ,      "-yuv",      true        ) ){
    vcl_cerr << "ERROR: Adding parameters in " __FILE__<< vcl_endl;
  }
}


//: Destructor
dbbgm_shadow_detect_process::~dbbgm_shadow_detect_process()
{
}


//: Clone the process
bpro1_process* 
dbbgm_shadow_detect_process::clone() const
{
  return new dbbgm_shadow_detect_process(*this);
}


//: Return the name of this process
vcl_string
dbbgm_shadow_detect_process::name()
{
  return "Detect Shadows";
}


//: Return the number of input frame for this process
int
dbbgm_shadow_detect_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
dbbgm_shadow_detect_process::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbbgm_shadow_detect_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  to_return.push_back( "dbbgm_distribution_image" );

  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbbgm_shadow_detect_process::get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "image" );
  to_return.push_back( "image" );

  return to_return;
}


//: Execute the process
bool
dbbgm_shadow_detect_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dbbgm_shadow_detect_process::execute() - "
             << "not exactly two input images \n";
    return false;
  }
  clear_output();

  // get the process parameters 
  float sigma = 0.0f, dist = 0.0f;
  bool yuv=false;
  parameters()->get_value( "-sigma" ,  sigma);
  parameters()->get_value( "-dist" ,   dist);
  parameters()->get_value( "-yuv" ,    yuv);

  // get image from the storage class
  vidpro1_image_storage_sptr frame_image;
  frame_image.vertical_cast(input_data_[0][0]);

  // convert the image to float and smooth it
  vil_image_resource_sptr image_rsc = frame_image->get_image();
  vil_image_view<float> img = *vil_convert_cast(float(), image_rsc->get_view());
  if(image_rsc->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
    vil_math_scale_values(img,1.0/255.0);


  static vil_image_view<float> smooth_img;
  if(sigma > 0.0f){
    vil_gauss_filter_5tap_params params( sigma );
    vil_gauss_filter_5tap( img , smooth_img , params );
  }
  else
    smooth_img.deep_copy(img);

  //convert to YUV
  if(yuv){
    for(unsigned int i=0; i<smooth_img.ni(); ++i){
      for(unsigned int j=0; j<smooth_img.nj(); ++j){
        float pixel[3] = {smooth_img(i,j,0), smooth_img(i,j,1), smooth_img(i,j,2)};
        float yuv[3];
        vil_colour_space_RGB_to_YUV(pixel, yuv);
        smooth_img(i,j,0) = yuv[0];
        smooth_img(i,j,1) = yuv[1];
        smooth_img(i,j,2) = yuv[2];
      }
    }
  }

  // get the background model
  static dbbgm_distribution_image_storage_sptr frame_model;
  if(input_data_[0][1]){ 
    frame_model.vertical_cast(input_data_[0][1]);
  }
  //frame_model.vertical_cast(input_data_[0][1]);
  dbbgm_distribution_image<float> *model = 
    dynamic_cast<dbbgm_distribution_image<float>*>(frame_model->dist_image().ptr());
  if(!model)
    return false;

  dbsta_mix_first_index_detector<float> detector1(new dbsta_g_mdist_detector<float>(dist), 0);
  dbsta_mix_first_index_detector<float> detector2(new dbsta_g_mdist_detector<float>(dist), 1);
  dbsta_mix_first_index_detector<float> detector3(new dbsta_g_mdist_detector<float>(dist), 2);
  vil_image_view<bool> mask(img.ni(),img.nj(),3);
  mask.fill(false);
  vil_image_view<bool> p0 = vil_plane(mask,0);
  vil_image_view<bool> p1 = vil_plane(mask,1);
  vil_image_view<bool> p2 = vil_plane(mask,2);
  detect(*model,smooth_img,detector1,p0);
  detect(*model,smooth_img,detector2,p1);
  detect(*model,smooth_img,detector3,p2);
  vidpro1_image_storage_sptr output_storage = vidpro1_image_storage_new();
  output_storage->set_image(vil_new_image_resource_of_view( mask ));
  output_data_[0].push_back(output_storage);

  dbsta_bayes_functor<float> bayes;
  vil_image_view<float> clsfy;
  apply(*model,bayes,smooth_img,clsfy);
  vil_image_view<bool> result(clsfy.ni(), clsfy.nj(), 3);
  result.fill(false);
  for(unsigned int i=0; i<clsfy.ni(); ++i){
    for(unsigned int j=0; j<clsfy.nj(); ++j){
      int c = static_cast<int>(clsfy(i,j));
      if(c >=0 && c<3) 
        result(i,j,c) = true;
    }
  }
  vidpro1_image_storage_sptr output_storage2 = vidpro1_image_storage_new();
  output_storage2->set_image(vil_new_image_resource_of_view( result ));
  output_data_[0].push_back(output_storage2);

  return true;
}



bool
dbbgm_shadow_detect_process::finish()
{
  return true;
}




