#include <testlib/testlib_test.h>

#include <dborl/dborl_evaluation.h>
#include <dborl/dborl_exp_stat_sptr.h>
#include <vsol/vsol_box_2d.h>
#include <dborl/algo/dborl_image_desc_parser.h>
#include <dborl/dborl_image_description.h>
#include <dborl/dborl_image_polygon_description.h>

#include <vcl_iostream.h>

MAIN_ARGS(test_evaluation)
{
  testlib_test_start("testing dborl_evaluation classes ");
 
  dborl_exp_stat_sptr es = new dborl_exp_stat(5, 6);

  TEST("test exp stat constructor ", es->positive_cnt_, 5);
  TEST("test exp stat constructor ", es->negative_cnt_, 6);
  es->increment_TP();
  es->increment_TP();
  es->increment_FN();
  TEST("test all_done() ", es->all_done(), false);
  es->increment_FN();
  es->increment_TP();

  es->increment_FP();
  es->increment_FP();
  es->increment_FP();
  TEST("test all_done() ", es->all_done(), false);
  es->increment_FP();
  es->increment_TN();
  es->increment_TN();

  TEST("test all_done() ", es->all_done(), true);
  vcl_cout << "es statistics:\n";
  es->print_stats();

  TEST_NEAR("test TPR() ", es->TPR(), 3.0f/5.0f, 0.001);
  TEST_NEAR("test FPR() ", es->FPR(), 4.0f/6.0f, 0.001);

  TEST_NEAR("test recall() ", es->recall(), 3.0f/5.0f, 0.001);
  TEST_NEAR("test precision() ", es->precision(), 3.0f/7.0f, 0.001);
  TEST_NEAR("test accuracy() ", es->accuracy(), 5.0f/11.0f, 0.001);  
  TEST_NEAR("test f_measure() ", es->f_measure(), 1.0f/2.0f, 0.001);  
  TEST_NEAR("test sensitivity() ", es->sensitivity(), 3.0f/5.0f, 0.001);  
  TEST_NEAR("test specifity() ", es->specifity(), 2.0f/6.0f, 0.001);  


  dborl_exp_stat_sptr es2 = new dborl_exp_stat(5, 6);
  vsol_box_2d_sptr gt_box;
  vsol_box_2d_sptr det_box;

  float t = 0.5f;
  dborl_evaluation_evaluate_detection(*es2, gt_box, det_box, t);  // TN
  dborl_evaluation_evaluate_detection(*es2, gt_box, det_box, t);  // TN
  det_box = new vsol_box_2d();
  det_box->add_point(0,0);
  det_box->add_point(1,1);
  dborl_evaluation_evaluate_detection(*es2, gt_box, det_box, t); // FP
  gt_box = new vsol_box_2d();
  gt_box->add_point(1.1,1.1);
  gt_box->add_point(2,2);
  dborl_evaluation_evaluate_detection(*es2, gt_box, det_box, t); // FP
  gt_box->add_point(1,1);
  gt_box->add_point(2,2);
  dborl_evaluation_evaluate_detection(*es2, gt_box, det_box, t); // FP
  gt_box->add_point(0.8, 0.8);
  dborl_evaluation_evaluate_detection(*es2, gt_box, det_box, t); // FP
  TEST("test all_done() ", es2->all_done(), false);
  gt_box = new vsol_box_2d();
  gt_box->add_point(0,0);
  gt_box->add_point(0.5,1);
  dborl_evaluation_evaluate_detection(*es2, gt_box, det_box, t); // TP
  gt_box->add_point(1,1);
  dborl_evaluation_evaluate_detection(*es2, gt_box, det_box, t); // TP
  gt_box->add_point(1.2,1.2);
  dborl_evaluation_evaluate_detection(*es2, gt_box, det_box, t); // TP
  det_box = 0;
  dborl_evaluation_evaluate_detection(*es2, gt_box, det_box, t); // FN
  dborl_evaluation_evaluate_detection(*es2, gt_box, det_box, t); // FN

  TEST("test all_done() ", es2->all_done(), true);
  vcl_cout << "es2 statistics:\n";
  es2->print_stats();

  TEST_NEAR("test es2 TPR() ", es2->TPR(), es->TPR(), 0.001);  
  TEST_NEAR("test es2 recall() ", es2->recall(), es->recall(), 0.001);  
  TEST_NEAR("test es2 FPR() ", es2->FPR(), es->FPR(), 0.001);  
  TEST_NEAR("test es2 precision() ", es2->precision(), es->precision(), 0.001);  
  TEST_NEAR("test es2 accuracy() ", es2->accuracy(), es->accuracy(), 0.001);  
  TEST_NEAR("test es2 f_measure() ", es2->f_measure(), es->f_measure(), 0.001);  
  TEST_NEAR("test es2 sensitivity() ", es2->sensitivity(), es->sensitivity(), 0.001);  
  TEST_NEAR("test es2 specifity() ", es2->specifity(), es->specifity(), 0.001);  

  //: check each gt_box of the model_category in this query image
  dborl_image_desc_parser parser;
  parser.clear();
  dborl_image_description_sptr query_desc1 = dborl_image_description_parse("bonefishesocc1.xml", parser);

  parser.clear();
  dborl_image_description_sptr query_desc2 = dborl_image_description_parse("kk0731.xml", parser);

  parser.clear();
  dborl_image_description_sptr model_desc = dborl_image_description_parse("bonefishes.xml", parser);
  
  vcl_vector<vsol_polygon_2d_sptr>& polys = query_desc1->category_data_->cast_to_image_polygon_description()->get_polygon_vector("fish");
  TEST("test polys ", polys.size(), 1);
  polys[0]->compute_bounding_box();
  vsol_box_2d_sptr query_box = polys[0]->get_bounding_box();

  dborl_exp_stat instance_stat;
  vsol_box_2d_sptr det_box2 = new vsol_box_2d();
  det_box2->add_point(10, 10);
  det_box2->add_point(20, 20);
  vsol_box_2d_sptr box = dborl_evaluation_evaluate_detection(instance_stat, "fish", query_desc1, det_box2, 0.5);
  instance_stat.print_stats();
  TEST("test dummy box() ", box, 0);  
  TEST("FP should be 1", instance_stat.FP_, 1);
  TEST("TP should be 0", instance_stat.TP_, 0);
  TEST("FN should be 0", instance_stat.FN_, 0);
  TEST("TN should be 0", instance_stat.TN_, 0);

  dborl_exp_stat instance_stat2;
  box = dborl_evaluation_evaluate_detection(instance_stat2, "fish", query_desc1, query_box, 0.5);
  TEST("FP should be 0", instance_stat2.FP_, 0);
  TEST("TP should be 1", instance_stat2.TP_, 1);
  TEST("FN should be 0", instance_stat2.FN_, 0);
  TEST("TN should be 0", instance_stat2.TN_, 0);

  dborl_exp_stat instance_stat3;
  box = dborl_evaluation_evaluate_detection(instance_stat3, "fish", query_desc1, 0, 0.5);
  TEST("FP should be 0", instance_stat3.FP_, 0);
  TEST("TP should be 0", instance_stat3.TP_, 0);
  TEST("FN should be 1", instance_stat3.FN_, 1);
  TEST("TN should be 0", instance_stat3.TN_, 0);

  dborl_exp_stat instance_stat4;
  box = dborl_evaluation_evaluate_detection(instance_stat4, "fish", query_desc2, 0, 0.5);
  TEST("FP should be 0", instance_stat4.FP_, 0);
  TEST("TP should be 0", instance_stat4.TP_, 0);
  TEST("FN should be 0", instance_stat4.FN_, 0);
  TEST("TN should be 1", instance_stat4.TN_, 1);

  dborl_exp_stat instance_stat5;
  box = dborl_evaluation_evaluate_detection(instance_stat5, "fish", query_desc2, query_box, 0.5);
  TEST("FP should be 1", instance_stat5.FP_, 1);
  TEST("TP should be 0", instance_stat5.TP_, 0);
  TEST("FN should be 0", instance_stat5.FN_, 0);
  TEST("TN should be 0", instance_stat5.TN_, 0);

  parser.clear();
  dborl_image_description_sptr query_desc3 = dborl_image_description_parse("dogfishsharks.xml", parser);
  vsol_box_2d_sptr detected_box = new vsol_box_2d();
  detected_box->add_point(16.571, 24.724);
  detected_box->add_point(69.362, 63.261);
  dborl_exp_stat instance_stat6;
  box = dborl_evaluation_evaluate_detection(instance_stat6, "fish", query_desc3, detected_box, 0.5);
  TEST("FP should be 0", instance_stat6.FP_, 0);
  TEST("TP should be 1", instance_stat6.TP_, 1);
  TEST("FN should be 0", instance_stat6.FN_, 0);
  TEST("TN should be 0", instance_stat6.TN_, 0);

  return testlib_test_summary();
}
