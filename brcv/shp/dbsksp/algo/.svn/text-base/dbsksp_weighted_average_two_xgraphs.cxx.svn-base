// This is shp/dbsksp/algo/dbsksp_weighted_average_two_xgraphs.cxx


//:
// \file
// 



#include "dbsksp_weighted_average_two_xgraphs.h"

#include <dbsksp/dbsksp_xshock_graph.h>
#include <dbsksp/algo/dbsksp_edit_distance.h>
#include <dbsksp/algo/dbsksp_xshock_directed_tree.h>
#include <dbsksp/algo/dbsksp_weighted_average_two_xgraphs_cost_function.h>
#include <dbsksp/algo/dbsksp_weighted_average_two_xgraphs_one_node_cost_function.h>
#include <dbsksp/algo/dbsksp_fit_xgraph.h>
#include <dbsksp/algo/dbsksp_xgraph_algos.h>
#include <dbsksp/algo/dbsksp_interp_xshock_fragment.h>
#include <dbsksp/xio/dbsksp_xio_xshock_graph.h>

#include <dbnl/dbnl_math.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vnl/algo/vnl_amoeba.h>
#include <vnl/algo/vnl_powell.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_distance.h>
#include <vcl_iostream.h>
#include <vul/vul_timer.h>
#include <vul/vul_sprintf.h>
#include <vul/vul_file.h>

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_cstdio.h>
#include <vcl_set.h>


//==============================================================================
// dbsksp_weighted_average_two_xgraphs
//==============================================================================


//------------------------------------------------------------------------------
//: Constructor
dbsksp_weighted_average_two_xgraphs::
dbsksp_weighted_average_two_xgraphs()
{
  this->parent_xgraph_[0] = 0;
  this->parent_xgraph_[1] = 0;

  // sampling params
  this->scurve_sample_ds_ = 1.0f;

  // weight param
  this->scurve_matching_R_ = 6.0f;

  // distance tolerance in xgraph fitting
  this->fit_xgraph_tol_ = 0.5;

  // Tolerance for relative error
  this->relative_error_tol_ = .01;
}



//------------------------------------------------------------------------------
//: Constructor
dbsksp_weighted_average_two_xgraphs::
dbsksp_weighted_average_two_xgraphs(const dbsksp_xshock_graph_sptr& xgraph1,
                           const dbsksp_xshock_graph_sptr& xgraph2,
                           double weight1,
                           double weight2,
                           float scurve_matching_R,
                           float scurve_sample_ds,
                           float fit_xgraph_tol,
                           double relative_error_tol,
                           const vcl_string& debug_base_name)
{
  this->set_parent_xgraph(0, xgraph1);
  this->set_parent_xgraph(1, xgraph2);
  this->set_scurve_matching_R(scurve_matching_R);
  this->set_scurve_sample_ds(scurve_sample_ds);
  this->fit_xgraph_tol_= fit_xgraph_tol;
  
  this->set_relative_error_tol(relative_error_tol);
  this->set_base_name(debug_base_name);


  this->parent_weight_[0] = weight1;
  this->parent_weight_[1] = weight2;
  return;
}


//------------------------------------------------------------------------------
//: Initialize intermediate and output variables
void dbsksp_weighted_average_two_xgraphs::
init()
{
  // Average xgraph
  this->average_xgraph_ = 0;

  // Distance the two original xgraph
  this->distance_to_parent_[0] = this->distance_to_parent_[1] = vnl_numeric_traits<double >::maxval;

  // relative error
  this->relative_error_ = vnl_numeric_traits<double >::maxval;

  this->distance_btw_parents_ = 0;
  //this->distance_parent_to_common_xgraph_[0] = 0;
  //this->distance_parent_to_common_xgraph_[1] = 0; 
  this->distance_to_parent_[0] = 0;
  this->distance_to_parent_[1] = 0;

  if (!vul_file::is_directory(vul_file::dirname(this->base_name_)))
  {
    this->base_name_ = "";
  }

  vcl_cout 
    << "\nBase name= " << this->base_name_
    << "\nWeight 1= " << this->parent_weight_[0]
    << "\nWeight 2= " << this->parent_weight_[1] << "\n";
  return;
}


////------------------------------------------------------------------------------
////: Get the common xgraph (edited from the parent xgraph to shared graph topology)
//dbsksp_xshock_graph_sptr dbsksp_weighted_average_two_xgraphs::
//get_common_xgraph(int index) const
//{
//  return this->common_tree_[index]->xgraph();
//}








//------------------------------------------------------------------------------
//: Compute distance between two xgraphs - keeping the intermediate work
double dbsksp_weighted_average_two_xgraphs::
compute_edit_distance(const dbsksp_xshock_graph_sptr& xgraph1, 
                      const dbsksp_xshock_graph_sptr& xgraph2,
                      dbsksp_edit_distance& work)
{
  dbsksp_edit_distance edit_distance;
  edit_distance.set(xgraph1, xgraph2, this->scurve_matching_R(), this->scurve_sample_ds());
  edit_distance.save_path(true);
  edit_distance.edit();
  work = edit_distance;
  return edit_distance.final_cost();
}


