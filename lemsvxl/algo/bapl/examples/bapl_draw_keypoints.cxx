// This is algo/bapl/bapl_make_pyramids.cxx
//:
// \file

#include <vcl_sstream.h>
#include <vcl_iostream.h>
#include <vul/vul_arg.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>
#include <vil/vil_new.h>
#include <bapl/bapl_keypoint.h>
#include <bapl/bapl_lowe_keypoint.h>
#include <bapl/bapl_keypoint_extractor.h>
#include <ipts/ipts_draw.h>



int main( int argc, char* argv[] )
{
  vul_arg<vcl_string> in_path("-i","Input image");
  vul_arg<vcl_string> out_path("-o","Output image");
  vul_arg_parse(argc, argv);

  if(!in_path.set())
    vul_arg_display_usage_and_exit();

  vil_image_resource_sptr color_image_sptr = vil_load_image_resource(in_path().c_str());
  vil_image_resource_sptr grey_image_sptr;
  grey_image_sptr = vil_new_image_resource_of_view( *vil_convert_to_grey_using_rgb_weighting ( color_image_sptr->get_view() ) );
  if (grey_image_sptr->ni()==0)
  {
    vcl_cout<<"Failed to load image."<<vcl_endl;
    return 1;
  }

  vcl_cout << "Finding Keypoints" << vcl_endl;

  vcl_vector< bapl_keypoint_sptr > keypoints;
  bapl_keypoint_extractor( grey_image_sptr, keypoints);

  vcl_cout << "Drawing Keypoints" << vcl_endl;

  vil_image_view<vxl_byte> color_img(color_image_sptr->get_view());
  for (unsigned i=0;i<keypoints.size();++i){
    bapl_lowe_keypoint_sptr kp;
    kp.vertical_cast(keypoints[i]);
    if (kp->scale()>1.1)
      ipts_draw_cross(color_img, int(kp->location_i()+0.5),
                                 int(kp->location_j()+0.5),
                                 int(kp->scale()+0.5), vxl_byte(255) );
  }

  vcl_cout << "Saving the results" << vcl_endl;
  
  vil_save(color_img, out_path().c_str() );


  vcl_cout <<  "done!" <<vcl_endl;
  return 0;
}


