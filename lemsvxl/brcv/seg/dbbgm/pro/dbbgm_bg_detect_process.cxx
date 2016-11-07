// This is brcv/seg/dbbgm/pro/dbbgm_bg_detect_process.cxx

//:
// \file

#include "dbbgm_bg_detect_process.h"
#include <bpro1/bpro1_parameters.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_convert.h>
#include <vil/vil_new.h>


#include "dbbgm_image_storage.h"
#include <bbgm/bbgm_image_sptr.h>
#include <bbgm/bbgm_image_of.h>
#include <bbgm/bbgm_apply.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture.h>
#include <bsta/bsta_gauss_sf1.h>
#include <bsta/bsta_gauss_if3.h>
#include <bsta/bsta_detector_gaussian.h>
#include <bsta/bsta_detector_mixture.h>

#include <vil/algo/vil_colour_space.h>
#include <vil/algo/vil_binary_opening.h>
#include <vil/algo/vil_binary_closing.h>
#include <vil/algo/vil_threshold.h>

//: Constructor
dbbgm_bg_detect_process::dbbgm_bg_detect_process()
{
  if( !parameters()->add( "Distance Theshold" ,    "-dist" ,    2.5f        ) ||
      !parameters()->add( "Weight Theshold" ,      "-wt" ,      0.7f        ) ||
      !parameters()->add( "Morphological Radius" , "-mradius" , 3.0f        ) ||
      !parameters()->add( "YUV Color Space" ,      "-yuv",      false )      ){
    vcl_cerr << "ERROR: Adding parameters in " __FILE__<< vcl_endl;
  }
}


//: Destructor
dbbgm_bg_detect_process::~dbbgm_bg_detect_process()
{
}


//: Clone the process
bpro1_process* 
dbbgm_bg_detect_process::clone() const
{
  return new dbbgm_bg_detect_process(*this);
}


//: Return the name of this process
vcl_string
dbbgm_bg_detect_process::name()
{
  return "Detect Background";
}


//: Return the number of input frame for this process
int
dbbgm_bg_detect_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
dbbgm_bg_detect_process::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbbgm_bg_detect_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  to_return.push_back( "dbbgm_image" );

  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbbgm_bg_detect_process::get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "image" );

  return to_return;
}


//: Execute the process
bool
dbbgm_bg_detect_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dbbgm_bg_detect_process::execute() - "
             << "not exactly one input image \n";
    return false;
  }
  clear_output();

  // get the process parameters 
  float dist = 0.0f, wt = 0.0f, mradius=0.0;
  bool yuv = false;
  parameters()->get_value( "-dist" ,   dist);
  parameters()->get_value( "-wt" ,     wt);
  parameters()->get_value( "-mradius", mradius);
  parameters()->get_value( "-yuv" ,    yuv);

  // get image from the storage class
  vidpro1_image_storage_sptr frame_image;
  frame_image.vertical_cast(input_data_[0][0]);

  // convert the image to float
  vil_image_resource_sptr image_rsc = frame_image->get_image();
  vil_image_view<float> img = *vil_convert_cast(float(), image_rsc->get_view());
  if(image_rsc->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
    vil_math_scale_values(img,1.0/255.0);

  //convert to YUV
  if(yuv){
    for(unsigned int i=0; i<img.ni(); ++i){
      for(unsigned int j=0; j<img.nj(); ++j){
        float pixel[3] = {img(i,j,0), img(i,j,1), img(i,j,2)};
        float yuv[3];
        vil_colour_space_RGB_to_YUV(pixel, yuv);
        img(i,j,0) = yuv[0];
        img(i,j,1) = yuv[1];
        img(i,j,2) = yuv[2];
      }
    }
  }


  // get the templated mixture model
  bbgm_image_sptr model_sptr;
  dbbgm_image_storage_sptr frame_distimg;
  frame_distimg.vertical_cast(input_data_[0][1]);
  if(!frame_distimg){
    return false;
  }
  else{
    model_sptr = frame_distimg->dist_image();
  }

  typedef bsta_gauss_if3 bsta_gauss_t;
  typedef bsta_gauss_t::vector_type _vector;
  typedef bsta_num_obs<bsta_gauss_t> gauss_type;
  typedef bsta_num_obs<bsta_mixture<gauss_type> > mix_gauss_type;
  bbgm_image_of<mix_gauss_type> *model = static_cast<bbgm_image_of<mix_gauss_type>*>(model_sptr.ptr());

  bsta_g_mdist_detector<bsta_gauss_t> gmd(dist);
  bsta_top_weight_detector<mix_gauss_type, bsta_g_mdist_detector<bsta_gauss_t> >
      detector(gmd, wt);

  vil_image_view<bool> mask(img.ni(),img.nj(),1);
  mask.fill(false);

  bbgm_apply(*model,detector,img,mask);

  if(mradius > 0.0f){
    vil_structuring_element disk;
    disk.set_to_disk(mradius);
    vil_binary_closing(mask,mask,disk);
    vil_binary_opening(mask,mask,disk); 
  }

  vidpro1_image_storage_sptr output_storage = vidpro1_image_storage_new();
  output_storage->set_image(vil_new_image_resource_of_view( mask ));
  output_data_[0].push_back(output_storage); 

  return true;
}



bool
dbbgm_bg_detect_process::finish()
{
  return true;
}




