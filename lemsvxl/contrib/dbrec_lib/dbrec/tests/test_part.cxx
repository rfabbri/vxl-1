// This is dbrec/tests/test_part.cxx
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

static void test_part()
{
  dbrec_type_id_factory* ins = dbrec_type_id_factory::instance();
  dbrec_part_sptr p1 = new dbrec_part(ins->new_type());
  dbrec_part_sptr p2 = new dbrec_part(ins->new_type());
  TEST("part constructor", p1->type() != p2->type(), true);
  dbrec_part_sptr p3 = new dbrec_part(*p1);
  TEST("copy constructor", p3->type(), p1->type());

  vcl_vector<dbrec_part_sptr> parts;
  parts.push_back(p1);
  parts.push_back(p2);
  dbrec_composition c_p1(ins->new_type(), parts, new dbrec_or_compositor, 10.0f); // constructed with default compositor which does nothing most of the time
  dbrec_part_sptr c_p2 = new dbrec_composition(c_p1);
  TEST("composition copy constructor", c_p2->type(), c_p1.type());
  vcl_cout << "printing c_p2:\n";
  vcl_cout << *c_p2;

  dbrec_composition* c_p3 = new dbrec_composition(c_p1);
  TEST("composition copy constructor", c_p3->size(), c_p1.size());

  parts.push_back(c_p3); // so c_p3 is now handled by a smart pointer and will be deleted properly
  dbrec_composition c_p4(ins->new_type(), parts, new dbrec_or_compositor, 10.0f);
  TEST("add children", c_p4.size(), 3);
  vcl_cout << c_p4;

  dbrec_part_sptr c_p3_sptr = c_p3;
  TEST("sameness test", c_p2->same_part(c_p3_sptr), true);
  dbrec_part_sptr c_p4_sptr = new dbrec_composition(c_p4);
  TEST("sameness test", c_p2->same_part(c_p4_sptr), false);
}

TESTMAIN( test_part );
