#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vul/vul_arg.h>
#include <vil/vil_save.h>
#include <vidl1/vidl1_io.h>
#include <vidl1/vidl1_movie.h>
#include <vidl1/vidl1_frame.h>

#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>

//--------------------------------------------------------------------
//  This executable extracts a specified frame from a video and writes it
//  out as an image.  Sample usage is:
//
//   frame_extractor -video-path c:/videos/PoliceCar/left -frame-index 32 
//                   -image-file c:/videos/PoliceCar/left-frame-32.jpeg
//
//  Note that the user is responsible for including the file extension,
//  if desired.  The file type for saving is determined from the file name.
//---------------------------------------------------------------------
int main(int argc, char** argv)
{
  // --- Program Arguments ---
  vul_arg<vcl_string> video_file("-video-path", "input video file");
  vul_arg<double> frame_index("-frame-index", "for N frames, range [1, N-1], fractional [0.0, 0.999]");
  vul_arg<vcl_string> image_file("-image-file", "output image file, ext=.jpeg");
  vul_arg<vcl_string> numframe_file("-numframe-file", "output number of frames file, ext=.txt");
  vul_arg_parse(argc, argv);

  // --- Load the Movie ---
  vidl1_movie_sptr my_movie = vidl1_io::load_movie(video_file().c_str());
  if (!my_movie)
    {
      vcl_cout << "Failed to load movie \n";
      return -1;
    }
  vcl_cout << "Movie has " << my_movie->length() << " frames\n";
  if(frame_index()<0||frame_index()>=my_movie->length())
    {
      vcl_cout << "Frame index out of range \n";
      return -2;
    }
  int pos;
  if(frame_index()<1.0)
    pos = (int)(frame_index()*(my_movie->length()-1));
  else
    pos = (int)frame_index();
  
   
  // vil_save expects a byte image so we must first make a byte
  // image out of whatever we have
  vil_image_view<vxl_byte> save_image;
//  vil_image_resource_sptr image_sptr = (*my_movie->get_frame(pos)).get_resource();
 // vil_image_view_base_sptr imageview_sptr = (*my_movie->get_frame(pos)).get_view();
  
  switch((*my_movie->get_frame(pos)).get_resource()->pixel_format())
  {
  case VIL_PIXEL_FORMAT_BYTE: 
    {
      save_image = *my_movie->get_view(pos);
      break;
    }
/* comment out since videos we have are 12 bit 
   case VIL_PIXEL_FORMAT_UINT_16:
    {
      vil_image_view<vxl_uint_16> uint_image = (*my_movie->get_frame(pos)).get_view();
      vil_image_view<double> double_image;
      vil_convert_stretch_range_limited(uint_image, double_image,
                                        vxl_uint_16(0),
                                        vxl_uint_16(65535), 
                                        0.0, 255.999);
      vil_convert_cast(double_image, save_image);
      break;
    }
   */
  case VIL_PIXEL_FORMAT_BOOL: 
    {
      vil_image_view<bool> bool_image = *my_movie->get_view(pos);
      vil_convert_cast(bool_image, save_image);
      vil_math_scale_values(save_image,255.0);
      break;
    }
    
  case VIL_PIXEL_FORMAT_FLOAT:
    {
      vil_image_view<float> float_image = *my_movie->get_view(pos);
      vil_image_view<double> double_image;
      vil_convert_stretch_range_limited(float_image, double_image,
                                        0.0f, 1.0f, 0.0, 255.999);
      vil_convert_cast(double_image, save_image);
      break;
    }
  
  case VIL_PIXEL_FORMAT_DOUBLE:
    {
      vil_image_view<double> double_image = *my_movie->get_view(pos);
      vil_convert_stretch_range_limited(double_image, double_image,
                                        0.0, 1.0, 0.0, 255.999);
      vil_convert_cast(double_image, save_image);
      break;
    }
  
  default:
    {
      // stretch and convert to a byte image for display
      save_image = *vil_convert_stretch_range(vxl_byte(), (*my_movie->get_frame(pos)).get_view());
    }
  }

 // --- Save the Frame ---

  vil_save(save_image,image_file().c_str());
 

  //vil_save(*my_movie->get_view(pos), image_file().c_str());






 
  // --- Save the number of Frames in a File ---
 
  if (!numframe_file.set()){
    vcl_cerr << "Error: The numframes file was not specified" << vcl_endl;
  return -3;
  }
  vcl_ofstream outstream(numframe_file().c_str());
  if (!outstream)
  {
    vcl_cerr << "Error: could not create numframe output file ["<<numframe_file()<<"]" << vcl_endl;
    return -4;
  }

  // Write number of frames information
   outstream <<my_movie->length();

  outstream.close();
  return 0;
}
