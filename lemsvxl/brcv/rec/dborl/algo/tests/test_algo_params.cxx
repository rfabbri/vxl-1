#include <testlib/testlib_test.h>
#include <dborl/algo/dborl_algo_params.h>
#include <dbskr/algo/io/dbskr_match_shock_params.h>  // for dbskr_tree_edit_params2

#include <vcl_iostream.h>
//#include <vcl_cmath.h>
#include <vil/vil_load.h>
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>
#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_point_2d.h>

#include <dborl/algo/dborl_utilities.h>

class test_params : public dborl_algo_params
{
public:
  
  //: parameters from the example algo in ORL documentations for parameter handling
  dborl_parameter<vcl_string> db_index_;
  dborl_parameter<vcl_string> output_folder_;

  dborl_parameter<bool> normalize_;
  dborl_parameter<bool> norm_reconst_bound_;  // only one of these should be true
  dborl_parameter<bool> norm_tot_splice_cost_;
  dborl_parameter<bool> norm_con_arclength_;  // assumes con files are saved in object directories

  dbskr_tree_edit_params2 edit_params_;
  
  test_params() : dborl_algo_params("test_params") { 

    output_folder_.set_values(param_list_, "io", "output_folder", "output folder", "", "", -1, dborl_parameter_system_info::OUTPUT_DIRECTORY);
    db_index_.set_values(param_list_, "io", "db_index", "path of index file", "", "", 0, dborl_parameter_system_info::NOT_DEFINED, "flat_image", dborl_parameter_type_info::FILEASSOC); 

    normalize_.set_values(param_list_, "data", "normalize", "normalize similarity values?", true, true);
    norm_reconst_bound_.set_values(param_list_, "data", "norm_reconst_bound", "normalize using reconstructed boundary length?", true, true);
    norm_tot_splice_cost_.set_values(param_list_, "data", "norm_tot_splice_cost", "normalize using total splice cost?", false, false);
    norm_con_arclength_.set_values(param_list_, "data", "norm_con_arclength", "normalize using arclength of the con file?", false, false);
  
    edit_params_.algo_name_ = "test_algo_name";
    add_params(edit_params_);  // adds with the defaults

  }
};

static void test_algo_params(int argc, char* argv[])
{
  testlib_test_start("testing algo_params class ");
 
  test_params p;

  p.edit_params_.curve_matching_R_ = 2.0;
  p.print_input_xml("test_params_default.xml");

  test_params p2;
  p2.input_param_filename_ = "test_params_default.xml";
  p2.parse_input_xml();

  TEST_NEAR("testing added params", p2.edit_params_.curve_matching_R_(), p.edit_params_.curve_matching_R_(), 0.01);

  vcl_string out_name = p2.edit_params_.output_file_postfix();
  vcl_cout << "out_name: " << out_name << vcl_endl;

  out_name = p2.edit_params_.output_file_postfix("replaced_name");
  vcl_cout << "out_name2: " << out_name << vcl_endl;

  //: test parsing evaluation.xml file
  //vcl_map<vcl_string, dborl_exp_stat_sptr> category_statistics;
  //vcl_string algo_name;
  //TEST("testing parse evaluation", parse_evaluation_file("evaluation_k_1_N_1_0.5.xml", category_statistics, algo_name), true);

  vcl_string out_file = "eval_obj.xml";
  vcl_string obj_name = "dummy_obj1";
  vcl_vector<vsol_box_2d_sptr> detected_boxes;
  vsol_box_2d_sptr b1 = new vsol_box_2d();
  b1->add_point(203, 809);
  b1->add_point(300, 500);
  detected_boxes.push_back(b1);
  vsol_box_2d_sptr b2 = new vsol_box_2d(*b1);
  b2->add_point(400, 1000);
  detected_boxes.push_back(b2);
  vcl_vector<vcl_string> categories;
  categories.push_back("dummy_cat1");
  categories.push_back("dummy_cat2");
  dborl_exp_stat stat;
  stat.increment_TP_by(4);
  stat.increment_FP_by(0);
  stat.increment_TN_by(400);
  stat.increment_FN_by(100);
  print_obj_evaluation(out_file, obj_name, detected_boxes, categories, stat);

  detected_boxes.clear();
  categories.clear();
  vcl_string new_name;
  dborl_exp_stat new_stat;
  bool out = parse_obj_evaluation(out_file, new_name, detected_boxes, categories, new_stat);
  TEST("test parsing obj evaluation parsing", out, true);
  TEST("test parsing obj evaluation obj name", new_name.compare(obj_name), 0);
  TEST("test parsing obj evaluation det box size", detected_boxes.size(), 2);
  TEST("test parsing obj evaluation cat size", categories.size(), 2);
  TEST_NEAR("test parsing obj evaluation 0 minx", detected_boxes[0]->get_min_x(), 203, 0.01);
  TEST_NEAR("test parsing obj evaluation 0 miny", detected_boxes[0]->get_min_y(), 500, 0.01);
  TEST_NEAR("test parsing obj evaluation 0 max x", detected_boxes[0]->get_max_x(), 300, 0.01);
  TEST_NEAR("test parsing obj evaluation 0 max y", detected_boxes[0]->get_max_y(), 809, 0.01);
  TEST_NEAR("test parsing obj evaluation 1 max y", detected_boxes[1]->get_max_y(), 1000, 0.01);
  TEST("test parsing obj evaluation TP", new_stat.TP_, 4);
  TEST("test parsing obj evaluation FP", new_stat.FP_, 0);
  TEST("test parsing obj evaluation TN", new_stat.TN_, 400);
  TEST("test parsing obj evaluation FN", new_stat.FN_, 100);

}

TESTMAIN_ARGS(test_algo_params)
