// This is pro/dbbgm_aerial_bg_model_process.cxx

//:
// \file
#include "dbbgm_aerial_bg_model_process.h"
#include <dbbgm/pro/dbbgm_distribution_image_storage.h>
#include <bpro1/bpro1_parameters.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <dbbgm/dbbgm_distribution_image.h>
#include <dbbgm/dbbgm_update.h>
#include <dbbgm/dbbgm_detect.h>
#include <dbsta/dbsta_gaussian_indep.h>
#include <dbsta/dbsta_detector_gaussian.h>
#include <dbsta/dbsta_detector_mixture.h>
#include <dbsta/dbsta_basic_functors.h>
#include <dbsta/dbsta_functor_sptr.h>
#include <dbsta/algo/dbsta_adaptive_updater.h>
#include <vnl/vnl_matrix.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_new.h>
#include <vil/vil_math.h>
#include <vil/vil_plane.h>
#include <vil/algo/vil_gauss_filter.h>
#include <vul/vul_timer.h>
#include <vul/vul_sprintf.h>
#include <vil/vil_save.h>
//: Constructor
dbbgm_aerial_bg_model_process::dbbgm_aerial_bg_model_process(): model_(NULL)
{

    if( !parameters()->add( "Max Num Components" ,   "-maxcmp" ,  unsigned(3) ) ||
        !parameters()->add( "Initial Variance" ,     "-initv" ,   15.0f       ))
          {
        vcl_cerr << "ERROR: Adding parameters in " __FILE__<< vcl_endl;
    }

}

//: Destructor
dbbgm_aerial_bg_model_process::~dbbgm_aerial_bg_model_process()
{
   
}


//: Return the name of this process
vcl_string
dbbgm_aerial_bg_model_process::name()
{
    return "Aerial BG model builder";
}


//: Return the number of input frame for this process
int
dbbgm_aerial_bg_model_process::input_frames()
{
    return 1;
}


//: Return the number of output frames for this process
int
dbbgm_aerial_bg_model_process::output_frames()
{
    return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbbgm_aerial_bg_model_process::get_input_type()
{
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "image" );
    to_return.push_back( "dbbgm_distribution_image" );

    return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbbgm_aerial_bg_model_process::get_output_type()
{  
    vcl_vector<vcl_string > to_return;
    return to_return;
}


//: Execute the process
bool
dbbgm_aerial_bg_model_process::execute()
{
    if ( input_data_.size() != 1 ){
    vcl_cout << "In dbbgm_model_process::execute() - "
             << "not exactly two input images \n";
    return false;
  }
  clear_output();

  // get the process parameters 
  //unused float sigma = 0.0f, init_weight = 0.0f, 
  float init_var = 0.0f;
  unsigned int max_components=0; //unused, window_size=0;
  parameters()->get_value( "-maxcmp" ,  max_components );
  parameters()->get_value( "-initv"  ,  init_var );

  // get image from the storage class
  vidpro1_image_storage_sptr frame_image;
  frame_image.vertical_cast(input_data_[0][0]);

 /* // convert the image to float and smooth it
  vil_image_resource_sptr image_rsc = frame_image->get_image();
  vil_image_view_base_sptr img_sptr = vil_convert_cast(float(), image_rsc->get_view());
  vil_image_view<float> img = *img_sptr;
*/

  //Dongjin Han 7-10-06 convert color image into grey

  // convert the image to float and smooth it
  vil_image_resource_sptr image_sptr1 = frame_image->get_image();

  vil_image_view< unsigned char > image_view = image_sptr1->get_view(0, image_sptr1->ni(), 0, image_sptr1->nj() );
  vil_image_view< unsigned char > image_rsc;
   if( image_view.nplanes() == 3 ) {
    vil_convert_planes_to_grey( image_view , image_rsc );
  }
  else if ( image_view.nplanes() == 1 ) {
    image_rsc = image_view;
  } else {
    vcl_cerr << "Returning false. nplanes(): " << image_rsc.nplanes() << vcl_endl;
    return false;
  }

  vil_image_view<float> img;
  vil_convert_cast(image_rsc,img);


  dbsta_gaussian_indep<float,1> init_gauss(vnl_vector_fixed<float,1>(0.0f),
                                           vnl_vector_fixed<float,1>(init_var) );
  init_gauss.set_data(dbsta_stats_data<float>(1));                                  
  dbsta_mg_window_updater<float> updater(init_gauss, max_components);

  dbbgm_distribution_image_storage_sptr frame_distimg;
  frame_distimg.vertical_cast(input_data_[0][1]);
  

  if(!frame_distimg){
      dbsta_mixture<float> mixture;
      mixture.set_data(dbsta_stats_data<float>(0));
      model_ = new dbbgm_distribution_image<float>(img.ni(),img.nj(),mixture);
      frame_distimg = dbbgm_distribution_image_storage_new(model_);
      frame_distimg->mark_global();
      input_data_[0][1] = frame_distimg;
      }
  else
      {
      model_ = frame_distimg->dist_image();
      // This marks the data as modified so the GUI will redraw
      output_data_[0].push_back(frame_distimg);
      }

  dbbgm_distribution_image<float> *model = static_cast<dbbgm_distribution_image<float>*>(model_.ptr());
  update(*model,img,updater,0.5f);

  // create the output storage classes
  //output_data_[0].push_back( dbbgm_distribution_image_storage_new(model_) );

  return true;  
}
//: Clone the process
bpro1_process*
dbbgm_aerial_bg_model_process::clone() const
{
    return new dbbgm_aerial_bg_model_process(*this);
}

bool
dbbgm_aerial_bg_model_process::finish()
{

 
    return true;
}

