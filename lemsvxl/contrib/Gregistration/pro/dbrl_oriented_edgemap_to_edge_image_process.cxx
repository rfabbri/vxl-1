//This is contrib/Gregistration/pro/dbrl_oriented_edgemap_to_edge_image_process.cxx

//:
// \file

#include "dbrl_oriented_edgemap_to_edge_image_process.h"
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <dbdet/pro/dbdet_edgemap_storage_sptr.h>
#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>

//: Constructor
dbrl_oriented_edgemap_to_edge_image_process::
dbrl_oriented_edgemap_to_edge_image_process()
{
}

//: Destructor
dbrl_oriented_edgemap_to_edge_image_process::
~dbrl_oriented_edgemap_to_edge_image_process()
{
}

//: Clone the process
bpro1_process* dbrl_oriented_edgemap_to_edge_image_process::
clone() const
{
  return new dbrl_oriented_edgemap_to_edge_image_process(*this);
}

//: Return the name of this process
vcl_string dbrl_oriented_edgemap_to_edge_image_process::
name()
{
  return "Oriented Edge Map to Edge Image";
}

//: Return the number of input frame for this process
int dbrl_oriented_edgemap_to_edge_image_process::
input_frames()
{
  return 1;
}

//: Return the number of output frames for this process
int dbrl_oriented_edgemap_to_edge_image_process::
output_frames()
{
  return 1;
}

//: Provide a vector of required input types
vcl_vector<vcl_string> dbrl_oriented_edgemap_to_edge_image_process::
get_input_type()
{
  vcl_vector<vcl_string> to_return;
  to_return.push_back( "edge_map" );
  return to_return;
}

//: Provide a vector of output types
vcl_vector<vcl_string> dbrl_oriented_edgemap_to_edge_image_process::
get_output_type()
{
  vcl_vector<vcl_string> to_return;
  to_return.push_back( "image" );
  return to_return;
}

//: Execute the process
bool dbrl_oriented_edgemap_to_edge_image_process::
execute()
{
  dbdet_edgemap_storage_sptr input_edgemap_storage;
  input_edgemap_storage.vertical_cast(input_data_[0][0]);
  
  dbdet_edgemap_sptr input_edgemap = input_edgemap_storage->get_edgemap();

  unsigned num_edges = input_edgemap->num_edgels();
  vcl_vector<dbdet_edgel*> edgels = input_edgemap->edgels;
  vil_image_view<float> dir_img(input_edgemap->width(), input_edgemap->height(), 3);
  dir_img.fill(-100.0);

  for(unsigned i=0; i<num_edges; ++i)
  {
      dbdet_edgel* current_edgel = edgels[i];
      vgl_point_2d<double> current_pt = current_edgel->pt;
      double current_orientation = current_edgel->tangent;

      dir_img((unsigned int)vcl_floor(current_pt.x()),(unsigned int)vcl_floor(current_pt.y()),0)=current_pt.x();
      dir_img((unsigned int)vcl_floor(current_pt.x()),(unsigned int)vcl_floor(current_pt.y()),1)=current_pt.y();
      dir_img((unsigned int)vcl_floor(current_pt.x()),(unsigned int)vcl_floor(current_pt.y()),2)=current_orientation;
  }
      
  vidpro1_image_storage_sptr output_img_storage = vidpro1_image_storage_new();
  output_img_storage->set_image(vil_new_image_resource_of_view(dir_img ) );
  output_data_[0].push_back(output_img_storage);
  output_img_storage->set_name("Edge Image");

  return true;
}

//: Finish the process
bool dbrl_oriented_edgemap_to_edge_image_process::
finish()
{
  return true;
}
