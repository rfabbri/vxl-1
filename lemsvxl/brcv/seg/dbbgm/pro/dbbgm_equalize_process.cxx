// This is brcv/seg/dbbgm/pro/dbbgm_equalize_process.cxx

//:
// \file

#include "dbbgm_equalize_process.h"
#include <bpro1/bpro1_parameters.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>

#include "dbbgm_image_storage.h"
#include <bbgm/bbgm_image_sptr.h>
#include <bbgm/bbgm_image_of.h>
#include <bbgm/bbgm_apply.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture.h>
#include <bsta/bsta_gauss_if3.h>
#include <bsta/bsta_basic_functors.h>
#include <bsta/bsta_detector_gaussian.h>
#include <bsta/bsta_detector_mixture.h>


#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_new.h>
#include <vil/vil_math.h>
#include <vil/vil_plane.h>
#include <vil/algo/vil_binary_closing.h>
#include <vil/algo/vil_binary_opening.h>
#include <vil/algo/vil_colour_space.h>
#include <bil/bil_math.h>
#include <bil/algo/bil_equalize.h>

#include <vul/vul_timer.h>


//: Constructor
dbbgm_equalize_process::dbbgm_equalize_process()
{
  if( !parameters()->add( "Detection Threshold" ,  "-dthresh" ,   3.0f    ) ||
      !parameters()->add( "Background Weight" ,    "-bgthresh" ,  0.7f    ) ||
      !parameters()->add( "Saturation Threshold" , "-sthresh" ,   0.01f   )||
      !parameters()->add( "Morphological Radius" , "-mradius" ,   0.0f   ) ||
      !parameters()->add( "YUV Model" ,            "-yuv",        false  ) ||
      !parameters()->add( "Debug" ,                "-debug",      false  )  ){
    vcl_cerr << "ERROR: Adding parameters in " __FILE__<< vcl_endl;
  }
}


//: Destructor
dbbgm_equalize_process::~dbbgm_equalize_process()
{
}


//: Clone the process
bpro1_process* 
dbbgm_equalize_process::clone() const
{
  return new dbbgm_equalize_process(*this);
}


//: Return the name of this process
vcl_string
dbbgm_equalize_process::name()
{
  return "Equalize with Background";
}


