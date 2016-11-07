//this is /contrib/bm/dvrml/test/test_draw_box.cxx

#include"../dvrml.h"

#include<testlib/testlib_test.h>

#include<vcl_string.h>

#include<vgl/vgl_point_3d.h>

static void test_draw_box()
{
    
    vcl_string test_draw_box_filename = "test_draw_box.wrl";

    vcl_ofstream os(test_draw_box_filename.c_str(),vcl_ios::out);

    dvrml vrml_out(os);

    vgl_point_3d<double> center1(0.0,0.0,0.0),center2(5.0,5.0,5.0);

    vnl_vector_fixed<double,3> xyz_ratio1(.5,2,1);

    dvrml_appearance_sptr appearance1 = new dvrml_appearance;

    vnl_vector_fixed<double,3> blue(0,0,1);

    dvrml_appearance_sptr appearance2 = new dvrml_appearance(blue);

    vrml_out.draw_box(center1,appearance1);

    vrml_out.draw_box(center2,appearance2,xyz_ratio1);

    vnl_vector_fixed<double,4> rotation1(1,0,0,0.785);

    vrml_out.draw_box(center2,appearance2,xyz_ratio1,rotation1);


}//end static void test_draw_box()

TESTMAIN(test_draw_box);