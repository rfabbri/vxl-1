#include <vul/vul_timer.h>
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <imgr/imgr_bounded_image_view_3d.h>
#include <imgr/imgr_scan_resource.h>
#include <imgr/imgr_scan_resource_io.h>
#include <imgr/imgr_dummy_scan.h>

static void test_scan_resource(int argc, char* argv[])
{
  vcl_cout << "===========Testing scan resource ========\n";
  //the dummy scan
  imgr_dummy_scan scan;
  //read in the resources using the file path provided by scan
 /* scan.set_image_file_path(argv[1]);

  int view_size = 2;
  scan.set_scan_size(view_size);
  
  imgr_scan_resource_sptr resc = imgr_scan_resource_io::read_resource(scan);

  TEST("load_resource", resc&&resc->nk() == view_size, true);

  //Will fail if you don't set up your own test scan 
  //later should make images in the test
  if(!resc)
    return;
  //Define a 3-d bounding box for the bounded image view
  double minp[3], maxp[3];
  minp[0]=-1.0;   minp[1]=-1.0; minp[2]=-1.0;
  maxp[0]= 1.0;   maxp[1]= 1.0; maxp[2]= 1.0;

  vgl_box_3d<double> box(minp, maxp);
  //actually read the data
  vul_timer t;
  imgr_image_view_3d_base_sptr v3d_ptr = resc->get_bounded_view(box);
  vcl_cout << "Create a "<< resc->ni() << " x " << resc->nj() 
           << " x " << resc->nk() << " 3-d view "<< t.real()/1000.0 << " secs.\n";

  //Cast to the desired type
  imgr_bounded_image_view_3d<unsigned short> v3d = v3d_ptr;
  //Get a 2-d view out of the 3-d view
  dbil_bounded_image_view<unsigned short> dv = v3d.view_2d(0);
  //Check its bonifides
  vcl_cout << "(ni, nj, nib, njb, ib0, jb0)=(" 
           << dv.ni() << ' ' << dv.nj() << ' ' << dv.nib() 
           << ' ' << dv.njb() << ' ' << dv.ib0() << ' '
           << dv.jb0() << ")\n";
           */
}

TESTMAIN_ARGS(test_scan_resource);
