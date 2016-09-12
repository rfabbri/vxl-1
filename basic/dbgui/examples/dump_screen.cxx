// Display 2d geometric objects on top of an image.

#include <vcl_iostream.h>
#include "GL/osmesa.h"
#include <vgui/vgui.h> 
#include <vgui/vgui_utils.h> 
#include <vgui/vgui_image_tableau.h> 
#include <vgui/vgui_easy2D_tableau.h> 
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vul/vul_arg.h>
#include <vidl1/vidl1_movie.h>
#include <vidl1/vidl1_clip.h>
#include <vidl1/vidl1_io.h>
#include <vidl1/vidl1_frame.h> 
#include <vil/vil_convert.h>
#include <vul/vul_sprintf.h>
//#include <expatpp/src_pp/expatpplib.h>
#include "segmented_contours.h"
#include "track.h"




int main(int argc, char **argv)

{ 
  vcl_FILE *xmlFile;
 
  vul_arg<vcl_string> video_file("-video-path", "input video file");
  vul_arg<vcl_string> image_dir("-image-path", "output file");
  vul_arg<vcl_string> xml_file("-xml-path", "input xml file");
  vul_arg<int> frame_index("-frame-index", "starting frame");
  vul_arg<int> num_frames("-numframes", "number of frames");


  vul_arg_parse(argc, argv);

  if (video_file() == "" )
  {
    vcl_cerr << "Please give an image filename on the command line" << vcl_endl;
    return 0;
  }

 

   segmented_contours_info sinfo;
   track_info tinfo;
   bool bIsTrack = false;


  

 vcl_cout << " Parsing..\n";

  if (  xml_file()!= "" ){
      xmlFile = vcl_fopen(xml_file().c_str(), "r");
      if (xmlFile){
         if (strstr(xml_file().c_str(),"track.xml") == NULL){
           segmented_contours_parser parser(&sinfo);
            if (!parser.parseFile(xmlFile)) {
               vcl_cerr << "Error: "  <<  XML_ErrorString(parser.XML_GetErrorCode()) 
            << " at line "
            << parser.XML_GetCurrentLineNumber()                     << vcl_endl;
            return 1;
            }
         }
        else{
           track_parser parser(&tinfo);
            bIsTrack = true;
                   if (!parser.parseFile(xmlFile)) {
             vcl_cerr << "Error: "  <<  XML_ErrorString(parser.XML_GetErrorCode()) 
          << " at line "
          << parser.XML_GetCurrentLineNumber()                     << vcl_endl;
          return 1;
          }
  
         }
      }
      else{
        vcl_cerr <<  xml_file().c_str()  << " error on opening.  \n"   << vcl_endl; 
        return(1);
      }
    
}
else{
       vcl_cout << " XML File not specified\n";

}
  // long lTrackWidth =0;
  // long lTrackHeight=0;
int i =0;
if (bIsTrack){
   i=tinfo.get_count();
//   vcl_string trackwidth = info.getWidth();
 //  vcl_string trackheight = info.getHeight();
 //  lTrackWidth = strtoul(trackwidth.c_str(),&pEnd,0);
 //  lTrackHeight  = strtoul(trackheight.c_str(),&pEnd,0);
}
else 
    i=sinfo.get_count();

   vcl_cout << " Parsing Done " << i << " Polygon(s) found.\n";



 vidl1_movie_sptr my_movie ;
  // --- Load the whole Movie ---
  my_movie = vidl1_io::load_movie(video_file().c_str());
  int currentframenumber=0;
  if (!my_movie){
               vcl_cerr << "Error: loading movie " <<video_file().c_str()   << vcl_endl;
               return(-1);

  }
  vcl_cout << "Movie has " << my_movie->length() << " frames \n";



     if(frame_index()<0||frame_index()>=my_movie->length())
    {
      vcl_cout << "Frame index out of range \n";
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

   long lastY=1;
   long lastX=1;
 
   //      for (vcl_vector<vcl_string>::iterator arg_it = info.coordinates.begin();
   //         arg_it != info.coordinates.end(); ++arg_it){
  //              vcl_string c = *arg_it;
  //          }

/*
#ifdef WIN32


  vgui::init(argc,argv);
   GLint vp[4]; // x,y,w,h
  glGetIntegerv(GL_VIEWPORT, vp);
  int vwidth =vp[2];
  int vheight = vp[3];

#else
*/  
  void *buffer;
  OSMesaContext ctx;
/*#endif*/




bool bInitWindow = false;
int iFrameOffset = 0;
// For each image build the screen view and them print to a file.
      for (vidl1_movie::frame_iterator pframe = my_movie->begin()
; pframe < my_movie->end(); ++pframe)
      {

        int i = pframe->get_real_frame_index();
        if ( i < start) continue;
        if ( i > end) continue;



          // vil_save expects a byte image so we must first make a byte
              // image out of whatever we have
              vil_image_view<vxl_byte> this_image;
            //  vil_image_resource_sptr image_sptr = (*my_movie->get_frame(pos)).get_resource();
             // vil_image_view_base_sptr imageview_sptr = (*my_movie->get_frame(pos)).get_view();
  
              switch(pframe->get_resource()->pixel_format())
              {
              case VIL_PIXEL_FORMAT_BYTE: 
                {
                  this_image = pframe->get_view();
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
                  vil_convert_cast(double_image, this_image);
                  break;
                }
               */
              case VIL_PIXEL_FORMAT_BOOL: 
                {
                  vil_image_view<bool> bool_image = pframe->get_view();
                  vil_convert_cast(bool_image, this_image);
                  vil_math_scale_values(this_image,255.0);
                  break;
                }
    
              case VIL_PIXEL_FORMAT_FLOAT:
                {
                  vil_image_view<float> float_image = pframe->get_view();
                  vil_image_view<double> double_image;
                  vil_convert_stretch_range_limited(float_image, double_image,
                                                    0.0f, 1.0f, 0.0, 255.999);
                  vil_convert_cast(double_image, this_image);
                  break;
                }
  
              case VIL_PIXEL_FORMAT_DOUBLE:
                {
                  vil_image_view<double> double_image = pframe->get_view();
                  vil_convert_stretch_range_limited(double_image, double_image,
                                                    0.0, 1.0, 0.0, 255.999);
                  vil_convert_cast(double_image, this_image);
                  break;
                }
  
              default:
                {
                  // stretch and convert to a byte image for display
                  this_image = *vil_convert_stretch_range(vxl_byte(), pframe->get_view());
                }
              }


         float xCoord[500];
         float yCoord[500];
         int iNumPoints=0;
         int iCoordFrameOffset = 0;


        if (bIsTrack == false){

          const char comma = ',';
          const char space = ' ';
          for (vcl_vector<vcl_string>::iterator arg_it = sinfo.coordinates.begin();
              arg_it != sinfo.coordinates.end(); ++arg_it){
                if (iCoordFrameOffset++ == iFrameOffset){
                     char *pNext  = (char *)(*arg_it).c_str();
                     char *pEnd = (char *)(*arg_it).c_str() + strlen((*arg_it).c_str());
                     //vcl_cout << "coord:  " <<  pNext << " \n";
                     int iPoints=0;
                     while (pNext != NULL && strcmp(pNext,"") != 0 && iPoints < 500){
                       char *pX = strtok(pNext,",");
                       //vcl_cout << "3 " << iNumPoints << "(" << pX << ") \n";
                       if (pX == NULL)
                           pX = pNext;
                       char *pY = strtok(pX+strlen(pX)+1," ");
                       //vcl_cout << "- " << iNumPoints << "(" << pX << "," << pY << ") \n";
                       xCoord[iNumPoints]=atof(pX);
                       yCoord[iNumPoints]= atof(pY);
                       iNumPoints++;
                       pNext = pY+ strlen(pY)+1;
                       if (pNext > pEnd)
                           pNext = NULL;
                     }

                }
          }
        }
        else{ //IsTrack== true
           vcl_string x = "0";
           vcl_string y ="0";
           vcl_string x_margin="0";
           vcl_string y_margin="0";
           long lTrackWidth =0;
           long lTrackHeight=0;
           char *pEnd;

           lTrackWidth = strtoul(tinfo.getWidth().c_str(),&pEnd,0);
           lTrackHeight  = strtoul(tinfo.getHeight().c_str(),&pEnd,0);

           bool bNotFoundTrackInfo = true;
           for (vcl_vector<track_coord_info>::iterator arg_it = tinfo.coordinates.begin();
            arg_it != tinfo.coordinates.end(); ++arg_it){
              if (iCoordFrameOffset++ == iFrameOffset){
                x = arg_it->x;
                y = arg_it->y;
                y_margin = arg_it->y_margin;
                x_margin = arg_it->y_margin;
                bNotFoundTrackInfo = false;
                break;
              }
           }
            if (bNotFoundTrackInfo == false){
              long iX = strtoul(x.c_str(),&pEnd,0) - lTrackWidth/2;
              long iY = strtoul(y.c_str(),&pEnd,0) - lTrackHeight/2;
              long iX_High = strtoul(x.c_str(),&pEnd,0) + lTrackWidth/2;
              long iY_High = strtoul(y.c_str(),&pEnd,0) + lTrackHeight/2;

              // Make sure it does not try to go off the beginning/end of the frame

              int maxX = pframe->get_resource()->ni();
              int maxY = pframe->get_resource()->nj();

              if (iX_High > maxX){
                  iX_High = maxX-1; 
                  vcl_cout << " Width"  << iX <<" reduced to fit";

              }

              if (iY_High > maxY){
                   iY_High = maxY-1;
                   vcl_cout << " Height"  << iY <<" reduced to fit";

              }

               if (iX  < 0) {
                  iX = 0;
                  vcl_cout << " x"  << iX <<" changed to fit";
              }
        
              if (iY  < 0 ){
                   iY = 0;
                   vcl_cout << " y"  << iY <<" changed to fit";
              }
  


       //       lastX = iX;
        //      lastY = iY;
              xCoord[0]=iX;
              yCoord[0]=iY;
              xCoord[1]=iX;
              yCoord[1]=iY_High;
              xCoord[2]=iX_High;
              yCoord[2]=iY_High;
              xCoord[3]=iX_High;
              yCoord[3]=iY ;

              iNumPoints=4;;

            }

      }

        vgui_image_tableau_new image(this_image);
     if (bInitWindow == false) { //set the windown to the image height and width.
/*#ifdef WIN32
        vgui_window *win = vgui::produce_window(image->width(), image->height(), "");
#else
        */
        int    width = image->width();
        int     height = image->height();
             ctx = OSMesaCreateContextExt( OSMESA_RGBA, 16, 0, 0, NULL );
                           /* Allocate the image buffer */
             buffer = malloc( width * height * 4 * sizeof(GLubyte) );
             if (!buffer) {
                printf("Alloc image buffer failed!\n");
                return 0;
             }

             /* Bind the buffer to the context and make it current */
             if (!OSMesaMakeCurrent( ctx, buffer, GL_UNSIGNED_BYTE, width, height)) {
                printf("OSMesaMakeCurrent failed!\n");
                return 0;
             }
    
//#endif


             bInitWindow = true;
       }

        // Put the image.tableau into a easy2D tableau 
        vgui_easy2D_tableau_new easy2D(image);

/*
  float polygonx[4]={948,1038,1038,948};
  float polygony[4]={548,548,496,496};
  easy2D->set_foreground(0,1,0); 
  easy2D->set_line_width(2); 
  easy2D->add_polygon(4,polygonx,polygony);

        // Build the polygon to draw on the screen.
        //float polygonx[4]={951,1035,1035,951};
        //float polygony[4]={528,528,496,496};
  easy2D->add_polygon(4,polygonx,polygony);

*/
        if (iNumPoints > 0){
          //vcl_cout << "Adding polygon with NumPoints " << iNumPoints << "..\n";
          easy2D->set_foreground(0,1,0); 
          easy2D->set_line_width(2); 
          easy2D->add_polygon(iNumPoints,xCoord,yCoord);
        }
        // Put the easy2D tableau into a viewer2D tableau: 
        vgui_viewer2D_tableau_new viewer(easy2D);

        // Change the screen size
        // glViewport(0,0,1280,720 );
        // vgui_modifier modifier = vgui_CTRL;
        //  vgui_key key=vgui_key('x');
        //  bool bresize = viewer->key_press(1280, 720, key,  modifier);

        viewer->handle(vgui_DRAW);
        vcl_string currentname = vul_sprintf("%s%05d.%s", image_dir().c_str(),
                                 currentframenumber++,"jpeg");

          //Print the image to a file.
        vgui_utils::dump_colour_buffer(currentname.c_str());
        iFrameOffset++;

//  vgui_shell_tableau_new shell(viewer);
         }
 

 // vgui::run(shell, image->width(), image->height());

   printf("all done\n");

#ifndef WIN32
   /* free the image buffer */
   free( buffer );

   /* destroy the context */
   OSMesaDestroyContext( ctx );
#endif


  return(0);
}
