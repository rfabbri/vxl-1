// This is contrib/ntrinh/en292/pro/vidpro1_klt_process.cxx

//:
// \file

#include "vidpro1_klt_process.h"

#include <vil/vil_image_resource_sptr.h>

#include <bpro1/bpro1_parameters.h>

#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>

#include <vsol/vsol_spatial_object_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>

#include<vtol/vtol_vertex_2d.h>
#include <vtol/vtol_vertex_2d_sptr.h>

#include <vgel/vgel_kl.h>
#include <vgel/vgel_multi_view_data_vertex_sptr.h>


//: Constructor
vidpro1_klt_process::vidpro1_klt_process() : bpro1_process(), kl_params()
{
  if( !parameters()->add( "Number of feature points" , "-kl_numpoints" , (int)100) ) 
  {
    vcl_cerr << "ERROR: Adding parameters in vidpro1_klt_process::vidpro1_klt_process()" << vcl_endl;
  }
  else
  {    
  }
}


//: Destructor
vidpro1_klt_process::~vidpro1_klt_process()
{
}


//: Return the name of this process
vcl_string
vidpro1_klt_process::name()
{
  return "KLT Tracking";
}


//: Return the number of input frame for this process
int
vidpro1_klt_process::input_frames()
{
  return 2;
}


//: Return the number of output frames for this process
int
vidpro1_klt_process::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > vidpro1_klt_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > vidpro1_klt_process::get_output_type()
{  
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "vsol2D" );
  to_return.push_back( "vsol2D" );
  return to_return;
}


//: Execute the process
bool
vidpro1_klt_process::execute()
{
  if ( this->input_data_.size() != 2 ){
    vcl_cout << "In vidpro1_klt_process::execute() - not exactly two"
             << " input frames \n";
    return false;
  }
  clear_output();

  // get input images from storage class
  vidpro1_image_storage_sptr image_storage_0, image_storage_1;
  // the order is reversed because frame n-1 is saved in input_data[1] and 
  // fram n (current) is saved in input_data[0]
  image_storage_0.vertical_cast(input_data_[1][0]);
  image_storage_1.vertical_cast(input_data_[0][0]);

  // the actual images
  vil_image_resource_sptr im0 = image_storage_0->get_image();
  vil_image_resource_sptr im1 = image_storage_1->get_image();

  // save all images in a vector
  vcl_vector<vil_image_resource_sptr> images;
  images.push_back(im0);
  images.push_back(im1);

  // use KLT algorithm to track corresponding feature points
  vgel_multi_view_data_vertex_sptr matched_points;
  matched_points=new vgel_multi_view_data<vtol_vertex_2d_sptr>(images.size());

  // set up klt tracker
  this->parameters()->get_value("-kl_numpoints",kl_params.numpoints);
 
  vgel_kl kl_points(this->kl_params);


  /////////////////////////////////////////////////////
  kl_points.match_sequence(images,matched_points);
  /////////////////////////////////////////////////////


  // output tracked featured points to storage class
  // "in-points" - tracked points from previous frame
  vcl_vector< vtol_vertex_2d_sptr > pts_in;
  matched_points->get(0, pts_in);

  vcl_vector< vsol_spatial_object_2d_sptr > klt_points_in;
  for (unsigned int i = 0; i< pts_in.size(); i ++)
  {
    vsol_point_2d_sptr pt = new vsol_point_2d(pts_in.at(i)->x(), pts_in.at(i)->y());
    klt_points_in.push_back(pt->cast_to_spatial_object());
  }

  // create the output storage class for "in-points"
  vidpro1_vsol2D_storage_sptr vsol_in = vidpro1_vsol2D_storage_new();
  vsol_in->add_objects(klt_points_in, "KLT point");
  output_data_[0].push_back(vsol_in);


  // "out-points" - tracked points from current frame
  vcl_vector< vtol_vertex_2d_sptr > pts_out;
  matched_points->get(1, pts_out);

  vcl_vector< vsol_spatial_object_2d_sptr > klt_points_out;
  for (unsigned int i = 0; i< pts_out.size(); i ++)
  {
    vsol_point_2d_sptr pt = new vsol_point_2d(pts_out.at(i)->x(), pts_out.at(i)->y());
    klt_points_out.push_back(pt->cast_to_spatial_object());
  }

  // create the output storage class and save data to it
  vidpro1_vsol2D_storage_sptr vsol_out = vidpro1_vsol2D_storage_new();
  vsol_out->add_objects(klt_points_out, "KLT point");
  output_data_[0].push_back(vsol_out);

  return true;
}


//: Clone the process
bpro1_process*
vidpro1_klt_process::clone() const
{
  return new vidpro1_klt_process(*this);
}
    
bool
vidpro1_klt_process::finish()
{
  return true;
}




