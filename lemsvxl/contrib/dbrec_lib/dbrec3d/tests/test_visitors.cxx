//:
// \file
// \author Isabel Restrepo
// \date 24-Aug-2010

#include <testlib/testlib_test.h>

#include <dbrec3d/dbrec3d_io_visitors.h>
#include <dbrec3d/dbrec3d_visitor.h>

#include <bvpl/kernels/bvpl_edge3d_kernel_factory.h>
#include <dbrec3d/dbrec3d_parts_manager.h>

bool test_bxml_write_visitor()
{
  //create a couple of kernels
  bvpl_edge3d_kernel_factory kernel1(-1,0,-2,1,0, 0, 2.5);
  vnl_float_3 axis1(1,0,0);
  kernel1.set_rotation_axis(axis1);
  kernel1.set_angle(0.0f);
  bvpl_kernel_sptr kernel1_sptr = new bvpl_kernel(kernel1.create());
  
  bvpl_edge3d_kernel_factory kernel2(-1,0,-2,1,0, 0, 2.5);
  vnl_float_3 axis2(0,0,1);
  kernel2.set_rotation_axis(axis2);
  kernel2.set_angle(0.0f);
  bvpl_kernel_sptr kernel2_sptr = new bvpl_kernel(kernel2.create());
  
  PARTS_MANAGER->register_kernel(kernel1_sptr);
  PARTS_MANAGER->register_kernel(kernel2_sptr);
  
  //request all parts in the database
  vcl_vector<dbrec3d_part_sptr> parts= PARTS_MANAGER->get_all_parts();
  
  //write the xml file
  dbrec3d_write_xml_visitor xml_writer;
  vcl_string doc("./test.xml");
  xml_writer.write_hierarchy(parts, doc);
  
  //parse the file
  dbrec3d_parse_xml_visitor xml_parser;
  xml_parser.parse_hierarchy(doc);
  return true; 
}

void test_visitors()
{
  bool result = true;
  
  result = test_bxml_write_visitor();
  
  TEST("Valid Test", result, true);
  
  brdb_database_manager::clear_all();

}


TESTMAIN(test_visitors);
