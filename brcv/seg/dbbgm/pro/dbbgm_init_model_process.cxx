// This is brcv/seg/dbbgm/pro/dbbgm_init_model_process.cxx

//:
// \file

#include "dbbgm_init_model_process.h"
#include <bpro1/bpro1_parameters.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>

#include "dbbgm_image_storage.h"
#include <bbgm/bbgm_image_sptr.h>
#include <bbgm/bbgm_image_of.h>
#include <bbgm/bbgm_update.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture.h>
#include <bsta/bsta_gauss_sf1.h>
#include <bsta/bsta_gauss_if3.h>
#include <bsta/algo/bsta_adaptive_updater.h>

#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_math.h>
#include <vil/algo/vil_colour_space.h>



//: Constructor
dbbgm_init_model_process::dbbgm_init_model_process()
{
  if( !parameters()->add( "Max Num Components" ,   "-maxcmp" ,  unsigned(3) ) ||
      !parameters()->add( "Initial Variance" ,     "-initv" ,   0.01f       ) ||
      !parameters()->add( "Component Threshold" ,  "-g_thresh", 3.0f        ) ||
      !parameters()->add( "Update Masked" ,        "-masked",   false       ) ||
      !parameters()->add( "YUV Color Space" ,      "-yuv",      false       )   ){
    vcl_cerr << "ERROR: Adding parameters in " __FILE__<< vcl_endl;
  }
}


//: Destructor
dbbgm_init_model_process::~dbbgm_init_model_process()
{
}


//: Clone the process
bpro1_process* 
dbbgm_init_model_process::clone() const
{
  return new dbbgm_init_model_process(*this);
}


//: Return the name of this process
vcl_string
dbbgm_init_model_process::name()
{
  return "Init Background";
}


//: Return the number of input frame for this process
int
dbbgm_init_model_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
dbbgm_init_model_process::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbbgm_init_model_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  // optionally use an image mask
  bool masked=false;
  parameters()->get_value( "-masked" , masked );
  if(masked)
    to_return.push_back( "image" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbbgm_init_model_process::get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "dbbgm_image" );
  return to_return;
}


//: Returns a vector of strings with suggested names for output classes
vcl_vector< vcl_string > dbbgm_init_model_process::suggest_output_names()
{
  vcl_vector< vcl_string > names;
  names.push_back("bg model");

  return names;
}


//: Execute the process
bool
dbbgm_init_model_process::execute()
{
  return true;  
}



bool
dbbgm_init_model_process::finish()
{
  // get the process parameters 
  float  init_var = 0.0f, g_thresh = 0.0f;
  unsigned int max_components=0;
  bool yuv=false, masked=false;
  parameters()->get_value( "-maxcmp" , max_components );
  parameters()->get_value( "-initv" ,  init_var );
  parameters()->get_value( "-g_thresh",g_thresh );
  parameters()->get_value( "-yuv", yuv );
  parameters()->get_value( "-masked" , masked );


  // find the first frame with input
  unsigned int frame = 0;
  for(; frame < input_data_.size(); ++frame)
    if(!input_data_[frame].empty() && input_data_[frame][0])
      break;

  if(frame >= input_data_.size())
    return false;

  vcl_vector<vil_image_view<float> > images;
  vcl_vector<vil_image_view<bool> > masks;
  for(; (frame < input_data_.size()) && input_data_[frame][0]; ++frame){

    // get image from the storage class
    vidpro1_image_storage_sptr frame_image;
    frame_image.vertical_cast(input_data_[frame][0]);

    // convert the image to float and smooth it
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
    images.push_back(img);

    if(masked){
      frame_image.vertical_cast(input_data_[frame][1]);

      // convert the image to float and smooth it
      image_rsc = frame_image->get_image();
      vil_image_view<bool> mask = image_rsc->get_view();
      masks.push_back(mask);
    }
  }

  typedef bsta_gauss_if3 bsta_gauss_t;
  typedef bsta_gauss_t::vector_type _vector;
  typedef bsta_num_obs<bsta_gauss_t> gauss_type;
  typedef bsta_mixture<gauss_type> bsta_mix_t;
  typedef bsta_num_obs<bsta_mix_t> mix_gauss_type;

  bsta_gauss_t init_gauss(_vector(0.0f), _vector(init_var) );
  bsta_mg_statistical_updater<bsta_mix_t> updater(init_gauss, max_components, g_thresh, 0.0f);

  mix_gauss_type mixture;
  bbgm_image_of<mix_gauss_type> *model = new bbgm_image_of<mix_gauss_type>(images[0].ni(),
                                                                             images[0].nj(),
                                                                             mixture);

  dbbgm_image_storage_sptr frame_distimg = dbbgm_image_storage_new(model);
  frame_distimg->mark_global();
  output_data_[0].push_back(frame_distimg);


  for(unsigned int i=0; i<images.size(); ++i){
    if(masked)
      update_masked(*model,images[i],updater,masks[i]);
    else
      update(*model,images[i],updater);
  }


  // learn the background in a different order
  bbgm_image_of<mix_gauss_type> alt_model(images[0].ni(),images[0].nj(),mixture);
  for(int i=images.size()-1; i>=0; --i){
    if(masked)
      update_masked(alt_model,images[i],updater,masks[i]);
    else
      update(alt_model,images[i],updater);
  }

  // take the mixtures from the alternate distribution image if they "seem better"
  bbgm_image_of<mix_gauss_type>::iterator itr1 = model->begin();
  bbgm_image_of<mix_gauss_type>::iterator itr2 = alt_model.begin();
  for(unsigned int j=0; j<model->nj(); ++j){
    for(unsigned int i=0; i<model->ni(); ++i, ++itr1, ++itr2){
      if(itr1->num_components() == 0 && itr2->num_components() == 0)
        continue;
      const gauss_type& g1 = itr1->distribution(0);
      const gauss_type& g2 = itr2->distribution(0);
      double e1 = itr1->weight(0)*g1.det_covar();
      double e2 = itr2->weight(0)*g2.det_covar();
      if(e1 > e2)
        *itr1 = *itr2;
    }
  }

  return true;
}




