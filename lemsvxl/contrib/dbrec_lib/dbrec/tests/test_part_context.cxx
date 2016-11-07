// This is dbrec/tests/test_part_context.cxx
//:
// \file
// \brief Various tests for dbrec library
// \author Ozge C. Ozcanli (Brown)
// \date   April 23, 2009
//
#include <testlib/testlib_test.h>
#include <dbrec/dbrec_part.h>
#include <dbrec/dbrec_part_sptr.h>
#include <dbrec/dbrec_part_context.h>
#include <dbrec/dbrec_gaussian_primitive.h>
#include <dbrec/dbrec_image_visitors.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_new.h>
#include <vil/vil_convert.h>

static void test_part_context()
{
  dbrec_part_context* pc = new dbrec_rtree_context();
  TEST("context constructor", !pc->first(), true);

  dbrec_part_ins_sptr pi1 = new dbrec_part_ins(1, 10.0f, 15.0f);
  dbrec_part_ins_sptr pi2 = new dbrec_part_ins(2, 20.0f, 35.0f);
  vcl_vector<dbrec_part_ins_sptr> children; children.push_back(pi1); children.push_back(pi2);
  dbrec_composition_ins* pc1 = new dbrec_composition_ins(3, pi1->pt(), children);
  dbrec_composition_ins* pc2 = new dbrec_composition_ins(4, pi2->pt(), children);
  pc->add_part(pc1);
  pc->add_part(pc2);
  
  TEST("context constructor", pc->first(), pc1);
  TEST("context constructor", pc->next(), pc2);
  TEST("context constructor", !pc->next(), true);

  vgl_box_2d<float> search_box;
  search_box.add(vgl_point_2d<float>(5.0f, 5.0f));
  search_box.add(vgl_point_2d<float>(40.0f, 40.0f));

  vcl_vector<dbrec_part_ins_sptr> out;
  pc->query(search_box, out);
  TEST("context constructor", out.size(), 2);

  vgl_box_2d<float> search_box2;
  search_box2.add(vgl_point_2d<float>(20.0f, 20.0f));
  search_box2.add(vgl_point_2d<float>(40.0f, 40.0f));

  out.clear();
  pc->query(search_box2, out);
  TEST("context constructor", out.size(), 1);
  if (out.size() > 0)
    TEST("context constructor", out[0], pc2);

  //: test parsing an image
  vil_image_resource_sptr img_r = vil_load_image_resource("./test_vehicle.png");
  if (!img_r) {
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

  unsigned type_id = 0;
  dbrec_gaussian g1(type_id, 3.0f, 3.0f, 0.0f, true);

  //: we need a parse visitor here
  dbrec_hierarchy_sptr h = new dbrec_hierarchy; // just an empth dummy hierarchy to construct visitor instance
  float class_prior = 0.15f; vcl_vector<float> comp_priors; comp_priors.push_back(class_prior); comp_priors.push_back(1.0f-class_prior);
  dbrec_parse_image_visitor pv(h, img_scaled, class_prior, comp_priors, "");
  
  g1.accept(&pv);
  dbrec_context_factory_sptr cf = pv.context_factory();
  TEST("test parse imge context factory push", !cf, false);
  dbrec_part_context_sptr c = cf->get_context(type_id);
  TEST("test context factory get_context", !c, false);
  TEST("test context", c->maps_size(), 2);
  TEST("test context", c->size(), 55);

}

TESTMAIN( test_part_context );
