// This is brcv/seg/dbbgm/pro/dbbgm_model_process.cxx

//:
// \file

#include "dbbgm_model_process.h"
#include <bpro1/bpro1_parameters.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>

#include "dbbgm_image_storage.h"
#include <bbgm/bbgm_image_sptr.h>
#include <bbgm/bbgm_image_of.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture.h>
#include <bsta/bsta_gauss_if3.h>
#include <bsta/bsta_gauss_sf1.h>
#include <bsta/algo/bsta_adaptive_updater.h>


#include <bbgm/bbgm_update.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_new.h>
#include <vil/vil_math.h>
#include <vil/algo/vil_colour_space.h>
#include <vil/algo/vil_threshold.h>
#include <bil/bil_math.h>

#include <vul/vul_timer.h>


//: Constructor
dbbgm_model_process::dbbgm_model_process()
{
  if( !parameters()->add( "Max Num Components" ,   "-maxcmp" ,   int(3)  ) ||
      !parameters()->add( "Initial Variance" ,     "-initv" ,    0.1f         ) ||
      !parameters()->add( "Window Size" ,          "-ws" ,       int(300)) ||
      !parameters()->add( "Component Threshold" ,  "-g_thresh",  3.0f         ) ||
      !parameters()->add( "Minimum St. Dev." ,     "-min_stdev", 0.02f        ) ||
      !parameters()->add( "YUV Color Space" ,      "-yuv",       false        ) ||
      !parameters()->add( "S-G Approx." ,          "-grimson",   true         ) ||
      !parameters()->add( "Use Mask" ,             "-bg_mask",   false        ) ){
    vcl_cerr << "ERROR: Adding parameters in " __FILE__<< vcl_endl;
  }
}


//: Destructor
dbbgm_model_process::~dbbgm_model_process()
{
}


//: Clone the process
bpro1_process* 
dbbgm_model_process::clone() const
{
  return new dbbgm_model_process(*this);
}


//: Return the name of this process
vcl_string
dbbgm_model_process::name()
{
  return "Model Background";
}


//: Return the number of input frame for this process
int
dbbgm_model_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
dbbgm_model_process::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbbgm_model_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  to_return.push_back( "dbbgm_image" );
  bool mask=false;
  parameters()->get_value( "-bg_mask",   mask );
  if(mask)
    to_return.push_back( "image" );

  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbbgm_model_process::get_output_type()
{
  vcl_vector<vcl_string > to_return;

  return to_return;
}


//: Execute the process
bool
dbbgm_model_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dbbgm_model_process::execute() - "
             << "not exactly one input frame \n";
    return false;
  }
  clear_output();

  // get the process parameters 
  float init_var = 0.0f, g_thresh = 0.0f, min_stdev = 0.0f;
  int max_components=0, window_size=0;
  bool yuv=false, grimson=false, use_mask=false;
  parameters()->get_value( "-maxcmp" ,   max_components );
  parameters()->get_value( "-initv" ,    init_var );
  parameters()->get_value( "-ws" ,       window_size );
  parameters()->get_value( "-g_thresh",  g_thresh );
  parameters()->get_value( "-min_stdev", min_stdev );
  parameters()->get_value( "-yuv",       yuv );
  parameters()->get_value( "-grimson",   grimson );
  parameters()->get_value( "-bg_mask",   use_mask );


  // get image from the storage class
  vidpro1_image_storage_sptr frame_image;
  frame_image.vertical_cast(input_data_[0][0]);

  // convert the image to float
  vil_image_resource_sptr image_rsc = frame_image->get_image();
  vil_image_view<float> img = *vil_convert_cast(float(), image_rsc->get_view());
  if(image_rsc->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
    vil_math_scale_values(img,1.0/255.0);

  unsigned ni = img.ni();
  unsigned nj = img.nj();

  //convert to YUV
  if(yuv){
    for(unsigned int i=0; i<ni; ++i){
      for(unsigned int j=0; j<nj; ++j){
        float pixel[3] = {img(i,j,0), img(i,j,1), img(i,j,2)};
        float yuv[3];
        vil_colour_space_RGB_to_YUV(pixel, yuv);
        img(i,j,0) = yuv[0];
        img(i,j,1) = yuv[1];
        img(i,j,2) = yuv[2];
      }
    }
  }

  //========================================================================================

  typedef bsta_gauss_if3 bsta_gauss_t;
  typedef bsta_gauss_t::vector_type _vector;
  typedef bsta_num_obs<bsta_gauss_t> gauss_type;
  typedef bsta_mixture<gauss_type> bsta_mix_t;
  typedef bsta_num_obs<bsta_mix_t> mix_gauss_type;
  // get the templated mixture model
  bbgm_image_sptr model_sptr;
  dbbgm_image_storage_sptr frame_distimg;
  frame_distimg.vertical_cast(input_data_[0][1]);
  if(!frame_distimg){
    mix_gauss_type mixture;
    model_sptr = new bbgm_image_of<mix_gauss_type>(ni,nj,mixture);
    frame_distimg = dbbgm_image_storage_new(model_sptr);
    frame_distimg->mark_global();
    input_data_[0][1] = frame_distimg;
  }
  else{
    model_sptr = frame_distimg->dist_image();
  }

  bbgm_image_of<mix_gauss_type> *model = static_cast<bbgm_image_of<mix_gauss_type>*>(model_sptr.ptr());

  bsta_gauss_t init_gauss(_vector(0.0f), _vector(init_var) );

  vil_image_view<bool> mask_img;
  if(use_mask){
    // get mask image from the storage class
    frame_image.vertical_cast(input_data_[0][2]);
    // assume a missing mask is an empty mask, nothing to update
    if(!frame_image)
      return true;

    image_rsc = frame_image->get_image();
    if(image_rsc->pixel_format() == VIL_PIXEL_FORMAT_BOOL)
      mask_img = image_rsc->get_view();
    else if(image_rsc->pixel_format() == VIL_PIXEL_FORMAT_BYTE){
      vil_image_view<vxl_byte> temp = image_rsc->get_view();
      vil_threshold_inside(temp,mask_img,vxl_byte(1),vxl_byte(1));
    }
    else{
      vcl_cerr << "can not use mask of type "<<image_rsc->pixel_format()<<vcl_endl;
      return false;
    }
  }

  if(grimson){
    bsta_mg_grimson_window_updater<bsta_mix_t> updater(init_gauss,
                                                               max_components,
                                                               g_thresh,
                                                               min_stdev,
                                                               window_size);
    if(use_mask)
      update_masked(*model,img,updater,mask_img);
    else
      update(*model,img,updater);
  }
  else{
    bsta_mg_window_updater<bsta_mix_t> updater(init_gauss,
                                                       max_components,
                                                       g_thresh,
                                                       min_stdev,
                                                       window_size);
    if(use_mask)
      update_masked(*model,img,updater,mask_img);
    else
      update(*model,img,updater);
  }

  return true;
}



bool
dbbgm_model_process::finish()
{
  return true;
}




