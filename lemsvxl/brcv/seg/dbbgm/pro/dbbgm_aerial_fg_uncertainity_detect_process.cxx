

// This is brcv/seg/dbbgm/pro/dbbgm_aerial_fg_uncertainity_detect_process.cxx

//:
// \file

#include "dbbgm_aerial_fg_uncertainity_detect_process.h"
#include <dbbgm/pro/dbbgm_distribution_image_storage.h>
#include <bpro1/bpro1_parameters.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_convert.h>
#include <vil/vil_new.h>
#include <vil/algo/vil_gauss_filter.h>
#include <dbbgm/dbbgm_distribution_image.h>
#include <dbsta/dbsta_gaussian_full.h>
#include <dbsta/dbsta_gaussian_sphere.h>
#include <dbsta/dbsta_detector_gaussian.h>
#include <dbsta/dbsta_detector_mixture.h>
#include <dbsta/dbsta_basic_functors.h>
#include <dbsta/dbsta_functor_sptr.h>
#include <dbsta/algo/dbsta_gaussian_stats.h>
#include <dbsta/algo/dbsta_bayes_functor.h>
#include <brip/brip_vil_float_ops.h>
#include <vul/vul_sprintf.h>
#include <vil/vil_save.h>
#include <dbbgm/dbbgm_detect.h>
//: Constructor
dbbgm_aerial_fg_uncertainity_detect_process::dbbgm_aerial_fg_uncertainity_detect_process()
{
  if(!parameters()->add( "Distance Theshold" ,    "-dist" ,    2.5f        ) ||
     !parameters()->add( "Minimum Weight Threshold" ,    "-minweightthresh" ,    0.3f        ) ||
     !parameters()->add( "Radius of Uncertainity" ,    "-rad" ,    (int)2       )||
         !parameters()->add( "Use Mask" ,    "-ismask" ,    (bool)false       ) ){
    vcl_cerr << "ERROR: Adding parameters in " __FILE__<< vcl_endl;
  }
}


//: Destructor
dbbgm_aerial_fg_uncertainity_detect_process::~dbbgm_aerial_fg_uncertainity_detect_process()
{
}


//: Clone the process
bpro1_process* 
dbbgm_aerial_fg_uncertainity_detect_process::clone() const
{
  return new dbbgm_aerial_fg_uncertainity_detect_process(*this);
}


//: Return the name of this process
vcl_string
dbbgm_aerial_fg_uncertainity_detect_process::name()
{
  return "Aerial Foreground Detection Uncertainity";
}


//: Return the number of input frame for this process
int
dbbgm_aerial_fg_uncertainity_detect_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
dbbgm_aerial_fg_uncertainity_detect_process::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbbgm_aerial_fg_uncertainity_detect_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  to_return.push_back( "dbbgm_distribution_image" );

  bool ismask=false;
  parameters()->get_value( "-ismask" ,   ismask);
  if(ismask)
  {
        to_return.push_back( "image" );
  }
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbbgm_aerial_fg_uncertainity_detect_process::get_output_type()
{  
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "image" );
  
  return to_return;
}


//: Execute the process
bool
dbbgm_aerial_fg_uncertainity_detect_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dbbgm_aerial_fg_uncertainity_detect_process::execute() - "
             << "not exactly two input images \n";
    return false;
  }
  clear_output();
  
  // get the process parameters 
  float minweightthresh = 0.2f, dist = 0.0f;
  static int rad=2;
  bool ismask=false;
  parameters()->get_value( "-dist" ,   dist);
  parameters()->get_value( "-minweightthresh" ,   minweightthresh);
  parameters()->get_value( "-rad" ,   rad);
  parameters()->get_value( "-ismask" ,   ismask);

  // get image from the storage class
  vidpro1_image_storage_sptr frame_image;

  frame_image.vertical_cast(input_data_[0][0]);

  vidpro1_image_storage_sptr mask_image_storage;
  vil_image_view<bool> mask_img;
  if(ismask)
  {
          mask_image_storage.vertical_cast(input_data_[0][2]);
          vil_image_resource_sptr mask_image_sptr = mask_image_storage->get_image();
          mask_img=brip_vil_float_ops::convert_to_bool(mask_image_sptr->get_view(0,mask_image_sptr->ni(),0,mask_image_sptr->nj()));
  }
  
  //Dongjin Han 7-10-06 convert color image into grey
  // convert the image to float and smooth it
  vil_image_resource_sptr image_sptr1 = frame_image->get_image();

  
  //vil_image_view<float> img=brip_vil_float_ops::convert_to_float(*image_rsc);

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

///  // convert the image to float and smooth it
///  vil_image_resource_sptr image_rsc = frame_image->get_image();

  vil_image_view<float> img=brip_vil_float_ops::convert_to_float(image_rsc);
  
  // get the background model
  dbbgm_distribution_image_storage_sptr frame_model;
  frame_model.vertical_cast(input_data_[0][1]);
  dbbgm_distribution_image<float> *model = 
    dynamic_cast<dbbgm_distribution_image<float>*>(frame_model->dist_image().ptr());
  if(!model)
    return false;

  dbsta_top_weight_detector<float> detector(new dbsta_g_mdist_detector<float>(dist), minweightthresh);
  vil_image_view<bool> fg(img.ni(),img.nj(),1);
  vil_image_view<float> fgbyteimg(img.ni(),img.nj(),1);
  fg.fill(true);
  if(ismask)
        detect_masked<float>(*model,img,detector,vnl_vector_fixed<float,1>(0.5f),rad,mask_img,fg);
  else
        detect<float>(*model,img,detector,vnl_vector_fixed<float,1>(0.5f),rad,fg);
  
  for (unsigned i=0;i<fg.ni();i++)
      for(unsigned j=0;j<fg.nj();j++)
          if(fg(i,j))fgbyteimg(i,j)=1.0;else fgbyteimg(i,j)=0.0;

  vidpro1_image_storage_sptr output_storage = vidpro1_image_storage_new();
  output_storage->set_image(vil_new_image_resource_of_view(brip_vil_float_ops::convert_to_byte(fgbyteimg,0,1.0) ));
  output_data_[0].push_back(output_storage);  


  
  
  
  return true;  
}
    


bool
dbbgm_aerial_fg_uncertainity_detect_process::finish()
{
  return true;
}




