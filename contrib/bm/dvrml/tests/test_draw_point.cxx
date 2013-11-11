//this is /contrib/bm/dvrml/tests/test_draw_point.cxx

#include"../dvrml.h"

#include<testlib/testlib_test.h>

#include<vcl_string.h>

#include<vgl/vgl_point_3d.h>



static void test_draw_point()
{
    vgl_point_3d<double> pt1(0,0,0),pt2(5,3.5,5);

    vgl_point_2d<unsigned> pt3(2,2);

    vnl_vector_fixed<double,3> green(0,1,0);

    dvrml_appearance_sptr appearance1 = new dvrml_appearance;

    dvrml_appearance_sptr appearance2 = new dvrml_appearance(green);
    
    vcl_string vrml_filename = "draw_point_text.wrl";

    vcl_ofstream os(vrml_filename.c_str(),vcl_ios::out);

    dvrml vrml_output(os);

    vrml_output.draw_point(pt1,appearance1);

    vrml_output.draw_point(pt2,appearance1);

    vrml_output.draw_point(pt3,appearance2);

    vcl_cout << "specularColor = " << appearance1->specularColor().get(0) << ' ' << appearance1->specularColor().get(1) << ' ' << appearance1->specularColor().get(2) << '\n';

    appearance1->set_specularColor(vnl_vector_fixed<double,3>(.2,.2,.2));

    vgl_point_3d<double> pt4(4,2.7,1);
    vrml_output.draw_point(pt4,appearance1);
}

TESTMAIN(test_draw_point);