// This is brl/bseg/dbinfo/tests/test_track_storage.cxx
//#include <crtdbg.h>
#include <testlib/testlib_test.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vpl/vpl.h>
#include <vsl/vsl_binary_io.h>
#include <vbl/vbl_array_1d.h>
#include <vul/vul_timer.h>
#include <vul/vul_file.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
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
#include <bvis1/bvis1_macros.h>
#include <bvis1/bvis1_manager.h>
#include <dbinfo/dbinfo_feature_format.h>
#include <dbinfo/dbinfo_feature_data.h>
#include <dbinfo/dbinfo_intensity_feature.h>
#include <dbinfo/dbinfo_gradient_feature.h>
#include <dbinfo/dbinfo_observation.h>
#include <dbinfo/dbinfo_track.h>
#include <dbinfo/pro/dbinfo_track_storage.h>
#include <dbinfo/dbinfo_observation_generator.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vidpro1/vidpro1_repository.h>
static void track_points(dbinfo_track_sptr const& trk, vcl_vector<float> &x,
                         vcl_vector<float> &y, vcl_vector<unsigned> &frame)
{
  if(!trk)
    return;
  unsigned n = trk->size();
  x.resize(n);
  y.resize(n);
  frame.resize(n);
  for(unsigned i = 0; i<n; ++i)
    {
      dbinfo_observation_sptr obs = trk->observ(i);
      frame[i]=obs->frame();
      dbinfo_region_geometry_sptr geo = obs->geometry();
      vsol_point_2d_sptr p = geo->cog();
      x[i]=p->x(); y[i]=p->y();
    }
}
static void test_track_storage(int argc, char* argv[])
{
  REG_STORAGE( dbinfo_track_storage );
  //
  //====================  TRACK STORAGE  ================================
  //
  //Test image for observation
  unsigned cols = 100,  rows = 100;
  vil_image_view<unsigned char> gimage(cols, rows, 1);
  for(unsigned r = 0; r<rows; ++r)
    for(unsigned c = 0; c<cols; ++c)
      gimage(c,r) = r + c;
  vil_image_resource_sptr imgr = vil_new_image_resource_of_view(gimage);
  //Define a centered window
  vsol_point_2d_sptr p0 = new vsol_point_2d(35,35);
  vsol_point_2d_sptr p1 = new vsol_point_2d(66,35);
  vsol_point_2d_sptr p2 = new vsol_point_2d(66,66);
  vsol_point_2d_sptr p3 = new vsol_point_2d(35,66);
  vcl_vector<vsol_point_2d_sptr> verts;
  verts.push_back(p0);   verts.push_back(p1);  verts.push_back(p2); 
  verts.push_back(p3); 
  vsol_polygon_2d_sptr poly2 = new vsol_polygon_2d(verts);
  //Construct the observation
  dbinfo_observation_sptr obs = new dbinfo_observation(0, imgr, poly2);
  vcl_cout << "obs geometry " << *(obs->geometry()) << '\n';
  dbinfo_feature_base_sptr intf = new dbinfo_intensity_feature();
  dbinfo_feature_base_sptr gradf = new dbinfo_gradient_feature();
  vcl_vector<dbinfo_feature_base_sptr> features;
  features.push_back(intf);   features.push_back(gradf);
  obs->set_features(features);
  obs->scan(0, imgr);
  vcl_cout << "Number of observation points " << obs->geometry()->size() << '\n';
  //Create a track from observations
  dbinfo_track_sptr track = new dbinfo_track();
  track->set_id(0);
  obs->set_score(0.0f);
  track->init_track(obs);
  vgl_h_matrix_2d<float> H;
  H.set_identity();
  unsigned nobs = 109;
  for(unsigned i = 0; i<nobs; ++i)
    {
    
      dbinfo_observation_sptr obsi = dbinfo_observation_generator::generate(obs, H);
      obsi->scan(i+1, imgr);
      obsi->set_score(static_cast<float>(i+1));
      track->extend_track(obsi);
    }
  dbinfo_track_storage_sptr track_sto = dbinfo_track_storage_new();
  track_sto->set_name("junk");
  vcl_vector<dbinfo_track_sptr> tracks;
  tracks.push_back(track);
  track_sto->set_tracks(tracks);
  vsl_b_ofstream sto_out("test_track_storage.tmp");
  vsl_b_write(sto_out, track_sto);
  sto_out.close();
  
  vsl_b_ifstream sto_in("test_track_storage.tmp");
  dbinfo_track_storage_sptr track_store_in;
  vsl_b_read(sto_in, track_store_in);
  sto_in.close();
  bool good = true;
  if(!track_store_in)
    good = false;
  unsigned s = vul_file::size("test_track_storage.tmp");
  TEST("Read track storage", good, true);
  vcl_cout << "The track storage size for " << nobs+1 << " observations is "
           << s/1000 << " KBytes\n"
           << " or " << s/(1000*(nobs+1))
           << " KBytes/observation\n";
  TEST("Size of track storage", s/1000 , 2501);
  vpl_unlink ("test_track_storage.tmp");

  vsl_b_ifstream sto_80("f:/videos/BrownBalloonI/refined-tracks-storage-4.trk");
  //vsl_b_ifstream sto_80("f:/videos/BrownBalloonI/short-house-sequence-track-storage-a-s-opt.trk");
  dbinfo_track_storage_sptr track_store_80 = dbinfo_track_storage_new();
  track_store_80->b_read(sto_80);
  sto_80.close();
  vcl_vector<dbinfo_track_sptr> tracks_80 = track_store_80->tracks();
  for(vcl_vector<dbinfo_track_sptr>::iterator trit = tracks_80.begin();
      trit != tracks_80.end(); ++trit)
    {
      vcl_vector<unsigned> frame;
      vcl_vector<float> x, y;
      track_points(*trit, x, y, frame);      
      float x0 = x[0], y0 = y[0];
      vcl_cout << "Track(" << x0 << ' ' << y0 << ")\n";
      vcl_vector<unsigned>::iterator fit=frame.begin();
      for(vcl_vector<float>::iterator xit = x.begin(), yit = y.begin();
          xit != x.end(); ++xit, ++yit, ++fit)
        vcl_cout << *fit << ' ' << x0-(*xit) << '\t' << y0-(*yit)<< '\n'; 
    }
}
TESTMAIN_ARGS(test_track_storage);
