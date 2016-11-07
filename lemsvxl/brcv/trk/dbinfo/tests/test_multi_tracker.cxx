// This is brl/bseg/dbinfo/tests/test_multi_tracker.cxx
#include <testlib/testlib_test.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vpl/vpl.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vbl/vbl_array_1d.h>
#include <vul/vul_timer.h>
#include <dbinfo/dbinfo_region_geometry.h>
#include <vbl/vbl_array_2d.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_view.h>
#include <vil/vil_pixel_format.h>
#include <vil/vil_new.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_rectangle_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <brip/brip_vil_float_ops.h>
#include <dbinfo/dbinfo_feature_format.h>
#include <dbinfo/dbinfo_feature_data.h>
#include <dbinfo/dbinfo_observation.h>
#include <dbinfo/dbinfo_track.h>
#include <dbinfo/dbinfo_mi_track.h>
#include <dbinfo/dbinfo_multi_tracker_params.h>
#include <dbinfo/dbinfo_multi_tracker.h>

static void test_multi_tracker(int argc, char* argv[])
{
  //
  //====================  OBSERVATION  ================================
  //
  //Test image for observation
  unsigned cols = 31,  rows = 31;
  vil_image_view<unsigned char> image0(cols, rows, 1), image1(cols, rows, 1);
  for(unsigned r = 0; r<rows; ++r)
    for(unsigned c = 0; c<cols; ++c)
      {
        image0(c,r) = 3*r + c;
        image1(c,r) = 4*r + 2*c;
      }
  vil_image_resource_sptr imgr0 = vil_new_image_resource_of_view(image0);
  vil_image_resource_sptr imgr1 = vil_new_image_resource_of_view(image1);

  //Define a centered window
  vsol_point_2d_sptr p0 = new vsol_point_2d(10,10);
  vsol_point_2d_sptr p1 = new vsol_point_2d(20,10);
  vsol_point_2d_sptr p2 = new vsol_point_2d(20,20);
  vsol_point_2d_sptr p3 = new vsol_point_2d(10,20);
  vcl_vector<vsol_point_2d_sptr> verts;
  verts.push_back(p0);   verts.push_back(p1);  verts.push_back(p2); 
  verts.push_back(p3); 
  vsol_polygon_2d_sptr poly = new vsol_polygon_2d(verts);

  vcl_vector<vsol_polygon_2d_sptr> polys;
  polys.push_back(poly);

  dbinfo_multi_tracker_params mtp;
  dbinfo_multi_tracker mt(mtp);
  mt.set_image(0, imgr0);
  bool success_init = mt.initiate_track(polys);
  mt.set_image(1, imgr1);
  bool success_track = mt.extend_tracks_from_seeds();
  dbinfo_track_sptr only_track = mt.track(0)->cast_to_mi_track();
  dbinfo_observation_sptr last_obs = only_track->observ(1);
  TEST("init and extend track ", success_init&&success_track&&only_track&&last_obs, true);
  TEST_NEAR("Best observation score ", last_obs->score(), 3.2707, 1e-5);

  //Test binary track I/O
  vsl_b_ofstream bp_out("test_track_io.tmp");
  vsl_b_write(bp_out, only_track);
  bp_out.close();

  vsl_b_ifstream bp_in("test_track_io.tmp");
  dbinfo_track_sptr trk_in;
  vsl_b_read(bp_in, trk_in);
  bp_in.close();
  if(trk_in)
    vcl_cout << "recovered track " << *trk_in << '\n';
  vpl_unlink ("test_track_io.tmp");
TEST("Track Binary I/O ", trk_in->size(), 2);
}
TESTMAIN_ARGS(test_multi_tracker);