//------------------------------------------------------------------------------
//: Compute distance between two xgraphs - keeping the intermediate work
double dbsksp_weighted_average_two_xgraphs::
compute_edit_distance(const dbsksp_xshock_directed_tree_sptr& tree1,
                      const dbsksp_xshock_directed_tree_sptr& tree2,
                      dbsksp_edit_distance& work)
{
  dbsksp_edit_distance edit_distance;
  edit_distance.set_scurve_matching_R(this->scurve_matching_R());
  edit_distance.set_tree1(tree1);
  edit_distance.set_tree2(tree2);
  edit_distance.save_path(true);
  edit_distance.edit();
  work = edit_distance;
  return edit_distance.final_cost();
}





////------------------------------------------------------------------------------
////: Resampled two xgraphs with common COARSE topology so that their fine 
//// topologies are also the same
//bool dbsksp_weighted_average_two_xgraphs::
//resample_to_match_fine_graph_topology(const dbsksp_xshock_graph_sptr& coarse_matching_xgraph1,
//                                      const dbsksp_xshock_graph_sptr& coarse_matching_xgraph2,
//                                      dbsksp_xshock_graph_sptr& fine_matching_xgraph1,
//                                      dbsksp_xshock_graph_sptr& fine_matching_xgraph2)
//{
//  //0) sanitize output container
//  fine_matching_xgraph1 = 0;
//  fine_matching_xgraph2 = 0;
//
//  //1) Put the two input in array form
//  dbsksp_xshock_graph_sptr trimmed_xgraph[2];
//  trimmed_xgraph[0] = coarse_matching_xgraph1;
//  trimmed_xgraph[1] = coarse_matching_xgraph2;
//
//
//  //2) resample the two original xgraphs so that fragments in each branch are roughly equal size
//  // Directed trees corresponding to the resample xgraphs
//  dbsksp_xshock_directed_tree_sptr tree1_b = 0;
//  dbsksp_xshock_directed_tree_sptr tree2_b = 0;
//  vcl_vector<pathtable_key > correspondence; //> correspondence
// 
//  // heuristic parameter values
//  double distance_rms_error_threshold = 0.5;
//  double sample_ds = 1;
//  
//
//  // in some cases, after fitting, the optimal correspondence change, which requires further trimming
//  // We'll keep doing this until the final topology match
//
//  
//  bool need_more_trimming = true;
//  while (need_more_trimming)
//  {
//    dbsksp_fit_xgraph fit_xgraph(distance_rms_error_threshold);
//    dbsksp_xshock_graph_sptr common_xgraph[2];
//
//    vcl_map<dbsksp_xshock_node_sptr, dbsksp_xshock_node_sptr > dummy;
//    fit_xgraph.fit_to(trimmed_xgraph[0], common_xgraph[0], dummy, sample_ds);
//    fit_xgraph.fit_to(trimmed_xgraph[1], common_xgraph[1], dummy, sample_ds);
//
//    if (this->debugging())
//    {
//      x_write(this->base_name_ + "-trimmed1-fitted.xml", common_xgraph[0]);
//      x_write(this->base_name_ + "-trimmed2-fitted.xml", common_xgraph[1]);
//    }
//
//    dbsksp_edit_distance work;
//    double edit_cost = this->compute_edit_distance(common_xgraph[0], common_xgraph[1], work);
//    work.get_final_correspondence(correspondence);
//    tree1_b = work.tree1();
//    tree2_b = work.tree2();
//
//    // topology match when all the costs come from deformation cost
//    double deform_cost = work.get_deform_cost(correspondence);
//
//    if (vnl_math_abs(edit_cost - deform_cost) < 1e-3)
//    {
//      need_more_trimming = false;
//    }
//    else
//    {
//      trimmed_xgraph[0] = work.edit_xgraph1_to_common_topology();
//      trimmed_xgraph[1] = work.edit_xgraph2_to_common_topology();
//    }
//  }
//
//  //4) Subsample each branch of the shock graphs, if necessary, so that they share the sample topology
//  vcl_map<dbsksp_xshock_edge_sptr, int > map_num_nodes_to_add;
//
//  bool subsampling_success = true;
//  for (unsigned k =0; k < correspondence.size(); ++k)
//  {
//    pathtable_key key = correspondence[k];
//
//    int tree1_d1 = key.first.first;
//    int tree1_d2 = key.first.second;
//
//    int tree2_d1 = key.second.first;
//    int tree2_d2 = key.second.second;
//
//    // retrieve list of darts
//    vcl_vector<int > path1 = tree1_b->get_dart_path(tree1_d1, tree1_d2);
//    vcl_vector<int > path2 = tree2_b->get_dart_path(tree2_d1, tree2_d2);
//
//    // retrieve edges for shock branches
//    dbsksp_xshock_node_sptr start_node1;
//    vcl_vector<dbsksp_xshock_edge_sptr > branch1;
//    tree1_b->get_edge_list(path1, start_node1, branch1);
//
//    dbsksp_xshock_node_sptr start_node2;
//    vcl_vector<dbsksp_xshock_edge_sptr > branch2;
//    tree2_b->get_edge_list(path2, start_node2, branch2);
//
//    // remove terminal edges at the begining and at the end of the branches
//    // branch 1
//    {
//      if (branch1.back()->is_terminal_edge())
//      {
//        branch1.pop_back();
//      }
//
//      if (branch1.front()->is_terminal_edge())
//      {
//        start_node1 = branch1.front()->opposite(start_node1);
//        vcl_vector<dbsksp_xshock_edge_sptr > temp = branch1;
//        branch1.clear();
//        for (unsigned i =1; i < temp.size(); ++i)
//        {
//          branch1.push_back(temp[i]);
//        }
//      }
//    }
//
//
//    // branch 2
//    {
//      if (branch2.back()->is_terminal_edge())
//      {
//        branch2.pop_back();
//      }
//
//      if (branch2.front()->is_terminal_edge())
//      {
//        start_node2 = branch2.front()->opposite(start_node2);
//        vcl_vector<dbsksp_xshock_edge_sptr > temp = branch2;
//        branch2.clear();
//        for (unsigned i =1; i < temp.size(); ++i)
//        {
//          branch2.push_back(temp[i]);
//        }
//      }
//    }
//
//
//
//    // each path should have 2^n edges
//    // verify this
//    double log2_size1 = vcl_log(double(branch1.size())) / vnl_math::ln2;
//    bool branch1_is_power_2 = (log2_size1 - vnl_math_floor(log2_size1)) < 1e-12;
//
//    double log2_size2 = vcl_log(double(branch2.size())) / vnl_math::ln2;
//    bool branch2_is_power_2 = (log2_size2 - vnl_math_floor(log2_size2)) < 1e-12;
//
//    if (!branch1_is_power_2 || !branch2_is_power_2)
//    {
//      vcl_cout << "\nERROR: Number of edges in one branch is not a power of 2.\n";
//      continue;
//    }
//
//    // Log of number of edges of each branch
//    int exp_branch1 = vnl_math_floor(log2_size1);
//    int exp_branch2 = vnl_math_floor(log2_size2);
//    if (exp_branch1 < exp_branch2)
//    {
//      // Need to add node to branch 1
//      for (unsigned i =0; i < branch1.size(); ++i)
//      {
//        dbsksp_xshock_edge_sptr xe = branch1[i];
//        map_num_nodes_to_add[xe] = dbnl_math_pow(2, exp_branch2 - exp_branch1) - 1;
//      }
//    }
//    else if (exp_branch2 < exp_branch1)
//    {
//      // need to add node to branch 2
//      for (unsigned i =0; i < branch2.size(); ++i)
//      {
//        dbsksp_xshock_edge_sptr xe = branch2[i];
//        map_num_nodes_to_add[xe] = dbnl_math_pow(2, exp_branch1 - exp_branch2) - 1;
//      }
//    }
//  } // for each pair of corresponding branches.
//
//
//  // adding nodes to the marked edges
//  for (vcl_map<dbsksp_xshock_edge_sptr, int >::iterator iter = map_num_nodes_to_add.begin();
//    iter != map_num_nodes_to_add.end(); ++iter)
//  {
//    dbsksp_xshock_edge_sptr xe = iter->first;
//    int num_nodes_to_add = iter->second;
//
//    vcl_vector<dbsksp_xshock_node_descriptor > list_xsample;
//
//    // new method
//    dbsksp_xshock_node_descriptor start = *(xe->source()->descriptor(xe));
//    dbsksp_xshock_node_descriptor end   = xe->target()->descriptor(xe)->opposite_xnode();
//    dbsksp_xshock_fragment xfrag(start, end);
//    int power_n = vnl_math_rnd(vcl_log(double(num_nodes_to_add+1)) / vnl_math::ln2);
//    dbsksp_divide_xfrag_into_2_power_n_fragments(xfrag, power_n, list_xsample);
//
//    
//    // \todo need a better way to compute samples on the xfragment as in the following function
//    // the number of intervals is only indicative. The returned number of samples is
//    // not guaranteed to be the same as requested.
//    ////
//    //dbsksp_xgraph_algos::compute_xsamples(num_nodes_to_add+1, 
//    //  xe->source(), 
//    //  vcl_vector<dbsksp_xshock_edge_sptr>(1, xe), list_xsample);
//
//    assert(num_nodes_to_add + 2 == list_xsample.size());
//
//    // The first and last samples coincide with the two end nodes. Don't use them
//    list_xsample.pop_back();
//    dbsksp_xshock_node_sptr xe_source = xe->source();
//
//    // determine which shock graph this edge belongs to
//    dbsksp_xshock_graph_sptr active_xgraph = 
//      (tree1_b->xgraph()->edge_from_id(xe->id()) == xe) ? 
//      tree1_b->xgraph() : tree2_b->xgraph();
//
//    while (list_xsample.size() > 1)
//    {
//      dbsksp_xshock_node_descriptor xdesc = list_xsample.back();
//      list_xsample.pop_back();
//
//      // place holders for the two new edges when the new node is inserted
//      dbsksp_xshock_edge_sptr xe_from_start_to_new_xv = 0;
//      dbsksp_xshock_edge_sptr xe_from_new_xv_to_end = 0;
//
//      active_xgraph->insert_xshock_node(xe, xe_source, xdesc, 
//        xe_from_start_to_new_xv,
//        xe_from_new_xv_to_end);
//
//      xe = xe_from_start_to_new_xv;
//    } // while list_xsample.size > 1 
//  } // for num_nodes_to_add
//
//  // save results
//  fine_matching_xgraph1 = tree1_b->xgraph(); //common_xgraph[0];
//  fine_matching_xgraph2 = tree2_b->xgraph(); //common_xgraph[1];
//
//  return true;
//}



