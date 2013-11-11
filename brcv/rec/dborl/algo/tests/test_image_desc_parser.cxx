#include <testlib/testlib_test.h>
#include <dborl/algo/dborl_image_desc_parser.h>
#include <dborl/dborl_image_description_sptr.h>
#include <dborl/dborl_image_description.h>
#include <dborl/dborl_image_data_description_base.h>
#include <dborl/dborl_image_bbox_description.h>
#include <dborl/dborl_image_polygon_description.h>

#include <vcl_iostream.h>
#include <vcl_fstream.h>
//#include <vcl_cmath.h>
#include <vil/vil_load.h>
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>
#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_point_2d.h>

static void test_image_desc_parser(int argc, char* argv[])
{
  testlib_test_start("testing image_desc_parser class ");
 
  dborl_image_desc_parser parser;
  parser.set_pascal_tags();
  parser.clear();
  dborl_image_description_sptr id = dborl_image_description_parse("pascal_000005.xml", parser);

  TEST("parse() ", id->category_exists("chair"), true);
  TEST("parse() ", id->category_exists("dog"), true);
  TEST("parse() ", id->category_list_["chair"], 4);
  TEST("parse() ", id->category_list_["dog"], 1);

  TEST("parse() ", !(id->category_data_->cast_to_image_bbox_description()), false);
  TEST("parse() ", id->category_data_->cast_to_image_bbox_description()->category_exists("chair"), true);
  TEST("parse() ", id->category_data_->cast_to_image_bbox_description()->category_exists("dog"), true);

  TEST("parse() ", id->category_data_->cast_to_image_bbox_description()->get_box_vector("chair").size(), 4);
  TEST("parse() ", id->category_data_->cast_to_image_bbox_description()->get_box_vector("dog").size(), 1);
  
  TEST("parse() ", id->category_data_->cast_to_image_bbox_description()->get_box_vector("dog")[0]->get_min_x(), 277);
  TEST("parse() ", id->category_data_->cast_to_image_bbox_description()->get_box_vector("dog")[0]->get_min_y(), 186);
  TEST("parse() ", id->category_data_->cast_to_image_bbox_description()->get_box_vector("dog")[0]->get_max_x(), 312);
  TEST("parse() ", id->category_data_->cast_to_image_bbox_description()->get_box_vector("dog")[0]->get_max_y(), 220);

  TEST("parse() ", id->category_data_->cast_to_image_bbox_description()->get_box_vector("chair")[2]->get_min_x(), 5);
  TEST("parse() ", id->category_data_->cast_to_image_bbox_description()->get_box_vector("chair")[2]->get_min_y(), 244);
  TEST("parse() ", id->category_data_->cast_to_image_bbox_description()->get_box_vector("chair")[2]->get_max_x(), 67);
  TEST("parse() ", id->category_data_->cast_to_image_bbox_description()->get_box_vector("chair")[2]->get_max_y(), 374);

  parser.set_default_tags();
  parser.clear();
  dborl_image_description_sptr id2 = dborl_image_description_parse("test_box.xml", parser);

  TEST("parse() bone 1", id2->category_exists("chair"), true);
  TEST("parse() bone 2", id2->category_exists("bone"), true);
  TEST("parse() bone 3", id2->category_list_["chair"], 2);
  TEST("parse() bone 4", id2->category_list_["bone"], 1);

  dborl_image_bbox_description_sptr bd = id2->category_data_->cast_to_image_bbox_description();
  TEST("parse() bone 5", !(bd), false);
  TEST("parse() bone 6", bd->category_exists("chair"), true);
  TEST("parse() bone 7", bd->category_exists("bone"), true);
  
  TEST("parse() bone 8", bd->get_box_vector("chair").size(), 2);
  TEST("parse() bone 9", bd->get_box_vector("bone").size(), 1);

  TEST_NEAR("parse() bone 10", bd->get_box_vector("chair")[0]->get_min_x(), 263.2, 0.001);
  TEST_NEAR("parse() bone 11", bd->get_box_vector("chair")[0]->get_min_y(), 211.345, 0.001);
  TEST_NEAR("parse() bone 12", bd->get_box_vector("chair")[0]->get_max_x(), 324.12345, 0.001);
  TEST_NEAR("parse() bone 13", bd->get_box_vector("chair")[0]->get_max_y(), 339.45, 0.001);

  TEST_NEAR("parse() bone 14", bd->get_box_vector("chair")[1]->get_min_x(), 165.0, 0.001);
  TEST_NEAR("parse() bone 15", bd->get_box_vector("chair")[1]->get_min_y(), 264.3, 0.001);
  TEST_NEAR("parse() bone 16", bd->get_box_vector("chair")[1]->get_max_x(), 253.0, 0.001);
  TEST_NEAR("parse() bone 17", bd->get_box_vector("chair")[1]->get_max_y(), 372.00, 0.001);

  TEST_NEAR("parse() bone 18", bd->get_box_vector("bone")[0]->get_min_x(), -35, 0.001);
  TEST_NEAR("parse() bone 19", bd->get_box_vector("bone")[0]->get_min_y(), -0.23, 0.001);
  TEST_NEAR("parse() bone 20", bd->get_box_vector("bone")[0]->get_max_x(), -3.5, 0.001);
  TEST_NEAR("parse() bone 21", bd->get_box_vector("bone")[0]->get_max_y(), 0.23, 0.001);

  parser.clear();
  dborl_image_description_sptr id3 = dborl_image_description_parse("test_polygon.xml", parser);

  TEST("parse() polygon 1", id3->category_exists("tool"), true);
  TEST("parse() polygon 2", id3->category_exists("skyhawk"), true);
  TEST("parse() polygon 3", id3->category_list_["tool"], 1);
  TEST("parse() polygon 4", id3->category_list_["skyhawk"], 3);

  dborl_image_polygon_description_sptr pd = id3->category_data_->cast_to_image_polygon_description();
  TEST("parse() polygon 5", !(pd), false);

  TEST("parse() polygon 6", pd->category_exists("tool"), true);
  TEST("parse() polygon 7", pd->category_exists("skyhawk"), true);

  TEST("parse() polygon 8", pd->get_polygon_vector("tool")[0]->size(), 80);
  TEST_NEAR("parse() polygon 9", pd->get_polygon_vector("skyhawk")[0]->size(), 4, 0.001);
  TEST("parse() polygon 10", pd->get_polygon_vector("skyhawk")[1]->size(), 122);
  TEST("parse() polygon 11", pd->get_polygon_vector("skyhawk")[2]->size(), 7);

  TEST_NEAR("parse() polygon 12", pd->get_polygon_vector("tool")[0]->vertex(0)->x(), 68, 0.001);
  TEST_NEAR("parse() polygon 13", pd->get_polygon_vector("tool")[0]->vertex(0)->y(), 32.643369, 0.001);
  TEST_NEAR("parse() polygon 14", pd->get_polygon_vector("tool")[0]->vertex(79)->x(), 66.9746, 0.001);
  TEST_NEAR("parse() polygon 15", pd->get_polygon_vector("tool")[0]->vertex(79)->y(), 33, 0.001);

  TEST_NEAR("parse() polygon 16", pd->get_polygon_vector("skyhawk")[0]->vertex(0)->x(), 68, 0.001);
  TEST_NEAR("parse() polygon 17", pd->get_polygon_vector("skyhawk")[0]->vertex(0)->y(), 15.6548, 0.001);
  TEST_NEAR("parse() polygon 18", pd->get_polygon_vector("skyhawk")[0]->vertex(3)->x(), 71.842, 0.001);
  TEST_NEAR("parse() polygon 19", pd->get_polygon_vector("skyhawk")[0]->vertex(3)->y(), 20, 0.001);

  TEST_NEAR("parse() polygon 20", pd->get_polygon_vector("skyhawk")[1]->vertex(0)->x(), 37, 0.001);
  TEST_NEAR("parse() polygon 21", pd->get_polygon_vector("skyhawk")[1]->vertex(0)->y(), 32.6284, 0.001);
  TEST_NEAR("parse() polygon 22", pd->get_polygon_vector("skyhawk")[1]->vertex(121)->x(), 35, 0.001);
  TEST_NEAR("parse() polygon 23", pd->get_polygon_vector("skyhawk")[1]->vertex(121)->y(), 33.3399, 0.001);

  TEST_NEAR("parse() polygon 24", pd->get_polygon_vector("skyhawk")[2]->vertex(0)->x(), 68, 0.001);
  TEST_NEAR("parse() polygon 25", pd->get_polygon_vector("skyhawk")[2]->vertex(0)->y(), 15.6548, 0.001);
  TEST_NEAR("parse() polygon 26", pd->get_polygon_vector("skyhawk")[2]->vertex(6)->x(), 74.2741, 0.001);
  TEST_NEAR("parse() polygon 27", pd->get_polygon_vector("skyhawk")[2]->vertex(6)->y(), 26, 0.001);

  vcl_ofstream os3;
  os3.open("test3.xml", vcl_ios_out);
  id3->write_xml(os3);
  vcl_cout << "test3.xml written.\n";
  os3.close();

  vcl_ofstream os4;
  os4.open("test2.xml", vcl_ios_out);
  id2->write_xml(os4);
  vcl_cout << "test2.xml written.\n";
  os4.close();

  parser.clear();
  dborl_image_description_sptr id33 = dborl_image_description_parse("test3.xml", parser);
  TEST("write() ", id33->category_list_["tool"], id3->category_list_["tool"]);

  parse_pascal_write_default_xml("pascal_000005.xml", "test3.xml");
  parser.clear();
  dborl_image_description_sptr id5 = dborl_image_description_parse("test3.xml", parser);
  TEST("write() ", id->category_list_["chair"], id5->category_list_["chair"]);
  TEST("parse() ", id5->category_data_->cast_to_image_bbox_description()->get_box_vector("dog")[0]->get_min_x(), 277);

  vcl_string input_f = "dog2.con";
  vcl_string output_f = "test2.xml";
  if (read_con_write_image_description_xml(input_f, "dog", output_f)) {
    parser.clear();
    dborl_image_description_sptr id6 = dborl_image_description_parse("test2.xml", parser);
    TEST("read con write xml() ", id6->category_list_["dog"], 1);
    TEST("read con write xml() ", !(id6->category_data_->cast_to_image_polygon_description()), false);
    TEST("read con write xml() ", id6->category_data_->cast_to_image_polygon_description()->get_polygon_vector("dog")[0]->size(), 272);
    TEST_NEAR("read con write xml() ", id6->category_data_->cast_to_image_polygon_description()->get_polygon_vector("dog")[0]->vertex(1)->x(), 96, 0.001);
    TEST_NEAR("read con write xml() ", id6->category_data_->cast_to_image_polygon_description()->get_polygon_vector("dog")[0]->vertex(271)->y(), 10, 0.001);

  } else
    vcl_cout << "Problems in reading dog2.con and writing test2.xml\n";

  parser.set_default_tags();
  parser.clear();
  dborl_image_description_sptr id7 = dborl_image_description_parse("test_mask.xml", parser);
  TEST("parse() mask description ", !(id7->category_data_->cast_to_image_mask_description()), false);
  TEST("parse() mask description", id7->category_exists("tool"), true);
  TEST("parse() mask description", id7->category_exists("skyhawk"), true);
  TEST("parse() mask description", id7->category_list_["tool"], 1);
  TEST("parse() mask description", id7->category_list_["skyhawk"], 2);



}

TESTMAIN_ARGS(test_image_desc_parser)
