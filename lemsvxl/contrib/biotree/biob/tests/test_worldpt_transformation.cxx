#include <testlib/testlib_test.h>
#include <biob/biob_worldpt_transformation.h>

static void test_worldpt_transformation(){

  vnl_double_4x4 rot(0.0);
  rot[0][1]=1;
  rot[1][2]=-1;
  rot[2][0]=-1;
  rot[3][3]=1;
  vgl_h_matrix_3d<double> R(rot), Rinv = R.get_inverse();
  
  biob_worldpt_transformation tr(R);
  biob_worldpt_transformation trInv(Rinv);
  worldpt pt(1.0, 2.0, 3.0);
#if 0
//this variable is not used in the code.  PLEASE FIX!  -MM
  vgl_homg_point_3d<double> hpt;
#endif
  worldpt trpt = tr(pt);
  worldpt pretrpt = tr.preimage(trpt);
  
  TEST_NEAR("x coordinate", pretrpt.x(), 1.0, 1e-06);
  TEST_NEAR("y coordinate", pretrpt.y(), 2.0, 1e-06);

  worldpt inv_tr_pt = (tr.inverse())(pt);
  worldpt inv_inv_tr_pt = tr(inv_tr_pt);

  TEST_NEAR("x coordinate", inv_inv_tr_pt.x(), 1.0, 1e-06);
  TEST_NEAR("y coordinate", inv_inv_tr_pt.y(), 2.0, 1e-06);
}

TESTMAIN(test_worldpt_transformation);