//------------------------------------------------------------------------------
//: Update distance values between model and common_xgraph (topology-matching xgraph)
bool dbsksp_weighted_average_two_xgraphs::
update_relative_error()
{
  this->model_tree_->clear_dart_path_scurve_map();

  // distance to parent xgraphs
  double err[2];
  for (int i =0; i < 2; ++i)
  {
    double deform_cost = this->compute_deform_cost(this->trimmed_tree_[i], 
                                                   this->model_tree_, 
                                                   this->corr_trimmed_to_model_[i]);

    this->distance_trimmed_to_model_[i] = deform_cost + 
                                          this->contract_cost_trimmed_to_model_[i];

    this->distance_to_parent_[i] = this->distance_parent_to_trimmed_xgraph_[i] + 
                                   this->distance_trimmed_to_model_[i];

    err[i] = vnl_math_abs(this->distance_trimmed_to_model_[i] - this->target_distance_trimmed_to_model_[i]);
  }

  this->relative_error_ = (err[0] + err[1]) / this->distance_btw_parents_;

  vcl_cout 
    << "\n  d1    = " << this->distance_to_parent_[0] << " w1 = " << this->parent_weight_[0]
    << "\n  d2    = " << this->distance_to_parent_[1] << " w2 = " << this->parent_weight_[1]
    << "\n  d     = " << this->distance_btw_parents_
    << "\n  error = " << this->relative_error_ << "\n\n";

  return true;
}


