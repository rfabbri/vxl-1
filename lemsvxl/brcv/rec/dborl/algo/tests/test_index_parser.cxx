#include <testlib/testlib_test.h>
#include <dborl/algo/dborl_index_parser.h>
#include <dborl/dborl_index.h>
#include <dborl/dborl_index_node.h>
#include <dborl/dborl_index_leaf.h>
#include <dborl/dborl_index_node_base_sptr.h>

#include <vcl_iostream.h>
#include <vcl_fstream.h>
//#include <vcl_cmath.h>

static void test_index_parser(int argc, char* argv[])
{
  testlib_test_start("testing index_parser class ");
 
  dborl_index_parser parser;
  parser.clear();
  dborl_index_sptr ind = dborl_index_parser::
      parse("test_index_flat_image.xml", parser);
  TEST("parse() index", !ind, false);
  TEST("parse() index", ind->name_.compare("test-index1"), 0);
  TEST("parse() index", ind->number_of_vertices(), 4);
  TEST("parse() index", ind->number_of_edges(), 3);
  TEST("parse() index", ind->root_->out_degree(), 3);
  TEST("parse() index", ind->root_->name_.compare("root"), 0);
  TEST("parse() index", !(ind->root_->cast_to_index_node()), false);
  TEST("parse() index", ind->root_->cast_to_index_node()->names().size(), 2);
  TEST("parse() index", ind->root_->cast_to_index_node()->paths().size(), 2);
  TEST("parse() index", ind->root_->cast_to_index_node()->names()[0].compare("obj11"), 0);
  TEST("parse() index", ind->root_->cast_to_index_node()->paths()[1].compare("/mnt/obj21/"), 0);
  vcl_vector<dborl_index_node_base_sptr> cs;
  ind->get_children(ind->root_, cs);
  TEST("parse() index", cs.size(), 3);
  TEST("parse() index", !(cs[0]->cast_to_index_leaf()), false);
  TEST("parse() index", cs[0]->cast_to_index_leaf()->name_.compare("car"), 0);
  TEST("parse() index", cs[1]->cast_to_index_leaf()->name_.compare("pickup"), 0);
  TEST("parse() index", cs[2]->cast_to_index_leaf()->name_.compare("truck"), 0);

  parser.clear();
  dborl_index_sptr ind2 = dborl_index_parser::
      parse("test_index_image_tree.xml", parser);
  TEST("parse() image_tree index", !ind2, false);
  TEST("parse() image_tree index", ind2->name_.compare("test-index2"), 0);
  TEST("parse() image_tree index", ind2->number_of_vertices(), 7);
  TEST("parse() image_tree index", ind2->number_of_edges(), 6);
  TEST("parse() image_tree index", ind2->root_->name_.compare("seeds"), 0);
  TEST("parse() image_tree index", ind2->root_->out_degree(), 3);
  TEST("parse() image_tree index", !(ind2->root_->cast_to_index_node()), false);
  TEST("parse() image_tree index", ind2->root_->cast_to_index_node()->names().size(), 9);
  TEST("parse() image_tree index", ind2->root_->cast_to_index_node()->paths().size(), 9);
  TEST("parse() image_tree index", ind2->root_->cast_to_index_node()->names()[0].compare("bonefishesocc1"), 0);
  TEST("parse() image_tree index", ind2->root_->cast_to_index_node()->names()[1].compare("desertcottontail"), 0);
  TEST("parse() image_tree index", ind2->root_->cast_to_index_node()->paths()[0].compare("---REMOTE---////objects//1//1198173654073//object"), 0);
  TEST("parse() image_tree index", ind2->root_->cast_to_index_node()->paths()[1].compare("---REMOTE---////objects//1//1198173658186//object"), 0);
  vcl_vector<dborl_index_node_base_sptr> cs2;
  ind2->get_children(ind2->root_, cs2);

  TEST("parse() image_tree index roots children", cs2.size(), 3);
  dborl_index_node_sptr ch1 = cs2[0]->cast_to_index_node();
  dborl_index_node_sptr ch2 = cs2[1]->cast_to_index_node();
  dborl_index_node_sptr ch3 = cs2[2]->cast_to_index_node();
  TEST("parse() image_tree index roots children", !ch1, false);
  TEST("parse() image_tree index roots children", !ch2, false);
  TEST("parse() image_tree index roots children", !ch3, false);
  
  TEST("parse() image_tree index roots child1", ch1->name_.compare("bonefishesocc1"), 0);
  TEST("parse() image_tree index roots child1", ch1->names().size(), 3);
  TEST("parse() image_tree index roots child1", ch1->paths().size(), 3);
  TEST("parse() image_tree index roots child1", ch1->names()[0].compare("bonefishes"), 0);
  TEST("parse() image_tree index roots child1", ch1->names()[1].compare("fish30"), 0);
  
  TEST("parse() image_tree index roots child2", ch2->name_.compare("desertcottontail"), 0);
  TEST("parse() image_tree index roots child2", ch2->names().size(), 3);
  TEST("parse() image_tree index roots child2", ch2->paths().size(), 3);
  TEST("parse() image_tree index roots child2", ch2->names()[0].compare("easterncottontail"), 0);
  TEST("parse() image_tree index roots child2", ch2->names()[1].compare("swamprabbit"), 0);
  TEST("parse() image_tree index roots child2", ch2->paths()[0].compare("---REMOTE---////objects//1//1198173667236//object"), 0);
  TEST("parse() image_tree index roots child2", ch2->paths()[1].compare("---REMOTE---////objects//1//1198173698918//object"), 0);

  TEST("parse() image_tree index roots child3", ch3->name_.compare("level1_3"), 0);
  TEST("parse() image_tree index roots child3", ch3->names().size(), 2);
  TEST("parse() image_tree index roots child3", ch3->paths().size(), 2);
  TEST("parse() image_tree index roots child3", ch3->names()[0].compare("obj2"), 0);
  TEST("parse() image_tree index roots child3", ch3->names()[1].compare("obj4"), 0);
  TEST("parse() image_tree index roots child3", ch3->paths()[0].compare("/mnt/obj2/"), 0);
  TEST("parse() image_tree index roots child3", ch3->paths()[1].compare("/mnt/obj4"), 1);

  TEST("parse() image_tree index roots child1", ch1->out_degree(), 1);
  TEST("parse() image_tree index roots child2", ch2->out_degree(), 1);
  TEST("parse() image_tree index roots child3", ch3->out_degree(), 1);

  vcl_vector<dborl_index_node_base_sptr> ch1c;
  ind2->get_children(ch1->cast_to_index_node_base(), ch1c);
  TEST("parse() image_tree index roots child1 children", ch1c.size(), 1);
  TEST("parse() image_tree index roots child1 children", !(ch1c[0]->cast_to_index_leaf()), false);
  TEST("parse() image_tree index roots child1 children", ch1c[0]->cast_to_index_leaf()->name_.compare("fish"), 0);

  vcl_vector<dborl_index_node_base_sptr> ch2c;
  ind2->get_children(ch2->cast_to_index_node_base(), ch2c);
  TEST("parse() image_tree index roots child2 children", ch2c.size(), 1);
  TEST("parse() image_tree index roots child2 children", !(ch2c[0]->cast_to_index_leaf()), false);
  TEST("parse() image_tree index roots child2 children", ch2c[0]->cast_to_index_leaf()->name_.compare("pickup"), 0);

  vcl_vector<dborl_index_node_base_sptr> ch3c;
  ind2->get_children(ch3->cast_to_index_node_base(), ch3c);
  TEST("parse() image_tree index roots child3 children", ch3c.size(), 1);
  TEST("parse() image_tree index roots child3 children", !(ch3c[0]->cast_to_index_leaf()), false);
  TEST("parse() image_tree index roots child3 children", ch3c[0]->cast_to_index_leaf()->name_.compare("truck"), 0);

  //: test writing index
  vcl_ofstream os;
  os.open("test.xml", vcl_ios_out);
  ind2->write_xml(os);
  vcl_cout << "test.xml written.\n";
  os.close();

  parser.clear();
  dborl_index_sptr ind3 = dborl_index_parser::parse("test.xml", parser);
  TEST("parse() image_tree index write & parse", !ind3, false);
  TEST("parse() image_tree index write & parse", ind3->name_.compare("test-index2"), 0);
  TEST("parse() image_tree index write & parse", ind3->number_of_vertices(), 7);
  TEST("parse() image_tree index write & parse", ind3->number_of_edges(), 6);
  TEST("parse() image_tree index write & parse", ind3->root_->name_.compare("seeds"), 0);
  TEST("parse() image_tree index write & parse", ind3->root_->out_degree(), 3);
  TEST("parse() image_tree index write & parse", !(ind3->root_->cast_to_index_node()), false);
  TEST("parse() image_tree index write & parse", ind3->root_->cast_to_index_node()->names().size(), 9);
  TEST("parse() image_tree index write & parse", ind3->root_->cast_to_index_node()->paths().size(), 9);
  TEST("parse() image_tree index write & parse", ind3->root_->cast_to_index_node()->names()[0].compare("bonefishesocc1"), 0);
  TEST("parse() image_tree index write & parse", ind3->root_->cast_to_index_node()->names()[1].compare("desertcottontail"), 0);
  TEST("parse() image_tree index write & parse", ind3->root_->cast_to_index_node()->paths()[0].compare("---REMOTE---////objects//1//1198173654073//object"), 0);
  TEST("parse() image_tree index write & parse", ind3->root_->cast_to_index_node()->paths()[1].compare("---REMOTE---////objects//1//1198173658186//object"), 0);
 
  vcl_vector<dborl_index_node_base_sptr> cs3;
  ind3->get_children(ind3->root_, cs3);

  TEST("parse() image_tree index roots children", cs3.size(), 3);
  ch1 = cs3[0]->cast_to_index_node();
  ch2 = cs3[1]->cast_to_index_node();
  ch3 = cs3[2]->cast_to_index_node();
  TEST("parse() image_tree index roots children", !ch1, false);
  TEST("parse() image_tree index roots children", !ch2, false);
  TEST("parse() image_tree index roots children", !ch3, false);

#if 0 // from the old test file
  TEST("parse() image_tree index roots child1", ch1->name_.compare("level1_1"), 0);
  TEST("parse() image_tree index roots child1", ch1->names().size(), 2);
  TEST("parse() image_tree index roots child1", ch1->paths().size(), 0);
  TEST("parse() image_tree index roots child1", ch1->names()[0].compare("obj1"), 0);
  TEST("parse() image_tree index roots child1", ch1->names()[1].compare("obj5"), 0);
  
  TEST("parse() image_tree index roots child2", ch2->name_.compare("level1_2"), 0);
  TEST("parse() image_tree index roots child2", ch2->names().size(), 2);
  TEST("parse() image_tree index roots child2", ch2->paths().size(), 2);
  TEST("parse() image_tree index roots child2", ch2->names()[0].compare("obj6"), 0);
  TEST("parse() image_tree index roots child2", ch2->names()[1].compare("obj9"), 0);
  TEST("parse() image_tree index roots child2", ch2->paths()[0].compare("/mnt/obj6/"), 0);
  TEST("parse() image_tree index roots child2", ch2->paths()[1].compare("/mnt/obj9"), 1);

  TEST("parse() image_tree index roots child3", ch3->name_.compare("level1_3"), 0);
  TEST("parse() image_tree index roots child3", ch3->names().size(), 2);
  TEST("parse() image_tree index roots child3", ch3->paths().size(), 2);
  TEST("parse() image_tree index roots child3", ch3->names()[0].compare("obj2"), 0);
  TEST("parse() image_tree index roots child3", ch3->names()[1].compare("obj4"), 0);
  TEST("parse() image_tree index roots child3", ch3->paths()[0].compare("/mnt/obj2/"), 0);
  TEST("parse() image_tree index roots child3", ch3->paths()[1].compare("/mnt/obj4"), 1);
#endif
  TEST("parse() image_tree index roots child1", ch1->out_degree(), 1);
  TEST("parse() image_tree index roots child2", ch2->out_degree(), 1);
  TEST("parse() image_tree index roots child3", ch3->out_degree(), 1);

  ch1c.clear();
  ind3->get_children(ch1->cast_to_index_node_base(), ch1c);
  TEST("parse() image_tree index roots child1 children", ch1c.size(), 1);
  TEST("parse() image_tree index roots child1 children", !(ch1c[0]->cast_to_index_leaf()), false);
  TEST("parse() image_tree index roots child1 children", ch1c[0]->cast_to_index_leaf()->name_.compare("fish"), 0);

  ch2c.clear();
  ind3->get_children(ch2->cast_to_index_node_base(), ch2c);
  TEST("parse() image_tree index roots child2 children", ch2c.size(), 1);
  TEST("parse() image_tree index roots child2 children", !(ch2c[0]->cast_to_index_leaf()), false);
  TEST("parse() image_tree index roots child2 children", ch2c[0]->cast_to_index_leaf()->name_.compare("pickup"), 0);

  ch3c.clear();
  ind3->get_children(ch3->cast_to_index_node_base(), ch3c);
  TEST("parse() image_tree index roots child3 children", ch3c.size(), 1);
  TEST("parse() image_tree index roots child3 children", !(ch3c[0]->cast_to_index_leaf()), false);
  TEST("parse() image_tree index roots child3 children", ch3c[0]->cast_to_index_leaf()->name_.compare("truck"), 0);

  parser.clear();
  dborl_index_sptr ind5 = dborl_index_parser::
      parse("99-db-tree-index2.xml", parser);
  TEST("parse() 99-db-tree-index2.xml", !ind5, false);
  TEST("parse() 99-db-tree-index2.xml", ind5->name_.compare("99_shapedb_test_index"), 0);

  TEST("parse() 99-db-tree-index2.xml", ind2->root_->cast_to_index_node()->names().size(), 9);
  TEST("parse() 99-db-tree-index2.xml", ind2->root_->cast_to_index_node()->paths().size(), 9);
  TEST("parse() 99-db-tree-index2.xml", ind2->root_->cast_to_index_node()->names()[0].compare("bonefishesocc1"), 0);
  TEST("parse() 99-db-tree-index2.xml", ind2->root_->cast_to_index_node()->names()[1].compare("desertcottontail"), 0);
  TEST("parse() 99-db-tree-index2.xml", ind2->root_->cast_to_index_node()->paths()[0].compare("---REMOTE---////objects//1//1198173654073//object"), 0);
  TEST("parse() 99-db-tree-index2.xml", ind2->root_->cast_to_index_node()->paths()[1].compare("---REMOTE---////objects//1//1198173658186//object"), 0);
  cs2.clear();
  ind2->get_children(ind2->root_, cs2);


}

TESTMAIN_ARGS(test_index_parser)
