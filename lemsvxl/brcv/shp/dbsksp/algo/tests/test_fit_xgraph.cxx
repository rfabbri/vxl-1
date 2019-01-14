// This is shp/dbsksp/tests/test_fit_xgraph.cxx

// \author Nhon Trinh
// \date Nov 17, 2009

#include <testlib/testlib_test.h>
#include <dbtest/dbtest_root_dir.h>
//#include <vul/vul_timer.h>
//#include <dbsksp/algo/dbsksp_xshock_directed_tree.h>
//#include <dbsksp/algo/dbsksp_edit_distance.h>
//
#include <dbsksp/xio/dbsksp_xio_xshock_graph.h>




// ----------------------------------------------------------------------------
void test_fit_xgraph()
{
  std::string data_dir = dbtest_root_dir() + "/brcv/shp/dbsksp/algo/tests/data";
  std::string xgraph_file = data_dir + "/calf1.xml";

  // Load the two shock graphs from file
  dbsksp_xshock_graph_sptr xgraph = 0;
  //dbsksp_xshock_graph_sptr xgraph2 = 0;

  x_read(xgraph_file, xgraph);
  TEST("Loading xgraph", xgraph != 0, true);
  
  if (!xgraph)
  {
    std::cerr << "\nERROR: Couldn't load the source shock graph!\n";
    return;
  }

}




//: Test closest point functions
MAIN( test_fit_xgraph )
{
  START ("Test fitting xgraph");
  test_fit_xgraph();
  SUMMARY();
}