//------------------------------------------------------------------------------
//: Execute the averaging function
bool dbsksp_weighted_average_two_xgraphs::
compute()
{
  //0) Initialize internal variables
  vcl_cout << "\n[Begin - Initialize internal variables]\n";
  this->init();
  vcl_cout << "\n[End   - Initialize internal variables]\n";

  // save the two original xgraphs
  if (this->debugging())
  {
    x_write(this->base_name_ + "-parent1.xml", this->parent_xgraph(0));
    x_write(this->base_name_ + "-parent2.xml", this->parent_xgraph(1));
  }

  //1) Determine trimmed xgraphs -- splice the "leaf branches" from the original graphs
  vcl_cout << "\n[Begin - Construct trimmed xgraphs]\n";
  bool success = this->construct_trimmed_xgraphs();
  vcl_cout << "\n[End   - Construct trimmed xgraphs]\n";

  if (!success) return false;

  // print intermediate results
  if (this->debugging())
  {
    x_write(this->base_name_ + "-trimmed1.xml", this->trimmed_tree_[0]->xgraph());
    x_write(this->base_name_ + "-trimmed2.xml", this->trimmed_tree_[1]->xgraph());
  }
  this->print_debug_info(vul_sprintf("distance_btw_parents %f\n", this->distance_btw_parents_));


  //2) Construct a model xgraph and find their correspondence to the trimmed xgraphs
  vcl_cout << "\n[Begin - Construct model xgraphs from trimmed xgraphs]\n";
  success = this->construct_model_xgraph_from_trimmed_xgraphs();
  vcl_cout << "\n[End   - Construct model xgraphs from trimmed xgraphs]\n";
  if (!success) return false;
  
  // print the initial model xgraph
  if (this->debugging())
  {
    x_write(this->base_name_ + "-model-init.xml", this->model_tree_->xgraph());
  }

  //3) Optimize model xgraph to minimize cost
  vcl_cout << "\n[Begin - Optimize model xgraph]\n";
  this->optimize_model_xgraph();
  vcl_cout << "\n[End - Optimize model xgraph]\n";

  return true;
}


