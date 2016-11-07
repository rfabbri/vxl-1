// This is brcv/seg/dbdet/pro/dbdet_edgemap_from_edge_image_process.cxx

//:
// \file

#include "dbdet_edgemap_from_edge_image_process.h"


#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/pro/dbdet_edgemap_storage_sptr.h>

#include <vcl_vector.h>
#include <vcl_string.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>

#include <dbdet/edge/dbdet_edgemap_sptr.h>

//: Constructor
dbdet_edgemap_from_edge_image_process::dbdet_edgemap_from_edge_image_process()
{
  if( !parameters()->add( "Edge Value", "-edge_val"   , (unsigned) 0 ))
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}

//: Destructor
dbdet_edgemap_from_edge_image_process::~dbdet_edgemap_from_edge_image_process()
{
}


//: Clone the process
bpro1_process*
dbdet_edgemap_from_edge_image_process::clone() const
{
  return new dbdet_edgemap_from_edge_image_process(*this);
}


//: Return the name of this process
vcl_string
dbdet_edgemap_from_edge_image_process::name()
{
  return "Edge Image to Edge Map";
}


//: Return the number of input frame for this process
int
dbdet_edgemap_from_edge_image_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
dbdet_edgemap_from_edge_image_process::output_frames()
{
  return 1;
}

//: Provide a vector of required input types
vcl_vector< vcl_string > dbdet_edgemap_from_edge_image_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbdet_edgemap_from_edge_image_process::get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "edge_map" );
  return to_return;
}


//: Execute the process
bool
dbdet_edgemap_from_edge_image_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dbdet_edgemap_from_edge_image_process::execute() - not exactly one"
             << " input images \n";
    return false;
  }
  clear_output();

  // get image from the storage class
  vidpro1_image_storage_sptr frame_image;
  frame_image.vertical_cast(input_data_[0][0]);
  vil_image_resource_sptr image_sptr = frame_image->get_image();

  //convert to grayscale
  vil_image_view<vxl_byte> image_view = image_sptr->get_view(0, image_sptr->ni(), 0, image_sptr->nj() );
  vil_image_view<vxl_byte> greyscale_view;

  if(image_view.nplanes() == 3){
    vil_convert_planes_to_grey(image_view, greyscale_view );
  }
  else if (image_view.nplanes() == 1){
    greyscale_view = image_view;
  }

  //get the parameters
  unsigned edge_val;
  parameters()->get_value( "-edge_val", edge_val);

  //create a new edgemap
  dbdet_edgemap_sptr edge_map = new dbdet_edgemap(greyscale_view.ni(), greyscale_view.nj());

  for (unsigned x = 0; x < greyscale_view.ni(); x++){
    for (unsigned y = 0; y < greyscale_view.nj(); y++)
    {
      if (greyscale_view(x,y) == edge_val){ //edge pixel found
        dbdet_edgel* e = new dbdet_edgel(vgl_point_2d<double>(x,y), 0.0, 2.0);
        e->gpt = vgl_point_2d<int>(x,y);
        edge_map->insert(e);
      }
    }
  }

  // create the output storage class
  dbdet_edgemap_storage_sptr output_edgemap = dbdet_edgemap_storage_new();
  output_edgemap->set_edgemap(edge_map);
  output_data_[0].push_back(output_edgemap);

  vcl_cout << "#edgels = " << edge_map->num_edgels() << vcl_endl;

  vcl_cout.flush();

  return true;
}

bool
dbdet_edgemap_from_edge_image_process::finish()
{
  return true;
}

