// This is brcv/seg/dbdet/pro/dbdet_convert_edgemap_to_image_process.cxx

//:
// \file

#include "dbdet_convert_edgemap_to_image_process.h"
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <dbdet/pro/dbdet_sel_storage.h>
#include <dbdet/pro/dbdet_sel_storage_sptr.h>
#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/pro/dbdet_edgemap_storage_sptr.h>
#include <dbdet/algo/dbdet_convert_edgemap.h>

//: Constructor
dbdet_convert_edgemap_to_image_process::
dbdet_convert_edgemap_to_image_process()
{
  if( 
      !parameters()->add( "Linked input?", "-linked",  (bool)true ) ||
      !parameters()->add( "Edge intensity (0-255)", "-edge_intensity",  int(255) )
    )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}

//: Destructor
dbdet_convert_edgemap_to_image_process::
~dbdet_convert_edgemap_to_image_process()
{
}


//: Clone the process
bpro1_process* dbdet_convert_edgemap_to_image_process::
clone() const
{
  return new dbdet_convert_edgemap_to_image_process(*this);
}


//: Return the name of this process
vcl_string dbdet_convert_edgemap_to_image_process::
name()
{
  return "Convert Edgemap to Image";
}


//: Return the number of input frame for this process
int dbdet_convert_edgemap_to_image_process::
input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int dbdet_convert_edgemap_to_image_process::
output_frames()
{
  return 1;
}

//: Provide a vector of required input types
vcl_vector< vcl_string > dbdet_convert_edgemap_to_image_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;

  bool linked = false;

  parameters()->get_value( "-linked" , linked);

  if (linked)
    to_return.push_back( "sel" ); // the linked edge map
  else
    to_return.push_back( "edge_map" ); // the un-linked subpixel edge map


  // to get the dimension of the output image. May change in the future.
  to_return.push_back( "image" ); 
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbdet_convert_edgemap_to_image_process::
get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "image" );
  return to_return;
}


//: Execute the process
bool
dbdet_convert_edgemap_to_image_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dbdet_convert_edgemap_to_image_process::execute() - not exactly one"
             << " input images \n";
    return false;
  }
  clear_output();


  // Parameters ---------------------------------------------------------------
  int edge_intensity = 255;
  this->parameters()->get_value("-edge_intensity", edge_intensity);


  // Input storage -------------------------------------------------------------

  // image storage
  vidpro1_image_storage_sptr input_image;
  input_image.vertical_cast(input_data_[0][1]);

  // compute bounding box from input image
  vgl_box_2d<int > bbox;
  bbox.add(vgl_point_2d<int > (0, 0));
  bbox.add(vgl_point_2d<int > (input_image->get_image()->ni()-1, 
    input_image->get_image()->nj()-1) );

  // Initialize the new image
  vil_image_view<vxl_byte > edge_image;

  bool linked = false;
  parameters()->get_value( "-linked" , linked);

  if (linked) {
    dbdet_sel_storage_sptr input_sel;
    input_sel.vertical_cast(input_data_[0][0]);
    dbdet_convert_edgemap_to_image(input_sel->CFG(), edge_image, bbox);
  } else {
    dbdet_edgemap_storage_sptr input_edgels;
    input_edgels.vertical_cast(input_data_[0][0]);
    dbdet_convert_edgemap_to_image(*(input_edgels->get_edgemap()), edge_image, bbox);
  }

  // Output storage

  // create the output storage class
  vidpro1_image_storage_sptr output_image = vidpro1_image_storage_new();
  output_image->set_image(vil_new_image_resource_of_view(edge_image));
  output_data_[0].push_back(output_image);

  return true;
}

bool
dbdet_convert_edgemap_to_image_process::finish()
{
  return true;
}