//------------------------------------------------------------------------------
//: Determine the trimmed xgraphs - edit the original shapes using splice operations
bool dbsksp_weighted_average_two_xgraphs::
construct_trimmed_xgraphs()
{
  dbsksp_edit_distance work;
  vcl_vector<pathtable_key > work_corr;
  double d_parents = this->compute_edit_distance(this->parent_xgraph(0), this->parent_xgraph(1), work); 
  work.get_final_correspondence(work_corr);

  // Print out
  vcl_cout 
    << "\nDirect distance parent1 --> parent2 = " << d_parents
    << "\n  Deformation cost                  = " << work.get_deform_cost(work_corr)
    << "\n  Total splice cost of parent1      = " << work.tree1()->total_splice_cost()
    << "\n  Total splice cost of parent2      = " << work.tree2()->total_splice_cost() << "\n";

  //2) Edit the parent xgraphs to the common graph topology
  dbsksp_xshock_graph_sptr trimmed_xgraph[2];
  trimmed_xgraph[0] = work.edit_xgraph1_to_common_topology();
  trimmed_xgraph[1] = work.edit_xgraph2_to_common_topology();

  // safety check
  if (!trimmed_xgraph[0] || !trimmed_xgraph[1])
  {
    vcl_cout << "\nERROR: either trimmed_xgraph[0] or trimmed_xgraph[1] was not computed.\n";
    return false;
  }

  //// Resample the trimmed xgraph to minimize number of fragments used
  //for (unsigned i =0; i < 2; ++i)
  //{
  //  dbsksp_xshock_graph_sptr temp = 0;
  //  dbsksp_fit_xgraph fit_xgraph(this->fit_xgraph_tol_);
  //  vcl_map<dbsksp_xshock_node_sptr, dbsksp_xshock_node_sptr > dummy;
  //  fit_xgraph.fit_to(trimmed_xgraph[i], temp, dummy);
  //  assert(temp);
  //  trimmed_xgraph[i] = temp;
  //}

  // Compute distance between parent xgraph and trimmed
  for (unsigned i =0; i < 2; ++i)
  {
    this->distance_parent_to_trimmed_xgraph_[i] = 
      this->compute_edit_distance(this->parent_xgraph(i), trimmed_xgraph[i], work);
    this->trimmed_tree_[i] = work.tree2();
    work.get_final_correspondence(work_corr);

    // Print out cost
    vcl_cout 
      << vul_sprintf("\nDistance parent%i --> trimmed_xgraph%i= %f", i+1, i+1, this->distance_parent_to_trimmed_xgraph_[i])
      << "\n  Deformation cost           = " << work.get_deform_cost(work_corr) << "\n";
  }

  // Distance between two trimmed xgraphs
  {
    this->distance_btw_trimmed_xgraphs_ = 
      this->compute_edit_distance(trimmed_xgraph[0], trimmed_xgraph[1], work); 
    work.get_final_correspondence(work_corr);

    // Print out
    vcl_cout 
      << "\nDistance trimmed1 --> trimmed2 = " << this->distance_btw_trimmed_xgraphs_
      << "\n  Deformation cost           = " << work.get_deform_cost(work_corr)
      << "\n  Total splice cost of trimmed1 = " << work.tree1()->total_splice_cost()
      << "\n  Total splice cost of trimmed2 = " << work.tree2()->total_splice_cost() << "\n";  
  }

  this->distance_btw_parents_ = this->distance_parent_to_trimmed_xgraph_[0] +
                                this->distance_parent_to_trimmed_xgraph_[1] +  
                                this->distance_btw_trimmed_xgraphs_;
  return true;
}


//------------------------------------------------------------------------------
//: Compute deformation cost of between two trees
double dbsksp_weighted_average_two_xgraphs::
compute_deform_cost(const dbsksp_xshock_directed_tree_sptr& tree1,
                    const dbsksp_xshock_directed_tree_sptr& tree2,
                    const vcl_vector<pathtable_key >& correspondence)
{
  dbsksp_edit_distance e1;
  e1.set_scurve_matching_R(this->scurve_matching_R());
  e1.set_tree1(tree1);
  e1.set_tree2(tree2);
  return e1.get_deform_cost(correspondence);
}


////------------------------------------------------------------------------------
////: Compute vertex- and edge-correspondence given their dart correspondence
//  // Assumption: the two trees have EXACT same topology
//bool dbsksp_weighted_average_two_xgraphs::
//compute_vertex_and_edge_correspondence(const dbsksp_xshock_directed_tree_sptr& tree1,
//    const dbsksp_xshock_directed_tree_sptr& tree2,
//    const vcl_vector<pathtable_key >& dart_correspondence_tree1_to_tree2,
//    vcl_map<dbsksp_xshock_node_sptr, dbsksp_xshock_node_sptr >& node_map_xgraph1_to_xgraph2,
//    vcl_map<dbsksp_xshock_node_sptr, dbsksp_xshock_node_sptr >& node_map_xgraph2_to_xgraph1,
//    vcl_map<dbsksp_xshock_edge_sptr, dbsksp_xshock_edge_sptr >& edge_map_xgraph1_to_xgraph2,
//    vcl_map<dbsksp_xshock_edge_sptr, dbsksp_xshock_edge_sptr >& edge_map_xgraph2_to_xgraph1)
//{
//  // sanitize output storage
//  node_map_xgraph1_to_xgraph2.clear();
//  node_map_xgraph2_to_xgraph1.clear();
//  edge_map_xgraph1_to_xgraph2.clear();
//  edge_map_xgraph2_to_xgraph1.clear();
//
//
//  // iterate thru the correspondence dart list and extract correspondence
//  // for node and edges of the two shock graphs
//  for (unsigned k =0; k < dart_correspondence_tree1_to_tree2.size(); ++k)
//  {
//    pathtable_key key = dart_correspondence_tree1_to_tree2[k];
//
//    int tree1_d1 = key.first.first;
//    int tree1_d2 = key.first.second;
//
//    int tree2_d1 = key.second.first;
//    int tree2_d2 = key.second.second;
//
//    // retrieve list of nodes and edges from the darts
//    vcl_vector<dbsksp_xshock_edge_sptr > edge_list1, edge_list2;
//    dbsksp_xshock_node_sptr start_node1, start_node2;
//
//    tree1->get_edge_list(tree1->get_dart_path(tree1_d1, tree1_d2), start_node1, edge_list1);
//    tree2->get_edge_list(tree2->get_dart_path(tree2_d1, tree2_d2), start_node2, edge_list2);
//
//    // establish correspondence
//    if (edge_list1.size() != edge_list2.size())
//      return false;
//
//    dbsksp_xshock_node_sptr cur_xv1 = start_node1;
//    dbsksp_xshock_node_sptr cur_xv2 = start_node2;
//
//    // correspondence for the first node
//    node_map_xgraph1_to_xgraph2.insert(vcl_make_pair(cur_xv1, cur_xv2));
//    node_map_xgraph2_to_xgraph1.insert(vcl_make_pair(cur_xv2, cur_xv1));
//
//    for (unsigned i =0; i < edge_list1.size(); ++i)
//    {
//      dbsksp_xshock_edge_sptr xe1 = edge_list1[i];
//      dbsksp_xshock_edge_sptr xe2 = edge_list2[i];
//
//      edge_map_xgraph1_to_xgraph2.insert(vcl_make_pair(xe1, xe2));
//      edge_map_xgraph2_to_xgraph1.insert(vcl_make_pair(xe2, xe1));
//
//      cur_xv1 = xe1->opposite(cur_xv1);
//      cur_xv2 = xe2->opposite(cur_xv2);
//
//      node_map_xgraph1_to_xgraph2.insert(vcl_make_pair(cur_xv1, cur_xv2));
//      node_map_xgraph2_to_xgraph1.insert(vcl_make_pair(cur_xv2, cur_xv1));
//    }
//
//  }
//
//  return true;
//
//}




