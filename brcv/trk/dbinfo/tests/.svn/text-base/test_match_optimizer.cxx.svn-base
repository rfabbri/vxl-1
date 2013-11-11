// This is brl/bseg/dbinfo/tests/test_tracking_face_2d.cxx
#include <testlib/testlib_test.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <dbinfo/dbinfo_region_geometry.h>
#include <vbl/vbl_array_2d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
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
#include <bsol/bsol_algs.h>
#include <brip/brip_vil_float_ops.h>
#include <dbinfo/dbinfo_feature_format.h>
#include <dbinfo/dbinfo_feature_data.h>
#include <dbinfo/dbinfo_intensity_feature.h>
#include <dbinfo/dbinfo_gradient_feature.h>
#include <dbinfo/dbinfo_observation.h>
#include <dbinfo/dbinfo_observation_generator.h>
#include <dbinfo/dbinfo_observation_matcher.h>
#include <dbinfo/dbinfo_match_optimizer.h>
vsol_polygon_2d_sptr gen_poly()
{
  vsol_point_2d_sptr p0 = new vsol_point_2d(32, 32);
  vsol_point_2d_sptr p1 = new vsol_point_2d(32, 96);
  vsol_point_2d_sptr p2 = new vsol_point_2d(96, 96);
  vsol_point_2d_sptr p3 = new vsol_point_2d(96, 32);

  vcl_vector<vsol_point_2d_sptr> verts;
  verts.push_back(p0);   verts.push_back(p1);  verts.push_back(p2); 
  verts.push_back(p3); 
  vsol_polygon_2d_sptr poly = new vsol_polygon_2d(verts);
  return poly;
}

dbinfo_observation_sptr gen_observ(unsigned frame, vsol_polygon_2d_sptr const& poly,
                                   vil_image_resource_sptr const& imgr)
{
  dbinfo_observation_sptr obs = new dbinfo_observation(frame, imgr, poly);
  dbinfo_feature_base_sptr intf = new dbinfo_intensity_feature();
  dbinfo_feature_base_sptr gradf = new dbinfo_gradient_feature();
  vcl_vector<dbinfo_feature_base_sptr> features;
  features.push_back(intf);   features.push_back(gradf);
  obs->set_features(features);
  obs->scan(0, imgr);
  return obs;
}

dbinfo_observation_sptr gen_observ(unsigned frame,
                                   dbinfo_observation_sptr const& seed,
                                   vil_image_resource_sptr const& imgr,
                                   const float tx, const float ty, 
                                   const float theta=0, const float scale = 1.0
                               )
{
  vgl_h_matrix_2d<float> H;
  H.set_identity();
  H.set_rotation(theta);
  H.set_scale(scale);
  H.set_translation(tx, ty);
  dbinfo_observation_sptr obs = dbinfo_observation_generator::generate(seed,H);
  obs->scan(frame, imgr);
  return obs;
}

void info_scan(vil_image_resource_sptr imgr)
{

 vsol_polygon_2d_sptr poly0 = gen_poly();
 dbinfo_observation_sptr obs0 = gen_observ(0,poly0, imgr);
  for(float dx = -5.0; dx<5.0f; dx++)
    for(float theta = -0.5f; theta<0.5f; theta+=0.05f)
    {
      dbinfo_observation_sptr seed = gen_observ(1, obs0, imgr, dx, dx, theta);
      float mi = dbinfo_observation_matcher::minfo(obs0, seed);
      vcl_cout << "InitialCost[" << dx << ' ' << theta << "]= " << 10.0 - mi << '\n';
      dbinfo_match_optimizer opt(10, 1.0f, 1.0f, 0.05f);
      opt.set_debug_level(0);
      opt.set_frame(1);
      opt.set_resource(imgr);
      opt.set_obs0(obs0);
      opt.set_best_seed(seed);
      if(!opt.optimize())
        {
          vcl_cout << "Optimize failed \n";
          return;
        }
      dbinfo_observation_sptr final = opt.optimized_obs();
      dbinfo_region_geometry_sptr reg = final->geometry();
      vcl_vector<double> p = opt.current_params();
      vcl_cout << "Parameters (" << p[0] << ' ' << p[1] << ' ' << p[2] <<")\n";
      vcl_cout << "Final Cost " << opt.current_cost() << '\n';
      vcl_cout << "optimized cog " << *(reg->cog()) << '\n';
    }      
}

static void test_match_optimizer(int argc, char* argv[])
{
  //
  //====================  OBSERVATION  ================================
  //
  //Test image for observation
  unsigned cols = 128,  rows = 128;
  vil_image_view<unsigned char> gimage(cols, rows, 1);
  gimage.fill(0);
  for(unsigned r = 32; r<96; ++r)
    for(unsigned c = 32; c<96; ++c)
      {
        //unused float fr = static_cast<float>(r), fc = static_cast<float>(c);
        float v = fabs(static_cast<float>((64-r)*(64-c)*255.0/1024.0));
        gimage(c,r) = static_cast<unsigned char>(v);
      }
  vil_image_resource_sptr imgr = vil_new_image_resource_of_view(gimage);

  // Test cost variation
#if 0
  info_scan(imgr);
#endif
  //Define a displaced initial window
  float theta = 0.1f;
 vsol_polygon_2d_sptr poly0 = gen_poly();
 dbinfo_observation_sptr obs0 = gen_observ(0,poly0, imgr);
  dbinfo_observation_sptr seed = gen_observ(1, obs0, imgr, 3, 4, theta);
  
  vcl_cout << "master geometry " << *(obs0->geometry()) << '\n';
  vcl_cout << "seed geometry " << *(seed->geometry()) << '\n';
  vcl_cout << "radius " << (obs0->geometry()->diameter())/2.0 << '\n';  
  dbinfo_match_optimizer opt(10, 1.0f, 0.05f);
  opt.set_debug_level(3);
  opt.set_frame(1);
  opt.set_resource(imgr);
  opt.set_obs0(obs0);
  opt.set_best_seed(seed);
  if(!opt.optimize())
    {
      vcl_cout << "Optimize failed \n";
      return;
    }
  vcl_cout << "Number of parameters " << opt.no_of_parameters() << '\n';
  dbinfo_observation_sptr final = opt.optimized_obs();
  dbinfo_region_geometry_sptr reg = final->geometry();
  vcl_cout << "optimized cog " << *(reg->cog()) << '\n';
  //Now test for final cost

  TEST_NEAR("Final Cost ", opt.current_cost(),  4.28882 , 1.0e-4);

}
TESTMAIN_ARGS(test_match_optimizer);
