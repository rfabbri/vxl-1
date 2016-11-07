// This is contrib/mleotta/modrec/pro/modrec_evaluate_process.cxx

//:
// \file

#include "modrec_evaluate_process.h"
#include <bpro1/bpro1_parameters.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_convert.h>
#include <vil/vil_new.h>



//: Constructor
modrec_evaluate_process::modrec_evaluate_process()
{
  if( !parameters()->add( "Number of classes" ,   "-num_classes" ,   (unsigned)3 ) ){
    vcl_cerr << "ERROR: Adding parameters in " __FILE__<< vcl_endl;
  }
}


//: Destructor
modrec_evaluate_process::~modrec_evaluate_process()
{
}


//: Clone the process
bpro1_process* 
modrec_evaluate_process::clone() const
{
  return new modrec_evaluate_process(*this);
}


//: Return the name of this process
vcl_string
modrec_evaluate_process::name()
{
  return "Evaluate Segmentation";
}


//: Return the number of input frame for this process
int
modrec_evaluate_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
modrec_evaluate_process::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > modrec_evaluate_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  to_return.push_back( "image" );

  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > modrec_evaluate_process::get_output_type()
{
  vcl_vector<vcl_string > to_return;
  return to_return;
}


//: Execute the process
bool
modrec_evaluate_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In modrec_evaluate_process::execute() - "
             << "not exactly one input frame \n";
    return false;
  }
  clear_output();

  unsigned int num_classes=0;
  parameters()->get_value( "-num_classes" , num_classes);

  if(results_.size() == 0){
    results_.resize(num_classes,num_classes);
    results_.fill(0);
  }

  // get image from the storage class
  vidpro1_image_storage_sptr frame_image;
  frame_image.vertical_cast(input_data_[0][0]);

  vil_image_view<vxl_byte> image1;
  if(frame_image){
    vil_image_resource_sptr image_rsc = frame_image->get_image();
    if(image_rsc->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
      image1 = image_rsc->get_view();
    else
      return false;
  }
  else
    return false;

  frame_image.vertical_cast(input_data_[0][1]);
  vil_image_view<vxl_byte> image2;
  if(frame_image){
    vil_image_resource_sptr image_rsc = frame_image->get_image();
    if(image_rsc->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
      image2 = image_rsc->get_view();
    else
      return false;
  }
  else
    return false;

  assert(image1.ni() == image2.ni());
  assert(image1.nj() == image2.nj());
  assert(image1.nplanes() == image2.nplanes());
  assert(image1.nplanes() == 1);


  for(unsigned int j=0; j<image1.nj(); ++j){
    for(unsigned int i=0; i<image1.ni(); ++i){
      assert(image1(i,j) < num_classes);
      assert(image2(i,j) < num_classes);
      ++results_(image1(i,j),image2(i,j));
    }
  }


  return true;
}



bool
modrec_evaluate_process::finish()
{

  vcl_cout << results_ << vcl_endl;

  return true;
}




