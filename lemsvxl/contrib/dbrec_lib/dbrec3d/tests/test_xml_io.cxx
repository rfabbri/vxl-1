//:
// \file
// \author Isabel Restrepo
// \date 6-Sep-2010

#include <testlib/testlib_test.h>

#include "test_utils.h"

#include <dbrec3d/dbrec3d_context_manager.h>
#include <dbrec3d/dbrec3d_io_visitors.h>

void test_xml_io()
{
  int cc_id = dbrec3d_test_utils::find_primitive_pairs();
  
  vcl_string p_doc("parts_hierarchy.xml");
  vcl_string c_doc("contexts.xml");
  dbrec3d_xml_write_parts_and_contexts(p_doc, c_doc);
  DATABASE->print();
  brdb_database_manager::clear_all();
  DATABASE->print();
  dbrec3d_xml_parse_parts_and_contexts(p_doc, c_doc);
  DATABASE->print();
  
  vcl_string p_doc2("parts_hierarchy2.xml");
  vcl_string c_doc2("contexts2.xml");
  dbrec3d_xml_write_parts_and_contexts(p_doc2, c_doc2);

  bool result = true;
  TEST("Valid Test", result, true);
}


TESTMAIN(test_xml_io);
