// This is brl/bseg/dbinfo/tests/test_tracking_face_2d.cxx
#include <testlib/testlib_test.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vpl/vpl.h>
#include <vsl/vsl_binary_io.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <dbinfo/dbinfo_region_geometry.h>
#include <vbl/vbl_array_2d.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
static void test_region_geometry(int argc, char* argv[])
{
  //Test construction and pointer casting
  unsigned cols = 10, rows = 10;
  vsol_point_2d_sptr p0 = new vsol_point_2d(0,0);
  vsol_point_2d_sptr p1 = new vsol_point_2d(5.0,0);
  vsol_point_2d_sptr p2 = new vsol_point_2d(3.0,2.0);
  vcl_vector<vsol_point_2d_sptr> verts;
  verts.push_back(p0);   verts.push_back(p1);  verts.push_back(p2); 
  vsol_polygon_2d_sptr poly = new vsol_polygon_2d(verts);
  vcl_vector<vsol_polygon_2d_sptr> polys;
  polys.push_back(poly);
  dbinfo_region_geometry_sptr rg = new dbinfo_region_geometry(cols, rows, polys);
  vcl_cout << "Number of points " << rg->size() << '\n';
  brip_roi_sptr roi = rg->roi();
  vsol_box_2d_sptr box = roi->region(0);
  vcl_cout << "ROI Bounds \n" << *box << '\n';
  //Test diameter
  double diameter = rg->diameter();
  vcl_cout << "region diameter " << diameter << '\n';

  TEST_NEAR("Diameter", diameter,  5.38516 , 1e-05);

  for(unsigned i = 0; i<rg->size();++i)
    vcl_cout << rg->point(i) << '\n';
  TEST("dbinfo_region_geometry constructor", 
       (rg->size()==9)&&( rg->point(8)).x()==4&&(rg->point(8)).y()==1, true);

  //test transformation
  vsol_point_2d_sptr c = poly->centroid();
  vcl_cout << "centroid " << *c << '\n';
  // === Translation ===
  vgl_h_matrix_2d<float> H;
  H.set_identity();
  H.set_translation(1.0f, 2.0f);
  dbinfo_region_geometry_sptr trans_rg 
    = new dbinfo_region_geometry(*rg, H);
  vcl_cout << "translated points \n";
  for(unsigned i = 0; i<trans_rg->size();++i)
    vcl_cout << trans_rg->point(i) << '\n';
  TEST_NEAR("region translation", trans_rg->point(0).x(), 1.0, 1e-05);
  // === Rotation + Translation ===
  H.set_rotation(0.7854f);
  dbinfo_region_geometry_sptr trot_rg 
    = new dbinfo_region_geometry(*rg, H);
  vcl_cout << "translated and rotated points \n";
  for(unsigned i = 0; i<trot_rg->size();++i)
    vcl_cout << trot_rg->point(i) << '\n';
  TEST_NEAR("region translation + rotation",
            trot_rg->point(0).x(), 2.25246, 1e-05);
  // === Scale + Rotation + Translation ===
  H.set_scale(2.0f);
  dbinfo_region_geometry_sptr trotsc_rg 
    = new dbinfo_region_geometry(*rg, H);
  vcl_cout << "translated, rotated and scaled points \n";
  for(unsigned i = 0; i<trotsc_rg->size();++i)
    vcl_cout << trotsc_rg->point(i) << '\n';
  TEST_NEAR("region translation + rotation + scale",
            trotsc_rg->point(0).x(), 1.83825, 1e-05);

  //====== Test binary I/0=========
  vsl_b_ofstream bp_out("test_region_geometry_io.tmp");
  TEST("Created test_region_geometry_io.tmp for writing",(!bp_out), false);
  
  vcl_cout << "Writing out " << *rg << '\n';
  vsl_b_write(bp_out, rg);
  bp_out.close();

  // binary test input file stream
  vsl_b_ifstream bp_in("test_region_geometry_io.tmp");
  TEST("Opened test_region_geometry_io.tmp for reading",(!bp_in), false);

  dbinfo_region_geometry_sptr rgin_p;
  vsl_b_read(bp_in, rgin_p);
  bp_in.close();
  if (rgin_p)
    vcl_cout << "Recovered region geometry " << *rgin_p << '\n';
  TEST("binary file recovered dbinfo_region_geometry ", 
       (rgin_p->size()==9)&&( rgin_p->point(8)).x()==4&&(rgin_p->point(8)).y()==1,
       true);
  // remove the temporary file
  vpl_unlink ("test_region_geometry_io.tmp");

}
TESTMAIN_ARGS(test_region_geometry);
