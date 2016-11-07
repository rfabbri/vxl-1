// This is contrib/ntrinh/en292/pro/vidpro1_homog_2d_preimage_process.cxx

//:
// \file


#include "vidpro1_homog_2d_preimage_process.h"

#include <vil/vil_new.h>
#include <vil/vil_convert.h>

#include <vnl/vnl_det.h>

#include <brip/brip_vil_float_ops.h>

#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>


#include <en292/pro/vidpro1_homog_2d_storage.h>

//: Constructor
vidpro1_homog_2d_preimage_process::
vidpro1_homog_2d_preimage_process() : bpro1_process()
{
  this->H_inv_.set_identity();
}


//: Destructor
vidpro1_homog_2d_preimage_process::
~vidpro1_homog_2d_preimage_process()
{
}


//: Return the name of this process
vcl_string vidpro1_homog_2d_preimage_process::
name()
{
  return "Homography 2D Pre-image";
}


//: Return the number of input frame for this process
int vidpro1_homog_2d_preimage_process::
input_frames()
{
  return 1;
}



//: Return the number of output frames for this process
int vidpro1_homog_2d_preimage_process::
output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > vidpro1_homog_2d_preimage_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  to_return.push_back( "homog_2d" );
  return to_return;
}

//: Provide a vector of output types
vcl_vector< vcl_string > vidpro1_homog_2d_preimage_process::
get_output_type()
{  
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "image" );
  return to_return;
}


//: Execute the process
bool vidpro1_homog_2d_preimage_process::
execute()
{
  vcl_cout << "\n Compute pre-image of current frame using homography matrix" << vcl_endl;
  if ( this->input_data_.size() != 1 )
  {
    vcl_cout << "In vidpro1_homog_2d_preimage_process::execute() - not exactly one"
             << " input frames \n";
    return false;
  }
  clear_output();

  // get input from storage classes
  vidpro1_homog_2d_storage_sptr homog_2d_storage;
  homog_2d_storage.vertical_cast(input_data_[0][1]);
  
  vgl_h_matrix_2d< double > H = homog_2d_storage->H();

  vcl_cout << "Homography matrix H = \n" << H << vcl_endl;
  vcl_cout << "Det(H) = " << vnl_det(H.get_matrix()) << vcl_endl;
  
  vgl_h_matrix_2d< double > H_inv = H.get_inverse();

  H_inv = this->H_inv_ * H_inv;
  this->H_inv_ = H_inv;
  
  // Now we've got the homography matrix, apply it to the image of the current frame.
  vcl_cout << "Apply homography to the current image \n";
  
  // get input image from storage class
  vidpro1_image_storage_sptr im_storage;
  im_storage.vertical_cast(input_data_[0][0]);
  vil_image_resource_sptr im_resource = im_storage->get_image();
  vil_image_view< vxl_byte > im_view(im_resource->get_view());
  // just for now
  // convert to grey image
  vil_image_view< float > im_view_float;
  
  if (im_view.nplanes() == 3)
    vil_convert_planes_to_grey< vxl_byte, float >(im_view, im_view_float);
  else
    vil_convert_cast< vxl_byte, float >(im_view, im_view_float);
  
  vil_image_view< float > return_im_view;
  return_im_view.deep_copy(im_view_float);

  /////////////////////////////////////////////////////////////////////////
  // homography using the inverse matrix
  if (! brip_vil_float_ops::homography(im_view_float, H_inv, return_im_view, true, 0.0f))
  {
    vcl_cout << "Homography transform failed" << vcl_endl;
    return false;
  }
  ///////////////////////////////////////////////////////////////////////////
  
  vcl_cout << "Homography succeeded " << vcl_endl;
  return_im_view.print(vcl_cout);
 
  // create the output storage class
  vidpro1_image_storage_sptr output_storage = vidpro1_image_storage_new();
  vil_image_resource_sptr return_im_resource = vil_new_image_resource_of_view(return_im_view);
  output_storage->set_image(return_im_resource);  
  output_data_[0].push_back(output_storage);

  return true;
}


//: Clone the process
bpro1_process*
vidpro1_homog_2d_preimage_process::clone() const
{
  vidpro1_homog_2d_preimage_process* to_return = new vidpro1_homog_2d_preimage_process(*this);
  to_return->H_inv_.set_identity();
  return (bpro1_process*)to_return;
  //   return new vidpro1_homog_2d_preimage_process(*this);
}
    
bool
vidpro1_homog_2d_preimage_process::finish()
{
  return true;
}




