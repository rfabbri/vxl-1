// This is dbrec/tests/test_part_selector.cxx
//:
// \file
// \brief Various tests for dbrec library
// \author Ozge C. Ozcanli (Brown)
// \date   April 23, 2009
//
#include <testlib/testlib_test.h>
#include <dbrec/dbrec_part_selector.h>
#include <dbrec/dbrec_gaussian_primitive.h>
#include <dbrec/dbrec_image_models.h>
#include <dbrec/dbrec_image_visitors.h>
#include <dbfs/dbfs_selector.h>
#include <dbfs/dbfs_selector_sptr.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_new.h>
#include <vil/vil_convert.h>
#include <vil/algo/vil_threshold.h>

static void test_part_selector()
{
  //: test parsing an image
  vil_image_resource_sptr img_r = vil_load_image_resource("./test_vehicle.png");
  vil_image_resource_sptr img_m = vil_load_image_resource("./test_vehicle_mask.png");
  if (!img_r || !img_m) {
    vcl_cout << "problems loading test_vehicle.png";
    return;
  }
  vil_image_view_base_sptr inp_img = img_r->get_view();
  vil_image_view<vxl_byte> orig_img(img_r->get_view());
  if (orig_img.nplanes() == 3) {
    vil_image_view<vxl_byte> *out_img = new vil_image_view<vxl_byte>(img_r->ni(),img_r->nj());
    vil_convert_planes_to_grey<vxl_byte, vxl_byte>(orig_img,*out_img);
    inp_img = out_img;
  }
  
  vil_image_view<float> img = *vil_convert_cast(float(), inp_img);
  vil_math_scale_values(img,1.0/255.0);
  vil_image_resource_sptr img_scaled = vil_new_image_resource_of_view(img);

  //: object regions are black in the image so threshold below
  vil_image_view<vxl_byte> img_mask(img_m->get_view());
  vil_image_view<bool> gt_mask(img_r->ni(), img_r->nj());
  vil_threshold_below(img_mask, gt_mask, (vxl_byte)128);
  vil_image_view<float> gt_map(gt_mask.ni(), gt_mask.nj());
  vil_convert_cast(gt_mask, gt_map);
  vil_image_view<bool> valid_region_mask(gt_mask.ni(), gt_mask.nj());
  valid_region_mask.fill(true);

  unsigned type_id = 0;
  dbrec_gaussian* g1 = new dbrec_gaussian(type_id, 2.0f, 2.0f, 0.0f, true);
  dbrec_part_sptr g1p = g1;

  dbrec_gaussian_appearance_model_sptr m_class = new dbrec_gaussian_weibull_appearance_model(0.0159533f, 1.69568f);
  dbrec_gaussian_appearance_model_sptr m_non_class = new dbrec_gaussian_weibull_appearance_model(0.00759679f, 7.64067f);
  vcl_vector<dbrec_gaussian_appearance_model_sptr> models;
  models.push_back(m_class); models.push_back(m_non_class);
  g1->set_models(models);
  
  //: we need a parse visitor here
  dbrec_hierarchy_sptr h = new dbrec_hierarchy; // just an empty dummy hierarchy to construct visitor instance
  float class_prior = 0.15f; vcl_vector<float> comp_priors; comp_priors.push_back(class_prior); comp_priors.push_back(1.0f-class_prior);
  dbrec_parse_image_visitor pv(h, img_scaled, class_prior, comp_priors, "");
  g1->accept(&pv);

  //: construct a second part
  unsigned type_id2 = 80;
  dbrec_gaussian* g2 = new dbrec_gaussian(type_id2, 1.0f, 1.0f, 0.0f, true);
  dbrec_part_sptr g2p = g2;

  dbrec_gaussian_appearance_model_sptr m_class2 = new dbrec_gaussian_weibull_appearance_model(0.0537034f, 1.53269f);
  dbrec_gaussian_appearance_model_sptr m_non_class2 = new dbrec_gaussian_weibull_appearance_model(0.0320286f, 7.44504f);
  vcl_vector<dbrec_gaussian_appearance_model_sptr> models2;
  models2.push_back(m_class2); models2.push_back(m_non_class2);
  g2->set_models(models2);

  g2->accept(&pv);
  
  dbrec_context_factory_sptr cf = pv.context_factory();
  TEST("test parse imge context factory push", !cf, false);
  
  dbrec_part_context_sptr c = cf->get_context(type_id);
  TEST("test context factory get_context", !c, false);
  dbrec_part_context_sptr c2 = cf->get_context(type_id2);
  TEST("test context factory get_context", !c2, false);
  
  vil_image_resource_sptr cfm = c->get_posterior_map(0, img.ni(), img.nj());
  vil_image_view<float> out = cfm->get_view();
  dbrec_mark_receptive_fields_visitor mrfv(c, out);
  g1->accept(&mrfv);
  vil_image_view<vxl_byte> out_colored_img(img.ni(), img.nj(), 3);
  mrfv.get_colored_img(inp_img, out_colored_img);
  vil_save(out_colored_img, "./test_vehicle_g1_out_map.png");

  vil_image_resource_sptr cfm2 = c2->get_posterior_map(0, img.ni(), img.nj());
  vil_image_view<float> out2 = cfm2->get_view();
  dbrec_mark_receptive_fields_visitor mrfv2(c2, out2);
  g2->accept(&mrfv2);
  vil_image_view<vxl_byte> out_colored_img2(img.ni(), img.nj(), 3);
  mrfv2.get_colored_img(inp_img, out_colored_img2);
  vil_save(out_colored_img2, "./test_vehicle_g2_out_map.png");

  vcl_vector<dbrec_part_sptr> parts; parts.push_back(g1p); parts.push_back(g2p);
  dbrec_part_selection_measure_sptr sm = new dbrec_part_selection_measure(parts, 2);

  sm->measure_training_image(cf, valid_region_mask, gt_map, 0);
  vcl_cout << "feature mutual info: " << sm->mutual_info(0, type_id) << vcl_endl;
  vcl_cout << "feature, class prob: " << sm->prob(0, type_id) << vcl_endl;
  vcl_cout << "feature prob: " << sm->prob_feature(type_id) << vcl_endl;
  vcl_cout << "class prob: " << sm->prob_class(0) << vcl_endl;
  
  vcl_cout << "feature2 mutual info: " << sm->mutual_info(0, type_id2) << vcl_endl;
  vcl_cout << "feature2, class prob: " << sm->prob(0, type_id2) << vcl_endl;
  vcl_cout << "feature2 prob: " << sm->prob_feature(type_id2) << vcl_endl;

  dbfs_measure_sptr smm = (sm.ptr());
  vcl_vector<int> features; features.push_back(type_id); features.push_back(type_id2);
  dbfs_selector_sptr s = new dbfs_selector(smm, features, 2); // 3 features and 2 classes
  vcl_vector<int> best_feature_ids;
  s->get_top_features(0, 1, best_feature_ids); 
  TEST("testing selector", best_feature_ids.size(), 1);
  TEST("testing selector", best_feature_ids[0], type_id);

  best_feature_ids.clear();
  s->get_top_features(0, 2, best_feature_ids);
  TEST("testing selector", best_feature_ids.size(), 2);
  TEST("testing selector", best_feature_ids[0], type_id);
  TEST("testing selector", best_feature_ids[1], type_id2);

  dbrec_hierarchy_sptr h2 = new dbrec_hierarchy();
  vcl_vector<dbrec_part_sptr> ch1; ch1.push_back(g1p); ch1.push_back(g2p);
  dbrec_part_sptr root1 = new dbrec_composition(200, ch1, new dbrec_or_compositor(), 10.0f);
  h2->add_root(root1);
  dbrec_part_sptr root2 = new dbrec_composition(201, ch1, new dbrec_or_compositor(), 10.0f);
  h2->add_root(root2);
  dbrec_part_selector ss(sm, dbrec_part_selector_algos::max_class_mutual_info, parts, h2, 2);

  vcl_vector<dbrec_part_sptr> best_parts;
  ss.get_top_features(0, 1, best_parts);
  TEST("testing dbrec selector", best_parts.size(), 1);
  TEST("testing dbrec selector", best_parts[0]->type(), type_id);

  ss.get_top_features(0, 2, best_parts);
  TEST("testing dbrec selector", best_parts.size(), 2);
  TEST("testing dbrec selector", best_parts[0]->type(), type_id);
  TEST("testing dbrec selector", best_parts[1]->type(), type_id2);

}

TESTMAIN( test_part_selector );
