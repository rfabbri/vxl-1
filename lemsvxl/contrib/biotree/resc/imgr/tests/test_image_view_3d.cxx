#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vgl/vgl_box_3d.h>
#include <dbil/dbil_bounded_image_view.h>
#include <imgr/imgr_bounded_image_view_3d.h>
static void test_image_view_3d()
{
  vcl_cout << "===========Testing image view 3-d ========\n";

  unsigned ni = 5, nj = 5;
  unsigned ib0 = 3, jb0=4;
  unsigned nib = 11, njb=11;
  //Make a 2-d view
  vil_image_view<unsigned short> v(ni, nj);
  v.fill(10);
  //construct a bounded 2-d view
  dbil_bounded_image_view<unsigned short>* bv = 
    new dbil_bounded_image_view<unsigned short>(v, ib0, jb0, nib, njb);
  //Add to a vector
  vcl_vector<dbil_bounded_image_view<unsigned short>* > views;
  views.push_back(bv);
  //Construct the 3-d view
  imgr_bounded_image_view_3d<unsigned short> v3d(views, vgl_box_3d<double>());
  vcl_cout << "(ni, nj, nk, planes, size, format)=(" 
           << v3d.ni() << ' ' << v3d.nj() << ' ' << v3d.nk() 
           << ' ' << v3d.nplanes() << ' ' << v3d.size() << ' '
           << v3d.pixel_format() << ")\n";
  //Get a 2-d view back out of the 3-d view
  dbil_bounded_image_view<unsigned short> biv = v3d.view_2d(0);
  vcl_cout << "(ni, nj, nib, njb, ib0, jb0)=(" 
           << biv.ni() << ' ' << biv.nj() << ' ' << biv.nib() 
           << ' ' << biv.njb() << ' ' << biv.ib0() << ' '
           << biv.jb0() << ")\n";
  //Check the data to see if it is what we expect
  bool good = biv.nib()==11&&biv.njb()==11&&biv.ib0()==3&&biv.jb0()==4;
  TEST("basic view_3d access", good,  true);
}
TESTMAIN(test_image_view_3d);
