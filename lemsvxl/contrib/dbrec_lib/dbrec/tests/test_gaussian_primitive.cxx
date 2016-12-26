// This is dbrec/tests/test_gaussian_primitive.cxx
//:
// \file
// \brief Various tests for dbrec library
// \author Ozge C. Ozcanli (Brown)
// \date   April 23, 2009
//
#include <testlib/testlib_test.h>
#include <dbrec/dbrec_part.h>
#include <dbrec/dbrec_part_sptr.h>
#include <dbrec/dbrec_type_id_factory.h>
#include <dbrec/dbrec_gaussian_primitive.h>
#include <dbrec/dbrec_image_compositor.h>
#include <dbrec/dbrec_image_visitors.h>
#include <vcl_limits.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>
#include <vil/algo/vil_threshold.h>
#include <brip/brip_vil_float_ops.h>

static void test_gaussian_primitive()
{
  dbrec_type_id_factory* ins = dbrec_type_id_factory::instance();
  //: now test the concrete part classes
  dbrec_gaussian g1(ins->new_type(), 1.0f, 1.0f, 0.0f, true);
  dbrec_part_sptr g2 = new dbrec_gaussian(g1);
  TEST("part constructor", g2->type() == g1.type(), true);

  //: check methods
  dbrec_gaussian* gop = new dbrec_gaussian(ins->new_type(), 1.0f, 2.0f, 90.0f, false);
  
  vnl_vector_fixed<float,2> v; vnl_vector_fixed<float,2> out_v; out_v(0) = -1.0f; out_v(1) = 0.0f;
  gop->get_direction_vector(v); 
  vcl_cout << "direction vector: " << v << vcl_endl;
  TEST("get direction vector", v == out_v, true);
  
  dbrec_part_sptr g3 = gop;
  vcl_vector<dbrec_part_sptr> parts; parts.push_back(g2); parts.push_back(g3);
  dbrec_composition c(ins->new_type(), parts, new dbrec_or_compositor, 10.0f);

  vcl_cout << c;
  float lambda0 = 2.0f; float lambda1 = 1.0f; float theta = 45.0f; bool brightness = true;
  dbrec_gaussian* gop4  = new dbrec_gaussian(ins->new_type(), lambda0, lambda1, theta, brightness);
  //gop4->set_cutoff_percentage(0.1f);
  
  out_v(0) = (float)vcl_cos(45*vnl_math::pi/180.0); out_v(1) = (float)vcl_sin(45*vnl_math::pi/180.0);
  gop4->get_direction_vector(v); 
  vcl_cout << "direction vector: " << v << vcl_endl;
  TEST("get direction vector", (v(0)-out_v(0) < vcl_numeric_limits<float>::epsilon())&&(v(1)-out_v(1) < vcl_numeric_limits<float>::epsilon()), true);

  dbrec_part_sptr g4 = gop4;
  dbrec_gaussian g5(ins->new_type(), 1.0f, 1.0f, 0.0f, true);
  TEST("operator equality", g5.equal(g1), true);

  dbrec_gaussian_factory* pf = new dbrec_gaussian_factory();
  pf->add(1.0f, 1.0f, 0.0f, false, false);
  pf->add(1.0f, 1.0f, 0.0f, true, false);
  pf->populate(4, 2.0f, 0.5f, true, false);

  vcl_cout << *pf;

  // test sampling and drawing parts
  int ni = 100; int nj = 100;
  vil_image_view<vxl_byte> img(ni, nj, 1);  
  //img.fill(255);
  img.fill(0);

  int sampling_cnt = 10; 
  vnl_random rng;
  for (int k = 0; k < sampling_cnt; k++) { // sample a location and angle in the image
    int i = rng.lrand32(0, ni-1); int j = rng.lrand32(0, nj-1);
    //int angle = rng.lrand32(0, 359);
    int angle = 0;
    dbrec_sample_and_draw_part_visitor sdpv(img, i, j, angle, 1.0f, rng);
    g4->accept(&sdpv);
  }
  vil_save(img, "sampled_img.png");

  //: test fg_prob_operator_rotational() method
  img.fill(0);
  dbrec_sample_and_draw_part_visitor sdpv(img, 50, 50, 45, 1.0f, rng);  // reduces to 2,1,90
  g4->accept(&sdpv);

  vil_image_view<bool> img_t;
  vil_threshold_above(img, img_t, (vxl_byte)128);
  vil_image_view<float> img_tf(img.ni(), img.nj());
  vil_convert_cast(img_t, img_tf);
  TEST_NEAR("testing fg_prob_operator_rotational", gop4->fg_prob_operator_rotational(img_tf, 0, 0, 90), 0.0f, 0.001);
  TEST_NEAR("testing fg_prob_operator_rotational", gop4->fg_prob_operator_rotational(img_tf, 50, 50, 90), 1.0f, 0.001);

}

TESTMAIN( test_gaussian_primitive );

#if 0
  vil_image_view_base_sptr img_sptr = new vil_image_view<vxl_byte>(img);
  vil_image_view<float> fimg = *vil_convert_cast(float(), img_sptr);
  vil_math_scale_values(fimg,1.0/255.0);

  vbl_array_2d<float> fa;
  vbl_array_2d<bool> maskarr;
  brip_vil_float_ops::extrema_kernel_mask(lambda0, lambda1, theta,
                                          fa, maskarr);
  vcl_cout << fa;

  vil_image_view<float> extr;
 // if (gop4->fast()) 
 //   extr = brip_vil_float_ops::fast_extrema(fimg, lambda0, lambda1, theta, brightness, true);
 // else
    extr = brip_vil_float_ops::extrema(fimg, lambda0, lambda1, theta, brightness, true, true);
  vil_image_view<float> res(ni, nj), unclipped(ni, nj), mask(ni, nj); vil_image_view<vxl_byte> overlayed(ni, nj, 3);
  for(unsigned j = 0; j<nj; ++j) {
    for(unsigned i = 0; i<ni; ++i)
      {
        res(i,j) = extr(i,j,0);
        mask(i,j) = extr(i,j,1);
        unclipped(i,j) = extr(i,j,2);
        
        overlayed(i,j,0) = (vxl_byte)(fimg(i,j)*255);
        overlayed(i,j,1) = (vxl_byte)(res(i,j)*255);
        overlayed(i,j,2) = (vxl_byte)(mask(i,j)*255);
        if (i == 50 && j == 50)
          vcl_cout << "res at 50, 50: " << res(i,j) << " unc value: " << unclipped(i,j) << " overlayed g: " << (int)overlayed(i,j,1) << vcl_endl;
        if (i == 46 && j == 54)
          vcl_cout << "res at 46, 54: " << res(i,j) << " unc value: " << unclipped(i,j) << " overlayed g: " << (int)overlayed(i,j,1) << vcl_endl;
        if (i == 54 && j == 46)
          vcl_cout << "res at 54, 46: " << res(i,j) << " unc value: " << unclipped(i,j) << " overlayed g: " << (int)overlayed(i,j,1) << vcl_endl;
      }
  }
  
  vil_save(overlayed, "sampled_img_extr.png");
#endif
