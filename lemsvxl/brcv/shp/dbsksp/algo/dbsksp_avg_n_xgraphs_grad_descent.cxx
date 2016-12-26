// This is shp/dbsksp/algo/dbsksp_avg_n_xgraphs_grad_descent.cxx


//:
// \file
// 



#include "dbsksp_avg_n_xgraphs_grad_descent.h"
#include <dbsksp/xio/dbsksp_xio_xshock_graph.h>
//#include <dbsksp/algo/dbsksp_weighted_average_two_xgraphs.h>
//#include <dbsksp/algo/dbsksp_average_two_xgraphs.h>

#include <dbsksp/algo/dbsksp_avg_xgraph_utils.h>
#include <dbsksp/algo/dbsksp_weighted_avg_n_xgraphs_cost_function.h>
#include <dbsksp/algo/dbsksp_xshock_directed_tree.h>
#include <dbsksp/dbsksp_xshock_graph.h>

#include <vnl/algo/vnl_powell.h>
#include <vul/vul_sprintf.h>
#include <vul/vul_file.h>
//#include <vcl_iostream.h>
#include <vcl_fstream.h>
//#include <vcl_cstdio.h>
#include <vnl/vnl_math.h>
//#include <vnl/vnl_numeric_traits.h>
#include <vul/vul_timer.h>


//==============================================================================
// dbsksp_avg_n_xgraphs_grad_descent
//==============================================================================


//------------------------------------------------------------------------------
//: Constructor
dbsksp_avg_n_xgraphs_grad_descent::
dbsksp_avg_n_xgraphs_grad_descent()
{
  this->parent_xgraphs_.clear();
  this->init_model_xgraph_ = 0;

  // edit distance parameters
  this->scurve_sample_ds_ = 3.0f;
  this->scurve_matching_R_ = 6.0f;  
  this->distance_ratio_tol_ = 0.001f;
}


//------------------------------------------------------------------------------
//: Initialize distances
void dbsksp_avg_n_xgraphs_grad_descent::
init()
{
  if (!vul_file::is_directory(vul_file::dirname(this->base_name_)))
  {
    this->base_name_ = "";
  }

  //: Average xgraph
  this->average_xgraph_ =0;
  this->distance_parent_to_avg_.clear();
  this->deform_cost_parent_to_avg_.clear();
}



//------------------------------------------------------------------------------
//: Compute distance between two xgraphs - keeping the intermediate work
double dbsksp_avg_n_xgraphs_grad_descent::
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
double dbsksp_avg_n_xgraphs_grad_descent::
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



//------------------------------------------------------------------------------
//: Update average distance from parent xgraphs to model xgraph
void dbsksp_avg_n_xgraphs_grad_descent::
update_avg_distance()
{
  double sum = 0;
  this->model_tree_->clear_dart_path_scurve_map();
  this->deform_cost_parent_to_avg_.resize(this->num_parents_);
  this->distance_parent_to_avg_.resize(this->num_parents_);
  for (int i =0; i < this->num_parents_; ++i)
  {
    // compute deform cost
    dbsksp_edit_distance edit;
    edit.set_scurve_matching_R(this->scurve_matching_R());
    edit.set_tree1(this->parent_trees_[i]);
    edit.set_tree2(this->model_tree_);
    double deform_cost = edit.get_deform_cost(this->dart_corr_parent_to_model_[i]);
    double distance = deform_cost + this->parent_contract_and_splice_cost_[i];
    sum += this->parent_weights_[i] * vnl_math::sqr(distance);

    this->deform_cost_parent_to_avg_.push_back(deform_cost);
    this->distance_parent_to_avg_[i] = distance;
  }
  this->cur_avg_distance_ = vcl_sqrt(sum/this->num_parents_);
}


