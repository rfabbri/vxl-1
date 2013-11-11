// This is dbrec/tests/test_hierarchy.cxx
//:
// \file
// \brief Various tests for dbrec library
// \author Ozge C. Ozcanli (Brown)
// \date   April 21, 2009
//
#include <testlib/testlib_test.h>
#include <dbrec/dbrec_part.h>
#include <dbrec/dbrec_part_sptr.h>
#include <dbrec/dbrec_type_id_factory.h>
#include <dbrec/dbrec_gaussian_primitive.h>
#include <dbrec/dbrec_image_visitors.h>
#include <dbrec/dbrec_image_hierarchy_factory.h>
#include <dbrec/dbrec_image_pairwise_models.h>
#include <vul/vul_awk.h>

static void test_hierarchy()
{
  dbrec_type_id_factory* ins = dbrec_type_id_factory::instance();
  dbrec_gaussian_factory* gf = new dbrec_gaussian_factory();
  gf->populate(4, 2.0f, 1.0f, true, false);

  dbrec_gaussian* g_mine = new dbrec_gaussian(150, 2.0f, 5.0f, 10.0f, true);
  gf->add(g_mine);
  
  dbrec_hierarchy_sptr h = gf->construct_random_classifier(2);
  TEST("testing random classifier", h->class_cnt(), 2);

  h->visualize("./random_classifier.svg");
  dbrec_draw_class(h, "./random_classifier_class_0.svg", 0, 10.0f, "blue");
  dbrec_draw_class(h, "./random_classifier_class_1.svg", 1, 10.0f, "red");

  for (dbrec_gaussian_factory::part_const_iterator it = gf->parts_begin(); it != gf->parts_end(); it++) {
    dbrec_part_sptr p = it->second;
    TEST("test get part", h->get_part(p->type()), p);
  }

  dbrec_part_sptr c3 = h->get_part(150);
  TEST("testing get_part()", c3 == g_mine, true);

  dbrec_hierarchy_sptr hs = dbrec_image_hierarchy_factory::construct_detector_steeple0();
  hs->print(vcl_cout);
  dbrec_part_sptr c = hs->get_part(11);
  if (!c)
    vcl_cout << "part not found!\n";
  dbrec_part_sptr c2 = hs->get_part(11);
  TEST("testing get_part()", c == c2, true);

  vcl_vector<dbrec_part_sptr> sps;
  hs->get_parts(1, sps);
  TEST("testing get_parts()", sps.size(), 5);

  sps.clear();
  hs->get_parts(2, sps);
  TEST("testing get_parts()", sps.size(), 4);

  sps.clear();
  hs->get_parts(3, sps);
  TEST("testing get_parts()", sps.size(), 2);

  sps.clear();
  hs->get_parts(4, sps);  // the root is at depth=4
  TEST("testing get_parts()", sps.size(), 1);

  sps.clear();
  h->get_parts(1, sps);
  TEST("testing get_parts()", sps.size(), 7);

  //: add a second model to one of the compositions for testing purposes
  bsta_joint_histogram<float> hist((float)(-vnl_math::pi), (float)(vnl_math::pi), 8, 0.0f, 10.0f, 10);
  hist.upcount((float)(vnl_math::pi)/2.0f, 1.0f, 5.0f, 1.0f);
  hist.upcount((float)(vnl_math::pi)/2.0f, 1.0f, 5.0f, 1.0f);
  hist.upcount((float)(vnl_math::pi)/2.0f, 1.0f, 5.0f, 1.0f);
  hist.upcount((float)(vnl_math::pi)/2.0f, 1.0f, 5.0f, 1.0f);
  hist.upcount((float)(vnl_math::pi)/4.0f, 1.0f, 3.0f, 1.0f);
  hist.upcount((float)(vnl_math::pi)/4.0f, 1.0f, 3.0f, 1.0f);
  hist.upcount((float)(vnl_math::pi)/4.0f, 1.0f, 3.0f, 1.0f);
  hist.upcount((float)(-vnl_math::pi), 1.0f, 3.0f, 1.0f);
  hist.upcount((float)(vnl_math::pi), 1.0f, 3.0f, 0.0f);
  dbrec_pairwise_model_sptr m = new dbrec_pairwise_discrete_model(hist);
  dbrec_part_sptr hs_root = hs->root(0);
  dbrec_composition* hs_root_comp = dynamic_cast<dbrec_composition*>(hs_root.ptr());
  dbrec_part_sptr hs_comp1 = hs_root_comp->children()[0];
  dbrec_composition* hs_comp1_comp = dynamic_cast<dbrec_composition*>(hs_comp1.ptr());
  dbrec_central_compositor* hs_comp1_comp_cc = dynamic_cast<dbrec_central_compositor*>(hs_comp1_comp->compositor().ptr());
  hs_comp1_comp_cc->models().push_back(m);

  //: test the xml writer and parser
  dbrec_write_xml_visitor wxml;
  for (unsigned i = 0; i < hs->class_cnt(); i++)
    hs->root(i)->accept(&wxml);
  vcl_string test_name = "./steeple0_detector.xml";
  wxml.write_doc(test_name);

  //: parse the hierarchy
  dbrec_parse_hierarchy_xml parser;
  TEST("testing parser", parser.parse(test_name), true);
  dbrec_hierarchy_sptr parsed_h = parser.get_hierarchy();
  TEST("testing parser cls cnt", parsed_h->class_cnt(), 1);
  TEST("testing parser root type", parsed_h->root(0)->type(), hs->root(0)->type()); 
  TEST("testing parser width", parsed_h->width(), hs->width());

  dbrec_write_xml_visitor wxml2;
  for (unsigned i = 0; i < parsed_h->class_cnt(); i++)
    parsed_h->root(i)->accept(&wxml2);
  vcl_string test_name2 = "./steeple0_detector2.xml";
  wxml2.write_doc(test_name2);

  //: test that the two files are the same word by word
  vcl_ifstream if1( test_name.c_str() );
  vcl_ifstream if2( test_name2.c_str() );
  
  bool equal = true;
  vul_awk lawk1( if1 ); vul_awk lawk2( if2 );
  for( ; lawk1 && lawk2; ++lawk1, ++lawk2 ) {
    if ( lawk1.NR() != lawk2.NR() ) { 
      equal = false; break; 
    }
    if ( lawk1.NF() != lawk2.NF() ) { 
      equal = false; break; 
    }
    if ( lawk1.NF() == 0 ) continue; 
    for (int i = 0; i < lawk1.NF(); i++ ){
      vcl_string s1(lawk1[i]); vcl_string s2(lawk2[i]);
      if (s1.compare(s2)) { 
        equal = false; break; 
      }
    }
    if (!equal) break;
  }
  TEST("testing parser and writer", equal, true);
}

TESTMAIN( test_hierarchy );
