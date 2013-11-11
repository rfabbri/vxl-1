// This is en292/pro/vidpro1_homog_2d_transform_image_process.cxx

//:
// \file

#include "vidpro1_homog_2d_transform_image_process.h"

#include <vil/vil_new.h>
#include <vil/vil_load.h>
#include <vil/vil_convert.h>

#include <vnl/vnl_det.h>

#include <brip/brip_vil_float_ops.h>

#include <bpro1/bpro1_parameters.h>

#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>

#include <en292/pro/vidpro1_homog_2d_storage.h>

//: Constructor
vidpro1_homog_2d_transform_image_process::
vidpro1_homog_2d_transform_image_process() : bpro1_process()
{
  this->H_.set_identity();
  /*if( !parameters()->add( "Image file <filename...>" , "-image_filename" , bpro1_filepath("","*") ))
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }*/

  if( !parameters()->add( "Video file <filename...>" , "-video_filename", bpro1_filepath("","*") ) )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
  this->my_movie_ = 0;
  this->frame_num_ = 0;
}


//: Destructor
vidpro1_homog_2d_transform_image_process::
~vidpro1_homog_2d_transform_image_process()
{
}


//: Return the name of this process
vcl_string vidpro1_homog_2d_transform_image_process::
name()
{
  return "Image Homography Transformation";
}


//: Return the number of input frame for this process
int vidpro1_homog_2d_transform_image_process::
input_frames()
{
  return 1;
}



//: Return the number of output frames for this process
int vidpro1_homog_2d_transform_image_process::
output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > vidpro1_homog_2d_transform_image_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "homog_2d" );
  to_return.push_back( "image" ); // image whose size will be the size of the new image
  return to_return;
}

//: Provide a vector of output types
vcl_vector< vcl_string > vidpro1_homog_2d_transform_image_process::get_output_type()
{  
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "image" );
  return to_return;
}


//: Execute the process
bool
vidpro1_homog_2d_transform_image_process::execute()
{
  vcl_cout << "\n Compute homography transformation of an image" << vcl_endl;
  if ( this->input_data_.size() != 1 )
  {
    vcl_cout << "In vidpro1_homog_2d_transform_image_process::execute() - not exactly one"
             << " input frames \n";
    return false;
  }
  clear_output();

  // get input from storage classes
  vidpro1_homog_2d_storage_sptr homog_2d_storage;
  homog_2d_storage.vertical_cast(input_data_[0][0]);
  
  vgl_h_matrix_2d< double > H = homog_2d_storage->H();

  vcl_cout << "Homography matrix H = \n" << H << vcl_endl;
  vcl_cout << "Det(H) = " << vnl_det(H.get_matrix()) << vcl_endl;
  

  H = H * this->H();
  this->set_H(H);

  // Now we've got the homography matrix, apply it to the image of the current frame.
  vcl_cout << "Apply homography to the current image \n";
  
  // option 1 - from an storage image
  //// get input image from storage class
  //vidpro1_image_storage_sptr im_storage;
  //im_storage.vertical_cast(input_data_[0][0]);
  //vil_image_resource_sptr im_resource = im_storage->get_image();

  //option 2 - from a static image in a file
  //// read a static image instead of a video sequence
  //bpro1_filepath image_path;
  //parameters()->get_value( "-image_filename" , image_path );

  //vcl_string image_filename = image_path.path;
  //vil_image_resource_sptr im_resource = vil_load_image_resource( image_filename.c_str() );
  //if( !im_resource ) {
  //  vcl_cerr << "Failed to load image file" << image_filename << vcl_endl;
  //  return false;
  //}

  // option 3 - from a video clip in file
  bpro1_filepath video_path;
  parameters()->get_value( "-video_filename" , video_path );
  vcl_string video_filename = video_path.path;

  
  if (!this->my_movie_)
  {
    this->my_movie_ = vidl1_io::load_movie(video_filename.c_str());
    if (! this->my_movie_) {
      vcl_cerr << "Failed to load movie file: "<< video_filename << vcl_endl;
      return false;
    }
    this->frame_num_ = 0;
  }

  this->frame_num_++;
  
  vidl1_frame_sptr pframe = this->my_movie_->get_frame(this->frame_num_);
 
  vil_image_resource_sptr im_resource = pframe->get_resource();
  
  vil_image_view< vxl_byte > im_view(im_resource->get_view());
  
  // just for now
  // convert to grey image
  vil_image_view< float > im_view_float;
  if (im_view.nplanes() == 3)
    vil_convert_planes_to_grey< vxl_byte, float >(im_view, im_view_float);
  else
    vil_convert_cast< vxl_byte, float >(im_view, im_view_float);


  // get storage class of the background image
  vidpro1_image_storage_sptr bg_storage;
  bg_storage.vertical_cast(input_data_[0][1]);
  vil_image_resource_sptr bg_resource = bg_storage->get_image();

  // foreground image
  vil_image_view< float > fg_view = vil_image_view< float >(bg_resource->ni(), bg_resource->nj());
  if (! brip_vil_float_ops::homography(im_view_float, H, fg_view, true, 0.0f))
  {
    vcl_cout << "Homography transform failed" << vcl_endl;
    return false;
  }
  vcl_cout << "Homography succeeded " << vcl_endl;
  fg_view.print(vcl_cout);
 
  // background image
  // convert to grey image
  vil_image_view< vxl_byte > temp_view = bg_resource->get_copy_view();
  vil_image_view< float > bg_view;

  if (temp_view.nplanes() == 3)
    vil_convert_planes_to_grey< vxl_byte, float >(temp_view, bg_view);
  else
    vil_convert_cast< vxl_byte, float >(temp_view, bg_view);

  // Now combine foreground and background
  // Since I have not found an existing function to do this, I will write my own one
  // Method: First we create a mask for the result image, i.e. fill the foreground and background with different colors.
  // Then we use the mask together with the foreground and background image to create the result image.

  // create a mask of the transform image to combine foreground and background
  vil_image_view< float > im_mask_world(im_resource->ni(), im_resource->nj());
  im_mask_world.fill(1.0f);
  vil_image_view< float > im_mask_image(bg_resource->ni(), bg_resource->nj());
  if (! brip_vil_float_ops::homography(im_mask_world, H, im_mask_image, true, 0.0f))
  {
    vcl_cout << "Creating mask failed \n";
    return false;
  }
  vcl_cout << "Image mask created \n";

  // Now combine foreground and background image
  // Since the background dominates the image, we will just modify the background image
  for (unsigned int i = 0; i < bg_view.ni(); i ++)
  {
    for (unsigned int j = 0; j < bg_view.nj(); j ++){
      // check if the pixel is foreground pixel
      if (im_mask_image(i, j) > 0)
      {
        bg_view(i, j) = fg_view(i, j);
      }
    }
  }

  // create the output storage class
  vidpro1_image_storage_sptr output_storage = vidpro1_image_storage_new();
  vil_image_resource_sptr return_im_resource = vil_new_image_resource_of_view(bg_view);
  output_storage->set_image(return_im_resource);  
  output_data_[0].push_back(output_storage);
  return true;
}


//: Clone the process
bpro1_process*
vidpro1_homog_2d_transform_image_process::clone() const
{
  vidpro1_homog_2d_transform_image_process* to_return = new vidpro1_homog_2d_transform_image_process(*this);
  to_return->H_.set_identity();
  to_return->my_movie_ = 0;
  to_return->frame_num_ = 0;
  return (bpro1_process*)to_return;
  //   return new vidpro1_homog_2d_transform_image_process(*this);
}
    
bool
vidpro1_homog_2d_transform_image_process::finish()
{
  return true;
}




