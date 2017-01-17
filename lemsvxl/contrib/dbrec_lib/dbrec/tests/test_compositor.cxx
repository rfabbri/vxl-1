// This is dbrec/tests/test_compositor.cxx
//:
// \file
// \brief Various tests for dbrec library
// \author Ozge C. Ozcanli (Brown)
// \date   April 29, 2009
//
#include <testlib/testlib_test.h>
#include <dbrec/dbrec_part.h>
#include <dbrec/dbrec_part_sptr.h>
#include <dbrec/dbrec_type_id_factory.h>
#include <dbrec/dbrec_gaussian_primitive.h>
#include <dbrec/dbrec_compositor.h>
#include <dbrec/dbrec_visitor.h>
#include <dbrec/dbrec_image_visitors.h>
#include <dbrec/dbrec_image_pairwise_models.h>
#include <vil/vil_save.h>

static void test_compositor()
{
  dbrec_pairwise_model_sptr pm1 = new dbrec_pairwise_indep_uniform_model(50.0f, 55.0f, 0.0f, (float)vnl_math::pi/2.0f);
  bsta_gaussian_sphere<double, 1> dist_model(100.0, 10.0);
  bsta_gaussian_sphere<double, 1> angle_model(vnl_math::pi, vnl_math::pi/100.0);
  dbrec_pairwise_model_sptr pm2 = new dbrec_pairwise_indep_gaussian_model(dist_model, angle_model);

  vcl_vector<dbrec_pairwise_model_sptr> models; models.push_back(pm1); models.push_back(pm2);
  dbrec_central_compositor* cc = new dbrec_central_compositor(models);

  dbrec_type_id_factory* ins = dbrec_type_id_factory::instance();
  dbrec_gaussian* g1 = new dbrec_gaussian(ins->new_type(), 2.0f, 1.0f, 45.0f, true);
  dbrec_gaussian* g2 = new dbrec_gaussian(ins->new_type(), 2.0f, 1.0f, 0.0f, true);
  dbrec_gaussian* g3 = new dbrec_gaussian(ins->new_type(), 1.0f, 1.0f, 0.0f, true);
  vcl_vector<dbrec_part_sptr> parts; parts.push_back(g1); parts.push_back(g2); parts.push_back(g3);
  dbrec_composition* c = new dbrec_composition(ins->new_type(), parts, cc, 10.0f);

  dbrec_draw_bsvg_visitor v(200.0f, 200.0f, 100.0f, 100.0f, 10.0f, "blue");
  v.visit_composition(c);
  v.write("./central_compositor.svg");  

  //: test discrete pairwise model
  dbrec_pairwise_discrete_model* dpdm = new dbrec_pairwise_discrete_model(8, 0.0f, 10.0f, 10);

  vnl_vector_fixed<float, 2> central_part_direction_vec; //(1.0f, 0.0f);
  g2->get_direction_vector(central_part_direction_vec);
  vgl_point_2d<float> cpl(10.0f, 10.0f);  vnl_vector_fixed<float, 2> central_part_loc(cpl.x(), cpl.y());  
  vgl_point_2d<float> spl1(10.0f, 12.0f); vnl_vector_fixed<float, 2> second_part_loc1(spl1.x(), spl1.y());  
  vgl_point_2d<float> spl2(10.0f, 13.0f); vnl_vector_fixed<float, 2> second_part_loc2(spl2.x(), spl2.y());  
  vgl_point_2d<float> spl3(10.0f, 14.0f); vnl_vector_fixed<float, 2> second_part_loc3(spl3.x(), spl3.y());  
  vgl_point_2d<float> spl4(10.0f, 15.0f); vnl_vector_fixed<float, 2> second_part_loc4(spl4.x(), spl4.y());  
  dpdm->update_models(central_part_direction_vec, central_part_loc, second_part_loc1);
  dpdm->update_models(central_part_direction_vec, central_part_loc, second_part_loc1);
  dpdm->update_models(central_part_direction_vec, central_part_loc, second_part_loc2);
  dpdm->update_models(central_part_direction_vec, central_part_loc, second_part_loc3);
  dpdm->update_models(central_part_direction_vec, central_part_loc, second_part_loc4);

  //: visualize
  vcl_ofstream out_vrml("./test_hist.vrml");
  dpdm->print_hist_to_vrml(out_vrml);
  out_vrml.close();

  TEST_NEAR("test discrete model", dpdm->prob_density(central_part_direction_vec, cpl, spl1), 2.0f/5.0f, 0.001f);
  TEST_NEAR("test discrete model", dpdm->prob_density(central_part_direction_vec, cpl, spl4), 1.0f/5.0f, 0.001f);
  TEST_NEAR("test discrete model", dpdm->prob_density(central_part_direction_vec, cpl, cpl), 0.0f, 0.001f);

  //: create a probe box using the model given the first part's location
  vgl_box_2d<float> probe_box(0.0f, 20.0f, 0.0f, 20.0f);
  TEST("test probe box", dpdm->get_probe_box(cpl, central_part_direction_vec), probe_box);

  //: test sample method by drawing a sampled composed model
  dbrec_pairwise_model_sptr dpdmpm = dpdm;
  models.clear(); models.push_back(dpdmpm);
  dbrec_central_compositor* cc2 = new dbrec_central_compositor(models);

  parts.clear(); parts.push_back(g2); parts.push_back(g3);
  dbrec_composition* c2 = new dbrec_composition(ins->new_type(), parts, cc2, 10.0f);

  dbrec_draw_bsvg_visitor v2(200.0f, 200.0f, 100.0f, 100.0f, 10.0f, "blue");
  v2.visit_composition(c2);
  v2.write("./central_compositor2.svg");  

  // test the pairwise model factory
  dbrec_pairwise_discrete_model_factory mf(8, 0.0f, 10.0f, 10);
  dbrec_pairwise_model_sptr mod = mf.generate_model();
  TEST("testing model factory", !mod, false);
  
  dbrec_gaussian_factory* gf = new dbrec_gaussian_factory();
  gf->populate(4, 2.0f, 1.0f, true, false);
  dbrec_hierarchy_sptr orig_h = gf->construct_random_classifier(2);
  dbrec_composition* co = dynamic_cast<dbrec_composition*>(orig_h->root(0).ptr());
  unsigned nco = co->children().size();
  dbrec_hierarchy_sptr new_h = populate_depth_central_compositor(orig_h, 1, 10.0f, mf);
  TEST("testing populate_central_compositor", !new_h, false);
  TEST("testing populate_central_compositor", new_h->class_cnt(), 2);
  dbrec_composition* nc = dynamic_cast<dbrec_composition*>(new_h->root(0).ptr());
  TEST_NEAR("testing populate_central_compositor", nc->children().size(), nco*nco, 0.1);

  //: test pairwise compositor  
  dbrec_gaussian* gg1 = new dbrec_gaussian(ins->new_type(), 6.0f, 3.0f, 0.0f, true);
  dbrec_gaussian* gg2 = new dbrec_gaussian(ins->new_type(), 6.0f, 3.0f, 0.0f, true);

  //: find alpha range for these two 

  dbrec_pairwise_rot_invariant_model* prim = new dbrec_pairwise_rot_invariant_model(30, 35, 80, 100, 180, 40, 50, 180, 5, 9);
  dbrec_pairwise_compositor* paircomp = new dbrec_pairwise_compositor(prim);

  parts.clear(); parts.push_back(gg1); parts.push_back(gg2);
  dbrec_composition* test_c = new dbrec_composition(ins->new_type(), parts, paircomp, 10.0f);

  // test sampling and drawing parts
  int ni = 500; int nj = 500;
  vil_image_view<vxl_byte> img(ni, nj, 1);  
  //img.fill(255);  
  img.fill(0);

  int sampling_cnt = 5; 
  vnl_random rng;
  for (int k = 0; k < sampling_cnt; k++) { // sample a location and angle in the image
    int i = rng.lrand32(10, ni-10); int j = rng.lrand32(10, nj-10);
    //int angle = rng.lrand32(0, 359);
    int angle = 0;
    //vcl_cout << "angle: " << angle << vcl_endl;
    dbrec_sample_and_draw_part_visitor sdpv(img, i, j, angle, 1.0f, rng);
    test_c->accept(&sdpv);
  }
  vil_save(img, "sampled_img_composition_6_3_angle_0.png");


}

TESTMAIN( test_compositor );
