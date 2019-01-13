// This is basic/bspid/pro/bspid_curvemap_process.cxx

//:
// \file

#include "bspid_curvemap_process.h"
#include <bpro1/bpro1_parameters.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vsol/vsol_digital_curve_2d.h>
#include <bspid/bspid_curve_map.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_convert.h>
#include <vil/vil_math.h>
#include <vil/vil_new.h>

#include <bspid/bspid_compare.h>

//: Constructor
bspid_curvemap_process::bspid_curvemap_process()
{
  if( !parameters()->add( "width" ,  "-width" ,  (int)1024) ||
      !parameters()->add( "height" , "-height" ,  (int)768) ) {
    std::cerr << "ERROR: Adding parameters in " __FILE__ << std::endl;
  }
}


//: Destructor
bspid_curvemap_process::~bspid_curvemap_process()
{
}


//: Clone the process
bpro1_process*
bspid_curvemap_process::clone() const
{
  return new bspid_curvemap_process(*this);
}


//: Return the name of this process
std::string
bspid_curvemap_process::name()
{
  return "Curve Distance Map";
}


//: Return the number of input frame for this process
int
bspid_curvemap_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
bspid_curvemap_process::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
std::vector< std::string > bspid_curvemap_process::get_input_type()
{
  std::vector< std::string > to_return;
  to_return.push_back( "vsol2D" );
  to_return.push_back( "vsol2D" );
  return to_return;
}


//: Provide a vector of output types
std::vector< std::string > bspid_curvemap_process::get_output_type()
{
  std::vector<std::string > to_return;
  to_return.push_back( "image" );
  return to_return;
}


//: Execute the process
bool
bspid_curvemap_process::execute()
{
  if ( input_data_.size() != 1 ){
    std::cout << "In bspid_curvemap_process::execute() - not exactly one"
             << " input images \n";
    return false;
  }
  clear_output();

  // get image from the storage class
  vidpro1_vsol2D_storage_sptr frame_curves;
  frame_curves.vertical_cast(input_data_[0][0]);

  std::vector<vsol_spatial_object_2d_sptr> objects = frame_curves->all_data();
  std::vector<vsol_digital_curve_2d_sptr> curves;
  for ( std::vector<vsol_spatial_object_2d_sptr>::const_iterator itr = objects.begin();
        itr != objects.end();  ++itr )
  {
    if((*itr)->cast_to_curve()){
      vsol_digital_curve_2d_sptr dc = (*itr)->cast_to_curve()->cast_to_digital_curve();
      if(dc)
        curves.push_back(dc);
    }
  }

  int width, height;
  parameters()->get_value( "-width" ,  width );
  parameters()->get_value( "-height" , height );
  bspid_curve_map map(curves, width, height);

  int ni = map.width();
  int nj = map.height();
  vil_image_view<float> fimg(ni, nj);
  for(int i=0; i<ni; ++i)
    for(int j=0; j<nj; ++j)
      fimg(i,j) = map.distance(i+map.x_offset(),j+map.y_offset());

  vil_math_truncate_range(fimg, 0.0f, 255.0f);
  vil_image_view<vxl_byte> image;
  vil_convert_round(fimg, image);

  
  frame_curves.vertical_cast(input_data_[0][1]);
  objects = frame_curves->all_data();
  std::vector<vsol_digital_curve_2d_sptr> curves2;
  for ( std::vector<vsol_spatial_object_2d_sptr>::const_iterator itr = objects.begin();
        itr != objects.end();  ++itr )
  {
    if((*itr)->cast_to_curve()){
      vsol_digital_curve_2d_sptr dc = (*itr)->cast_to_curve()->cast_to_digital_curve();
      if(dc)
        curves2.push_back(dc);
    }
  }
  bspid_compare comp(curves);

  double tp, fp;
  comp.eval(curves2, tp, fp);
  std::cout << "true pos = " << tp << "\tfalse pos = " << fp << std::endl;
 


  // create the output storage class
  vidpro1_image_storage_sptr output_storage = vidpro1_image_storage_new();
  output_storage->set_image(vil_new_image_resource_of_view(image));
  output_data_[0].push_back(output_storage);
  
  return true;
}



bool
bspid_curvemap_process::finish()
{
  return true;
}



