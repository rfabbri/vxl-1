#include <testlib/testlib_test.h>
#include <dbskr/algo/io/dbskr_extract_patch_params.h>
#include <bxml/bxml_write.h>
#include <bxml/bxml_read.h>
#include <bxml/bxml_find.h>

static void test_params(int argc, char* argv[])
{
  testlib_test_start("testing params");
 
  dbskr_extract_patch_params params;
  bxml_data_sptr elem = params.create_default_document_data();
  
  TEST("dbskr_extract_patch_params() ", !elem, false);

  bxml_document doc;
  bxml_element * root = new bxml_element("dborl_extract_patch_processor");
  doc.set_root_element(root);

  root->append_data(elem);
  root->append_text("\n");
  bxml_write(vcl_string("test-doc.xml"), doc);


  bxml_document param_doc = bxml_read("test-doc.xml");
  TEST("bxml_read() ", !param_doc.root_element(), false);
  TEST("bxml_read() ", param_doc.root_element()->type() == bxml_data::ELEMENT, true);

  bxml_element query("dbskr_extract_patch_params");
  bxml_data_sptr result = bxml_find_by_name(root, query);
  TEST("bxml_find_by_name() ", !result, false);
  TEST("bxml_find_by_name() ", result->type() == bxml_data::ELEMENT, true);
  bxml_element *res_elem = static_cast<bxml_element*>(result.ptr());
  TEST("bxml_find_by_name() ", !res_elem, false);
  TEST("bxml_find_by_name() ", res_elem->name().compare("dbskr_extract_patch_params") == 0, true);

  //bxml_element query2("patch_data");
  //bxml_data_sptr result2 = bxml_find_by_name(result, query2);

  dbskr_extract_patch_params params_doc;
  TEST("parse_from_data() ", params_doc.parse_from_data(param_doc.root_element()), true);

  //TEST("load_xshock_graph() ", !sg, false);
  //TEST("load_xshock_graph() ", sg->number_of_vertices(), 14);

  

  //TEST_NEAR("parse() bone 10", bd->get_box_vector("chair")[0]->get_min_x(), 263.2, 0.001);
  //TEST_NEAR("parse() bone 11", bd->get_box_vector("chair")[0]->get_min_y(), 211.345, 0.001);
  
}

TESTMAIN_ARGS(test_params)
