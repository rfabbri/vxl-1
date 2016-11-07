// This is brl/vidpro/process/vidpro_kl_process.cxx

//:
// \file

#include "vidpro_kl_process.h"
#include <vidpro/storage/vidpro_image_storage.h>
#include <vidpro/storage/vidpro_image_storage_sptr.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_math.h>
#include<bpro/bpro_parameters.h>
#include<vsol/vsol_point_2d.h>
#include<vidpro/storage/vidpro_vsol2D_storage_sptr.h>
#include<vidpro/storage/vidpro_vsol2D_storage.h>
#include<vtol/vtol_vertex_2d.h>
#include<vtol/vtol_vertex_2d_sptr.h>

//: Constructor
vidpro_kl_process::vidpro_kl_process(void): bpro_process(),kl_params(),kl_points(NULL)
{
    
    if( !parameters()->add( "No of Points" , "-klnumpoints" , (int)100) ||
      !parameters()->add( "Search Range" , "-klrange" , (int) 15 ) ) 
     {
        vcl_cerr << "ERROR: Adding parameters in vidpro_kl_process::vidpro_kl_process()" << vcl_endl;
     }
    else
    {
        
    }
}

//: Destructor
vidpro_kl_process::~vidpro_kl_process()
{
    delete(kl_points);
}


//: Return the name of this process
vcl_string
vidpro_kl_process::name()
{
  return "KL tracking";
}


//: Return the number of input frame for this process
int
vidpro_kl_process::input_frames()
{
  return 2;
}


//: Return the number of output frames for this process
int
vidpro_kl_process::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > vidpro_kl_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > vidpro_kl_process::get_output_type()
{  
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "vsol2D" );
  return to_return;
}


//: Execute the process
bool
vidpro_kl_process::execute()
{
  if ( input_data_.size() != 2 ){
    vcl_cout << "In vidpro_kl_process::execute() - "
             << "not exactly two input images \n";
    return false;
  }
  clear_output();

  // get image from the storage class
  vidpro_image_storage_sptr frame_image1, frame_image2;
  frame_image1.vertical_cast(input_data_[1][0]);
  frame_image2.vertical_cast(input_data_[0][0]);

  vil_image_resource_sptr image1_sptr = frame_image1->get_image();
  vil_image_resource_sptr image2_sptr = frame_image2->get_image();
  
  parameters()->get_value("-klnumpoints",kl_params.numpoints);
  parameters()->get_value("-klrange",kl_params.search_range);

  kl_points=new vgel_kl(kl_params);
  vcl_vector<vil_image_resource_sptr> images;
  images.push_back(image1_sptr);
  images.push_back(image2_sptr);

  vgel_multi_view_data_vertex_sptr matched_points;
  matched_points=new vgel_multi_view_data<vtol_vertex_2d_sptr>(2);
  kl_points->match_sequence(images,matched_points);
  vcl_vector<vsol_point_2d_sptr> points ;
  vcl_vector<vtol_vertex_2d_sptr> pts;
  matched_points->get( 1,pts);

  vcl_vector<vtol_vertex_2d_sptr> prevpts;
  matched_points->get( 0,prevpts);

  for(unsigned int i=0;i<pts.size() && i<prevpts.size();i++)
  {
        vsol_point_2d_sptr p=new vsol_point_2d(pts[i]->x(),pts[i]->y());
        vsol_point_2d_sptr prevp=new vsol_point_2d(prevpts[i]->x(),prevpts[i]->y());

        //if(p->distance(prevp)>5)
        points.push_back(p);
        points.push_back(prevp);
  }
  vcl_vector< vsol_spatial_object_2d_sptr > klt_points;
  for( unsigned int i = 0 ; i < points.size() ; i++ ) {
    vsol_spatial_object_2d_sptr point = points[i]->cast_to_spatial_object();
    klt_points.push_back(point);
  }

  // create the output storage class
  vidpro_vsol2D_storage_sptr output_vsol = vidpro_vsol2D_storage_new();
  output_vsol->add_objects(klt_points, "KL feature point");
  output_data_[0].push_back(output_vsol);
  return true;  
}
//: Clone the process
bpro_process*
vidpro_kl_process::clone() const
{
  return new vidpro_kl_process(*this);
}
    
bool
vidpro_kl_process::finish()
{
  return true;
}