//: Return the number of input frame for this process
int
dbbgm_equalize_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
dbbgm_equalize_process::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbbgm_equalize_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  to_return.push_back( "dbbgm_image" );
  to_return.push_back( "image" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbbgm_equalize_process::get_output_type()
{  
  vcl_vector<vcl_string > to_return;
  // return mask image in debug mode only
  bool debug=false;
  parameters()->get_value( "-debug" ,  debug );
  if(debug)
    to_return.push_back( "image" );
  return to_return;
}


//: Execute the process
bool
dbbgm_equalize_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dbbgm_equalize_process::execute() - "
             << "not exactly one input frame \n";
    return false;
  }
  clear_output();

  // get the process parameters 
  float dthresh=0, bgthresh=0, sthresh=0, mradius=0;
  bool yuv=false, debug=false;
  parameters()->get_value( "-dthresh" ,  dthresh );
  parameters()->get_value( "-bgthresh" , bgthresh );
  parameters()->get_value( "-sthresh" ,  sthresh );
  parameters()->get_value( "-mradius" ,  mradius );
  parameters()->get_value( "-yuv" ,      yuv );
  parameters()->get_value( "-debug" ,    debug );

  // get image from the storage class
  vidpro1_image_storage_sptr frame_image;
  frame_image.vertical_cast(input_data_[0][0]);

  // convert the image to float 
  vil_image_resource_sptr image_rsc = frame_image->get_image();
  vil_image_view<float> img = *vil_convert_cast(float(), image_rsc->get_view());
  if(image_rsc->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
    vil_math_scale_values(img,1.0/255.0);


  // get the mixture model
  bbgm_image_sptr model_sptr = NULL;
  dbbgm_image_storage_sptr frame_distimg;
  frame_distimg.vertical_cast(input_data_[0][1]);
  if(frame_distimg){
    model_sptr = frame_distimg->dist_image();
  }


  // get output image from the storage class
  frame_image.vertical_cast(input_data_[0][2]);
  vil_image_view<float> out_img;
  if(!frame_image){
    out_img.set_size(img.ni(),img.nj(),img.nplanes());
    frame_image = vidpro1_image_storage_new();
    frame_image->set_image(vil_new_image_resource_of_view( out_img ));
    frame_image->mark_global();
    input_data_[0][2] = frame_image;
  }
  else{
    image_rsc = frame_image->get_image();
    assert(image_rsc->pixel_format() == vil_pixel_format_of(float()));
    out_img = image_rsc->get_view();
  }

  if(!model_sptr){
    out_img.deep_copy(img);
    return true;
  }

  typedef bsta_num_obs<bsta_gauss_if3> gauss_type;
  typedef bsta_num_obs<bsta_mixture<gauss_type> > mix_gauss_type;
  bbgm_image_of<mix_gauss_type> *model = static_cast<bbgm_image_of<mix_gauss_type>*>(model_sptr.ptr());

  vul_timer t;

  bsta_g_mdist_detector<gauss_type> gdet(dthresh);
  bsta_top_weight_detector<mix_gauss_type, bsta_g_mdist_detector<gauss_type> > bg_detector(gdet, bgthresh);
  vil_image_view<bool> bg_mask(img.ni(),img.nj(),1);

  // get a background image
  bsta_mean_functor<gauss_type> mean_functor;
  bsta_mixture_functor<mix_gauss_type, bsta_mean_functor<gauss_type> > bg_functor(mean_functor, (unsigned) 0);
  static vil_image_view<float> bg_img(img.ni(),img.nj(),img.nplanes());
  bbgm_apply(*model,bg_functor,bg_img);

  //convert YUV to RGB
  if(yuv){
    for(unsigned int i=0; i<bg_img.ni(); ++i){
      for(unsigned int j=0; j<bg_img.nj(); ++j){
        float pixel[3] = {bg_img(i,j,0), bg_img(i,j,1), bg_img(i,j,2)};
        float rgb[3];
        vil_colour_space_YUV_to_RGB(pixel, rgb);
        bg_img(i,j,0) = rgb[0];
        bg_img(i,j,1) = rgb[1];
        bg_img(i,j,2) = rgb[2];
      }
    }
  }

  static vil_image_view<bool> bg_mask_sat(img.ni(),img.nj(),1);

  // compute a mask for saturated pixels
  static vil_image_view<bool> saturated_mask(img.ni(),img.nj(),1);
  saturated_mask.fill(true);
  bil_math_threshold_mask(vil_plane(img,0),sthresh,1.0f-sthresh,saturated_mask);
  bil_math_threshold_mask(vil_plane(img,1),sthresh,1.0f-sthresh,saturated_mask);
  bil_math_threshold_mask(vil_plane(img,2),sthresh,1.0f-sthresh,saturated_mask); 

  static double scale[3] = {1.0,1.0,1.0}, offset[3] = {0.0,0.0,0.0};

  // apply previous equalization (if any)
  out_img.deep_copy(img);
  if(scale[0] != 1.0 && offset[0] != 0.0){
    for(unsigned p=0; p<img.nplanes(); ++p)
    {
      vil_image_view<float> plane = vil_plane(out_img,p);
      vil_math_scale_and_offset_values(plane,scale[p],offset[p]);
    }
  }

  for(unsigned int k=0; k<5; ++k){
    if(yuv){
      static vil_image_view<float> yuv_img(out_img.ni(),out_img.nj(),out_img.nplanes());
      for(unsigned int i=0; i<out_img.ni(); ++i){
        for(unsigned int j=0; j<out_img.nj(); ++j){
          float pixel[3] = {out_img(i,j,0), out_img(i,j,1), out_img(i,j,2)};
          float yuv[3];
          vil_colour_space_RGB_to_YUV(pixel, yuv);
          yuv_img(i,j,0) = yuv[0];
          yuv_img(i,j,1) = yuv[1];
          yuv_img(i,j,2) = yuv[2];
        }
      }
      bbgm_apply(*model,bg_detector,yuv_img,bg_mask);
    }
    else
      bbgm_apply(*model,bg_detector,out_img,bg_mask);

    if(mradius >=1.0){
      vil_structuring_element disk;
      disk.set_to_disk(mradius);
      vil_binary_closing(bg_mask,bg_mask,disk);
      vil_binary_opening(bg_mask,bg_mask,disk);
    }

#if 0
    unsigned int fg_count=0;
    for(unsigned int j=0; j<bg_mask.nj(); ++j){
      for(unsigned int i=0; i<bg_mask.ni(); ++i){
        if(!bg_mask(i,j) && saturated_mask(i,j))
          ++fg_count;
      }
    }
    double fg_ratio = double(fg_count) /(bg_mask.ni()*bg_mask.nj());
    if(fg_ratio < 0.1){
        scale[0] = scale[1] = scale[2] =1.0; 
        offset[0] = offset[1] = offset[2] = 0.0;
        break;
    }
#endif

    double last_s = scale[0], last_o = offset[0];
    bil_math_boolean_and(bg_mask,saturated_mask,bg_mask_sat);
    bil_equalize_linear_vals(vil_plane(bg_img,0), vil_plane(img,0), bg_mask_sat, scale[0], offset[0]);
    bil_equalize_linear_vals(vil_plane(bg_img,1), vil_plane(img,1), bg_mask_sat, scale[1], offset[1]);
    bil_equalize_linear_vals(vil_plane(bg_img,2), vil_plane(img,2), bg_mask_sat, scale[2], offset[2]);

    if(last_s == scale[0] && last_o == offset[0])
        break;
    else{
      //vcl_cout << "scale = " << scale[0] << " offset = "<<offset[0]<<vcl_endl;
      out_img.deep_copy(img);
      for(unsigned p=0; p<img.nplanes(); ++p)
      {
        vil_image_view<float> plane = vil_plane(out_img,p);
        vil_math_scale_and_offset_values(plane,scale[p],offset[p]);
      }
    }
  }

  if(debug){
    vidpro1_image_storage_sptr output_storage = vidpro1_image_storage_new();
    output_storage->set_image(vil_new_image_resource_of_view( bg_mask ));
    output_data_[0].push_back(output_storage);
  }

  //double eq_time = t.real() / 1000.0;
  //vcl_cout << "equalized in "<<eq_time<<" sec"<<vcl_endl;

  return true;
}



bool
dbbgm_equalize_process::finish()
{
  return true;
}




