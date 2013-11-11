#include <testlib/testlib_test.h>
#include <mw/mw_discrete_corresp.h>
#include <vcl_limits.h>
#include <vnl/vnl_math.h>
#include <dborl/dborl_evaluation.h>



static const double tolerance=vcl_numeric_limits<double>::epsilon()*100;

//: Currently tests both IO and other functions of mw_discrete_corresp
MAIN( test_discrete_corresp_io )
{
  START ("mw_discrete_corresp io");

  {

  //----- BINARY I/O ----------------------------------------------------------
  mw_discrete_corresp cp_out(4,7);

  cp_out.corresp_[0].push_back(mw_attributed_object(13,0,1));

  cp_out.corresp_[1].push_back(mw_attributed_object(0,0,1));
  cp_out.corresp_[1].push_back(mw_attributed_object(69,0,33.1425));
  cp_out.corresp_[1].push_back(mw_attributed_object(69,0,2.24789));

  // null match
  cp_out.corresp_[2].push_back(mw_attributed_object(0,1,1));

  // empty list 
  // cp_out.corresp_[3]

  vcl_cout << "Initial data:" << vcl_endl;
  vcl_cout << cp_out;

  // ------ Writing -------
  vsl_b_ofstream bfs_out("mw_discrete_corresp.tmp");
  TEST("Created mw_discrete_corresp.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out, cp_out);
  bfs_out.close();

  // ------ Reading -------

  mw_discrete_corresp cp_in;

  vsl_b_ifstream bfs_in("mw_discrete_corresp.tmp");
  TEST("Opened mw_discrete_corresp.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, cp_in);
  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();
  
  TEST("cp_out == cp_in", cp_out, cp_in);

  vcl_cout << "Recovered data:" << vcl_endl;
  vcl_cout << cp_in;


  //----- OTHER TESTS ----------------------------------------------------------
  
  cp_out.sort();
  vcl_cout << vcl_endl;
  vcl_cout << "Sorted:\n " << cp_out << vcl_endl;

  cp_out.threshold_by_cost(5);
  vcl_cout << vcl_endl;
  vcl_cout << "Remove all costs > 5:\n " << cp_out << vcl_endl;

  }


  //---- STATISTICS ----------------------------------------------------------

  {
  vcl_cout << "\nTesting statistics ----------------------------------------\n";
  mw_discrete_corresp gt(10,10);

  gt.corresp_[0].push_back(mw_attributed_object(7,0,0));
  gt.corresp_[1].push_back(mw_attributed_object(2));
  gt.corresp_[1].push_back(mw_attributed_object(3));
  gt.corresp_[2].push_back(mw_attributed_object(3));
  gt.corresp_[3].push_back(mw_attributed_object(6));
  gt.corresp_[4].push_back(mw_attributed_object(4));
  gt.corresp_[4].push_back(mw_attributed_object(5));
  // 5 : empty
  gt.corresp_[6].push_back(mw_attributed_object(3));
  // 7 : empty
  gt.corresp_[8].push_back(mw_attributed_object(8));
  gt.corresp_[8].push_back(mw_attributed_object(7));
  gt.corresp_[8].push_back(mw_attributed_object(6));

  gt.corresp_[9].push_back(mw_attributed_object(6));

  TEST("#empty (including dummy)", gt.count_empty(), 3);


  double inf = vcl_numeric_limits<double>::infinity();
  mw_discrete_corresp cp(10,10);
  cp.corresp_[0].push_back(mw_attributed_object(4,0,inf));
  cp.corresp_[0].push_back(mw_attributed_object(5,0,inf));
  cp.corresp_[0].push_back(mw_attributed_object(7,0,1e-2));  // T

  //: Infinite cost doesn't count.
  cp.corresp_[1].push_back(mw_attributed_object(5,0,0));  // F
  cp.corresp_[1].push_back(mw_attributed_object(7,0,inf));
  cp.corresp_[1].push_back(mw_attributed_object(4,0,inf));
  cp.corresp_[1].push_back(mw_attributed_object(6,0,inf));
  cp.corresp_[1].push_back(mw_attributed_object(1,0,inf));
  cp.corresp_[1].push_back(mw_attributed_object(2,0,inf));

  cp.corresp_[2].push_back(mw_attributed_object(3));  // T
  cp.corresp_[2].push_back(mw_attributed_object(4));  // F
  cp.corresp_[2].push_back(mw_attributed_object(5));  // F
  cp.corresp_[2].push_back(mw_attributed_object(6));  // F


  cp.corresp_[3].push_back(mw_attributed_object(1,0,0.1));  // F
  cp.corresp_[3].push_back(mw_attributed_object(2,0,0.2));  // F
  cp.corresp_[3].push_back(mw_attributed_object(3,0,0.3));  // F
  cp.corresp_[3].push_back(mw_attributed_object(4,0,0.4));  // F
  cp.corresp_[3].push_back(mw_attributed_object(6,0,0.6));  // T

  cp.corresp_[4].push_back(mw_attributed_object(5,0,0));  // T

  cp.corresp_[6].push_back(mw_attributed_object(4,0,inf));
  cp.corresp_[6].push_back(mw_attributed_object(2,0,inf));
  cp.corresp_[6].push_back(mw_attributed_object(7,0,inf));
  cp.corresp_[6].push_back(mw_attributed_object(3,0,0));  // T
  cp.corresp_[6].push_back(mw_attributed_object(0,0,inf));
  cp.corresp_[6].push_back(mw_attributed_object(1,0,inf));

  //: empty g-t doesn't count.
  cp.corresp_[7].push_back(mw_attributed_object(1,0,inf));
  cp.corresp_[7].push_back(mw_attributed_object(2,0,inf));
  cp.corresp_[7].push_back(mw_attributed_object(3,0,inf));

  cp.corresp_[8].push_back(mw_attributed_object(8,0,inf));
  cp.corresp_[8].push_back(mw_attributed_object(7,0,2.3));  // T
  cp.corresp_[8].push_back(mw_attributed_object(6,0,0.1));  // T
  cp.corresp_[8].push_back(mw_attributed_object(5,0,0));  // F


  cp.corresp_[9].push_back(mw_attributed_object(1,0,0.1));  // F
  cp.corresp_[9].push_back(mw_attributed_object(2,0,0.2));  // F
  cp.corresp_[9].push_back(mw_attributed_object(3,0,0.3));  // F
  cp.corresp_[9].push_back(mw_attributed_object(4,0,0.4));  // F
  cp.corresp_[9].push_back(mw_attributed_object(5,0,0.5));  // F
  cp.corresp_[9].push_back(mw_attributed_object(6,0,0.6));  // T
  

  // sort & print

  cp.sort();

  // compare
  cp.compare_and_print(&gt);

  /* 
     # objects in image 1 having correct candidates ranked among top 50% matches:
        71.4286% (5 out of 7).
     # objects in image 1 having correct candidates in top 5 matches:
        85.7143% (6 out of 7).
  */

  unsigned p1_idx = 0;
  vcl_list<mw_attributed_object>::const_iterator  itr;
  itr = cp.find_right_corresp_mincost(p1_idx,&gt);

  TEST("find_right_corresp_mincost idx 0",itr->obj_,7);

  p1_idx = 4;
  itr = cp.find_right_corresp_mincost(p1_idx,&gt);

  TEST("find_right_corresp_mincost idx 4",itr->obj_,5);

  //: empty
  p1_idx = 5; 
  itr = cp.find_right_corresp_mincost(p1_idx,&gt);

  TEST("find_right_corresp_mincost idx 5",itr,cp.corresp_[p1_idx].end());

  //: if many ground-truth possibilities, then have to get minimum cost:
  p1_idx = 8; 
  itr = cp.find_right_corresp_mincost(p1_idx,&gt);

  TEST("find_right_corresp_mincost idx 8",itr->obj_,6);

  //----------------------------------------------
  // Test for new function that gives % correspondences having cost above right
  // ones


//  unsigned n, n_valid;
//  cp.percentage_of_matches_above_truth(n,n_valid,&gt);

//  vcl_cout << "n: " << n << " n_valid: " << n_valid << vcl_endl;
//  TEST("percentage_of_matches_above_truth:  nmatches",n,1);
//  TEST("percentage_of_matches_above_truth:  n_valid",n_valid,15);

  unsigned n_w_gt;
  cp.number_of_pts1_with_gt_among_any_candidates(n_w_gt,&gt);
  vcl_cout << "# of objects in image 1 having ground-truth among any candidates:\n\t" 
    << 100.0*(float)n_w_gt/(float)(cp.size()) << "% ("  << n_w_gt << " out of " << cp.size()<< ")." << vcl_endl;
  TEST("# of objects in image 1 having ground-truth among any candidates: n_w_gt", n_w_gt,9);
  // remember the policy that if both gt and candidates are empty, then it counts.

  vcl_vector<bool> expected_result;
  expected_result.resize(cp.size());
  expected_result[0] = true;
  expected_result[1] = false;
  expected_result[2] = true;
  expected_result[3] = true;
  expected_result[4] = true;
  expected_result[5] = true;
  expected_result[6] = true;
  expected_result[7] = true;
  expected_result[8] = true;
  for (unsigned i=0; i < cp.size(); ++i) {
    bool result = cp.is_gt_among_top5(i, &gt);
    TEST("is_gt_among_top5",result,expected_result[i]);
  }

  {
  dborl_exp_stat s;
  gt.exp_stats(s, &gt);
  s.print_stats();
  TEST("consistent FP/FN/TP/TN", s.all_done(), true);
  TEST_NEAR("self-test", s.precision(), 1.0, 1e-9);
  TEST_NEAR("self-test", s.recall(), 1.0, 1e-9);
  TEST_EQUAL("n0", gt.n0(), 11);
  TEST_EQUAL("n1", gt.n1(), 11);
  TEST_EQUAL("#positives == num corresps", s.positive_cnt_, 12);
  TEST_EQUAL("#negatives == possible corresps - num corresps", s.negative_cnt_, 11*11 - 12);
  }

  {
  dborl_exp_stat s;
  cp.exp_stats(s, &gt);
  s.print_stats();
  TEST("consistent FP/FN/TP/TN", s.all_done(), true);
  TEST_EQUAL("TP", s.TP_, 8);
  TEST_EQUAL("FP", s.FP_, 14);
  TEST_EQUAL("TN", s.TN_, s.negative_cnt_ - s.FP_);
  TEST_EQUAL("FN", s.FN_, s.positive_cnt_ - s.TP_);
  }

  {
  dborl_exp_stat s;
  gt.exp_stats_hitmiss(s, &gt);
  s.print_stats();
  TEST("consistent FP/FN/TP/TN", s.all_done(), true);
  TEST_NEAR("self-test", s.precision(), 1.0, 1e-9);
  TEST_NEAR("self-test", s.recall(), 1.0, 1e-9);
  TEST_EQUAL("#positives", s.positive_cnt_, 12);
  TEST_EQUAL("#negatives", s.negative_cnt_, 11*11-s.positive_cnt_);
  }

  {
  dborl_exp_stat s;
  cp.exp_stats_hitmiss(s, &gt);
  vcl_cout << vcl_endl;
  s.print_stats();
  TEST("consistent FP/FN/TP/TN", s.all_done(), true);
  // cp[0]: TP
  // cp[1]: FP
  // cp[2]: FP FP FP FP
  // cp[3]: FP FP FP FP FP
  // cp[4]: TP TP
  // cp[5]:
  // cp[6]: TP
  // cp[7]: 
  // cp[8]: FP FP FP
  // cp[9]: FP FP FP FP FP FP
  // cp[10]:
  TEST_EQUAL("TP", s.TP_, 4);
  TEST_EQUAL("FP", s.FP_, 19);
  TEST_EQUAL("TN", s.TN_, s.negative_cnt_ - s.FP_);
  TEST_EQUAL("FN", s.FN_, s.positive_cnt_ - s.TP_);
  }

  { 
  mw_discrete_corresp cp_copy = cp;
  cp_copy.threshold_by_cost_lteq(0);
  cp_copy.threshold_by_cost(1e6);
  mw_discrete_corresp cp_copy2 = cp_copy;

  cp_copy.keep_only_extreme_cost(true);
  cp_copy2.keep_only_unambiguous_max(0);
  TEST("keep_only_unambiguous_max degenerates to max", cp_copy == cp_copy2, true);
  }

  }

  SUMMARY();
}
