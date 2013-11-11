//: 
// \file     test_proc_filter_stats.cxx
// \brief    testing for running statistics on filter responses
// \author   Gamze Tunali
// \date     2005-10-10
// 

#include <testlib/testlib_test.h>
#include <xmvg/xmvg_filter_response.h>
#include <util/proc_filter_stats.h>
#include <vnl/vnl_matrix.h>
#include <vcl_vector.h>

static void test_proc_filter_stats(){
  //generate some responses
  xmvg_filter_response<double> r1(6, 2.);
  xmvg_filter_response<double> r2(6, 6.);
  xmvg_filter_response<double> r3(6, 10.);

  vcl_vector<xmvg_filter_response<double> > resp;
  resp.push_back(r1);
  resp.push_back(r2);
  resp.push_back(r3);

  proc_filter_stats stats;

  xmvg_filter_response<double> mean(6, 0);
  stats.mean(resp, mean);
  
  double val = (2.+6.+10.)/3.;
  TEST_NEAR("mean 0", mean[0], val, 1e-06);
  TEST_NEAR("mean 1", mean[1], val, 1e-06);
  TEST_NEAR("mean 2", mean[2], val, 1e-06);
  TEST_NEAR("mean 3", mean[3], val, 1e-06);
  TEST_NEAR("mean 4", mean[4], val, 1e-06);
  TEST_NEAR("mean 5", mean[5], val, 1e-06);

  vnl_matrix<double> covar;
  stats.covar(resp, covar);
  // covariance matrix has 10.6667 at each [i][j]
  TEST_NEAR("Covariance-->", covar[0][0], 10.666, 1e-03);
  TEST_NEAR("Covariance min value-->", covar.min_value(), 10.666, 1e-03);
  TEST_NEAR("Covariance max value-->", covar.min_value(), 10.666, 1e-03);

}

TESTMAIN(test_proc_filter_stats);
