#include <expatpp/expatpplib.h>
#include "track.h"
#include <fstream>
#include <vul/vul_arg.h>
#include <vil/vil_save.h>
#include <vil/vil_crop.h>
#include <vil/vil_smart_ptr.h> 
#include <bvis1/bvis1_gl_codec.h>
#include <vidl1/vidl1_movie.h>
#include <vidl1/vidl1_clip.h>
#include <vidl1/vidl1_io.h>
#include <vidl1/vidl1_frame.h> 
#include <vil/vil_convert.h>
//#include <vidl1/vidl1_avicodec.h>
#include <vul/vul_sprintf.h>


//char  const *guess_file_format(char const* filename);
//--------------------------------------------------------------------
//  This executable extracts specified frames from a video, optionally 
//  crops them, and writes them  out as a sequence of frames.  
//  Sample usage is:
//
//   vorl1_movie_extractor -video-path  c:/videos/PoliceCar/left 
//  -xml-path  //c:/videos/Policecar/track.xml -frame-index 0 -numframes 10 
//  -image-dir c:/videos/PoliceCar/left.mpeg
//
//---------------------------------------------------------------------
int main(int argc, char** argv)
{
  // --- Program Arguments ---

  vul_arg<std::string> video_file("-video-path", "input video file");
  vul_arg<std::string> xml_file("-xml-path", "input xml file");
  vul_arg<int> frame_index("-frame-index", "starting frame");
  vul_arg<int> num_frames("-numframes", "number of frames");
  vul_arg<std::string> image_dir("-image-dir", "output image directory");
  vul_arg_parse(argc, argv);

  std::FILE *xmlFile;
  std::vector<vil_image_resource_sptr> lImages; 
  std::string format="jpeg";

  track_info info;
  track_parser parser(&info);

   std::cout << " Parsing..\n";

  if (  xml_file()!= "" ){
      xmlFile = std::fopen(xml_file().c_str(), "r");
      if (xmlFile){
         if (!parser.parseFile(xmlFile)) {
             std::cerr << "Error: "  <<  XML_ErrorString(parser.XML_GetErrorCode()) 
          << " at line "
          << parser.XML_GetCurrentLineNumber()                     << std::endl;
          return 1;
          }
      }
      else{
        std::cerr <<  xml_file().c_str()  << " error on opening.  \n"   << std::endl; 
        return(1);
      }
    
}
else{
       std::cout << " XML File not specified\n";

}
   std::cout << " Parsing Done.\n";
  
  bool bHasTrack = false;
  std::string height ="";
  std::string width ="";
  if (info.get_count() > 0) 
     bHasTrack = true;
  if (bHasTrack){
      width = info.getWidth();
      height = info.getHeight();
  //    std::cout << "Crop Width:"  << width << " height: " << height << "\n" ;


     for (std::vector<track_coord_info>::iterator arg_it = info.coordinates.begin();
        arg_it != info.coordinates.end(); ++arg_it){
          std::string x = arg_it->x;
         std::string y = arg_it->y;
  //        std::cout << "Crop"  << x.c_str() << " y: " << y.c_str() << "\n" ;
      }

  } //if




  vidl1_movie_sptr my_movie ;
  // --- Load the whole Movie ---

  my_movie = vidl1_io::load_movie(video_file().c_str());
  //else   // --- Load only the frames of the  Movie we need ---
  //   my_movie = vidl1_io::load_movie(video_file().c_str(),frame_index(),frame_index() + num_frames() -1 ,1,'r');

  if (!my_movie)
    {
      std::cout << "Failed to load movie \n";
      return -1;
    }
  std::cout << "Movie has " << my_movie->length() << " frames \n";

  if(frame_index()<0||frame_index()>=my_movie->length())
    {
      std::cout << "Frame index out of range \n";
      return -2;
    }
  int start;
  int end =0;

  if(frame_index()<1.0)
    start = (int)(frame_index()*(my_movie->length()-1));
  else
    start = (int)frame_index();

   if(num_frames() > 0 )
     end = (int)start + (int)num_frames()-1;
   if (end ==0 || end > (int)my_movie->length()-1)
     end = (int)my_movie->length()-1;

//    std::cout << "Creating new movie with frames " << start << " to " << end <<"\n";
 //   std::cout << " in dir " << image_dir().c_str() <<" \n";

        long lastY=1;
        long lastX=1;
 // --- Save numframes Frames as a movie---
      for (vidl1_movie::frame_iterator pframe = my_movie->begin()
; pframe < my_movie->end(); ++pframe)
      {

        int i = pframe->get_real_frame_index();
 //       std::cout << ","<< i ;
        if ( i < start) continue;
        if ( i > end) continue;
  //      std::cout << "*";
   //    vil_image_resource_sptr vi = pframe->get_resource();
  //     vil_image_view_base_sptr image = pframe->get_view();
        if (bHasTrack){
           int j = start;
           std::string x = "0";
           std::string y ="0";
           std::string x_margin="0";
           std::string y_margin="0";

           for (std::vector<track_coord_info>::iterator arg_it = info.coordinates.begin();
            arg_it != info.coordinates.end(); ++arg_it){
              if (j++ == i){
                x = arg_it->x;
                y = arg_it->y;
                y_margin = arg_it->y_margin;
                x_margin = arg_it->y_margin;
                break;
              }
           }
          char * pEnd;
  //        std::cout << "Crop "  << i <<"," << --j << " x: " << strtoul(x.c_str(),&pEnd,0) << " y: " <<strtoul(y.c_str(),&pEnd,0) << "\n" ;

           //     vil_image_resource_sptr crop= vil_crop( pframe->get_resource(), 100, 100, 200, 200 );
   //crop with x,y is the upper left corner
    //vil_image_resource_sptr crop= vil_crop( pframe->get_resource(), strtoul(x.c_str(),&pEnd,0) ,strtoul(width.c_str(),&pEnd,0),strtoul(y.c_str(),&pEnd,0) ,strtoul(height.c_str(),&pEnd,0) );
          long iWidth = strtoul(width.c_str(),&pEnd,0);
          long iHeight  = strtoul(height.c_str(),&pEnd,0);
          long iX = strtoul(x.c_str(),&pEnd,0) - iWidth/2;
          long iY = strtoul(y.c_str(),&pEnd,0) - iHeight/2;

          // Make sure cropping does not try to go off the end or beginning of the frame
          int maxX = pframe->get_resource()->ni();
          if (iX + iWidth > maxX){
              iX = maxX - iWidth -1;
              std::cout << "Crop x"  << iX <<" reduced to fit";
              if (iX <0) continue;   // Make sure width is small enough for image. 

          }
          int maxY = pframe->get_resource()->nj();
          if (iY + iHeight > maxY){
               iY = maxY - iHeight -1;
               std::cout << "Crop y"  << iY <<" reduced to fit";
               if (iY<0) continue;   // Make sure height is small enough for image. 

          }

       // Make sure cropping does not try to go off the beginning of the frame
          if (iX  < 0) {
              iX = 0;
              std::cout << "Crop x"  << iX <<" changed to fit";
          }
          if (iY  < 0 ){
               iY = 0;
               std::cout << "Crop y"  << iY <<" changed to fit";
          }
        


          // Check for missing track information and use the last valid one.
          if (strtoul(x.c_str(),&pEnd,0)== 0)
             iX = lastX;
          if (strtoul(y.c_str(),&pEnd,0) == 0)
             iY = lastY;
          lastX = iX;
          lastY = iY;

              
   //       std::cout << "Crop "  << i <<"," << --j << " x: " << iX << " y: " << iY << "\n" ;

 
          vil_image_resource_sptr crop= vil_crop( pframe->get_resource(), iX ,strtoul(width.c_str(),&pEnd,0),iY ,strtoul(height.c_str(),&pEnd,0) );

           //   vil_image_view_base_sptr cimage = crop->get_view();
          //     vil_image_view_base_sptr image = pframe->get_view();

          lImages.push_back(crop );
        }
        else
          lImages.push_back( pframe->get_resource());
      }//for
      std::cout << "\nBuilding new movie in memory and saving to file..\n";
      vidl1_movie_sptr movie = new vidl1_movie(new vidl1_clip(lImages));

      int currentframenumber=0;
      for (vidl1_movie::frame_iterator pframe = movie->begin();
         pframe <= movie->last();
         ++pframe)
      {

              // vil_save expects a byte image so we must first make a byte
              // image out of whatever we have
              vil_image_view<vxl_byte> save_image;
            //  vil_image_resource_sptr image_sptr = (*my_movie->get_frame(pos)).get_resource();
             // vil_image_view_base_sptr imageview_sptr = (*my_movie->get_frame(pos)).get_view();
  
              switch(pframe->get_resource()->pixel_format())
              {
              case VIL_PIXEL_FORMAT_BYTE: 
                {
                  save_image = pframe->get_view();
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
                  vil_image_view<bool> bool_image = pframe->get_view();
                  vil_convert_cast(bool_image, save_image);
                  vil_math_scale_values(save_image,255.0);
                  break;
                }
    
              case VIL_PIXEL_FORMAT_FLOAT:
                {
                  vil_image_view<float> float_image = pframe->get_view();
                  vil_image_view<double> double_image;
                  vil_convert_stretch_range_limited(float_image, double_image,
                                                    0.0f, 1.0f, 0.0, 255.999);
                  vil_convert_cast(double_image, save_image);
                  break;
                }
  
              case VIL_PIXEL_FORMAT_DOUBLE:
                {
                  vil_image_view<double> double_image = pframe->get_view();
                  vil_convert_stretch_range_limited(double_image, double_image,
                                                    0.0, 1.0, 0.0, 255.999);
                  vil_convert_cast(double_image, save_image);
                  break;
                }
  
              default:
                {
                  // stretch and convert to a byte image for display
                  save_image = *vil_convert_stretch_range(vxl_byte(), pframe->get_view());
                }
              }
          // Create a name for the current image to be saved
             std::string currentname = vul_sprintf("%s%05d.%s", image_dir().c_str(),
                                         currentframenumber++,"jpeg");

             vil_save(save_image,currentname.c_str());
 
           
           
           
           
           // Get the image from the frame
       //   vil_image_view_base_sptr image = pframe->get_view();

      
       }
    //   vidl1_io::save(movie, image_dir().c_str(),  format);
      return 0;
}
