#include <testlib/testlib_test.h>

#include <dborl/dborl_index_node_base.h>
#include <dborl/dborl_index_node_base_sptr.h>
#include <dborl/dborl_index_node_sptr.h>
#include <dborl/dborl_index_node.h>
#include <dborl/dborl_index_leaf_sptr.h>
#include <dborl/dborl_index_leaf.h>
#include <dborl/dborl_index_edge_sptr.h>
#include <dborl/dborl_index_edge.h>
#include <dborl/dborl_index_sptr.h>
#include <dborl/dborl_index.h>
#include <dborl/dborl_image_object_sptr.h>
#include <dborl/dborl_image_object.h>

#include <vil/vil_image_view.h>
#include <vil/vil_new.h>

#include <vcl_iostream.h>

MAIN_ARGS(test_index)
{
  testlib_test_start("testing dborl_index_node_base class ");
 
  dborl_index_node_sptr n = new dborl_index_node("root");

  TEST("test index_node constructor ", n->name_.compare("root"), 0);
  TEST("test index_node constructor ", !(n->cast_to_index_node_base()), false);
  n->add_name("test1");
  n->add_name("test2");
  TEST("test index_node constructor ", n->names().size(), 2);
  TEST("test index_node constructor ", n->objects().size(), 0);
  TEST("test index_node constructor ", n->paths().size(), 0);
  n->fill_in_the_paths_using_names("mnt");
  TEST("test index_node constructor ", n->paths().size(), 2);
#ifdef VCL_WIN32
  TEST("test index_node constructor ", n->paths()[1].compare("mnt\\test2\\"), 0);
#else
  TEST("test index_node constructor ", n->paths()[1].compare("mnt/test2/"), 0);
#endif

  const unsigned int ni = 55;
  const unsigned int nj = 35;
  vil_image_view<float> image;
  image.set_size(ni,nj);  image.fill(17.f);
  vil_image_resource_sptr img = vil_new_image_resource_of_view(image);  
  //: an object with no description
  dborl_image_object_sptr rio1 = new dborl_image_object("test1", img);
  dborl_image_object_sptr rio2 = new dborl_image_object("test2", img);
  dborl_image_object_sptr rio3 = new dborl_image_object("test3", img);
  vcl_vector<dborl_object_base_sptr> objs; objs.push_back(rio1->cast_to_object_base()); objs.push_back(rio2->cast_to_object_base()); objs.push_back(rio3->cast_to_object_base());
  TEST("test index node fill_in_the_pointers() ", n->fill_in_the_pointers(objs), true);
  TEST("test index_node fill_in_the_pointers() ", n->objects().size(), 2);

  dborl_index_node_sptr c1 = new dborl_index_node("child1");
  c1->add_name("child1-test1");
  c1->add_name("child1-test2");
  TEST("test index node constructor ", c1->is_leaf(), false);
  TEST("test index node fill_in_the_pointers() ", c1->fill_in_the_pointers(objs), false);
  TEST("test index_node fill_in_the_pointers() ", c1->objects().size(), 0);

  dborl_index_node_sptr c2 = new dborl_index_node("child2");
  c2->add_name("child2-test1");
  c2->add_name("child2-test2");

  //: an object with no description
  dborl_image_object_sptr io = new dborl_image_object("", img);

  dborl_index_edge_sptr e = new dborl_index_edge(n->cast_to_index_node_base(), c1->cast_to_index_node_base());
  TEST("test edge constructor ", e->source()->name_.compare("root"), 0);
  TEST("test edge constructor ", e->target()->name_.compare("child1"), 0);

  dborl_index_sptr ind = new dborl_index("test-index");
  ind->set_type(dborl_index_type::flat_image);
  TEST("test index constructor ", ind->get_type(), dborl_index_type::flat_image);
  TEST("test index constructor ", ind->is_type(dborl_index_type::flat_point_cloud), false);
  TEST("test index constructor ", ind->get_type_string().compare("flat_image"), 0);
  ind->add_root(n->cast_to_index_node_base());
  TEST("test index add_root() ", ind->number_of_vertices(), 1);
  TEST("test index add_root() ", ind->number_of_edges(), 0);
  TEST("test index add_root() ", ind->add_root(c1->cast_to_index_node_base()), false);
  TEST("test index add_root() ", ind->number_of_vertices(), 1);
  TEST("test index add_root() ", ind->node_exists(c1->cast_to_index_node_base()), false);
  TEST("test index add_root() ", ind->node_exists(n->cast_to_index_node_base()), true);

  TEST("test index add_child() ", ind->add_child(n->cast_to_index_node_base(), c1->cast_to_index_node_base()), true);
  TEST("test index add_child() ", ind->node_exists(c1->cast_to_index_node_base()), true);
  TEST("test index add_child() ", ind->add_child(n->cast_to_index_node_base(), c2->cast_to_index_node_base()), true);
  TEST("test index add_dhild() ", ind->node_exists(c2->cast_to_index_node_base()), true);
  
  dborl_index_leaf_sptr c21 = new dborl_index_leaf("child2-leaf1");
  dborl_index_leaf_sptr c22 = new dborl_index_leaf("child2-leaf2");
  TEST("test index leaf constructor ", c21->is_leaf(), true);
  
  TEST("test index add_child() ", ind->add_child(c2->cast_to_index_node_base(), c21->cast_to_index_node_base()), true);
  TEST("test index add_child() ", ind->node_exists(c21->cast_to_index_node_base()), true);
  TEST("test index add_child() ", ind->add_child(c2->cast_to_index_node_base(), c22->cast_to_index_node_base()), true);
  TEST("test index add_dhild() ", ind->node_exists(c22->cast_to_index_node_base()), true);
  
  vcl_vector<dborl_index_node_base_sptr> cs;
  TEST("test index get_children() ", ind->get_children(n->cast_to_index_node_base(), cs), true);
  TEST("test index get_children() ", cs.size(), 2);
  TEST("test index get_children() ", cs[0]->name_.compare("child1"), 0);
  TEST("test index get_children() ", cs[1]->name_.compare("child2"), 0);
  TEST("test index get_children() ", !(cs[0]->cast_to_index_node()), false);
  TEST("test index get_children() ", !(cs[1]->cast_to_index_node()), false);
  TEST("test index get_children() ", cs[0]->cast_to_index_node()->names().size(), 2);
  TEST("test index get_children() ", cs[0]->cast_to_index_node()->names()[0].compare("child1-test1"), 0);
  TEST("test index get_children() ", cs[0]->cast_to_index_node()->names()[1].compare("child1-test2"), 0);
  TEST("test index get_children() ", cs[1]->cast_to_index_node()->names().size(), 2);
  TEST("test index get_children() ", cs[1]->cast_to_index_node()->names()[0].compare("child2-test1"), 0);
  TEST("test index get_children() ", cs[1]->cast_to_index_node()->names()[1].compare("child2-test2"), 0);

  cs.clear();
  TEST("test index get_children() ", ind->get_children(c2->cast_to_index_node_base(), cs), true);
  TEST("test index get_children() ", cs.size(), 2);
  TEST("test index get_children() ", cs[0]->name_.compare("child2-leaf1"), 0);
  TEST("test index get_children() ", cs[1]->name_.compare("child2-leaf2"), 0);
  TEST("test index get_children() ", !(cs[0]->cast_to_index_leaf()), false);
  TEST("test index get_children() ", !(cs[1]->cast_to_index_leaf()), false);
  TEST("test index get_children() ", cs[0]->cast_to_index_leaf()->name_.compare("child2-leaf1"), 0);

  TEST("test index ", ind->number_of_vertices(), 5);
  TEST("test index ", ind->number_of_edges(), 4);
  

  return testlib_test_summary();
}
