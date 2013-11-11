//this is /contrib/bm/dvrml/test/test_draw_axis.cxx

#include"../dvrml.h"

#include<testlib/testlib_test.h>

#include<vcl_string.h>

#include<vgl/vgl_point_3d.h>

static void test_draw_axis()
{
    vcl_string filename = "test_draw_axis.wrl";

    vcl_ofstream os( filename.c_str() );

    dvrml vrml_out(os);

    vrml_out.draw_axis();

}//end static void test_draw_axis()

TESTMAIN(test_draw_axis);