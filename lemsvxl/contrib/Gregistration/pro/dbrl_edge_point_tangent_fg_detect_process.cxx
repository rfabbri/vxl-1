// This is pro/dbrl_edge_point_tangent_fg_detect_process.cxx

//:
// \file

#include "dbrl_edge_point_tangent_fg_detect_process.h"
#include <dbbgm/pro/dbbgm_distribution_image_storage.h>
#include <bpro1/bpro1_parameters.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_convert.h>
#include <vil/vil_new.h>
#include <vil/algo/vil_gauss_filter.h>
#include <dbbgm/dbbgm_distribution_image.h>
#include <dbbgm/dbbgm_detect.h>

#include <dbsta/dbsta_gaussian_full.h>
#include <dbsta/dbsta_gaussian_sphere.h>
#include <dbsta/dbsta_detector_gaussian.h>
#include <dbsta/dbsta_detector_mixture.h>
#include <dbsta/dbsta_basic_functors.h>
#include <dbsta/dbsta_functor_sptr.h>
#include <dbsta/algo/dbsta_gaussian_stats.h>
#include <dbsta/algo/dbsta_bayes_functor.h>
#include <brip/brip_vil_float_ops.h>
#include <georegister/dbrl_id_point_2d_sptr.h>
#include <georegister/dbrl_id_point_2d.h>
#include <pro/dbrl_id_point_2d_storage_sptr.h>
#include <pro/dbrl_id_point_2d_storage.h>


//: Constructor
dbrl_edge_point_tangent_fg_detect_process::dbrl_edge_point_tangent_fg_detect_process()
{
if(!parameters()->add( "Distance Theshold" ,    "-dist" ,    2.5f        ) ||
   !parameters()->add( "Minimum Weight Threshold" ,    "-minweightthresh" ,    0.3f        )||
   !parameters()->add( "Radius of Uncertainity" ,    "-rad" ,    (int)2       ) ||
   !parameters()->add( "Decimate by" ,    "-decimateby" ,    2        ) ){
    vcl_cerr << "ERROR: Adding parameters in " __FILE__<< vcl_endl;
  }
}


//: Destructor
dbrl_edge_point_tangent_fg_detect_process::~dbrl_edge_point_tangent_fg_detect_process()
{
}


//: Clone the process
bpro1_process* 
dbrl_edge_point_tangent_fg_detect_process::clone() const
{
  return new dbrl_edge_point_tangent_fg_detect_process(*this);
}


//: Return the name of this process
vcl_string
dbrl_edge_point_tangent_fg_detect_process::name()
{
  return "Subpixel edge point tangent Foreground Detection";
}


//: Return the number of input frame for this process
int
dbrl_edge_point_tangent_fg_detect_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
dbrl_edge_point_tangent_fg_detect_process::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbrl_edge_point_tangent_fg_detect_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  to_return.push_back( "dbbgm_distribution_image" );

  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbrl_edge_point_tangent_fg_detect_process::get_output_type()
{  
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "image" );
  to_return.push_back( "dbrl_id_point_2d" );
  //to_return.push_back( "image" );
  return to_return;
}


//: Execute the process
bool
dbrl_edge_point_tangent_fg_detect_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dbrl_edge_point_tangent_fg_detect_process::execute() - "
             << "not exactly two input images \n";
    return false;
  }
  clear_output();
  
  // get the process parameters 
  float minweightthresh = 0.2f, dist = 0.0f;
    static int rad=2;

  parameters()->get_value( "-dist" ,   dist);
  parameters()->get_value( "-minweightthresh" ,   minweightthresh);
    parameters()->get_value( "-rad" ,   rad);


  // get image from the storage class
  vidpro1_image_storage_sptr frame_image;
  frame_image.vertical_cast(input_data_[0][0]);

  // convert the image to float and smooth it
  vil_image_resource_sptr image_rsc = frame_image->get_image();
  vil_image_view_base_sptr img_sptr = vil_convert_cast(float(), image_rsc->get_view());
  vil_image_view<float> img = *img_sptr;
  
  // get the background model
  dbbgm_distribution_image_storage_sptr frame_model;
  frame_model.vertical_cast(input_data_[0][1]);
  dbbgm_distribution_image<float> *model = 
    dynamic_cast<dbbgm_distribution_image<float>*>(frame_model->dist_image().ptr());
  if(!model)
    return false;
    
  dbsta_top_weight_detector<float> detector(new dbsta_g_mdist_detector<float>(dist), minweightthresh);

  vil_image_view<float> floatimg(img.ni(),img.nj(),img.nplanes());
  floatimg=img;
  
   vil_image_view<bool> fg;
  if(img.nplanes()==3)
      {
      vil_image_view<float> currplane=vil_plane(img,2);
      detect<float>(*model,currplane,detector,vnl_vector_fixed<float,1>(-1.0f),rad,fg);
      }


  //fg.deep_copy(model->detect(img,detector,vnl_vector_fixed<float,1>(-1.0f)));
  vcl_vector<dbrl_id_point_2d_sptr> pointids;
  static int decimateby=1;
  parameters()->get_value( "-decimateby" ,   decimateby);
  int cnt=1;
  for (unsigned i=0;i<fg.ni();i++)
      for(unsigned j=0;j<fg.nj();j++)
          {
          if(fg(i,j))
              {
              floatimg(i,j,0)=img(i,j,0);
              floatimg(i,j,1)=img(i,j,1);
              floatimg(i,j,2)=img(i,j,2);
              if(cnt>=decimateby)
                  {
                   dbrl_id_point_2d_sptr point=new dbrl_id_point_2d(floatimg(i,j,0),floatimg(i,j,1),floatimg(i,j,2),pointids.size()+1);
                   pointids.push_back(point);
                   cnt=1;
                  }
              else
                  cnt++;
              }
          else 
              {
              floatimg(i,j,0)=-100;
              floatimg(i,j,1)=-100;
              floatimg(i,j,2)=-100;
              }
          }

      vcl_cout<<"No of Foreground edges or points "<<pointids.size()<<"\n";
  vidpro1_image_storage_sptr output_storage = vidpro1_image_storage_new();
  output_storage->set_image(vil_new_image_resource_of_view( floatimg));
  output_data_[0].push_back(output_storage);  
  
  dbrl_id_point_2d_storage_sptr id_point_2d_storage=  dbrl_id_point_2d_storage_new();
  id_point_2d_storage->set_id_points(pointids);
  output_data_[0].push_back(id_point_2d_storage); 
  /*vidpro1_image_storage_sptr output_storage1 = vidpro1_image_storage_new();
  output_storage1->set_image(vil_new_image_resource_of_view( brip_vil_float_ops::convert_to_byte(fg)));
  output_data_[0].push_back(output_storage1);  */
  return true;  
}
    


bool
dbrl_edge_point_tangent_fg_detect_process::finish()
{
  return true;
}




