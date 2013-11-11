// This is brl/vidpro1/process/vidpro1_frame_diff_process.cxx

//:
// \file

#include "dbbgm_process.h"
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_math.h>
#include <brip/brip_vil_float_ops.h>


//: Constructor
dbbgm_process::dbbgm_process():bgM(0)
{
    frame=0;
}


//: Destructor
dbbgm_process::~dbbgm_process()
{
    delete(bgM);
}


//: Clone the process
bpro1_process* 
dbbgm_process::clone() const
{
  return new dbbgm_process(*this);
}


//: Return the name of this process
vcl_string
dbbgm_process::name()
{
  return "BG model";
}


//: Return the number of input frame for this process
int
dbbgm_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
dbbgm_process::output_frames()
{
  return 4;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbbgm_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );

  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbbgm_process::get_output_type()
{  
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "image" );
  to_return.push_back( "image" );
  to_return.push_back( "image" );
  to_return.push_back( "image" );


  return to_return;
}


//: Execute the process
bool
dbbgm_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dbbgm_process::execute() - "
             << "not exactly two input images \n";
    return false;
  }
  clear_output();

  // get image from the storage class
  vidpro1_image_storage_sptr frame_image1, frame_image2;
  frame_image1.vertical_cast(input_data_[0][0]);

  vil_image_resource_sptr image1_sptr = frame_image1->get_image();

  vil_image_view<unsigned char> image1, image2, grey_img1, grey_img2;
  image1 =image1_sptr->get_view();

  // Convert to greyscale images if needed
  if( image1.nplanes() == 3 ) {
    vil_convert_planes_to_grey( image1 , grey_img1 );
  }
  else if ( image1.nplanes() == 1 ) {
    grey_img1 = image1;
  } else {
    vcl_cerr << "Returning false. image1.nplanes(): " << image1.nplanes() << vcl_endl;
    return false;
  }

  if(frame==0)
     bgM= new dbbgm_bgmodel<float>(grey_img1);
  else
     bgM->updateModel(grey_img1);

  

  // create the output storage class
  vidpro1_image_storage_sptr output_storage = vidpro1_image_storage_new();
  output_storage->set_image(vil_new_image_resource_of_view(bgM->writebackground(0)));
  output_data_[0].push_back(output_storage);

  vidpro1_image_storage_sptr output_storage3 = vidpro1_image_storage_new();
  output_storage3->set_image(vil_new_image_resource_of_view(bgM->writebackground(1)));
  output_data_[0].push_back(output_storage3);

  vidpro1_image_storage_sptr output_storage4 = vidpro1_image_storage_new();
  output_storage4->set_image(vil_new_image_resource_of_view(bgM->writebackground(2)));
  output_data_[0].push_back(output_storage4);

  vidpro1_image_storage_sptr output_storage2 = vidpro1_image_storage_new();
  output_storage2->set_image(vil_new_image_resource_of_view(bgM->writeforeground()));
  output_data_[0].push_back(output_storage2);

  frame++;
  return true;  
}
    


bool
dbbgm_process::finish()
{
  return true;
}




