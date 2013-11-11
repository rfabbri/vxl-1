#include <testlib/testlib_test.h>
#include <dbskr/dbskr_tree.h>
#include <dbskr/dbskr_tree_sptr.h>
#include <dbskr/dbskr_tree_edit.h>
#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>
#include <vcl_iostream.h>
#include <vcl_ctime.h>
//#include <vcl_cmath.h>

MAIN_ARGS(test_tree_edit)
{

  vcl_string dir_base;

  if ( argc >= 2 ) {
    dir_base = argv[1];
#ifdef VCL_WIN32
    dir_base += "\\";
#else
    dir_base += "/";
#endif
  }

  testlib_test_start("testing tree edit class ");

  vcl_string data_file1, data_file2, pathtable_file, shgm_filename;
  data_file1 = "data/cat01.shg";
  data_file2 = "data/cat01.shg";
  pathtable_file = "data/cat01-cat01-table.txt";
  clock_t time1, time2;
  float val;
  dbskr_tree_sptr tree1, tree2;
  dbsk2d_xshock_graph_fileio loader;

/*
  tree1 = new dbskr_tree();
  tree2 = new dbskr_tree();

  time1 = clock();
  dbskr_tree_edit edit0(tree1, tree2);
  TEST("read_data() cat01 cat01 ", edit0.read_data(dir_base+data_file1, dir_base+data_file2, dir_base+pathtable_file), true);
  TEST("edit() ", edit0.edit(), true);
  val = edit0.final_cost();
  TEST_NEAR("final_cost() ", val, 0.0, 0.001);
  time2 = clock();
  vcl_cout << "found in "<< ((double)(time2-time1))/CLOCKS_PER_SEC << " seconds. " <<vcl_endl;

  data_file1 = "data/brk001.shg";
  data_file2 = "data/pln006.shg";
  pathtable_file = "data/brk001-pln006-table.txt";
  
  tree1 = new dbskr_tree();
  tree2 = new dbskr_tree();

  time1 = clock();
  dbskr_tree_edit edit(tree1, tree2);
  TEST("read_data() brk001 pln006 ", edit.read_data(dir_base+data_file1, dir_base+data_file2, dir_base+pathtable_file), true);
  TEST("edit() ", edit.edit(), true);
  val = edit.final_cost();
  TEST_NEAR("final_cost() ", val, 566.2, 0.001);
  time2 = clock();
  vcl_cout << "found in "<< ((double)(time2-time1))/CLOCKS_PER_SEC << " seconds. " <<vcl_endl;
*/
  //: NOW TESTING TO FIND PATHS TOGETHER WITH COSTS (essentially same methods but they also save 
  //  paths in the dynamic programming part
  //  this takes time so there is also an option not to do so 
/*
  //data_file1 = "data/cat01.shg";
  //data_file2 = "data/cat01.shg";
  //pathtable_file = "data/cat01-cat01-table.txt";
  data_file1 = "data/cat001.esf";
  data_file2 = "data/cat001.esf";
  shgm_filename = "data/cat01-cat01.shgm";
  
  tree1 = new dbskr_tree();
  tree2 = new dbskr_tree();

  time1 = clock();
  dbskr_tree_edit edit6(tree1, tree2);
  TEST("read_data() cat01 cat01 ", edit6.read_data(dir_base+data_file1, dir_base+data_file2, dir_base+pathtable_file), true);
  edit6.save_path(true);
  TEST("edit() ", edit6.edit(), true);
  time2 = clock();
  vcl_cout << "found in "<< ((double)(time2-time1))/CLOCKS_PER_SEC << " seconds. " <<vcl_endl;
  TEST("compare_path() ", edit6.compare_path(dir_base+shgm_filename, 0.001), true);


  data_file1 = "data/fish0106.shg";
  data_file2 = "data/fish0104.shg";
  pathtable_file = "data/fish0106-fish0104-table.txt";
  shgm_filename = "data/fish0106-fish0104.shgm";
  
  tree1 = new dbskr_tree();
  tree2 = new dbskr_tree();

  time1 = clock();
  dbskr_tree_edit edit7(tree1, tree2);
  TEST("read_data() fish0106 fish0104 ", edit7.read_data(dir_base+data_file1, dir_base+data_file2, dir_base+pathtable_file), true);
  edit7.save_path(true);
  TEST("edit() ", edit7.edit(), true);
  time2 = clock();
  vcl_cout << "found in "<< ((double)(time2-time1))/CLOCKS_PER_SEC << " seconds. " <<vcl_endl;
  TEST("compare_path() ", edit7.compare_path(dir_base+shgm_filename, 0.001), true);


  data_file1 = "data/car20.shg";
  data_file2 = "data/car15.shg";
  pathtable_file = "data/car20-car15-table.txt";
  shgm_filename = "data/car20-car15.shgm";
  
  tree1 = new dbskr_tree();
  tree2 = new dbskr_tree();

  time1 = clock();
  dbskr_tree_edit edit8(tree1, tree2);
  TEST("read_data() car20 car15 ", edit8.read_data(dir_base+data_file1, dir_base+data_file2, dir_base+pathtable_file), true);
  edit8.save_path(true);
  TEST("edit() ", edit8.edit(), true);
  time2 = clock();
  vcl_cout << "found in "<< ((double)(time2-time1))/CLOCKS_PER_SEC << " seconds. " <<vcl_endl;
  TEST("compare_path() ", edit8.compare_path(dir_base+shgm_filename, 0.001), true);

  data_file1 = "data/horse19.shg";
  data_file2 = "data/flightbird02.shg";
  pathtable_file = "data/horse19-flightbird02-table.txt";
  shgm_filename = "data/horse19-flightbird02.shgm";
  
  tree1 = new dbskr_tree();
  tree2 = new dbskr_tree();

  time1 = clock();
  dbskr_tree_edit edit9(tree1, tree2);
  TEST("read_data() horse19 flightbird02 ", edit9.read_data(dir_base+data_file1, dir_base+data_file2, dir_base+pathtable_file), true);
  edit9.save_path(true);
  TEST("edit() ", edit9.edit(), true);
  time2 = clock();
  vcl_cout << "found in "<< ((double)(time2-time1))/CLOCKS_PER_SEC << " seconds. " <<vcl_endl;
  TEST("compare_path() ", edit9.compare_path(dir_base+shgm_filename, 0.001), true);


  data_file1 = "bon005.shg";
  data_file2 = "pln010.shg";
  pathtable_file = "bon005-pln010-table.txt";
  vcl_string shgm_filename = "bon005-pln010.shgm";

  tree1 = new dbskr_tree();
  tree2 = new dbskr_tree();

  time1 = clock();
  dbskr_tree_edit edit10(tree1, tree2);
  TEST("read_data() bon005 pln010 ", edit10.read_data(dir_base+data_file1, dir_base+data_file2, dir_base+pathtable_file), true);
  edit10.save_path(true);
  TEST("edit() ", edit10.edit(), true);
  float val = edit10.final_cost();
  //TEST_NEAR("final_cost() ", val, 644.57, 0.001);
  time2 = clock();
  vcl_cout << "cost calculated: " << val << vcl_endl;
  vcl_cout << "found in "<< ((double)(time2-time1))/CLOCKS_PER_SEC << " seconds. " <<vcl_endl;
  TEST("compare_path() ", edit10.compare_path(dir_base+shgm_filename, 0.001), true);

  
  data_file1 = "brd010.shg";
  data_file2 = "fsh025.shg";
  pathtable_file = "brd010-fsh025-table.txt";
  shgm_filename = "brd010-fsh025.shgm";

  tree1 = new dbskr_tree();
  tree2 = new dbskr_tree();

  time1 = clock();
  dbskr_tree_edit edit11(tree1, tree2);
  TEST("read_data() brd010 fsh025 ", edit11.read_data(dir_base+data_file1, dir_base+data_file2, dir_base+pathtable_file), true);
  edit11.save_path(true);
  TEST("edit() ", edit11.edit(), true);
  val = edit11.final_cost();
  //TEST_NEAR("final_cost() ", val, 644.57, 0.001);
  time2 = clock();
  vcl_cout << "cost calculated: " << val << vcl_endl;
  vcl_cout << "found in "<< ((double)(time2-time1))/CLOCKS_PER_SEC << " seconds. " <<vcl_endl;
  TEST("compare_path() ", edit11.compare_path(dir_base+shgm_filename, 0.001), true);
*/

//: TESTING BY LOADING FROM ESF FILE

  data_file1 = "data/brk001.esf";
  data_file2 = "data/pln006.esf";
  shgm_filename = "data/brk001-pln006.shgm";

  tree1 = new dbskr_tree();
  tree2 = new dbskr_tree();

  
  dbsk2d_shock_graph_sptr sg1 = loader.load_xshock_graph(dir_base+data_file1);
  tree1->acquire(sg1, false, true, false);

  dbsk2d_shock_graph_sptr sg2 = loader.load_xshock_graph(dir_base+data_file2);
  TEST("shock graph load() pln010.esf vertices ", sg2->number_of_vertices(), 32);
  TEST("shock graph load() pln010.esf edges ", sg2->number_of_edges(), 31);
  vcl_cout << " number of edges: " << sg2->number_of_edges() << vcl_endl;
  tree2->acquire(sg2, false, true, false);

  time1 = clock();
  dbskr_tree_edit edit12(tree1, tree2);
  edit12.save_path(true);

  //edit12.populate_table("mytable.txt");
  
  TEST("edit() ", edit12.edit(), true);
  val = edit12.final_cost();
  //TEST_NEAR("final_cost() ", val, 644.57, 0.001);
  time2 = clock();
  vcl_cout << "cost calculated: " << val << vcl_endl;
  vcl_cout << "found in "<< ((double)(time2-time1))/CLOCKS_PER_SEC << " seconds. " <<vcl_endl;
  TEST("compare_path() ", edit12.compare_path(dir_base+shgm_filename, 50), true);

/**
  data_file1 = "data/brd010.esf";
  data_file2 = "data/fsh025.esf";
  shgm_filename = "data/brd010-fsh025.shgm";

  tree1 = new dbskr_tree();
  tree2 = new dbskr_tree();
  
  dbsk2d_shock_graph_sptr sg3 = loader.load_xshock_graph(dir_base+data_file1);
  tree1->acquire(sg3);

  dbsk2d_shock_graph_sptr sg4 = loader.load_xshock_graph(dir_base+data_file2);
  tree2->acquire(sg4);

  time1 = clock();
  dbskr_tree_edit edit13(tree1, tree2);
  edit13.save_path(true);

  //edit13.populate_table("mytable.txt");
  
  TEST("edit() ", edit13.edit(), true);
  val = edit13.final_cost();
  //TEST_NEAR("final_cost() ", val, 644.57, 0.001);
  time2 = clock();
  vcl_cout << "cost calculated: " << val << vcl_endl;
  vcl_cout << "found in "<< ((double)(time2-time1))/CLOCKS_PER_SEC << " seconds. " <<vcl_endl;
  TEST("compare_path() ", edit13.compare_path(dir_base+shgm_filename, 50), true);
  */
  return testlib_test_summary();
}
