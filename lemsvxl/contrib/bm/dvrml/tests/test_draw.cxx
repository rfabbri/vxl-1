//this is /contrib/bm/dvrml/test/test_draw.cxx

#include"../dvrml.h"

#include<testlib/testlib_test.h>

static void test_draw()
{
    vcl_string filename = "test_draw.wrl";

    vgl_point_3d<double> pt1_center(2.0,3.0,1.0), pt2_center(0.0,0.0,0.0), pt3_center(5.0,5.0,5.0);
    double pt2_radius = 5;

    vnl_vector_fixed<double,3> green(0.0,1.0,0.0),blue(0.0,0.0,1.0);

    dvrml_appearance_sptr ap_green = new dvrml_appearance(green);
    dvrml_appearance_sptr ap_blue = new dvrml_appearance(blue);
    dvrml_appearance_sptr ap_blue2 = new dvrml_appearance(blue,0.4);

    dvrml_point_sptr pt1 = new dvrml_point(pt1_center,ap_green);
    dvrml_point_sptr pt2 = new dvrml_point(pt2_center,ap_blue2);
    dvrml_point_sptr pt3 = new dvrml_point(pt3_center,ap_blue2);

    vcl_ofstream os(filename.c_str(),vcl_ios::out);

    dvrml vrml_out(os);

    vrml_out.draw(pt1.as_pointer());
    vrml_out.draw(pt2.as_pointer());
    vrml_out.draw(pt3.as_pointer());

    vcl_vector<vgl_point_3d<double> > line_point_list;

    line_point_list.push_back(pt2_center);
    line_point_list.push_back(pt3_center);

    vcl_vector<vcl_vector<unsigned> > connection_list;
    vcl_vector<unsigned> connection1;
    connection1.push_back(0);
    connection1.push_back(1);
    connection_list.push_back(connection1);
    vcl_vector<dvrml_appearance_sptr> appearance_list;
    appearance_list.push_back(ap_blue);

    dvrml_indexed_lineset_sptr lineset_sptr = new dvrml_indexed_lineset(line_point_list,connection_list, appearance_list);

    vrml_out.draw(lineset_sptr.as_pointer());






}

TESTMAIN(test_draw);