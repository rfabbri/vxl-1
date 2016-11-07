// This is shp/dbsksp/tests/test_shock_graph.cxx

// \author Nhon Trinh
// \date Sep 27, 2006

#include <testlib/testlib_test.h>
#include <dbsksp/dbsksp_shock_graph.h>
#include <dbsksp/dbsksp_shock_graph_sptr.h>


// ----------------------------------------------------------------------------
void test_shock_graph()
{
  dbsksp_shock_graph_sptr graph = new dbsksp_shock_graph();

  //TEST_NEAR("Compute center of enclosing circle", center_err, 0, 1e-10);
}






//: Test closest point functions
MAIN( test_shock_graph )
{
  START (" Test shock graph ");
  test_shock_graph();
  SUMMARY();
}
