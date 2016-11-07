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
#include <dbrec/dbrec_image_hierarchy_factory.h>
#include <vil/vil_save.h>
#include <vil/vil_new.h>
#include <vil/vil_convert.h>

static void test_rot_inv()
{
  dbrec_hierarchy_sptr h = dbrec_image_hierarchy_factory::construct_detector_rot_inv_1();
  dbrec_part_sptr comp1 = h->get_part(11);
  dbrec_part_sptr comp2 = h->get_part(12);
  dbrec_part_sptr comp3 = h->get_part(13);
  dbrec_part_sptr g1 = h->get_part(1);
  dbrec_part_sptr g2 = h->get_part(2);

  // test sampling and drawing parts
  int ni = 200; int nj = 200;
  vil_image_view<vxl_byte> img(ni, nj, 1);  
  img.fill(0);

  vnl_random rng;
  int i = 100, j = 100;
  int angle = 180;
  dbrec_sample_and_draw_part_visitor sdpv(img, i, j, angle, 0.8f, rng);
  comp1->accept(&sdpv);
  vcl_stringstream angle_str; angle_str << angle;
  vcl_string name = "sampled_comp1_"+angle_str.str()+".png";
  vil_save(img, name.c_str());

  vil_image_view_base_sptr img_bptr = new vil_image_view<vxl_byte>(img);
  vil_image_view<float> img_f = *vil_convert_cast(float(), img_bptr);
  vil_math_scale_values(img_f,1.0/255.0);
  vil_image_resource_sptr img_sptr = vil_new_image_resource_of_view(img_f);
  vil_image_view<bool> valid_region_mask(img_sptr->ni(), img_sptr->nj());
  valid_region_mask.fill(true);
  dbrec_parse_image_rot_inv_visitor pv(h, img_sptr, valid_region_mask, 0.1f, 15.0f, "./");
  comp1->accept(&pv);
  dbrec_context_factory_sptr cf = pv.context_factory();
  dbrec_part_context_sptr cont1 = cf->get_context(comp1->type());
  dbrec_part_context_sptr cont_g1 = cf->get_context(g1->type());
  dbrec_part_context_sptr cont_g2 = cf->get_context(g2->type());

  vil_image_resource_sptr cfm = cont_g1->get_posterior_map(0, img.ni(), img.nj());
  if (cfm) {
    vil_image_view<float> out = cfm->get_view();
    dbrec_mark_receptive_fields_visitor mrfv(cont_g1, out);
    g1->accept(&mrfv);
    vil_image_view<vxl_byte> out_colored_img(img.ni(), img.nj(), 3);
    mrfv.get_colored_img(img, out_colored_img);
    name = "sampled_comp1_g1_posterior_"+angle_str.str()+".png";
    //vil_save(out_colored_img, name.c_str());
  }
  
  cfm = cont1->get_posterior_map(0, img.ni(), img.nj());
  if (cfm) {
    vil_image_view<float> out = cfm->get_view();
    dbrec_mark_receptive_fields_visitor mrfv1(cont1, out);
    comp1->accept(&mrfv1);
    vil_image_view<vxl_byte> out_colored_img1(img.ni(), img.nj(), 3);
    mrfv1.get_colored_img(img, out_colored_img1);
    name = "sampled_comp1_posterior_"+angle_str.str()+".png";
    //vil_save(out_colored_img1, name.c_str());
  }
  
  TEST("testing posterior computation" , cont1->size(), 1);

  img.fill(0);
  angle = 40;
  dbrec_sample_and_draw_part_visitor sdpv2(img, i, j, angle, 0.8f, rng);
  comp2->accept(&sdpv2);
  vcl_stringstream angle_str2; angle_str2 << angle;
  name = "sampled_comp2_"+angle_str2.str()+".png";
  vil_save(img, name.c_str());

  img_bptr = new vil_image_view<vxl_byte>(img);
  vil_image_view<float> img_f2 = *vil_convert_cast(float(), img_bptr);
  vil_math_scale_values(img_f2,1.0/255.0);
  img_sptr = vil_new_image_resource_of_view(img_f2);
  dbrec_parse_image_rot_inv_visitor pv2(h, img_sptr, valid_region_mask, 0.1f, 15.0f, "./");
  comp2->accept(&pv2);
  cf = pv2.context_factory();
  dbrec_part_context_sptr cont2 = cf->get_context(comp2->type());
  cont_g1 = cf->get_context(g1->type());
  cont_g2 = cf->get_context(g2->type());

  cfm = cont_g2->get_posterior_map(0, img.ni(), img.nj());
  if (cfm) {
    vil_image_view<float> out = cfm->get_view();
    dbrec_mark_receptive_fields_visitor mrfv(cont_g2, out);
    g2->accept(&mrfv);
    vil_image_view<vxl_byte> out_colored_img(img.ni(), img.nj(), 3);
    mrfv.get_colored_img(img, out_colored_img);
    name = "sampled_comp2_g2_posterior_"+angle_str2.str()+".png";
    //vil_save(out_colored_img, name.c_str());
  }
  
  cfm = cont2->get_posterior_map(0, img.ni(), img.nj());
  if (cfm) {
    vil_image_view<float> out = cfm->get_view();
    dbrec_mark_receptive_fields_visitor mrfv1(cont2, out);
    comp2->accept(&mrfv1);
    vil_image_view<vxl_byte> out_colored_img1(img.ni(), img.nj(), 3);
    mrfv1.get_colored_img(img, out_colored_img1);
    name = "sampled_comp2_posterior_"+angle_str2.str()+".png";
    //vil_save(out_colored_img1, name.c_str());
  }
  
  TEST("testing posterior computation" , cont2->size(), 1);

  img.fill(0);
  angle = 45;
  dbrec_sample_and_draw_part_visitor sdpv3(img, i, j, angle, 0.8f, rng);
  comp3->accept(&sdpv3);
  vcl_stringstream angle_str3; angle_str3 << angle;
  name = "sampled_comp3_"+angle_str3.str()+".png";
  vil_save(img, name.c_str());

  img_bptr = new vil_image_view<vxl_byte>(img);
  vil_image_view<float> img_f3 = *vil_convert_cast(float(), img_bptr);
  vil_math_scale_values(img_f3,1.0/255.0);
  img_sptr = vil_new_image_resource_of_view(img_f3);
  dbrec_parse_image_rot_inv_visitor pv3(h, img_sptr, valid_region_mask, 0.1f, 15.0f, "./");
  comp3->accept(&pv3);
  cf = pv3.context_factory();
  dbrec_part_context_sptr cont3 = cf->get_context(comp3->type());
  cont_g1 = cf->get_context(g1->type());
  cont_g2 = cf->get_context(g2->type());

  cfm = cont_g2->get_posterior_map(0, img.ni(), img.nj());
  if (cfm) {
    vil_image_view<float> out = cfm->get_view();
    dbrec_mark_receptive_fields_visitor mrfv(cont_g2, out);
    g2->accept(&mrfv);
    vil_image_view<vxl_byte> out_colored_img(img.ni(), img.nj(), 3);
    mrfv.get_colored_img(img, out_colored_img);
    name = "sampled_comp3_g2_posterior_"+angle_str3.str()+".png";
    //vil_save(out_colored_img, name.c_str());
  }
  
  cfm = cont3->get_posterior_map(0, img.ni(), img.nj());
  if (cfm) {
    vil_image_view<float> out = cfm->get_view();
    dbrec_mark_receptive_fields_visitor mrfv1(cont3, out);
    comp3->accept(&mrfv1);
    vil_image_view<vxl_byte> out_colored_img1(img.ni(), img.nj(), 3);
    mrfv1.get_colored_img(img, out_colored_img1);
    name = "sampled_comp3_posterior_"+angle_str3.str()+".png";
    //vil_save(out_colored_img1, name.c_str());
  }
  
  TEST_NEAR("testing posterior computation" , cont3->size(), 1, 0.1);
  
  //: test parsing
  dbrec_hierarchy_sptr hroi = dbrec_image_hierarchy_factory::construct_detector_rot_inv_roi1_0();
  dbrec_write_xml_visitor wxml;
  for (unsigned i = 0; i < hroi->class_cnt(); i++)
    hroi->root(i)->accept(&wxml);
  vcl_string test_name = "test.xml";
  wxml.write_doc(test_name);

  dbrec_parse_hierarchy_xml parser;
  TEST("testing parsing", parser.parse(test_name.c_str()), true);
  dbrec_hierarchy_sptr parsed_h = parser.get_hierarchy();

  //: test the measure foreground prob visitor for rotationally invariant parts
  //vil_image_view<float> fg_prob(ni, nj, 1);
  //fg_prob.fill(1.0f);

}

TESTMAIN( test_rot_inv );
