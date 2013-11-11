#include <testlib/testlib_test.h>
#include <dborl/algo/dborl_category_info_parser.h>
#include <dborl/dborl_category_info.h>

#include <vcl_iostream.h>
//#include <vcl_cmath.h>
#include <vil/vil_load.h>
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>
#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_point_2d.h>

static void test_category_info_parser(int argc, char* argv[])
{
  testlib_test_start("testing category_info_parser class ");
 
  dborl_category_info_parser parser;
  parser.clear();
  vcl_vector<dborl_category_info_sptr> cats;
  TEST("parse() category info", parse("99-db-description.xml", parser, cats), true);
  TEST("parse() category info", cats.size(), 9);
  
  TEST("parse() category info", cats[0]->name_.compare("fish"), 0);
  TEST("parse() category info", cats[1]->id_, 1);
  TEST("parse() category info", cats[2]->prefix_list_.size(), 6);
  TEST("parse() category info", cats[2]->prefix_list_[3].compare("dog"), 0);
  TEST("parse() category info", cats[3]->color_.R(), 0);
  TEST("parse() category info", cats[3]->color_.G(), 1);
  TEST("parse() category info", cats[3]->color_.B(), 1);
  TEST("parse() category info", cats[8]->name_.compare("flatfish"), 0);
  TEST("parse() category info", cats[8]->prefix_list_.size(), 1);
  TEST("parse() category info", cats[8]->prefix_list_[0].compare("kk0"), 0);


}

TESTMAIN_ARGS(test_category_info_parser)