//------------------------------------------------------------------------------
//: Compute root node and edge for a given xgraph tree
bool dbsksp_weighted_average_two_xgraphs::
compute_xgraph_root_node(const dbsksp_xshock_directed_tree_sptr& tree,
                         dbsksp_xshock_node_sptr& root_node, dbsksp_xshock_edge_sptr& pseudo_parent_edge)
{
  dbsksp_xshock_directed_tree_sptr common_tree1 = tree;

  // use tree1 as the basis for choosing root node
  int centroid = common_tree1->centroid();

  // find the dart with maximum costs
  vcl_vector<int > out_darts = common_tree1->out_darts(centroid);
  float max_subtree_cost = -1.0f;
  int max_dart_id = 0;
  for (unsigned i =0; i < out_darts.size(); ++i)
  {
    float subtree_cost = common_tree1->subtree_delete_cost(out_darts[i]);
    if ( subtree_cost > max_subtree_cost)
    {
      max_subtree_cost = subtree_cost;
      max_dart_id = out_darts[i];
    }
  }

  vcl_vector<dbsksp_xshock_edge_sptr > edge_path;
  dbsksp_xshock_node_sptr start_node = 0;
  common_tree1->get_edge_list(vcl_vector<int >(1, max_dart_id), start_node, edge_path);

  // save results
  root_node = start_node;
  pseudo_parent_edge = edge_path.front();

  return true;
}


//------------------------------------------------------------------------------
//: Compute coarse Euler tour for an xgraph (ignore degree-2 nodes)
bool dbsksp_weighted_average_two_xgraphs::
compute_coarse_euler_tour(const dbsksp_xshock_graph_sptr& model_xgraph, 
    vcl_vector<dbsksp_xshock_node_sptr >& coarse_euler_tour) const
{
  // sanitize output storage
  coarse_euler_tour.clear();

  // Begin with a degree-1 node
  dbsksp_xshock_node_sptr start_xv = *(model_xgraph->vertices_begin());
  dbsksp_xshock_edge_sptr start_xe = *(start_xv->edges_begin());
  while (start_xv->degree() != 1)
  {
    start_xv = start_xe->opposite(start_xv);
    start_xe = model_xgraph->cyclic_adj_succ(start_xe, start_xv);
  }

  // Keep traversing along the Euler tour, keeping only degree-1 and -3 nodes
  dbsksp_xshock_node_sptr cur_xv = start_xv;
  dbsksp_xshock_edge_sptr cur_xe = start_xe;
  do
  {
    // save the node
    if (cur_xv->degree() != 2)
    {
      coarse_euler_tour.push_back(cur_xv);
    }

    cur_xv = cur_xe->opposite(cur_xv);
    cur_xe = model_xgraph->cyclic_adj_succ(cur_xe, cur_xv);
  }
  while (cur_xv != start_xv || cur_xe != start_xe);

  return true;
}








//------------------------------------------------------------------------------
//: Print debug info to a file
void dbsksp_weighted_average_two_xgraphs::
print_debug_info(const vcl_string& str) const
{
  if (this->debugging())
  {
    vcl_string fname = this->base_name_ + "-debug.txt";
    vcl_ofstream ofs(fname.c_str(), vcl_ofstream::app);
    ofs << str;
    ofs.close();
  }
  return;
}