//------------------------------------------------------------------------------
//: Execute the averaging function
bool dbsksp_avg_n_xgraphs_grad_descent::
compute()
{
  vcl_cout 
    << "\nCompute average of N xgraphs using gradient descent approach"
    << "\n  base name for saving intermediate files= " << this->base_name_ << "\n";

  // save the original xgraphs
  if (this->debugging())
  {
    for (unsigned i =0; i < this->parent_xgraphs_.size(); ++i)
    {
      vcl_string fname = this->base_name_ + vul_sprintf("-parent%d.xml", i+1); 
      x_write(fname, this->parent_xgraphs_[i]);
    }
  }


  if (this->parent_xgraphs_.empty())
  {
    vcl_cout << "\nERROR: No parent xgraphs....\n";
    return false;
  }

  //1) Construct a directed tree for the parent and model xgraphs

  // parents
  this->parent_trees_.resize(num_parents_);
  for (int i =0; i < this->num_parents_; ++i)
  {
    this->parent_trees_[i] = dbsksp_edit_distance::new_tree(this->parent_xgraphs_[i], 
      this->scurve_matching_R(), this->scurve_sample_ds());
  }
  // model
  this->model_xgraph_ = new dbsksp_xshock_graph(*(this->init_model_xgraph_));
  this->model_tree_ = dbsksp_edit_distance::new_tree(this->model_xgraph_, 
    this->scurve_matching_R(), this->scurve_sample_ds());

  vcl_cout 
    << "\n Model xgraph: #vertices = " << this->model_xgraph_->number_of_vertices()
    << "\n               #edges    = " << this->model_xgraph_->number_of_edges() << "\n";

  this->print_debug_info(
    vul_sprintf("num_vertices_in_model_xgraph %d\n", this->model_xgraph_->number_of_vertices()));

  //4) Choose a root node for the model xgraph
  vcl_cout << "\nChoosing a root node ...";
  {
    dbsksp_xshock_node_sptr model_root_node;
    dbsksp_xshock_edge_sptr model_pseudo_parent_edge;
    dbsksp_compute_xgraph_root_node(this->model_tree_, model_root_node, model_pseudo_parent_edge);

    this->model_root_vid_ = model_root_node->id();
    this->model_pseudo_parent_eid_ = model_pseudo_parent_edge->id();
  }
  vcl_cout << "done.\n";

  this->print_debug_info(
    vul_sprintf("model_root_vid %d\n", this->model_root_vid_) + 
    vul_sprintf("model_pseudo_parent_eid %d\n", this->model_pseudo_parent_eid_));


  //5) Compute depths for all trees
  this->model_xgraph_->compute_vertex_depths(this->model_root_vid_);


  //6) Compute correspondence, contract, and splice cost of parents
  this->dart_corr_parent_to_model_.resize(this->num_parents_);
  this->parent_contract_and_splice_cost_.resize(this->num_parents_);
  for (int i =0; i < this->num_parents_; ++i)
  {
    dbsksp_edit_distance work;
    double edit_cost = this->compute_edit_distance(this->parent_trees_[i], this->model_tree_, work);
    work.get_final_correspondence(this->dart_corr_parent_to_model_[i]);
    double deform_cost = work.get_deform_cost(this->dart_corr_parent_to_model_[i]);
    this->parent_contract_and_splice_cost_[i] = edit_cost - deform_cost;

    vcl_cout 
      << "\nDistance trimmed"<< i+1 << " --> model = " << edit_cost
      << "\n  Deformation cost           = " << deform_cost
      << "\n  Contract + splice cost     = " << this->parent_contract_and_splice_cost_[i] << "\n";
  }
  
  //7) Optimize model tree

  //a) Form Euler tour of nodes to visit
  vcl_vector<dbsksp_xshock_node_sptr > nodes_on_euler_tour;
  dbsksp_compute_coarse_euler_tour(this->model_xgraph_, nodes_on_euler_tour);

  //b) Shape model to modify the model_xgraph
  dbsksp_xgraph_model_using_L_alpha_phi_radius xgraph_model(this->model_xgraph_, 
    this->model_root_vid_, this->model_pseudo_parent_eid_);

  
  //c) Determine initial state of optimization
  
  // update relative error
  this->update_avg_distance();
  this->init_avg_distance_ = this->cur_avg_distance_;

  // Timer to keep track of time
  vul_timer timer;
  timer.mark();
  
  // Tolerance in optimization
  double f_tol = this->distance_ratio_tol_ * this->init_avg_distance_;

  // print debug info
  
  this->print_debug_info(
    vul_sprintf("Init average distance %f\n", this->init_avg_distance_) + 
    vul_sprintf("f_tol              %f\n", f_tol));

  //b) Move along the Euler tour and optimize
  vcl_vector<double > trace_avg_distance;
  this->print_debug_info("begin_trace_average_distance [time(ms) average_distance]\n");
  this->print_debug_info(vul_sprintf("%f %g\n", 0, this->cur_avg_distance_));

  dbsksp_xshock_node_sptr prev_xv = 0;


  double min_avg_distance = this->cur_avg_distance_;
  vnl_vector<double > argmin_avg_distance;
  xgraph_model.get_xgraph_state(argmin_avg_distance);
  for (unsigned kk = 0; kk < 5; ++kk)
  {
    vcl_cout << "\nEuler tour round = " << kk << "\n";
    
    double start_avg_distance = this->cur_avg_distance_;
    for (unsigned k =0; k < nodes_on_euler_tour.size(); ++k)
    {
      vul_timer timer1;
      timer1.mark();

      // active node
      dbsksp_xshock_node_sptr xv = nodes_on_euler_tour[k];

      // skip repeated nodes
      if (xv == prev_xv)
        continue;

      // update prev_xv
      prev_xv = xv;

      //i) construct cost function
      vcl_cout << "\nnode id= " << xv->id() << "\n";
      xv->print(vcl_cout);

      vcl_vector<double > weights(this->num_parents_, 1);
      dbsksp_weighted_avg_n_xgraphs_one_node_cost_function one_node_cost(
        this->model_tree_, this->parent_trees_, weights,
        this->dart_corr_parent_to_model_,
        this->parent_contract_and_splice_cost_,
        this->scurve_matching_R(), &xgraph_model,
        xv->id());

      // Optimize

      // Initial value for the state vector
      vnl_vector<double > x;
      one_node_cost.cur_x(x);

      // Use Powell to minimize cost
      double f0 = one_node_cost.f(x);
      vcl_cout << "Init cost = " << f0 << "\n";

      // Powell
      vnl_powell powell(&one_node_cost);
      powell.set_verbose(true);
      powell.set_f_tolerance(f_tol);
      powell.minimize(x);
      
      double final_cost = one_node_cost.f(x);
      vcl_cout << "Final cost = " << final_cost << "\n";

      // True distance to two original shape
      this->update_avg_distance();

      // We want to keep lowest relative error
      if (this->cur_avg_distance_ < min_avg_distance)
      {
        min_avg_distance    = this->cur_avg_distance_;
        xgraph_model.get_xgraph_state(argmin_avg_distance);
      }
      // keep status-quo if the new error is too big
      else if (this->cur_avg_distance_ > 2* min_avg_distance) // \todo 2 is an arbitrary choice.
      {
        xgraph_model.set_xgraph_state(argmin_avg_distance);
        this->update_avg_distance();
      }

      trace_avg_distance.push_back(this->cur_avg_distance_);

      // print debug info
      this->print_debug_info(vul_sprintf("%f %f\n", float(timer.real())/1000, this->cur_avg_distance_));
      vcl_cout << "Time: " << timer1.real() / 1000 << " secs\n";
    }
    double end_avg_distance = this->cur_avg_distance_;

    // Stop when improvement is too small.
    if ( (start_avg_distance - end_avg_distance) < this->distance_ratio_tol_*start_avg_distance)
    {
      break;
    }
  }
  this->print_debug_info("end_trace_relative_error\n");

  // retrieve the xgraph state with minimum relative error
  xgraph_model.set_xgraph_state(argmin_avg_distance);
  this->average_xgraph_ = new dbsksp_xshock_graph(* (xgraph_model.xgraph()));


  if (this->debugging())
  {
    x_write(this->base_name_ + "-model-grad-descent-last.xml", this->average_xgraph_);
  }
  vcl_cout << "\nTimer - real time: " << timer.real() / 1000 << " seconds\n";

  this->update_avg_distance();
  for (int i =0; i < this->num_parents_; ++i)
  {
    this->print_debug_info(vul_sprintf("distance_parent%d_to_average %f\n", i+1, this->distance_parent_to_avg_[i]));
  }
  return true;
}


//------------------------------------------------------------------------------
//: Print debug info to a file
void dbsksp_avg_n_xgraphs_grad_descent::
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





