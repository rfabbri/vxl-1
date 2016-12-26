// This is dbrec/tests/test_visitors.cxx
//:
// \file
// \brief Various tests for dbrec library
// \author Ozge C. Ozcanli (Brown)
// \date   May 01, 2009
//
#include <testlib/testlib_test.h>
#include <dbrec/dbrec_type_id_factory.h>
#include <dbrec/dbrec_gaussian_primitive.h>
#include <dbrec/dbrec_visitor.h>
#include <dbrec/dbrec_image_visitors.h>
#include <dbrec/dbrec_image_learner_visitors.h>
#include <dbrec/dbrec_image_models.h>
#include <dbrec/dbrec_image_pairwise_models.h>
#include <dbrec/dbrec_part_context.h>
#include <vil/vil_new.h>
#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <vil/vil_math.h>
#include <vil/vil_convert.h>

static void test_visitors()
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

  //: prepare appearance models to test parser
  dbrec_gaussian_appearance_model_sptr cfam = new dbrec_gaussian_weibull_appearance_model(2.1f, 1.1f);
  dbrec_gaussian_appearance_model_sptr ncfam = new dbrec_gaussian_weibull_appearance_model(1.1f, 1.5f);
  //: we want to model class-foreground, non-class-foreground, class-background, non-class background appearance 
  vcl_vector<dbrec_gaussian_appearance_model_sptr> app_models; 
  app_models.push_back(cfam); 
  app_models.push_back(ncfam); 
  
  //: for testing purposes make all the primitives' models the same
  g1->set_models(app_models);
  g2->set_models(app_models);
  g3->set_models(app_models);

  //: set up the prior models to be used for parsing
  float class_prior = 0.2f;
  
  //: set up test image and a random classifier
  //vil_image_view<float> test_img(100, 100); test_img.fill(10.0f);
  //vil_image_resource_sptr test_img_sptr = vil_new_image_resource_of_view(test_img);
  
  vil_image_resource_sptr test_img_byte_sptr = vil_load_image_resource("test_vehicle.png");
  vil_image_view<vxl_byte> test_img_byte = test_img_byte_sptr->get_view();
  unsigned ni = test_img_byte.ni();
  unsigned nj = test_img_byte.nj();

  vil_image_view<float> fg_prob(ni, nj);
  fg_prob.fill(0.8f);

  vil_image_view<float> test_img(ni, nj);
  vil_convert_cast(test_img_byte, test_img);
  vil_math_scale_values(test_img, 1.0/255.0);
  vil_image_resource_sptr test_img_sptr = vil_new_image_resource_of_view(test_img);
  dbrec_gaussian_factory* gf = new dbrec_gaussian_factory();
  gf->populate(4, 2.0f, 1.0f, true, false);
  dbrec_hierarchy_sptr h = gf->construct_random_classifier(2);

  //: for now use the same class_prior for both compositional parts and primitive parts
  vcl_vector<float> comp_priors; comp_priors.push_back(class_prior); comp_priors.push_back(1.0f-class_prior);
  dbrec_parse_image_visitor piv(h, test_img_sptr, class_prior, comp_priors);
  g1->accept(&piv);
  dbrec_part_context_sptr out_c1 = piv.context_factory()->get_context(g1->type());
  TEST("testing image parser visitor", !out_c1, false); 
  TEST("testing image parser visitor", out_c1->maps_size(), 2); 

  //: set up a background model for the view
  vil_image_view<float> mu_img(ni, nj); mu_img.fill(0.2f);
  vil_image_view<float> sigma_img(ni, nj); sigma_img.fill(0.3f);
  dbrec_image_construct_bg_model_visitor bgmv(mu_img, sigma_img, "./");
  for (unsigned i = 0; i < h->class_cnt(); i++) {
    h->root(i)->accept(&bgmv);
  }

  vcl_vector<float> comp_fg_bg_priors; 
  comp_fg_bg_priors.push_back(0.15f); comp_fg_bg_priors.push_back(0.15f); 
  comp_fg_bg_priors.push_back(0.15f); comp_fg_bg_priors.push_back(1.0f - (0.15f + 0.15f + 0.15f));
  dbrec_parse_image_with_fg_map_visitor pv(h, test_img_sptr, fg_prob, class_prior, comp_fg_bg_priors, "./", "./");

  pv.visit_gaussian_primitive(g1);
  dbrec_context_factory_sptr cf = pv.context_factory();
  dbrec_part_context_sptr out_c = cf->get_context(g1->type());
  TEST("testing image parser visitor", !out_c, false); 
  TEST("testing image parser visitor", out_c->maps_size(), 2); 
  
  vil_image_view<vxl_byte> out_byte(test_img.ni(), test_img.nj(), 3);

  vil_image_view<float> out_map2 = out_c->get_posterior_map(0, ni, nj)->get_view();
  dbrec_mark_receptive_fields_visitor mrfv2(out_c, out_map2);
  g1->accept(&mrfv2);
  mrfv2.get_colored_img(test_img_byte, out_byte);
  vil_save(out_byte, "./test_img_byte_overlayed_cf.png");

  vil_image_view<float> out_map3 = out_c->get_posterior_map(1, ni, nj)->get_view();
  dbrec_mark_receptive_fields_visitor mrfv3(out_c, out_map3);
  g1->accept(&mrfv3);
  mrfv3.get_colored_img(test_img_byte, out_byte);
  vil_save(out_byte, "./test_img_byte_overlayed_ncf.png");

  vil_image_view<float> out_map4 = out_c->get_posterior_map(2, ni, nj)->get_view();
  dbrec_mark_receptive_fields_visitor mrfv4(out_c, out_map4);
  g1->accept(&mrfv4);
  mrfv4.get_colored_img(test_img_byte, out_byte);
  vil_save(out_byte, "./test_img_byte_overlayed_cb.png");

  vil_image_view<float> out_map5 = out_c->get_posterior_map(3, ni, nj)->get_view();
  dbrec_mark_receptive_fields_visitor mrfv5(out_c, out_map5);
  g1->accept(&mrfv5);
  mrfv5.get_colored_img(test_img_byte, out_byte);
  vil_save(out_byte, "./test_img_byte_overlayed_ncb.png");

  //: test 
}

TESTMAIN( test_visitors );