//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//--------------------------------------------------------------------------------
//: Construct a model xgraph and find their correspondence to the trimmed xgraphs
bool dbsksp_weighted_average_two_xgraphs::
construct_model_xgraph_from_trimmed_xgraphs()
{
  //1) Construct an initial version of model xgraph
  
  // Model xgraph will have the same coarse topology as trimmed_xgraph1
  // but will be resampled to so that it can represent both parent graphs

  dbsksp_xshock_graph_sptr model_xgraph = new dbsksp_xshock_graph(*(this->trimmed_tree_[0]->xgraph()));

  // Ressample model_xgraph to minimize number of fragments
  {
    dbsksp_xshock_graph_sptr temp = 0;
    dbsksp_fit_xgraph fit_xgraph(this->fit_xgraph_tol_);
    vcl_map<dbsksp_xshock_node_sptr, dbsksp_xshock_node_sptr > dummy;
    fit_xgraph.fit_to(model_xgraph, temp, dummy);
    assert(temp);
    model_xgraph = temp;
  }


  //2) Establish correspondence with the trimmed xgraph
  dbsksp_xshock_directed_tree_sptr model_tree = 
    dbsksp_edit_distance::new_tree(model_xgraph, this->scurve_matching_R(), this->scurve_sample_ds());
  
  // correspondence
  vcl_vector<pathtable_key > corr_trimmed_to_model[2];
  
  // contract cost
  double                     contract_cost_trimmed_to_model[2];

  for (unsigned i =0; i < 2; ++i)
  {
    dbsksp_edit_distance work;
    double edit_cost = this->compute_edit_distance(this->trimmed_tree_[i], model_tree, work);
    work.get_final_correspondence(corr_trimmed_to_model[i]);
    double deform_cost = work.get_deform_cost(corr_trimmed_to_model[i]);
    contract_cost_trimmed_to_model[i] = edit_cost - deform_cost;

    vcl_cout 
      << "\nDistance trimmed"<< i+1 << " --> model = " << edit_cost
      << "\n  Deformation cost           = " << deform_cost
      << "\n  Contract + splice cost     = " << contract_cost_trimmed_to_model[i] << "\n";
  }

  //3) Verify that all edges of model xgraph are involved in the two correpondence maps
  vcl_set<dbsksp_xshock_edge_sptr > active_edges;
  for (unsigned idx =0; idx < 2; ++idx)
  {
    for (unsigned i =0; i < corr_trimmed_to_model[idx].size(); ++i)
    {
      pathtable_key key = corr_trimmed_to_model[idx][i];
      int model_d1 = key.second.first;
      int model_d2 = key.second.second;

      // retrieve list of darts
      vcl_vector<int > model_path = model_tree->get_dart_path(model_d1, model_d2);

      // retrieve list of edges
      dbsksp_xshock_node_sptr start_node;
      vcl_vector<dbsksp_xshock_edge_sptr > edges;
      model_tree->get_edge_list(model_path, start_node, edges);

      // save list of edges
      active_edges.insert(edges.begin(), edges.end());
    }
  }

  // Check that all edges are active
  vcl_vector<dbsksp_xshock_edge_sptr > inactive_edges;
  for (dbsksp_xshock_graph::edge_iterator eit = model_xgraph->edges_begin(); 
    eit != model_xgraph->edges_end(); ++eit)
  {
    dbsksp_xshock_edge_sptr xe = *eit;
    if (active_edges.find(xe) == active_edges.end())
    {
      inactive_edges.push_back(xe);
    }
  }

  if (!inactive_edges.empty())
  {
    return false;
  }

  this->model_tree_ = model_tree;
  this->corr_trimmed_to_model_[0] = corr_trimmed_to_model[0];
  this->corr_trimmed_to_model_[1] = corr_trimmed_to_model[1];
  this->contract_cost_trimmed_to_model_[0] = contract_cost_trimmed_to_model[0];
  this->contract_cost_trimmed_to_model_[1] = contract_cost_trimmed_to_model[1];

  return true;
}




