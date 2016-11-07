// This is brcv/seg/dbbgm/pro/dbbgm_truth_model_process.cxx

//:
// \file

#include "dbbgm_truth_model_process.h"
#include <bpro1/bpro1_parameters.h>
#include <vidpro1/storage/vidpro1_image_storage.h>

#include "dbbgm_image_storage.h"
#include <bbgm/bbgm_image_sptr.h>
#include <bbgm/bbgm_image_of.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture.h>
#include <bsta/bsta_gauss_if3.h>
#include <bsta/algo/bsta_truth_updater.h>
#include <bsta/algo/bsta_gaussian_updater.h>
#include <bsta/algo/bsta_gaussian_stats.h>

#include <bbgm/bbgm_update.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_plane.h>
#include <vil/algo/vil_colour_space.h>


//: Constructor
dbbgm_truth_model_process::dbbgm_truth_model_process()
{
  if( !parameters()->add( "YUV color space" ,      "-yuv" ,     false       )){
    vcl_cerr << "ERROR: Adding parameters in " __FILE__<< vcl_endl;
  }
}


//: Destructor
dbbgm_truth_model_process::~dbbgm_truth_model_process()
{
}


//: Clone the process
bpro1_process* 
dbbgm_truth_model_process::clone() const
{
  return new dbbgm_truth_model_process(*this);
}


//: Return the name of this process
vcl_string
dbbgm_truth_model_process::name()
{
  return "Truth BG Model";
}


//: Return the number of input frame for this process
int
dbbgm_truth_model_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
dbbgm_truth_model_process::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbbgm_truth_model_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" ); // input image
  to_return.push_back( "image" ); // label image
  to_return.push_back( "dbbgm_image" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbbgm_truth_model_process::get_output_type()
{  
  vcl_vector<vcl_string > to_return;
  //to_return.push_back( "image" );
  //to_return.push_back( "image" ); 
  return to_return;
}


//: Execute the process
bool
dbbgm_truth_model_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dbbgm_truth_model_process::execute() - "
             << "not exactly two input images \n";
    return false;
  }
  clear_output();

  // get the process parameters 
  bool yuv = false;
  parameters()->get_value( "-yuv" ,  yuv );

  // get image from the storage class
  vidpro1_image_storage_sptr frame_image;
  frame_image.vertical_cast(input_data_[0][0]);

  // convert the image to float and smooth it
  vil_image_resource_sptr image_rsc = frame_image->get_image();
  vil_image_view<float> img = *vil_convert_cast(float(), image_rsc->get_view());
  if(image_rsc->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
    vil_math_scale_values(img,1.0/255.0);

  vil_image_view<float> img_truth(img.ni(),img.nj(),4);
  vil_image_view<float> img_out = vil_planes(img_truth,0,1,3);
  img_out.deep_copy(img);

  if(yuv){
    for(unsigned int i=0; i<img_out.ni(); ++i){
      for(unsigned int j=0; j<img_out.nj(); ++j){
        float pixel[3] = {img_out(i,j,0), img_out(i,j,1), img_out(i,j,2)};
        float yuv[3];
        vil_colour_space_RGB_to_YUV(pixel, yuv);
        img_out(i,j,0) = yuv[0];
        img_out(i,j,1) = yuv[1];
        img_out(i,j,2) = yuv[2];
      }
    }
  }


  // get label image from the storage class
  vidpro1_image_storage_sptr frame_image2;
  frame_image2.vertical_cast(input_data_[0][1]);

  vil_image_view<vxl_byte> label_image = frame_image2->get_image()->get_view();
  assert(label_image.ni() == img.ni());
  assert(label_image.nj() == img.nj());

  // copy the labels into the last plane of the "truth" image
  vil_image_view<float> idx_image = vil_plane(img_truth,3);
  vil_convert_cast(label_image, idx_image);

  typedef bsta_num_obs<bsta_gauss_if3> gauss_type;
  typedef bsta_num_obs<bsta_mixture<gauss_type> > mix_gauss_type;

  bsta_gauss_if3 init_gauss;

  bsta_truth_updater<gauss_type, bsta_gaussian_updater<bsta_gauss_if3> >
      updater(init_gauss, bsta_gaussian_updater<bsta_gauss_if3>());
  bsta_mixture_weight_by_obs_updater<bsta_gauss_if3> reweight;

  // get the templated mixture model
  bbgm_image_sptr model_sptr;
  dbbgm_image_storage_sptr frame_distimg;
  frame_distimg.vertical_cast(input_data_[0][2]);
  if(!frame_distimg){
    mix_gauss_type mixture;
    model_sptr = new bbgm_image_of<mix_gauss_type>(img.ni(),img.nj(),mixture);
    frame_distimg = dbbgm_image_storage_new(model_sptr);
    frame_distimg->mark_global();
    input_data_[0][2] = frame_distimg;
  }
  else{
    model_sptr = frame_distimg->dist_image();
  }

  bbgm_image_of<mix_gauss_type> *model = static_cast<bbgm_image_of<mix_gauss_type>*>(model_sptr.ptr());

  update(*model,img_truth,updater);
  update(*model,reweight);

#if 0 
  for(unsigned int i=0; i<img_truth.ni(); ++i){
    for(unsigned int j=0; j<img_truth.nj(); ++j){
      if(img_truth(i,j,3) == 1.0f){
        vnl_vector_fixed<float,3> p(img_truth(i,j,0), img_truth(i,j,1), img_truth(i,j,2));
        data[i][j].push_back(p);
      }
    }
  }
#endif
  return true;
}



bool
dbbgm_truth_model_process::finish()
{
#if 0
  dbbgm_distribution_image_sptr model_sptr;
  dbbgm_distribution_image_storage_sptr frame_distimg;
  frame_distimg.vertical_cast(input_data_[0][2]);

  model_sptr = frame_distimg->dist_image();

  dbbgm_distribution_image<float> *model = static_cast<dbbgm_distribution_image<float>*>(model_sptr.ptr());


  for(unsigned int i=0; i<data.size(); ++i){
    for(unsigned int j=0; j<data[i].size(); ++j){
      if(!data[i][j].empty()){
        const dbsta_mixture<float>& m = static_cast<const dbsta_mixture<float>&>((*model)(i,j));
        const dbsta_gaussian_indep<float,3>& g = static_cast<const dbsta_gaussian_indep<float,3>&>(m.distribution(0));
        vcl_cout << g.mean() <<"\n-------------------"<< vcl_endl;
        for(unsigned int k=0; k<data[i][j].size(); ++k){
          vcl_cout << data[i][j][k] << vcl_endl;
        }
        vcl_cout << vcl_endl;
      }
    }
  }
#endif
  return true;
}




