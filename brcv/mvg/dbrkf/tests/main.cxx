#include <rectification/brrect.h>
#include <vil/vil_load.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_view_base.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_vector_io.h>
#include <vil/vil_convert.h>
#include <vil/vil_math.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_save.h>
int main()
{
    vcl_string filename="d:\\data\\stereo\\stereo-pair\\pp-aug16.tmp";
    vcl_string imgl="d:\\data\\stereo\\stereo-pair\\camera0-00000.png";
    vcl_string imgr="d:\\data\\stereo\\stereo-pair\\camera1-00000.png";

    vcl_vector<vsol_point_2d_sptr > pointsl_,pointsr_;
    vsl_b_ifstream bfs(filename);
    short ver;
    vsl_b_read(bfs, ver);
    vsl_b_read(bfs, pointsl_);
    vsl_b_read(bfs, pointsr_);
    bfs.close();

    vil_image_resource_sptr imagel_sptr=vil_load_image_resource(imgl.c_str());
    vil_image_resource_sptr imager_sptr=vil_load_image_resource(imgr.c_str());

    vil_image_view<vxl_byte> imagel = imagel_sptr->get_view();
    vil_image_view<vxl_byte> imager = imager_sptr->get_view();

    vil_image_view<vxl_byte> grey_imgl,grey_imgr;

    if( imagel.nplanes() == 3 ) {
        vil_convert_planes_to_grey( imagel , grey_imgl );
    }
    else if ( imagel.nplanes() == 1 ) {
      grey_imgl = imagel;
     } else {
    vcl_cerr << "Returning false. imagel.nplanes(): " << imagel.nplanes() << vcl_endl;
    return false;
    }

  if( imager.nplanes() == 3 ) {
    vil_convert_planes_to_grey( imager , grey_imgr );
  }
  else if ( imager.nplanes() == 1 ) {
    grey_imgr = imager;
  } else {
    vcl_cerr << "Returning false. imager.nplanes(): " << imager.nplanes() << vcl_endl;
    return false;
  } 

    brrect testcase(pointsl_,pointsr_,grey_imgl,grey_imgr);
    testcase.rect_step1();
    testcase.rect_step2();


    vil_save_image_resource(vil_new_image_resource_of_view( *(testcase.imgl_step2)),"d:\\step2l.png");
    vil_save_image_resource(vil_new_image_resource_of_view( *(testcase.imgr_step2)),"d:\\step2r.png");

}
