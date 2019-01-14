#include <dbrkf/dbrkf_simplerect.h>
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
#include <testlib/testlib_test.h>

int test_simplerect_main(int argc, char *argv[])
{
    testlib_test_begin("simple rectification");
    std::string image_base;
    if(argc>=2)
        image_base=argv[1];
   #ifdef VCL_WIN32
    image_base += "\\";
   #else
    image_base += "/";
   #endif
    std::string filename=image_base+"pp-aug16.tmp";
    std::string imgl=image_base+"camera0-00000.png";
    std::string imgr=image_base+"camera1-00000.png";

    std::vector<vsol_point_2d_sptr > pointsl_,pointsr_;
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
    std::cerr << "Returning false. imagel.nplanes(): " << imagel.nplanes() << std::endl;
    return false;
    }

  if( imager.nplanes() == 3 ) {
    vil_convert_planes_to_grey( imager , grey_imgr );
  }
  else if ( imager.nplanes() == 1 ) {
    grey_imgr = imager;
  } else {
    std::cerr << "Returning false. imager.nplanes(): " << imager.nplanes() << std::endl;
    return false;
  } 

    dbrkf_simplerect testcase(pointsl_,pointsr_,grey_imgl,grey_imgr);
    testcase.rect_step1();
    testcase.rect_step2();

    std::string fileoutl=image_base+"step2l.png";
    std::string fileoutr=image_base+"step2r.png";
    vil_save_image_resource(vil_new_image_resource_of_view( *(testcase.imgl_step2)),fileoutl.c_str());
    vil_save_image_resource(vil_new_image_resource_of_view( *(testcase.imgr_step2)),fileoutr.c_str());

     return testlib_test_summary(); 
}
