//:
// \file
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>

#include <dbfs/dbfs_selector_sptr.h>
#include <dbfs/dbfs_selector.h>
#include <dbfs/dbfs_measure.h>

//: Test the selector class
//
void selector_test()
{
  //: create a document count table for a class-non-class problem with 1 feature
  vcl_vector<int> f_cnts(6, 0);
  f_cnts[0] = 49;   // documents in class containing f0
  f_cnts[1] = 141;  // doucments in class not containing f0

  f_cnts[2] = 190;   // documents in class containing f1
  f_cnts[3] = 0;  // doucments in class not containing f2

  f_cnts[4] = 100;   // documents in class containing f2
  f_cnts[5] = 90;  // doucments in class not containing f2

  vcl_vector<vcl_vector<int> > class_f_table;
  class_f_table.push_back(f_cnts);

  f_cnts[0] = 27652;   // documents not in class containing f0
  f_cnts[1] = 774106;  // doucments not in class not containing f0

  f_cnts[2] = 0;   // documents not in class containing f1
  f_cnts[3] = 801758;  // doucments not in class not containing f1

  f_cnts[4] = 1758;   // documents not in class containing f2
  f_cnts[5] = 800000;  // doucments not in class not containing f2
  
  class_f_table.push_back(f_cnts);

  dbfs_measure_sptr ms = new dbfs_measure_simple(class_f_table);

  vcl_cout << "mutual info for feature 0: " << ms->mutual_info(0,0) << "\nmutual info for feature 1: " << ms->mutual_info(0,1);
  vcl_cout << "\nmutual info for feature 2: " << ms->mutual_info(0,2) << "\n";

  double val1 = (801948.0*49.0)/((49.0+27652.0)*(49.0+141.0));
  double val = (49.0/801948.0)*(vcl_log(val1)/vcl_log(2.0));
  TEST_NEAR("testing mutual info", ms->mutual_info(0,0), val, 0.0000001); 

  float val2 = float((801948.0*27652.0)/((49.0+27652.0)*(27652.0+774106.0)));
  float val3 = float((27652.0/801948.0))*float((vcl_log(val2)/vcl_log(2.0)));
  TEST_NEAR("testing mutual info", ms->mutual_info(1,0), val3, 0.0000001); 

  TEST_NEAR("testing mutual info class average", ms->mutual_info_class_average(0), val+val3, 0.0000001); 

  float val4 = float((801948.0*190.0)/((190.0+0.0)*(190.0+0.0)));
  float val5 = float((190.0/801948.0))*float((vcl_log(val4)/vcl_log(2.0)));
  TEST_NEAR("testing mutual info", ms->mutual_info(0,1), val5, 0.0000001); 

  dbfs_selector_sptr s = new dbfs_selector(ms, 3, 2); // 3 features and 2 classes
  vcl_vector<int> best_feature_ids;
  s->get_top_features(0, 1, best_feature_ids); 
  TEST("testing selector", best_feature_ids.size(), 1);
  TEST("testing selector", best_feature_ids[0], 1);
  s->get_top_features(0, 2, best_feature_ids); 
  TEST("testing selector", best_feature_ids.size(), 2);
  TEST("testing selector", best_feature_ids[0], 1);
  TEST("testing selector", best_feature_ids[1], 2);
  s->get_top_features(0, 3, best_feature_ids); 
  TEST("testing selector", best_feature_ids.size(), 3);
  TEST("testing selector", best_feature_ids[0], 1);
  TEST("testing selector", best_feature_ids[1], 2);
  TEST("testing selector", best_feature_ids[2], 0);

  dbfs_selector_sptr s2 = new dbfs_selector_max_class_min_non_class(ms, 3, 2); // 3 features and 2 classes
  best_feature_ids.clear();
  s2->get_top_features(0, 1, best_feature_ids); 
  TEST("testing selector max min", best_feature_ids.size(), 1);
  TEST("testing selector max min", best_feature_ids[0], 1);
  s2->get_top_features(0, 2, best_feature_ids); 
  TEST("testing selector max min", best_feature_ids.size(), 2);
  TEST("testing selector max min", best_feature_ids[0], 1);
  TEST("testing selector max min", best_feature_ids[1], 2);
  s2->get_top_features(0, 3, best_feature_ids); 
  TEST("testing selector max min", best_feature_ids.size(), 3);
  TEST("testing selector max min", best_feature_ids[0], 1);
  TEST("testing selector max min", best_feature_ids[1], 2);
  TEST("testing selector max min", best_feature_ids[2], 0);  
  
}

MAIN( selector_test )
{
  START( "dbfs_selector" );
  selector_test();
  SUMMARY();
}
