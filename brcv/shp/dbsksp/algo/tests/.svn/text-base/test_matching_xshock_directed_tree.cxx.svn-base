// This is shp/dbsksp/tests/test_matchingshock_graph.cxx

// \author Nhon Trinh
// \date Oct 30, 2009

#include <testlib/testlib_test.h>
#include <dbtest/dbtest_root_dir.h>
//#include <dbskr/dbskr_sm_cor.h>

//#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>
#include <vul/vul_timer.h>
#include <dbsksp/algo/dbsksp_xshock_directed_tree.h>
#include <dbsksp/algo/dbsksp_edit_distance.h>

#include <dbsksp/xio/dbsksp_xio_xshock_graph.h>




// ----------------------------------------------------------------------------
void test_matching_xshock_direced_tree()
{
  vcl_string data_dir = dbtest_root_dir() + "/brcv/shp/dbsksp/algo/tests/data";
  vcl_string xgraph_file1 = data_dir + "/calf1.xml";
  vcl_string xgraph_file2 = data_dir + "/dog1.xml";  

  //vcl_string shgm_file = "D:/vision/data/contour-files/shocks/match-calf1-dog1.shgm";

  // Load the two shock graphs from file
  dbsksp_xshock_graph_sptr xgraph1 = 0;
  dbsksp_xshock_graph_sptr xgraph2 = 0;

  x_read(xgraph_file1, xgraph1);
  x_read(xgraph_file2, xgraph2);
  
  if (!xgraph1 || !xgraph2)
  {
    vcl_cerr << "\nERROR: Couldn't load two source shock graphs!\n";
    return;
  }

  //bool norm_reconst = true;

  //////get the subsampling parameter
  //bool elastic_splice_cost    = false; 
  //float scurve_sample_ds      = 5.0f;
  //float scurve_interpolate_ds = 1.0f;
  //bool localized_edit         = false;
  //float scurve_matching_R    = 6.0f;
  //bool circular_ends          = true;
  //bool combined_edit          = false;

  ////bool coarse_edit            = false;


  //: prepare the trees also
  dbsksp_xshock_directed_tree_sptr tree1 = new dbsksp_xshock_directed_tree();
  tree1->acquire(xgraph1);

  dbsksp_xshock_directed_tree_sptr tree2 = new dbsksp_xshock_directed_tree();
  tree2->acquire(xgraph2);

  vcl_cout << "matching shock graphs...\n";
  vul_timer t;
  t.mark();




  //
  //float   norm_val = 0;

  //  << "original edit distance"<<vcl_endl;

  //instantiate the edit distance algorithm
  dbsksp_edit_distance edit_distance;
  edit_distance.set_tree1(tree1);
  edit_distance.set_tree2(tree2);
  

  edit_distance.save_path(true);
  if (!edit_distance.edit()) 
  {
    vcl_cerr << "Problems in editing trees"<<vcl_endl;
    return;
  }


  //float val = edit_distance.final_cost();

  vcl_cout << "\nFinal cost: " << edit_distance.final_cost() << "\n"
    << "Tree1 splice cost = " << tree1->total_splice_cost() << "\n"
    << "Tree2 splice cost = " << tree2->total_splice_cost() << "\n";


  int a  = 0;
  //if ( !norm_reconst )
  //{
  //  norm_val = val/(tree1->total_splice_cost()+
  //    tree2->total_splice_cost());
  //}
  //else
  //{
  //  norm_val = val/(tree1->total_reconstructed_boundary_length()+
  //    tree2->total_reconstructed_boundary_length());
  //}

  //
  //if ( !norm_reconst )
  //{
  //  vcl_cout << "final cost: " << val << " final norm cost: " 
  //    << norm_val << "( tree1 tot splice: " 
  //    << tree1->total_splice_cost() << ", tree2: " 
  //    << tree2->total_splice_cost() << ")" << vcl_endl;
  //}
  //else
  //{
  //  vcl_cout << "final cost: " << val << " final norm cost: " 
  //    << norm_val << "( tree1 tot length: " 
  //    << tree1->total_reconstructed_boundary_length() 
  //    << ", tree2: " 
  //    << tree2->total_reconstructed_boundary_length() << ")" 
  //    << vcl_endl;
  //}
  //
  //// this functions set curve_matching_R and other parameters of sm_cor 
  //// same as edit's --> so no need to reset
  //dbskr_sm_cor_sptr sm_cor = edit.get_correspondence();  
  //sm_cor->set_final_cost(val);
  //sm_cor->set_final_norm_cost(norm_val);

  //sm_cor->clear_map_list();

  //// fills in the cost vectors for debugging
  //sm_cor->recover_dart_ids_and_scurves();
  //edit.write_shgm(shgm_file);  
}




//: Test closest point functions
MAIN( test_matching_xshock_directed_tree )
{
  START ("Test Generic Tree Edit Distance");
  test_matching_xshock_direced_tree();
  SUMMARY();
}