//------------------------------------------------------------------------------
// Optimize model xgraph to minimize cost
bool dbsksp_weighted_average_two_xgraphs::
optimize_model_xgraph()
{
  //1) Preliminary checks
  if (!this->model_tree_ || !this->trimmed_tree_[0] || !this->trimmed_tree_[1])
    return false;

  dbsksp_xshock_graph_sptr mg = this->model_tree_->xgraph();

  vcl_cout 
    << "\n Model xgraph: #vertices = " << mg->number_of_vertices()
    << "\n               #edges    = " << mg->number_of_edges() << "\n";

  this->print_debug_info(
    vul_sprintf("num_vertices_in_model_xgraph %d\n", mg->number_of_vertices()));

  //4) Choose a root node for the model xgraph
  vcl_cout << "\nChoosing a root node ...";
  {
    dbsksp_xshock_node_sptr model_root_node;
    dbsksp_xshock_edge_sptr model_pseudo_parent_edge;
    this->compute_xgraph_root_node(this->model_tree_, model_root_node, model_pseudo_parent_edge);

    this->model_root_vid_ = model_root_node->id();
    this->model_pseudo_parent_eid_ = model_pseudo_parent_edge->id();
  }
  vcl_cout << "done.\n";

  this->print_debug_info(vul_sprintf("model_root_vid %d\n", this->model_root_vid_) + 
    vul_sprintf("model_pseudo_parent_eid %d\n", this->model_pseudo_parent_eid_));


  //5) Compute depths for all trees
  mg->compute_vertex_depths(this->model_root_vid_);

  //6) Compute target distance from model to the trimmed xgraph
  double d  = this->distance_btw_parents_;
  double w1 = this->parent_weight_[0];
  double w2 = this->parent_weight_[1];
  double d1 = this->distance_parent_to_trimmed_xgraph_[0];
  double d2 = this->distance_parent_to_trimmed_xgraph_[1];

  this->target_distance_trimmed_to_model_[0] = d * w2/(w1+w2) - d1;
  this->target_distance_trimmed_to_model_[1] = d * w1/(w1+w2) - d2;

  vcl_cout << "Target distance trimmed1 --> model = " << this->target_distance_trimmed_to_model_[0] << "\n"
    << "Target distance trimmed2 --> model = " << this->target_distance_trimmed_to_model_[1] << "\n";

  this->print_debug_info(
    vul_sprintf("target_distance1 %f\n", this->target_distance_trimmed_to_model_[0]) + 
    vul_sprintf("target_distance2 %f\n", this->target_distance_trimmed_to_model_[0]));

  
  //7) Optimize model tree so that distance to trimmed xgraphs match with 
  // computed target distance

  //a) Form Euler tour of nodes to visit
  vcl_vector<dbsksp_xshock_node_sptr > nodes_on_euler_tour;
  this->compute_coarse_euler_tour(mg, nodes_on_euler_tour);

  //b) Shape model to modify the model_xgraph
  dbsksp_xgraph_model_using_L_alpha_phi_radius xgraph_model(mg, 
    this->model_root_vid_, this->model_pseudo_parent_eid_);
  
  //c) Determine initial state of optimization
  
  // update relative error
  this->update_relative_error();

  vul_timer timer;
  timer.mark();

  double f_tol = 0.00001 * this->distance_btw_parents_;


  // print debug info
  this->update_relative_error();
  this->print_debug_info(
    vul_sprintf("relative_error_tol %f\n", this->relative_error_tol_) + 
    vul_sprintf("f_tol              %f\n", f_tol));

  //b) Move along the Euler tour and optimize
  vcl_vector<double > trace_relative_error;

  //
  this->print_debug_info("begin_trace_relative_error [time(ms) relative_error]\n");
  this->print_debug_info(vul_sprintf("%f %g\n", 0, this->relative_error_));

  dbsksp_xshock_node_sptr prev_xv = 0;
  double min_relative_error = vnl_numeric_traits<double >::maxval;
  vnl_vector<double > argmin_relative_error;
  for (unsigned kk = 0; kk < 5 && this->relative_error() > this->relative_error_tol_; ++kk)
  {
    vcl_cout << "\nEuler tour round = " << kk << "\n";
    for (unsigned k =0; k < nodes_on_euler_tour.size() && this->relative_error() > this->relative_error_tol_; ++k)
    {
      vul_timer timer1;
      timer1.mark();

      // active node
      dbsksp_xshock_node_sptr xv = nodes_on_euler_tour[k];

      //// skip degree-1 node to save time
      //if (xv->degree() == 1)
      //  continue;

      // skip repeated nodes
      if (xv == prev_xv)
        continue;

      // update prev_xv
      prev_xv = xv;

      //i) construct cost function
      vcl_cout << "\nnode id= " << xv->id() << "\n";
      xv->print(vcl_cout);

      dbsksp_weighted_average_two_xgraphs_one_node_cost_function local_deform_cost(
        this->trimmed_tree_[0],
        this->trimmed_tree_[1], 
        this->model_tree_,
        this->corr_trimmed_to_model_[0],
        this->corr_trimmed_to_model_[1], 
        this->scurve_matching_R(),
        &xgraph_model,
        xv->id(),
        this->target_distance_trimmed_to_model_[0], 
        this->target_distance_trimmed_to_model_[1], 
        this->contract_cost_trimmed_to_model_[0],
        this->contract_cost_trimmed_to_model_[1]);

      // Optimize

      // Initial value for the state vector
      vnl_vector<double > x;
      local_deform_cost.last_x(x);

      // Use Powell to minimize cost
      double f0 = local_deform_cost.f(x);
      vcl_cout << "Init cost = " << f0 << "\n";


      // Powell

      vnl_powell powell(&local_deform_cost);
      // powell.set_verbose(true);
      powell.set_f_tolerance(f_tol);
      powell.minimize(x);
      
      double final_cost = local_deform_cost.f(x);
      vcl_cout << "Final cost = " << final_cost << "\n";

      // True distance to two original shape
      this->update_relative_error();

      // We want to keep lowest relative error
      if (this->relative_error_ < min_relative_error)
      {
        min_relative_error    = this->relative_error_;
        xgraph_model.get_xgraph_state(argmin_relative_error);
      }
      // keep status-quo if the new error is too big
      else if (this->relative_error_ > 3* min_relative_error) // \todo 3 is an arbitrary choice.
      {
        xgraph_model.set_xgraph_state(argmin_relative_error);
        this->update_relative_error();
      }

      trace_relative_error.push_back(this->relative_error_);

      // print debug info
      this->print_debug_info(vul_sprintf("%f %f\n", float(timer.real())/1000, this->relative_error_));

      vcl_cout << "Time: " << timer1.real() / 1000 << " secs\n";
    }
  }

  this->print_debug_info("end_trace_relative_error\n");

  // retrieve the xgraph state with minimum relative error
  xgraph_model.set_xgraph_state(argmin_relative_error);
  this->average_xgraph_ = new dbsksp_xshock_graph(* (xgraph_model.xgraph()));


  if (this->debugging())
  {
    x_write(this->base_name_ + "-model-last.xml", this->average_xgraph());
  }

  vcl_cout << "\nTimer - real time: " << timer.real() / 1000 << " seconds\n";

  this->update_relative_error();
  for (int i =0; i < 2; ++i)
  {
    this->print_debug_info(vul_sprintf("distance_average_to_parent%d %f\n", i+1, this->distance_to_parent(i)));
  }
  return true;
}


