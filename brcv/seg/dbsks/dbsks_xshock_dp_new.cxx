// This is file seg/dbsks/dbsks_xshock_dp_new.cxx

//:
// \file

#include "dbsks_xshock_dp_new.h"


#include <dbsks/dbsks_xshock_utils.h>
#include <dbsksp/dbsksp_xshock_graph.h>
#include <dbsks/dbsks_xfrag_geom_model.h>
#include <dbsks/dbsks_xgraph_geom_model.h>
#include <dbsks/dbsks_xnode_geom_model.h>
#include <dbsks/dbsks_biarc_sampler.h>
#include <dbsks/dbsks_xshock_likelihood.h>
//#include <dbsksp/dbsksp_xshock_fragment.h>

#include <vul/vul_timer.h>
#include <vul/vul_sprintf.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_matrix.h>
#include <vcl_utility.h>
#include <vcl_iostream.h>

// ============================================================================
// dbsks_xshock_dp_new
// ============================================================================








static int miner_child_eid;

//------------------------------------------------------------------------------
//: Compute node visisting schedule
void dbsks_xshock_dp_new::
compute_node_visit_schedule(const dbsksp_xshock_graph_sptr& xgraph,
                            unsigned root_vid,
                            vcl_vector<dbsksp_xshock_node_sptr >& node_visit_schedule)
{
  node_visit_schedule.clear();

  // for each node, add its children first and then add itself
  dbsksp_xshock_node_sptr root_xv = xgraph->node_from_id(root_vid);
  
  if (!root_xv)
    return;

  this->compute_subtree_node_visit_schedule_recursive(root_xv, node_visit_schedule);
  return;
}





// ---------------------------------------------------------------------------
//: Dynamic programming on the graph of nodes
// "root_vid" is id of the root node, should be of degree-2
// "child_eid" is id of the main child edge
bool dbsks_xshock_dp_new::
optimize()
{  
  // DYNAMIC PROGRAMMING TO MINIMIZE ENERGY ------------------------------------

  // Set sampling parameters
  this->num_samples_c_ = 60; // degree-2 node: number of samples for child

  // timer to count time spent for each stage
  vul_timer timer;
  double total_time = 0; // total time to run the algorithm, in seconds

  // Compute depths of graph vertices, in case it hasn't been done
  this->graph()->compute_vertex_depths(this->root_vid_);

  //> Compute maxmimum acceptable cost from min_acceptable_loglike and the likelihood function
  // Note that high confidence --> low cost
  this->compute_max_acceptable_xgraph_cost();


  //> Determine a visiting schedule that minimize that number of max "active nodes"
  vcl_vector<dbsksp_xshock_node_sptr > node_visit_schedule;
  this->compute_node_visit_schedule(this->graph(), this->root_vid_, node_visit_schedule);

  if (node_visit_schedule.empty())
  {
    vcl_cout << "\nERROR: node visit schedule is empty.\n";
    return false;
  }
  // the last node should be root node
  assert(node_visit_schedule.back()->id() == this->root_vid_);
  

  //> Follow the node-visit-schedule and optimize the nodes sequentially, except root node
  for (unsigned kk =0; (kk+1) < node_visit_schedule.size(); ++kk)
  {
    dbsksp_xshock_node_sptr v_p = node_visit_schedule[kk];
      
    // ignore leaf-node
    if (v_p->degree() == 1)
      continue;

    // Optimize degree-2 node
    if (v_p->degree() == 2)
    {
      dbsksp_xshock_edge_sptr parent_xe = v_p->parent_edge();
      dbsksp_xshock_edge_sptr child_edge = this->graph()->cyclic_adj_succ(parent_xe, v_p);
      
      vcl_cout 
        << "\nOptimizing degree-2 node, parent_vid = " << v_p->id() 
        << "\n                          child vid  = " << child_edge->opposite(v_p)->id() 
        << "\n                          child eid  = " << child_edge->id() << vcl_endl;
      timer.mark();

      ////////////////////////////////////////////////////////////////////////
      this->optimize_degree_2_node(v_p, child_edge);
      ////////////////////////////////////////////////////////////////////////

      double real_time = double(timer.real()) / 100;
      total_time += real_time;

      vcl_cout 
        << "\n    Real time spent  = " << vul_sprintf("%6.8g", real_time) 
        << "\n    Total time spent = " << vul_sprintf("%6.8g", total_time) << vcl_endl;
    }
    else if (v_p->degree() == 3)
    {
      // Parent edge of v_p (pointing to the v_p's parent node)
      dbsksp_xshock_edge_sptr parent_edge = v_p->parent_edge();
      
      // Two child edges (pointing to v_p's child vertices)
      dbsksp_xshock_edge_sptr child_edge1 = this->graph()->cyclic_adj_succ(parent_edge, v_p);
      dbsksp_xshock_edge_sptr child_edge2 = this->graph()->cyclic_adj_succ(child_edge1, v_p);

      vcl_cout 
        << "\nOptimizing degree-3 node, parent_vid = " << v_p->id() 
        << "\n                          child_vid1 = " << child_edge1->opposite(v_p)->id()
        << "\n                          child_eid1 = " << child_edge1->id()
        << "\n                          child_vid2 = " << child_edge2->opposite(v_p)->id() 
        << "\n                          child_eid2 = " << child_edge2->id() << vcl_endl;

      timer.mark();

      ////////////////////////////////////////////////////////////////////////
      this->optimize_degree_3_node(v_p, parent_edge, child_edge1, child_edge2);
      ////////////////////////////////////////////////////////////////////////

      double real_time = double(timer.real()) / 100;
      total_time += real_time;
      vcl_cout 
        << "\n    Real time spent = " << vul_sprintf("%6.8g", real_time)
        << "\n    Total time spent= " << vul_sprintf("%6.8g", total_time) << vcl_endl;
    }
    else
    {
      vcl_cout << "ERROR: Can't handle nodes with degree > 3 now.\n";
      return false;
    }
  } // for kk


  // Optimize the cost for each state of the root node
  vcl_cout << "\nOptimizing root-node, root_vid = " << this->root_vid_ << vcl_endl;
  timer.mark();

  //////////////////////////////////////////////////////////////////////////////
  this->optimize_root_node(this->root_vid_, this->major_child_eid_);
  //////////////////////////////////////////////////////////////////////////////

  double real_time = double(timer.real()) / 100;
  vcl_cout << "    Real time spent = " << vul_sprintf("%6.8g", real_time) << vcl_endl;
  total_time += real_time;
  vcl_cout << "    Total time spent= " << vul_sprintf("%6.8g", total_time) << vcl_endl;


  //> Compute local optimum solutions
  timer.mark();
  vcl_cout << "\n> Compute locally optimized solutions and their costs \n";
  this->find_xgraph_state_local_optimum(this->root_vid_, this->list_opt_xgraph_state, this->list_opt_cost); 
  
  real_time = double(timer.real()) / 1000;
  vcl_cout << "    Real time spent = " << vul_sprintf("%6.8g", real_time) << vcl_endl;
  total_time += real_time;
  vcl_cout << "    Total time spent= " << vul_sprintf("%6.8g", total_time) << vcl_endl;

  return true;
}


//------------------------------------------------------------------------------
//: Compute maximum acceptable cost during DP, given the min acceptable level
// and the dynamic of the likelihood function
void dbsks_xshock_dp_new::
compute_max_acceptable_xgraph_cost()
{
  // xgraph_cost will have range [0, \infty) and the cost of each fragment
  // is always non-negative (computed as (-loglike) - (-max_loglike))
  double lowerbound = 0;
  for (dbsksp_xshock_graph::edge_iterator iter = this->graph()->edges_begin();
    iter != this->graph()->edges_end(); ++iter)
  {
    unsigned eid = (*iter)->id();
    lowerbound += (-this->xshock_likelihood()->loglike_upperbound(eid));
  }
  this->max_acceptable_xgraph_cost_ = float(-this->min_acceptable_xgraph_loglike_ - lowerbound);
  return;
}





//: ----------------------------------------------------------------------------
//: Set the working graph (and compute its size)
void dbsks_xshock_dp_new::
set_graph(const dbsksp_xshock_graph_sptr& graph) 
{ 
  this->graph_ = graph; 
  this->graph_size_ = vcl_sqrt(graph->area());
}









//: ----------------------------------------------------------------------------
//: Initialize all the cost grids
bool dbsks_xshock_dp_new::
init_cost_grids()
{
  // clean up
  this->map_opt_cost.clear();
  this->map_opt_child.clear();
  this->map_opt_child1.clear();
  this->map_opt_child2.clear();
  this->map_opt_child_of_root.clear();
  this->map_opt_phi1.clear();


  // construct optimal cost and optimal child grids for each node that has a set of states
  for (vcl_map<unsigned, dbsks_xnode_grid>::iterator git = this->map_xnode_grid->begin();
    git != this->map_xnode_grid->end(); ++git)
  {
    dbsksp_xshock_node_sptr xv = this->graph()->node_from_id(git->first);
    dbsks_xnode_grid& grid = git->second;

    float defaul_value = (xv->degree() == 1) ? 0.0f : vnl_numeric_traits<float >::maxval;

    // opt_cost
    grid2d_float& cost_grid = this->map_opt_cost[git->first] = grid2d_float();
    cost_grid.clear();

    //>> grids containing states of optimal child
    
    // special handling of root node
    if (xv->id() == this->root_vid_)
    {
      // add a separate grid for each child node of the root
      for (dbsksp_xshock_node::edge_iterator eit = xv->edges_begin(); eit !=
        xv->edges_end(); ++eit)
      {
        dbsksp_xshock_edge_sptr child_edge = *eit;
        dbsksp_xshock_node_sptr child_node = child_edge->opposite(xv);
        grid2d_int& child_grid = this->map_opt_child_of_root[child_node->id()] = grid2d_int();
      }
    } // root node
    else if (xv->degree()==1)
    {
      continue;
    }
    else if (xv->degree()==2)
    {
      grid2d_int& child_grid = this->map_opt_child[git->first] = grid2d_int();
    }
    else if (xv->degree()==3)
    {
      grid2d_int& child1_grid = this->map_opt_child1[git->first] = grid2d_int();
      grid2d_int& child2_grid = this->map_opt_child2[git->first] = grid2d_int();
    }

    // opt_phi1
    if (xv->degree()==3)
    {
      grid2d_int& phi1_grid = this->map_opt_phi1[git->first] = grid2d_int();
    }
  }
  return true;
}



//------------------------------------------------------------------------------
//: Compute node visit schedule for a subtree
void dbsks_xshock_dp_new::
compute_subtree_node_visit_schedule_recursive(
  const dbsksp_xshock_node_sptr& subtree_root,
  vcl_vector<dbsksp_xshock_node_sptr >& node_visit_schedule)
{
  // first add the children. Then add itself
  for (dbsksp_xshock_node::edge_iterator eit = subtree_root->edges_begin(); eit !=
    subtree_root->edges_end(); ++eit)
  {
    dbsksp_xshock_edge_sptr xe = *eit;
    if (xe == subtree_root->parent_edge())
      continue;

    dbsksp_xshock_node_sptr child_xv = xe->opposite(subtree_root);
    this->compute_subtree_node_visit_schedule_recursive(child_xv, node_visit_schedule);
  }
  node_visit_schedule.push_back(subtree_root);
}








//------------------------------------------------------------------------------
//: Optimize child node of an edge given a state of parent node
void dbsks_xshock_dp_new::
optimize_child_node_given_parent_node(const dbsksp_xshock_edge_sptr& xe,
                                      const dbsks_xnode_grid& grid_p, //> grid containing states of parent node
                                      float max_acceptable_subtree_cost, //> upper bound for cost
                                      grid2d_float& opt_cost_p, //> min cost of parent node, given its state
                                      grid2d_int& opt_child)  //> state of child node, given parent node's state
{
  // number of samples of the child to check for each state of the parent
  int num_samples_c = this->num_samples_c_;

  // parent node
  dbsksp_xshock_node_sptr xv_p = xe->parent_node();

  // child node
  dbsksp_xshock_node_sptr xv_c = xe->opposite(xv_p);


  vcl_cout << "  Max acceptable subtree cost = " << max_acceptable_subtree_cost << vcl_endl;
  
  // base descriptors for the child and parent nodes
  dbsksp_xshock_node_descriptor xdesc_p = *xv_p->descriptor(xe);
  dbsksp_xshock_node_descriptor xdesc_c = xv_c->descriptor(xe)->opposite_xnode();
  
  // Geometric model for this fragment
  dbsks_xfrag_geom_model_sptr geom_model = this->xgraph_geom->map_edge2geom()[xe->id()];
  geom_model->set(xdesc_p, xdesc_c);

  // Grids containing all possible states of child nodes
  dbsks_xnode_grid& grid_c = (*this->map_xnode_grid)[xv_c->id()]; // child

  // Retrieve optimal cost of subtree rooted at child node.
  grid2d_float& opt_cost_c = map_opt_cost[xv_c->id()];

  // Allocate memory for the opt_cost_p and opt_child grids
  {
    int num_x = grid_p.x_.size();
    int num_y = grid_p.y_.size();
    int num_states_per_cell = grid_p.num_states_per_cell();

    this->allocate_grid2d(opt_cost_p, num_x, num_y, num_states_per_cell, vnl_numeric_traits<float >::maxval);
    this->allocate_grid2d(opt_child, num_x, num_y, num_states_per_cell, -1);
  }

  // grid of states for the parent node
  vcl_vector<double > x_vec, y_vec;
  vcl_vector<int > index_x_vec, index_y_vec;
  vcl_vector<double > psi_vec, phi0_vec, r_vec;
  vcl_vector<int > index_psi_vec, index_phi0_vec, index_r_vec;

  {
    // all x's
    int num_x_backward = grid_p.x_.size()/2;
    int num_x_forward = num_x_backward;
    grid_p.sample_x(grid_p.x_[grid_p.x_.size()/2], num_x_backward, num_x_forward, x_vec, index_x_vec);

    // all y's
    int num_y_backward = grid_p.y_.size()/2;
    int num_y_forward = num_y_backward;
    grid_p.sample_y(grid_p.y_[grid_p.y_.size()/2], num_y_backward, num_y_forward, y_vec, index_y_vec);

    // all psi's
    int num_psi_backward = grid_p.psi_.size()/2;
    int num_psi_forward  = num_psi_backward;
    grid_p.sample_psi(grid_p.psi_[grid_p.psi_.size()/2], num_psi_backward, num_psi_forward, psi_vec, index_psi_vec);

    // all phi0's
    int num_phi0_backward = grid_p.phi0_.size()/2;
    int num_phi0_forward = num_phi0_backward;
    grid_p.sample_phi0(grid_p.phi0_[grid_p.phi0_.size()/2], num_phi0_backward, num_phi0_forward, phi0_vec, index_phi0_vec);

    // all r's
    int num_r_backward = grid_p.r_.size()/2;
    int num_r_forward = num_r_backward;
    grid_p.sample_r(grid_p.r_[grid_p.r_.size()/2], num_r_backward, num_r_forward, r_vec, index_r_vec);
  }

  unsigned size_x_vec = x_vec.size();
  unsigned size_y_vec = y_vec.size();
  unsigned size_psi_vec = psi_vec.size();
  unsigned size_phi0_vec = phi0_vec.size();
  unsigned size_r_vec = r_vec.size();

  vcl_cout << "  Parent Node States:\n";
  vcl_cout << "  size x_vec = " << size_x_vec << vcl_endl;
  vcl_cout << "  size y_vec = " << size_y_vec << vcl_endl;
  vcl_cout << "  size psi_vec = " << size_psi_vec << vcl_endl;
  vcl_cout << "  size phi0_vec = " << size_phi0_vec << vcl_endl;
  vcl_cout << "  size r_vec = " << size_r_vec << vcl_endl;


  //// Propagate min-cost from the child node to the parent node using constraint on
  //// orientation and radius
  //
  //unsigned num_x_p = grid_p.x_.size();
  //unsigned num_y_p = grid_p.y_.size();
  //vnl_matrix<float > min_cost_xy_p(num_x_p, num_y_p, vnl_numeric_traits<float >::maxval);
  //this->propagate_min_cost_from_children_to_parent_node(min_cost_xy_p, xv_p->id());


  // THE BIG LOOP
  //int count_pos_skipped = 0;
  //int count_test = 0;

  vcl_vector<dbsksp_xshock_node_descriptor > edesc_list;
  for (unsigned k1 =0; k1 < size_x_vec; ++k1)
  {
    int ip_x = index_x_vec[k1];
    for (unsigned k2 =0; k2 < size_y_vec; ++k2)
    {
      int ip_y = index_y_vec[k2];
      vnl_vector<float >& opt_cost = opt_cost_p(ip_x, ip_y);


      //// ignore this location its (potentially) best cost is higher than max_acceptable_subtree_cost
      //if (min_cost_xy_p(ip_x, ip_y) > max_acceptable_subtree_cost)
      //{
      //  ++count_pos_skipped;
      //  continue;
      //}


      for (unsigned k3 =0; k3 < size_psi_vec; ++k3)
      {
        int ip_psi = index_psi_vec[k3];
        for (unsigned k4 =0; k4 < size_phi0_vec; ++k4)
        {
          int ip_phi0 = index_phi0_vec[k4];
          for (unsigned k5 =0; k5 < size_r_vec; ++k5)
          {
			//count_test ++;
			//vcl_cout << count_test << vcl_endl;
            int ip_r = index_r_vec[k5];
            int ip_desc = grid_p.cell_grid_to_linear(ip_psi, ip_phi0, ip_r);

            // descriptor at the parent node
            dbsksp_xshock_node_descriptor xd_p = grid_p.xdesc(ip_x, ip_y, ip_psi, ip_phi0, ip_r);    

            // given the parent's state, we will now find the child state that 
            // will minimize the cost for this branch (from xv_p downward)

            // place holder for the optimal cost and optimal child state
            float min_cost = vnl_numeric_traits<float >::maxval;
            int min_cost_idx = -1; //default is -1:  non-existing or dependent (for degree-1 node)

            this->find_optimal_child_node_given_parent_state(xe->id(), xd_p, 
              xv_c->degree(), geom_model, num_samples_c, grid_c, opt_cost_c, 
              min_cost, min_cost_idx, max_acceptable_subtree_cost);

            // record optimal cost
            opt_cost_p(ip_x, ip_y)[ip_desc] = min_cost;
            opt_child(ip_x, ip_y)[ip_desc] = min_cost_idx;
          } // k5
        } // k4
      } // k3
    } // k2
  } // k1

  //vcl_cout << "\nNumer of position skipped: " << count_pos_skipped 
  //  << "/" << size_x_vec * size_y_vec << "\n";

  return;
}

//------------------------------------------------------------------------------
//: Optimize child node of an edge given a state of parent node
void dbsks_xshock_dp_new::
optimize_child_node_given_parent_node2(const dbsksp_xshock_edge_sptr& xe,
                                      const dbsks_xnode_grid& grid_p, //> grid containing states of parent node
                                      float max_acceptable_subtree_cost, //> upper bound for cost
                                      grid2d_float& opt_cost_p, //> min cost of parent node, given its state
                                      grid2d_int& opt_child)  //> state of child node, given parent node's state
{
  // number of samples of the parent to check for each state of the child
  int num_samples_p = this->num_samples_c_;

  // parent node
  dbsksp_xshock_node_sptr xv_p = xe->parent_node();


  // child node
  dbsksp_xshock_node_sptr xv_c = xe->opposite(xv_p);


  vcl_cout << "  Max acceptable subtree cost = " << max_acceptable_subtree_cost << vcl_endl;
  
  // base descriptors for the child and parent nodes
  dbsksp_xshock_node_descriptor xdesc_p = *xv_p->descriptor(xe);
  dbsksp_xshock_node_descriptor xdesc_c = xv_c->descriptor(xe)->opposite_xnode();
  

  //if(xv_p->id()==this->root_vid_)
	//num_samples_p*=5;

  // Geometric model for this fragment
  dbsks_xfrag_geom_model_sptr geom_model = this->xgraph_geom->map_edge2geom()[xe->id()];
  geom_model->set(xdesc_p, xdesc_c);

  // Grids containing all possible states of child nodes
  dbsks_xnode_grid& grid_c = (*this->map_xnode_grid)[xv_c->id()]; // child

  // Retrieve optimal cost of subtree rooted at child node.
  grid2d_float& opt_cost_c = map_opt_cost[xv_c->id()];

  // Allocate memory for the opt_cost_p and opt_child grids
  {
    int num_x = grid_p.x_.size();
    int num_y = grid_p.y_.size();
    int num_states_per_cell = grid_p.num_states_per_cell();

    this->allocate_grid2d(opt_cost_p, num_x, num_y, num_states_per_cell, vnl_numeric_traits<float >::maxval);
    this->allocate_grid2d(opt_child, num_x, num_y, num_states_per_cell, -1);
  }

  // grid of states for the child node
  vcl_vector<double > x_vec, y_vec;
  vcl_vector<int > index_x_vec, index_y_vec;
  vcl_vector<double > psi_vec, phi0_vec, r_vec;
  vcl_vector<int > index_psi_vec, index_phi0_vec, index_r_vec;

  {
    // all x's
    int num_x_backward = grid_c.x_.size()/2;
    int num_x_forward = num_x_backward;
    grid_c.sample_x(grid_c.x_[grid_c.x_.size()/2], num_x_backward, num_x_forward, x_vec, index_x_vec);

    // all y's
    int num_y_backward = grid_c.y_.size()/2;
    int num_y_forward = num_y_backward;
    grid_c.sample_y(grid_c.y_[grid_c.y_.size()/2], num_y_backward, num_y_forward, y_vec, index_y_vec);

    // all psi's
    int num_psi_backward = grid_c.psi_.size()/2;
    int num_psi_forward  = num_psi_backward;
    grid_c.sample_psi(grid_c.psi_[grid_c.psi_.size()/2], num_psi_backward, num_psi_forward, psi_vec, index_psi_vec);

    // all phi0's
    int num_phi0_backward = grid_c.phi0_.size()/2;
    int num_phi0_forward = num_phi0_backward;
    grid_c.sample_phi0(grid_c.phi0_[grid_c.phi0_.size()/2], num_phi0_backward, num_phi0_forward, phi0_vec, index_phi0_vec);

    // all r's
    int num_r_backward = grid_c.r_.size()/2;
    int num_r_forward = num_r_backward;
    grid_c.sample_r(grid_c.r_[grid_c.r_.size()/2], num_r_backward, num_r_forward, r_vec, index_r_vec);
  }

  unsigned size_x_vec = x_vec.size();
  unsigned size_y_vec = y_vec.size();
  unsigned size_psi_vec = psi_vec.size();
  unsigned size_phi0_vec = phi0_vec.size();
  unsigned size_r_vec = r_vec.size();

  vcl_cout << "  Child Node States:\n";
  vcl_cout << "  size x_vec = " << size_x_vec << vcl_endl;
  vcl_cout << "  size y_vec = " << size_y_vec << vcl_endl;
  vcl_cout << "  size psi_vec = " << size_psi_vec << vcl_endl;
  vcl_cout << "  size phi0_vec = " << size_phi0_vec << vcl_endl;
  vcl_cout << "  size r_vec = " << size_r_vec << vcl_endl;


  //// Propagate min-cost from the child node to the parent node using constraint on
  //// orientation and radius
  //
  //unsigned num_x_p = grid_p.x_.size();
  //unsigned num_y_p = grid_p.y_.size();
  //vnl_matrix<float > min_cost_xy_p(num_x_p, num_y_p, vnl_numeric_traits<float >::maxval);
  //this->propagate_min_cost_from_children_to_parent_node(min_cost_xy_p, xv_p->id());


  // THE BIG LOOP
  int count_skipped = 0;
  //int count_test = 0;

  //vcl_vector<dbsksp_xshock_node_descriptor > edesc_list;
  for (unsigned k1 =0; k1 < size_x_vec; ++k1)
  {
    int ic_x = index_x_vec[k1];
    for (unsigned k2 =0; k2 < size_y_vec; ++k2)
    {
      int ic_y = index_y_vec[k2];
      //vnl_vector<float >& opt_cost = opt_cost_p(ic_x, ic_y);

      for (unsigned k3 =0; k3 < size_psi_vec; ++k3)
      {
        int ic_psi = index_psi_vec[k3];
        for (unsigned k4 =0; k4 < size_phi0_vec; ++k4)
        {
          int ic_phi0 = index_phi0_vec[k4];
          for (unsigned k5 =0; k5 < size_r_vec; ++k5)
          {
			//count_test ++;
			//vcl_cout << count_test << vcl_endl;
            int ic_r = index_r_vec[k5];
            int ic_desc = grid_c.cell_grid_to_linear(ic_psi, ic_phi0, ic_r);
			int ic_idx = grid_c.grid_to_linear(ic_x, ic_y, ic_psi, ic_phi0, ic_r);

			float subtree_cost = opt_cost_c(ic_x, ic_y)[ic_desc];

			// if the child state is not updated before, it's kept MAX and free to skip
			// this is a important step to increase efficiency
			if( subtree_cost > max_acceptable_subtree_cost)
			{
				count_skipped ++;
				continue;
			}
            // descriptor at the child node
            dbsksp_xshock_node_descriptor xd_c = grid_c.xdesc(ic_x, ic_y, ic_psi, ic_phi0, ic_r);    


            // place holder for the optimal cost and optimal child state
            float min_cost = vnl_numeric_traits<float >::maxval;
            int min_cost_idx = -1; //default is -1:  non-existing or dependent (for degree-1 node)


			this->update_parent_node_cost_given_child_state(xe->id(), xd_c, 
              xv_c->degree(), geom_model, num_samples_p, grid_p, subtree_cost, ic_idx,
              max_acceptable_subtree_cost, opt_cost_p, opt_child);

          } // k5
        } // k4
      } // k3
    } // k2
  } // k1

  vcl_cout << "Numer of states skipped: " << count_skipped 
    << "/" << size_x_vec * size_y_vec * size_psi_vec * size_phi0_vec * size_r_vec << "\n\n";

  return;
}


//------------------------------------------------------------------------------
void dbsks_xshock_dp_new::
optimize_degree_2_node(const dbsksp_xshock_node_sptr& xv_p, 
                       const dbsksp_xshock_edge_sptr& xe)
{
  // child node
  dbsksp_xshock_node_sptr xv_c = xe->opposite(xv_p);

  //> determine the maximum acceptable cost for the sub-tree rooted at xv_p
  float max_acceptable_subtree_cost = this->compute_max_acceptable_subtree_cost(xv_c->id());

  // base descriptors for the child and parent nodes
  dbsksp_xshock_node_descriptor xdesc_p = *xv_p->descriptor(xe);
  dbsksp_xshock_node_descriptor xdesc_c = xv_c->descriptor(xe)->opposite_xnode();
  
  // Grids containing all possible states of parent and child node
  dbsks_xnode_grid& grid_p = (*this->map_xnode_grid)[xv_p->id()]; // parent
  
  //////////////////////////////////////////////////////////////////////////////
  // Our goal in this function is to compute the two following matrices       //
  
  //a. Optimal cost of subtree given a state of the parent node               //
  grid2d_float& opt_cost_p = map_opt_cost[xv_p->id()];                        //
                                                                              // 
  //b. State of optimal child node given a state of the parent node           //
  grid2d_int& opt_child = this->map_opt_child[xv_p->id()];                    // 
  //////////////////////////////////////////////////////////////////////////////


  if(xv_c->degree()==1)
  	//> Main function-------------------------------------------------------------
  	this->optimize_child_node_given_parent_node(xe, grid_p, max_acceptable_subtree_cost, opt_cost_p, opt_child); // use the orignal version to optimize the branch end node
  else
  	this->optimize_child_node_given_parent_node(xe, grid_p, max_acceptable_subtree_cost, opt_cost_p, opt_child);		
  //------------------------------------------------------------------------------

  // Release memory to store optimal child costs since they will no longer be used
  grid2d_float& opt_cost_c = map_opt_cost[xv_c->id()];
  opt_cost_c.clear();

  // Compute min cost of the subtree rooted at xv_P
  float min_cost_p = this->compute_min_value(opt_cost_p);
  vcl_cout << "Min cost for for subtree (xv= "<< xv_p->id() << ") is: " << min_cost_p << "\n";

  // modify the cost map
  this->map_node_to_min_cost_.erase(xv_c->id());
  this->map_node_to_min_cost_[xv_p->id()] = min_cost_p;

  //// modify the map of min-cost matrix
  //this->map_node_to_min_cost_matrix_.erase(xv_c->id());
  //this->compute_min_value_matrix(opt_cost_p, this->map_node_to_min_cost_matrix_[xv_p->id()]);

  return;
}


// -----------------------------------------------------------------------------
//: Optimize a degree-3 node
void dbsks_xshock_dp_new::
optimize_degree_3_node(const dbsksp_xshock_node_sptr xv_p,
                       const dbsksp_xshock_edge_sptr& parent_edge,
                       const dbsksp_xshock_edge_sptr& child_edge1,
                       const dbsksp_xshock_edge_sptr& child_edge2)
{
  // child vertices
  dbsksp_xshock_node_sptr xv_c1 = child_edge1->opposite(xv_p);
  dbsksp_xshock_node_sptr xv_c2 = child_edge2->opposite(xv_p);

  // Compute upper bound of subtree costs
  float max_acceptable_subtree_cost = 
    this->compute_max_acceptable_subtree_cost(xv_c1->id(), xv_c2->id(), xv_p->id());

  float max_acceptable_subtree1_cost = 
    this->compute_max_acceptable_subtree_cost(xv_c1->id(), xv_p->id());

  float max_acceptable_subtree2_cost = 
    this->compute_max_acceptable_subtree_cost(xv_c2->id(), xv_p->id());

  vcl_cout << "  Max acceptable subtree cost = " << max_acceptable_subtree_cost << "\n"
    << "  Max acceptable subtree1 cost = " << max_acceptable_subtree1_cost << vcl_endl;

  // base descriptors

  // xdesc_p "convers" xdesc_p1 and xdesc_p2
  dbsksp_xshock_node_descriptor xdesc_p = xv_p->descriptor(parent_edge)->opposite_xnode();
  dbsksp_xshock_node_descriptor xdesc_p1 = *(xv_p->descriptor(child_edge1));
  dbsksp_xshock_node_descriptor xdesc_p2 = *(xv_p->descriptor(child_edge2));
  dbsksp_xshock_node_descriptor xdesc_c1 = xv_c1->descriptor(child_edge1)->opposite_xnode();
  dbsksp_xshock_node_descriptor xdesc_c2 = xv_c2->descriptor(child_edge2)->opposite_xnode();

  // Geometric model for this fragment
  dbsks_xfrag_geom_model_sptr geom_model1 = this->xgraph_geom->map_edge2geom()[child_edge1->id()];
  geom_model1->set(xdesc_p1, xdesc_c1);

  dbsks_xfrag_geom_model_sptr geom_model2 = this->xgraph_geom->map_edge2geom()[child_edge2->id()];
  geom_model2->set(xdesc_p2, xdesc_c2);

  // Loop thru all possibilities of xp
  dbsks_xnode_grid& grid_p = (*this->map_xnode_grid)[xv_p->id()];
  dbsks_xnode_grid& grid_c1 = (*this->map_xnode_grid)[xv_c1->id()];
  dbsks_xnode_grid& grid_c2 = (*this->map_xnode_grid)[xv_c2->id()];

  // optimal cost of subtrees rooted at the child nodes 
  grid2d_float& opt_cost_c1 = map_opt_cost[xv_c1->id()]; // child1
  grid2d_float& opt_cost_c2 = map_opt_cost[xv_c2->id()]; // child2  
  
  //////////////////////////////////////////////////////////////////////////////
  // The goal of this function is to compute the following grids
  
  // Optimal costs of the subtree rooted at the parent node
  grid2d_float& opt_cost_p = map_opt_cost[xv_p->id()];
  
  // optimal states of the child nodes given a state of the parent node
  grid2d_int& opt_child1 = this->map_opt_child1[xv_p->id()];
  grid2d_int& opt_child2 = this->map_opt_child2[xv_p->id()];
  grid2d_int& opt_phi1 = this->map_opt_phi1[xv_p->id()];
  //////////////////////////////////////////////////////////////////////////////




  // construct a grid for the parent node wrt child edge 1
  dbsks_xnode_grid grid_p1;
  {
    double angle_padding_ratio = 1.5;

    double min_psi_start, max_psi_start, min_phi_start, max_phi_start;
    geom_model1->get_range_psi_start(min_psi_start, max_psi_start);
    geom_model1->get_range_phi_start(min_phi_start, max_phi_start);

    double range_psi = angle_padding_ratio * (max_psi_start - min_psi_start);
    double mean_psi = (max_psi_start+min_psi_start) / 2;
    
    vcl_cout 
      << "\n  Old range for psi_start: [" << min_psi_start << ", " << max_psi_start << "]"
      << "\n  New range for psi_start: [" << (mean_psi-range_psi/2) << ", " << (mean_psi+range_psi/2) << "]\n";

    double range_phi0 = angle_padding_ratio * (max_phi_start - min_phi_start);
    double mean_phi0 = (max_phi_start + min_phi_start) / 2;

    
    vcl_cout 
      << "\n  Old range for phi0: [" << min_phi_start << ", " << max_phi_start << "]"
      << "\n  New range for phi0: [" << (mean_phi0-range_phi0/2) << ", " << (mean_phi0+range_phi0/2) << "]\n";

    grid_p1 = grid_p;
    grid_p1.phi1_.clear();
    grid_p1.set_psi_vec(mean_psi-range_psi/2, mean_psi+range_psi/2, grid_p.step_psi_);
    grid_p1.set_phi0_vec(mean_phi0 - range_phi0/2, mean_phi0 + range_phi0/2, grid_p.step_phi0_);
  }


  // Optimize child edge 1
  grid2d_float opt_cost_p1; 
  grid2d_int   opt_child_p1;
  this->optimize_child_node_given_parent_node(child_edge1, grid_p1, max_acceptable_subtree1_cost, opt_cost_p1, opt_child_p1);
  {
    // Compute min cost of the subtree
    float min_cost_p1 = this->compute_min_value(opt_cost_p1);
    vcl_cout << "\nMin cost for for subtree (xv= "<< xv_p->id() << ", xe= " << child_edge1->id() << ") is: " << min_cost_p1 << "\n";
  }




  // construct a grid for the parent node wrt child edge 1
  dbsks_xnode_grid grid_p2;
  {
    double angle_padding_ratio = 1.5;

    double min_psi_start, max_psi_start, min_phi_start, max_phi_start;
    geom_model2->get_range_psi_start(min_psi_start, max_psi_start);
    geom_model2->get_range_phi_start(min_phi_start, max_phi_start);

    double range_psi = angle_padding_ratio * (max_psi_start - min_psi_start);
    double mean_psi = (max_psi_start+min_psi_start) / 2;

    vcl_cout 
      << "\n  Old range for psi_start: [" << min_psi_start << ", " << max_psi_start << "]"
      << "\n  New range for psi_start: [" << (mean_psi-range_psi/2) << ", " << (mean_psi+range_psi/2) << "]\n";

    double range_phi0 = angle_padding_ratio * (max_phi_start - min_phi_start);
    double mean_phi0 = (max_phi_start + min_phi_start) / 2;

    vcl_cout 
      << "\n  Old range for phi0: [" << min_phi_start << ", " << max_phi_start << "]"
      << "\n  New range for phi0: [" << (mean_phi0-range_phi0/2) << ", " << (mean_phi0+range_phi0/2) << "]\n";

    grid_p2 = grid_p;
    grid_p2.phi1_.clear();
    grid_p2.set_psi_vec(mean_psi-range_psi/2, mean_psi+range_psi/2, grid_p.step_psi_);
    grid_p2.set_phi0_vec(mean_phi0 - range_phi0/2, mean_phi0 + range_phi0/2, grid_p.step_phi0_);
  }



  


  // Optimize child edge 2
  grid2d_float opt_cost_p2;
  grid2d_int opt_child_p2;
  this->optimize_child_node_given_parent_node(child_edge2, grid_p2, max_acceptable_subtree2_cost, opt_cost_p2, opt_child_p2);
  {
    // Compute min cost of the subtree
    float min_cost_p2 = this->compute_min_value(opt_cost_p2);
    vcl_cout << "\nMin cost for for subtree (xv= "<< xv_p->id() << ", xe= " << child_edge2->id() << ") is: " << min_cost_p2 << "\n";
  }







  // Allocate memory for grids to be computed

  {
    int size_per_cell = grid_p.num_states_per_cell();
    int num_x = grid_p.x_.size();
    int num_y = grid_p.y_.size();
    this->allocate_grid2d(opt_cost_p, num_x, num_y, size_per_cell, vnl_numeric_traits<float >::maxval);
    this->allocate_grid2d(opt_phi1, num_x, num_y, size_per_cell, -1);
    this->allocate_grid2d(opt_child1, num_x, num_y, size_per_cell, -1);
    this->allocate_grid2d(opt_child2, num_x, num_y, size_per_cell, -1);
  }

  //>> sample the parent node

  // index vectors
  vcl_vector<int > index_x_vec, index_y_vec, index_psi_vec, index_phi0_vec, index_r_vec, index_phi1_vec;
  vcl_vector<double > x_vec, y_vec, psi_vec, phi0_vec, r_vec, phi1_vec;
  {
    // we cover every point spatially
    int num_x_backward = grid_p.x_.size()/2;
    int num_x_forward = grid_p.x_.size()/2;
    grid_p.sample_x(grid_p.x_[grid_p.x_.size()/2], num_x_backward, num_x_forward, x_vec, index_x_vec); // everything

    
    int num_y_backward = grid_p.y_.size()/2;
    int num_y_forward = grid_p.y_.size()/2;
    grid_p.sample_y(grid_p.y_[grid_p.y_.size()/2], num_y_backward, num_y_forward, y_vec, index_y_vec); // everything

    int num_psi_backward = grid_p.psi_.size() / 2;
    int num_psi_forward = grid_p.psi_.size() / 2;
    grid_p.sample_psi(grid_p.psi_[grid_p.psi_.size()/2], num_psi_backward, num_psi_forward, psi_vec, index_psi_vec); // everything

    int num_phi0_backward = grid_p.phi0_.size() / 2;
    int num_phi0_forward = grid_p.phi0_.size() / 2;
    grid_p.sample_phi0(grid_p.phi0_[grid_p.phi0_.size()/2], num_phi0_backward, num_phi0_forward, phi0_vec, index_phi0_vec); // everything

    int num_r_backward = grid_p.r_.size()/2;
    int num_r_forward = grid_p.r_.size()/2;
    grid_p.sample_r(grid_p.r_[grid_p.r_.size()/2], num_r_backward, num_r_forward, r_vec, index_r_vec); // everything

    int num_phi1_backward = grid_p.phi1_.size() / 2;
    int num_phi1_forward = grid_p.phi1_.size() / 2;
    grid_p.sample_phi1(grid_p.phi1_[grid_p.phi1_.size()/2], num_phi1_backward, num_phi1_forward, phi1_vec, index_phi1_vec); // everything
  }

  // THE BIG LOOP

  unsigned size_x_vec = index_x_vec.size();
  unsigned size_y_vec = index_y_vec.size();
  unsigned size_psi_vec = index_psi_vec.size();
  unsigned size_phi0_vec = index_phi0_vec.size();
  unsigned size_r_vec = index_r_vec.size();
  unsigned size_phi1_vec = index_phi1_vec.size();

  vcl_cout << "  size x_vec = " << size_x_vec << vcl_endl;
  vcl_cout << "  size y_vec = " << size_y_vec << vcl_endl;
  vcl_cout << "  size psi_vec = " << size_psi_vec << vcl_endl;
  vcl_cout << "  size phi0_vec = " << size_phi0_vec << vcl_endl;
  vcl_cout << "  size r_vec = " << size_r_vec << vcl_endl;
  vcl_cout << "  size phi1_vec = " << size_phi1_vec << vcl_endl;

  for (unsigned k1 =0; k1 < size_x_vec; ++k1)
  {
    int ip_x = index_x_vec[k1];
    double x_p = x_vec[k1];
    for (unsigned k2 =0; k2 < size_y_vec; ++k2)
    {
      int ip_y = index_y_vec[k2];
      double y_p = y_vec[k2];
      
      vnl_vector<float >& opt_cost = opt_cost_p(ip_x, ip_y);

      for (unsigned k3 =0; k3 < size_psi_vec; ++k3)
      {
        int ip_psi = index_psi_vec[k3];
        double psi_p = psi_vec[k3];
        for (unsigned k4 =0; k4 < size_phi0_vec; ++k4)
        {
          int ip_phi0 = index_phi0_vec[k4];
          double phi0_p = phi0_vec[k4];

          for (unsigned k5 =0; k5 < size_r_vec; ++k5)
          {
            int ip_r = index_r_vec[k5];
            double r_p = r_vec[k5];

            // descriptor index at the parent node
            int ip_desc = grid_p.cell_grid_to_linear(ip_psi, ip_phi0, ip_r);

            float min_cost = vnl_huge_val(float());
            int min_cost_idx1 = -1;
            int min_cost_idx2 = -1;
            int min_cost_idx_phi1 = -1;



            // iterate thru phi1 and find the minimum cost
            for (unsigned k6 =0; k6 < size_phi1_vec; ++k6)
            {
              int ip_phi1 = index_phi1_vec[k6];
              double phi1_p = phi1_vec[k6];

              // we now step by step construct xdesc_p1 and xdesc_p2, which are
              // node descriptors at the parent node xv_p for the two child edges c1 and c2

              // first we compute the angle phi's for each descriptor
              // phi1 is the difference between the phi angles of xdesc_p1 and xdesc_p2
              // phi0 is the sum of the phi angles of xdesc_p1 and xdesc_p2
              double phi_p1 = (phi0_p + phi1_p) / 2;
              double phi_p2 = (phi0_p - phi1_p) / 2;

              if (phi_p1 < 0 || phi_p2 < 0) continue; // illegal case

              // we then compute shock tangents of the two descriptors
              // psi_p is the tangent of the "umbrella" descriptor xdesc_p, which covers xdesc_p1 and xdesc_p2
              double psi_p1 = psi_p - phi0_p + phi_p1;  // algebra on the angles
              double psi_p2 = psi_p + phi0_p - phi_p2;
              
              // construct the descriptors at the parent node for the two child edges
              dbsksp_xshock_node_descriptor xd_p1(x_p, y_p, psi_p1, phi_p1, r_p);
              dbsksp_xshock_node_descriptor xd_p2(x_p, y_p, psi_p2, phi_p2, r_p);

              // optimize the child on each side separately

              // for child with degree==1, there is only possible state
              // for child with degree==2 or 3, we sample a whole bunch and only optimize within these

              //---------------------------------------------------------------------
              // optimize branch 1
              float min_cost1 = vnl_numeric_traits<float >::maxval;
              int min_cost1_idx = -1;

              
              ////
              //this->find_optimal_child_node_given_parent_state(child_edge1->id(), xd_p1, 
              //  xv_c1->degree(), geom_model1, num_samples_c1, grid_c1, opt_cost_c1, 
              //  min_cost1, min_cost1_idx, max_acceptable_subtree1_cost);

              // retrieve min cost from computed cost grid
              int i_x_p1, i_y_p1, i_psi_p1, i_phi0_p1, i_r_p1;
              if (!grid_p1.xdesc_to_grid(xd_p1, i_x_p1, i_y_p1, i_psi_p1, i_phi0_p1, i_r_p1))
                continue;
              
              int i_desc_p1 = grid_p1.cell_grid_to_linear(i_psi_p1, i_phi0_p1, i_r_p1);
              min_cost1     = opt_cost_p1 (i_x_p1, i_y_p1)[i_desc_p1];
              min_cost1_idx = opt_child_p1(i_x_p1, i_y_p1)[i_desc_p1];

              //---------------------------------------------------------------------

              // optimize branch 2
              float min_cost2 = vnl_numeric_traits<float >::maxval;
              int min_cost2_idx = -1;

              ////
              //this->find_optimal_child_node_given_parent_state(child_edge2->id(), xd_p2,
              //  xv_c2->degree(), geom_model2, num_samples_c2, grid_c2, opt_cost_c2,
              //  min_cost2, min_cost2_idx, max_acceptable_subtree_cost - min_cost1);

              // retrieve min cost from computed cost grid
              int i_x_p2, i_y_p2, i_psi_p2, i_phi0_p2, i_r_p2;
              if (! grid_p2.xdesc_to_grid(xd_p2, i_x_p2, i_y_p2, i_psi_p2, i_phi0_p2, i_r_p2))
                continue;
              int i_desc_p2 = grid_p2.cell_grid_to_linear(i_psi_p2, i_phi0_p2, i_r_p2);
              min_cost2     = opt_cost_p2 (i_x_p2, i_y_p2)[i_desc_p2];
              min_cost2_idx = opt_child_p2(i_x_p2, i_y_p2)[i_desc_p2];


              // now sum up the two min_cost's and compare with the current minimum
              if (min_cost > (min_cost1 + min_cost2))
              {
                min_cost = min_cost1 + min_cost2;
                min_cost_idx1 = min_cost1_idx;
                min_cost_idx2 = min_cost2_idx;
                min_cost_idx_phi1 = ip_phi1;
              }
            } // k6

            // record the optimal value
            opt_cost_p(ip_x, ip_y)[ip_desc] = min_cost;
            opt_child1(ip_x, ip_y)[ip_desc] = min_cost_idx1;
            opt_child2(ip_x, ip_y)[ip_desc] = min_cost_idx2;
            opt_phi1(ip_x, ip_y)[ip_desc] = min_cost_idx_phi1;
          } // k5
        } // k4
      } // k3
    } // k2
  } // k1

  // release memory of the optimal cost grids of child1 and child2 since
  // they will never be used again
  opt_cost_c1.clear();
  opt_cost_c2.clear();

  // update cost map
  float min_cost_p = this->compute_min_value(opt_cost_p);
  this->map_node_to_min_cost_.erase(xv_c1->id());
  this->map_node_to_min_cost_.erase(xv_c2->id());
  this->map_node_to_min_cost_[xv_p->id()] = min_cost_p;

  //// modify map of min cost matrix
  //this->map_node_to_min_cost_matrix_.erase(xv_c1->id());
  //this->map_node_to_min_cost_matrix_.erase(xv_c2->id());
  //this->compute_min_value_matrix(opt_cost_p, this->map_node_to_min_cost_matrix_[xv_p->id()]);

  return;
}




// -----------------------------------------------------------------------------
//: Optimize the root node (should be a degree-2 node)
// "root_vid" is id of the root node
// "child_eid" is id of the main edge (bigger branch)
bool dbsks_xshock_dp_new::
optimize_root_node(unsigned root_vid, unsigned major_child_eid)
{
  dbsksp_xshock_node_sptr xv_root = this->graph()->node_from_id(root_vid);
  if (!xv_root)
    return false;

  if (xv_root->degree() == 2)
  {
    return this->optimize_degree2_root_node_use_both_branches(root_vid, major_child_eid);

    //return this->optimize_degree2_root_node_ignore_minor_branch(root_vid, major_child_eid);
  }
  else if (xv_root->degree() == 3)
  {
    return this->optimize_degree3_root_node_use_all_branches(root_vid, major_child_eid);
  }
  
  return false;
}



// -----------------------------------------------------------------------------
//: Optimize the root node (should be a degree-2 node), considering the cost 
// of both branches
// "root_vid" is id of the root node, should have degree-2
// "major_child_eid" is id of the main edge (bigger branch)
bool dbsks_xshock_dp_new::
optimize_degree2_root_node_use_both_branches(unsigned root_vid, unsigned major_child_eid)
{
  // root node needs to be a degree-2 node
  dbsksp_xshock_node_sptr xv_root = this->graph()->node_from_id(root_vid);
  if (!xv_root || xv_root->degree() != 2) 
    return false;
  
  // the "major" edge
  dbsksp_xshock_edge_sptr xe1 = this->graph()->edge_from_id(major_child_eid);
  
  // check validity of the major edge
  if (!xe1 || !xe1->is_vertex(xv_root))
  {
    return false;
  }

  // the "minor" edge
  dbsksp_xshock_edge_sptr xe2 = this->graph()->cyclic_adj_succ(xe1, xv_root);
  
  // Optimizing the root node is similar to optimizing a degree-2 node, except that
  // the cost for each state of the root is the sum from "both sides" instead of just one side

  // number of samples of the child to check for each state of the parent
  int num_samples_c = this->num_samples_c_;

  // Child node of major edge
  dbsksp_xshock_node_sptr xv_c1 = xe1->opposite(xv_root);

  //: Child node of minor edge
  dbsksp_xshock_node_sptr xv_c2 = xe2->opposite(xv_root);

  //> Max acceptable costs for the subtree
  {
    unsigned a1 = (xv_c1->degree() == 1 ? 0 : 1);
    unsigned a2 = (xv_c2->degree() == 1 ? 0 : 1);
    assert(this->map_node_to_min_cost_.size() == a1+a2);
  }
  float max_acceptable_subtree1_cost = this->compute_max_acceptable_subtree_cost(xv_c1->id());

  // base descriptors for the child and parent nodes
  dbsksp_xshock_node_descriptor xdesc_p1 = *xv_root->descriptor(xe1);
  dbsksp_xshock_node_descriptor xdesc_c1 = xv_c1->descriptor(xe1)->opposite_xnode();
  
  dbsksp_xshock_node_descriptor xdesc_c2 = xv_c2->descriptor(xe2)->opposite_xnode();
  
  // Geometric model for the two fragments
  dbsks_xfrag_geom_model_sptr geom_model1 = this->xgraph_geom->map_edge2geom()[xe1->id()];
  geom_model1->set(xdesc_p1, xdesc_c1);

  dbsks_xfrag_geom_model_sptr geom_model2 = this->xgraph_geom->map_edge2geom()[xe2->id()];
  geom_model2->set(xdesc_p1.opposite_xnode(), xdesc_c2);

  
  // Grid of all possible states of the root and its child nodes

  // we only keep the grid for one side of root node (the major child edge). 
  // The other side can be computed directly from this side
  dbsks_xnode_grid& grid_p = (*this->map_xnode_grid)[xv_root->id()]; 

  // the two children have different grids
  dbsks_xnode_grid& grid_c1 = (*this->map_xnode_grid)[xv_c1->id()];
  dbsks_xnode_grid& grid_c2 = (*this->map_xnode_grid)[xv_c2->id()];

  // Retrieve the optimal cost of subtrees rooted at the child nodes
  grid2d_float& opt_cost_c1 = map_opt_cost[xv_c1->id()]; // already computed
  grid2d_float& opt_cost_c2 = map_opt_cost[xv_c2->id()]; // already computed

  //////////////////////////////////////////////////////////////////////////////
  // The goal of this function is to compute the following grids ///////////////
  
  // Optimal cost of the graph for each state of the root node
  grid2d_float& opt_cost_p = map_opt_cost[xv_root->id()]; // will compute
  
  // optimal state of the child nodes for each state of the root node
  grid2d_int& opt_child1 = this->map_opt_child_of_root[xv_c1->id()];
  grid2d_int& opt_child2 = this->map_opt_child_of_root[xv_c2->id()];
  //////////////////////////////////////////////////////////////////////////////

  // Allocate memory for the to-be-computed grids
  {
    int num_x = grid_p.x_.size();
    int num_y = grid_p.y_.size();
    int size_per_cell = grid_p.num_states_per_cell();
    
    this->allocate_grid2d(opt_cost_p, num_x, num_y, size_per_cell, vnl_numeric_traits<float >::maxval);
    this->allocate_grid2d(opt_child1, num_x, num_y, size_per_cell, -1);
    this->allocate_grid2d(opt_child2, num_x, num_y, size_per_cell, -1);
  }

  // grid of states for the parent node
  vcl_vector<double > x_vec, y_vec;
  vcl_vector<int > index_x_vec, index_y_vec;
  vcl_vector<double > psi_vec, phi0_vec, r_vec;
  vcl_vector<int > index_psi_vec, index_phi0_vec, index_r_vec;
  {
    // all x's
    int num_x_backward = grid_p.x_.size()/2;
    int num_x_forward = num_x_backward;
    grid_p.sample_x(grid_p.x_[grid_p.x_.size()/2], num_x_backward, num_x_forward, x_vec, index_x_vec);

    // all y's
    int num_y_backward = grid_p.y_.size()/2;
    int num_y_forward = num_y_backward;
    grid_p.sample_y(grid_p.y_[grid_p.y_.size()/2], num_y_backward, num_y_forward, y_vec, index_y_vec);

    // all psi's
    int num_psi_backward = grid_p.psi_.size()/2;
    int num_psi_forward  = num_psi_backward;
    grid_p.sample_psi(grid_p.psi_[grid_p.psi_.size()/2], num_psi_backward, num_psi_forward, psi_vec, index_psi_vec);

    // all phi0's
    int num_phi0_backward = grid_p.phi0_.size()/2;
    int num_phi0_forward = num_phi0_backward;
    grid_p.sample_phi0(grid_p.phi0_[grid_p.phi0_.size()/2], num_phi0_backward, num_phi0_forward, phi0_vec, index_phi0_vec);

    // all r's
    int num_r_backward = grid_p.r_.size()/2;
    int num_r_forward = num_r_backward;
    grid_p.sample_r(grid_p.r_[grid_p.r_.size()/2], num_r_backward, num_r_forward, r_vec, index_r_vec);
  }

  unsigned size_x_vec = x_vec.size();
  unsigned size_y_vec = y_vec.size();
  unsigned size_psi_vec = psi_vec.size();
  unsigned size_phi0_vec = phi0_vec.size();
  unsigned size_r_vec = r_vec.size();

  vcl_cout << "\n>> Optimizing degree-2 root node, vid= " << xv_root->id() << vcl_endl;
  vcl_cout << "  major_child_eid = " << this->major_child_eid_ << vcl_endl;
  vcl_cout << "  size x_vec = " << size_x_vec << vcl_endl;
  vcl_cout << "  size y_vec = " << size_y_vec << vcl_endl;
  vcl_cout << "  size psi_vec = " << size_psi_vec << vcl_endl;
  vcl_cout << "  size phi0_vec = " << size_phi0_vec << vcl_endl;
  vcl_cout << "  size r_vec = " << size_r_vec << vcl_endl;
    
  // THE BIG LOOP
  vcl_vector<dbsksp_xshock_node_descriptor > edesc_list;
  for (unsigned k1 =0; k1 < size_x_vec; ++k1)
  {
    int ip_x = index_x_vec[k1];
    for (unsigned k2 =0; k2 < size_y_vec; ++k2)
    {
      int ip_y = index_y_vec[k2];
      vnl_vector<float >& opt_cost = opt_cost_p(ip_x, ip_y);
      for (unsigned k3 =0; k3 < size_psi_vec; ++k3)
      {
        int ip_psi = index_psi_vec[k3];
        for (unsigned k4 =0; k4 < size_phi0_vec; ++k4)
        {
          int ip_phi0 = index_phi0_vec[k4];
          for (unsigned k5 =0; k5 < size_r_vec; ++k5)
          {
            int ip_r = index_r_vec[k5];
            int ip_desc = grid_p.cell_grid_to_linear(ip_psi, ip_phi0, ip_r);

            // descriptor at the parent node facing child 1
            dbsksp_xshock_node_descriptor xd_p1 = grid_p.xdesc(ip_x, ip_y, ip_psi, ip_phi0, ip_r);

            // descriptor at the parent node facing child 2: opposite of xd_p1
            dbsksp_xshock_node_descriptor xd_p2 = xd_p1.opposite_xnode();

            float min_cost1 = vnl_numeric_traits<float >::maxval;
            int min_cost1_idx = -1;
            this->find_optimal_child_node_given_parent_state(xe1->id(), xd_p1,
              xv_c1->degree(), geom_model1, this->num_samples_c_, grid_c1, opt_cost_c1,
              min_cost1, min_cost1_idx, max_acceptable_subtree1_cost);

            float min_cost2 = vnl_numeric_traits<float >::maxval;
            int min_cost2_idx = -1;
            this->find_optimal_child_node_given_parent_state(xe2->id(), xd_p2, 
              xv_c2->degree(), geom_model2, this->num_samples_c_, grid_c2, opt_cost_c2,
              min_cost2, min_cost2_idx, this->max_acceptable_xgraph_cost_ - min_cost1);

            // optimal cost at the root is sum of optimal cost of two branches
            float min_cost = min_cost1 + min_cost2;
            int min_cost_idx1 = min_cost1_idx;
            int min_cost_idx2 = min_cost2_idx;
            
            // Record the optimal results
            opt_cost_p(ip_x, ip_y)[ip_desc] = min_cost;
            opt_child1(ip_x, ip_y)[ip_desc] = min_cost_idx1;
            opt_child2(ip_x, ip_y)[ip_desc] = min_cost_idx2;
          } // k5
        } // k4
      } // k3
    } // k2
  } // k1

  // release memory for the optimal cost grids of the child nodes
  opt_cost_c1.clear();
  opt_cost_c2.clear();

  // modify the cost map
  float min_cost_p = this->compute_min_value(opt_cost_p);
  
  vcl_cout << "\nMin cost for the whole graph (xv_root= "<< xv_root->id() << ") is: " << min_cost_p << "\n";

  this->map_node_to_min_cost_.erase(xv_c1->id());
  this->map_node_to_min_cost_.erase(xv_c2->id());
  this->map_node_to_min_cost_[xv_root->id()] = min_cost_p;
  
  //// modify map of cost matrix
  //this->map_node_to_min_cost_matrix_.erase(xv_c1->id());
  //this->map_node_to_min_cost_matrix_.erase(xv_c2->id());
  //this->compute_min_value_matrix(opt_cost_p, this->map_node_to_min_cost_matrix_[xv_root->id()]);

  return true;
}

// -----------------------------------------------------------------------------
//: Optimize the root node (should be a degree-2 node), considering the cost 
// of both branches
// "root_vid" is id of the root node, should have degree-2
// "major_child_eid" is id of the main edge (bigger branch)

// modify the scheme by sampling from child node and update parent node states. combine two sets of opt_cost_p for output

bool dbsks_xshock_dp_new::
optimize_degree2_root_node_use_both_branches2(unsigned root_vid, unsigned major_child_eid)
{
  // root node needs to be a degree-2 node
  dbsksp_xshock_node_sptr xv_root = this->graph()->node_from_id(root_vid);
  if (!xv_root || xv_root->degree() != 2) 
    return false;
  
  // the "major" edge
  dbsksp_xshock_edge_sptr xe1 = this->graph()->edge_from_id(major_child_eid);
  
  // check validity of the major edge
  if (!xe1 || !xe1->is_vertex(xv_root))
  {
    return false;
  }

  // the "minor" edge
  dbsksp_xshock_edge_sptr xe2 = this->graph()->cyclic_adj_succ(xe1, xv_root);
  
  miner_child_eid = xe2->id();
  // Optimizing the root node is similar to optimizing a degree-2 node, except that
  // the cost for each state of the root is the sum from "both sides" instead of just one side

  // number of samples of the child to check for each state of the parent
  int num_samples_c = this->num_samples_c_;

  // Child node of major edge
  dbsksp_xshock_node_sptr xv_c1 = xe1->opposite(xv_root);

  //: Child node of minor edge
  dbsksp_xshock_node_sptr xv_c2 = xe2->opposite(xv_root);

  //> Max acceptable costs for the subtree
  {
    unsigned a1 = (xv_c1->degree() == 1 ? 0 : 1);
    unsigned a2 = (xv_c2->degree() == 1 ? 0 : 1);
    assert(this->map_node_to_min_cost_.size() == a1+a2);
  }
  float max_acceptable_subtree1_cost = this->compute_max_acceptable_subtree_cost(xv_c1->id());
  float max_acceptable_subtree2_cost = this->compute_max_acceptable_subtree_cost(xv_c2->id());

/*
  // base descriptors for the child and parent nodes
  dbsksp_xshock_node_descriptor xdesc_p1 = *xv_root->descriptor(xe1);
  dbsksp_xshock_node_descriptor xdesc_c1 = xv_c1->descriptor(xe1)->opposite_xnode();
  
  dbsksp_xshock_node_descriptor xdesc_c2 = xv_c2->descriptor(xe2)->opposite_xnode();
  
  // Geometric model for the two fragments
  dbsks_xfrag_geom_model_sptr geom_model1 = this->xgraph_geom->map_edge2geom()[xe1->id()];
  geom_model1->set(xdesc_p1, xdesc_c1);

  dbsks_xfrag_geom_model_sptr geom_model2 = this->xgraph_geom->map_edge2geom()[xe2->id()];
  geom_model2->set(xdesc_p1.opposite_xnode(), xdesc_c2);
*/
  
  // Grid of all possible states of the root and its child nodes

  // we only keep the grid for one side of root node (the major child edge). 
  // The other side can be computed directly from this side
  dbsks_xnode_grid& grid_p = (*this->map_xnode_grid)[xv_root->id()]; 
/*
  // the two children have different grids
  dbsks_xnode_grid& grid_c1 = (*this->map_xnode_grid)[xv_c1->id()];
  dbsks_xnode_grid& grid_c2 = (*this->map_xnode_grid)[xv_c2->id()];

  // Retrieve the optimal cost of subtrees rooted at the child nodes
  grid2d_float& opt_cost_c1 = map_opt_cost[xv_c1->id()]; // already computed
  grid2d_float& opt_cost_c2 = map_opt_cost[xv_c2->id()]; // already computed

*/
  //////////////////////////////////////////////////////////////////////////////
  // The goal of this function is to compute the following grids ///////////////
  
  // Optimal cost of the graph for each state of the root node
  grid2d_float& opt_cost_p = map_opt_cost[xv_root->id()]; // will compute

  // should not be direct referred. Need new grids to save the costs and update opt_cost_p by combining those
  grid2d_float opt_cost_p1 = map_opt_cost[xv_root->id()];
  grid2d_float opt_cost_p2 = map_opt_cost[xv_root->id()];

  // optimal state of the child nodes for each state of the root node
  grid2d_int& opt_child1 = this->map_opt_child_of_root[xv_c1->id()];
  grid2d_int& opt_child2 = this->map_opt_child_of_root[xv_c2->id()];
  //////////////////////////////////////////////////////////////////////////////

  // Allocate memory for the to-be-computed grids
  {
    int num_x = grid_p.x_.size();
    int num_y = grid_p.y_.size();
    int size_per_cell = grid_p.num_states_per_cell();
    
    this->allocate_grid2d(opt_cost_p, num_x, num_y, size_per_cell, vnl_numeric_traits<float >::maxval);
    this->allocate_grid2d(opt_cost_p1, num_x, num_y, size_per_cell, vnl_numeric_traits<float >::maxval);
    this->allocate_grid2d(opt_cost_p2, num_x, num_y, size_per_cell, vnl_numeric_traits<float >::maxval);
    this->allocate_grid2d(opt_child1, num_x, num_y, size_per_cell, -1);
    this->allocate_grid2d(opt_child2, num_x, num_y, size_per_cell, -1);
  }

  vcl_cout << "\n>> Optimizing degree-2 root node, vid= " << xv_root->id() << vcl_endl;
  vcl_cout << "  major_child_eid = " << this->major_child_eid_ << vcl_endl;


  // optimize too cost matrix opt_cost_p1, and opt_cost_p2. Main functions of optimization
  vcl_cout << "\n>> Optimizing for child node 1" << vcl_endl;
  optimize_child_node_given_parent_node2(xe1, grid_p, max_acceptable_subtree1_cost, opt_cost_p1, opt_child1);
  vcl_cout << ">> min cost from branch 1: " << this->compute_min_value(opt_cost_p1) << vcl_endl;

  vcl_cout << "\n>> Optimizing for child node 2" << vcl_endl;
  optimize_child_node_given_parent_node2(xe2, grid_p, max_acceptable_subtree2_cost, opt_cost_p2, opt_child2);
  vcl_cout << ">> min cost from branch 2: " << this->compute_min_value(opt_cost_p2) << vcl_endl;

  // just do the operation of : opt_cost_p = opt_cost_p1 + opt_cost_p2;
  // grid of states for the parent node
  vcl_vector<double > x_vec, y_vec;
  vcl_vector<int > index_x_vec, index_y_vec;
  vcl_vector<double > psi_vec, phi0_vec, r_vec;
  vcl_vector<int > index_psi_vec, index_phi0_vec, index_r_vec;
  {
    // all x's
    int num_x_backward = grid_p.x_.size()/2;
    int num_x_forward = num_x_backward;
    grid_p.sample_x(grid_p.x_[grid_p.x_.size()/2], num_x_backward, num_x_forward, x_vec, index_x_vec);

    // all y's
    int num_y_backward = grid_p.y_.size()/2;
    int num_y_forward = num_y_backward;
    grid_p.sample_y(grid_p.y_[grid_p.y_.size()/2], num_y_backward, num_y_forward, y_vec, index_y_vec);

    // all psi's
    int num_psi_backward = grid_p.psi_.size()/2;
    int num_psi_forward  = num_psi_backward;
    grid_p.sample_psi(grid_p.psi_[grid_p.psi_.size()/2], num_psi_backward, num_psi_forward, psi_vec, index_psi_vec);

    // all phi0's
    int num_phi0_backward = grid_p.phi0_.size()/2;
    int num_phi0_forward = num_phi0_backward;
    grid_p.sample_phi0(grid_p.phi0_[grid_p.phi0_.size()/2], num_phi0_backward, num_phi0_forward, phi0_vec, index_phi0_vec);

    // all r's
    int num_r_backward = grid_p.r_.size()/2;
    int num_r_forward = num_r_backward;
    grid_p.sample_r(grid_p.r_[grid_p.r_.size()/2], num_r_backward, num_r_forward, r_vec, index_r_vec);
  }

  unsigned size_x_vec = x_vec.size();
  unsigned size_y_vec = y_vec.size();
  unsigned size_psi_vec = psi_vec.size();
  unsigned size_phi0_vec = phi0_vec.size();
  unsigned size_r_vec = r_vec.size();

/*
  vcl_cout << "  size x_vec = " << size_x_vec << vcl_endl;
  vcl_cout << "  size y_vec = " << size_y_vec << vcl_endl;
  vcl_cout << "  size psi_vec = " << size_psi_vec << vcl_endl;
  vcl_cout << "  size phi0_vec = " << size_phi0_vec << vcl_endl;
  vcl_cout << "  size r_vec = " << size_r_vec << vcl_endl;
*/
  // THE BIG LOOP
  vcl_vector<dbsksp_xshock_node_descriptor > edesc_list;
  for (unsigned k1 =0; k1 < size_x_vec; ++k1)
  {
    int ip_x = index_x_vec[k1];
    for (unsigned k2 =0; k2 < size_y_vec; ++k2)
    {
      int ip_y = index_y_vec[k2];
      vnl_vector<float >& opt_cost = opt_cost_p(ip_x, ip_y);
      for (unsigned k3 =0; k3 < size_psi_vec; ++k3)
      {
        int ip_psi = index_psi_vec[k3];
        for (unsigned k4 =0; k4 < size_phi0_vec; ++k4)
        {
          int ip_phi0 = index_phi0_vec[k4];
          for (unsigned k5 =0; k5 < size_r_vec; ++k5)
          {
            int ip_r = index_r_vec[k5];
            int ip_desc = grid_p.cell_grid_to_linear(ip_psi, ip_phi0, ip_r);            
            // Record the optimal results

			//if(opt_cost_p1(ip_x, ip_y)[ip_desc] > this->max_acceptable_xgraph_cost_ || opt_cost_p2(ip_x, ip_y)[ip_desc] > this->max_acceptable_xgraph_cost_)
				//continue;

            opt_cost_p(ip_x, ip_y)[ip_desc] = opt_cost_p1(ip_x, ip_y)[ip_desc] + opt_cost_p2(ip_x, ip_y)[ip_desc];
          } // k5
        } // k4
      } // k3
    } // k2
  } // k1
    
  // release memory for the optimal cost grids of the child nodes
  //opt_cost_c1.clear();
  //opt_cost_c2.clear();

  // modify the cost map
  float min_cost_p = this->compute_min_value(opt_cost_p);
  
  vcl_cout << "\nMin cost for the whole graph (xv_root= "<< xv_root->id() << ") is: " << min_cost_p << "\n";

  this->map_node_to_min_cost_.erase(xv_c1->id());
  this->map_node_to_min_cost_.erase(xv_c2->id());
  this->map_node_to_min_cost_[xv_root->id()] = min_cost_p;
  
  //// modify map of cost matrix
  //this->map_node_to_min_cost_matrix_.erase(xv_c1->id());
  //this->map_node_to_min_cost_matrix_.erase(xv_c2->id());
  //this->compute_min_value_matrix(opt_cost_p, this->map_node_to_min_cost_matrix_[xv_root->id()]);

  return true;
}


//------------------------------------------------------------------------------
//: Optimize the degree-3 root node using all three branches
// "root_vid" is id of the root node, should have degree-3
// "major_child_eid" is id of the first child
// "pseudo_parent" edge  = predecessor of major_child
// "minor_child" edge = successor of major_child
bool dbsks_xshock_dp_new::
optimize_degree3_root_node_use_all_branches(unsigned root_vid, unsigned major_child_eid)
{
  // root node needs to be a degree-3 node
  dbsksp_xshock_node_sptr xv_root = this->graph()->node_from_id(root_vid);
  if (!xv_root || xv_root->degree() != 3) 
    return false;
  
  // the "major" child edge
  dbsksp_xshock_edge_sptr child_edge1 = this->graph()->edge_from_id(major_child_eid);
  
  // check validity of the major edge
  if (!child_edge1 || !child_edge1->is_vertex(xv_root))
  {
    return false;
  }

  // the "minor" child edge
  dbsksp_xshock_edge_sptr child_edge2 = this->graph()->cyclic_adj_succ(child_edge1, xv_root);

  // the pseudo parent edge
  dbsksp_xshock_edge_sptr child_edge0 = this->graph()->cyclic_adj_pred(child_edge1, xv_root);
  
  // Optimizing the root node is similar to optimizing a degree-2 node, except that
  // the cost for each state of the root is the sum from "both sides" instead of just one side

  // Child node of major child edge
  dbsksp_xshock_node_sptr xv_c1 = child_edge1->opposite(xv_root);

  //: Child node of minor child edge
  dbsksp_xshock_node_sptr xv_c2 = child_edge2->opposite(xv_root);

  // Child node of pseudo parent edge
  dbsksp_xshock_node_sptr xv_c0 = child_edge0->opposite(xv_root);


  //> Compute max acceptable costs for each subtree
  float max_acceptable_subtree0_cost = this->compute_max_acceptable_subtree_cost(xv_c0->id());
  float max_acceptable_subtree1_cost = this->compute_max_acceptable_subtree_cost(xv_c1->id());
  float max_acceptable_subtree2_cost = this->compute_max_acceptable_subtree_cost(xv_c2->id());

  // base descriptors for the child and parent nodes
  dbsksp_xshock_node_descriptor xdesc_p1 = *xv_root->descriptor(child_edge1);
  dbsksp_xshock_node_descriptor xdesc_c1 = xv_c1->descriptor(child_edge1)->opposite_xnode();
  
  dbsksp_xshock_node_descriptor xdesc_p2 = *xv_root->descriptor(child_edge2);
  dbsksp_xshock_node_descriptor xdesc_c2 = xv_c2->descriptor(child_edge2)->opposite_xnode();

  dbsksp_xshock_node_descriptor xdesc_p0 = *xv_root->descriptor(child_edge0);
  dbsksp_xshock_node_descriptor xdesc_c0 = xv_c0->descriptor(child_edge0)->opposite_xnode();
  
  // Geometric model for the 3 fragments
  dbsks_xfrag_geom_model_sptr geom_model1 = this->xgraph_geom->map_edge2geom()[child_edge1->id()];
  geom_model1->set(xdesc_p1, xdesc_c1);

  dbsks_xfrag_geom_model_sptr geom_model2 = this->xgraph_geom->map_edge2geom()[child_edge2->id()];
  geom_model2->set(xdesc_p2.opposite_xnode(), xdesc_c2);

  dbsks_xfrag_geom_model_sptr geom_model0 = this->xgraph_geom->map_edge2geom()[child_edge0->id()];
  geom_model0->set(xdesc_p0, xdesc_c0);

  
  // Grid of all possible states of the root and its child nodes

  // we only keep the grid for one side of root node (the major child edge). 
  // The other side can be computed directly from this side
  dbsks_xnode_grid& grid_root = (*this->map_xnode_grid)[xv_root->id()]; 

  // the two children have different grids
  dbsks_xnode_grid& grid_c1 = (*this->map_xnode_grid)[xv_c1->id()];
  dbsks_xnode_grid& grid_c2 = (*this->map_xnode_grid)[xv_c2->id()];
  dbsks_xnode_grid& grid_c0 = (*this->map_xnode_grid)[xv_c0->id()];

  // Retrieve the optimal cost of subtrees rooted at the child nodes
  grid2d_float& opt_cost_c1 = map_opt_cost[xv_c1->id()]; // already computed
  grid2d_float& opt_cost_c2 = map_opt_cost[xv_c2->id()]; // already computed
  grid2d_float& opt_cost_c0 = map_opt_cost[xv_c0->id()]; // already computed

  //////////////////////////////////////////////////////////////////////////////
  // The goal of this function is to compute the following grids ///////////////
  
  // Optimal cost of the graph for each state of the root node
  grid2d_float& opt_cost_p = map_opt_cost[xv_root->id()]; // will compute
  
  // optimal state of the child nodes for each state of the root node
  grid2d_int& opt_child1 = this->map_opt_child_of_root[xv_c1->id()]; // will compute
  grid2d_int& opt_child2 = this->map_opt_child_of_root[xv_c2->id()]; // will compute
  grid2d_int& opt_child0 = this->map_opt_child_of_root[xv_c0->id()]; // will compute


  //  \todo double-check this
  grid2d_int& opt_phi1 = this->map_opt_phi1[xv_root->id()];
  ////////////////////////////////////////////////////////////////////////////////



  // Optimize each branch separately first

  // construct a grid for the parent node wrt child edge 1
  dbsks_xnode_grid grid_p1;
  {
    double angle_padding_ratio = 1.5;

    double min_psi_start, max_psi_start, min_phi_start, max_phi_start;
    geom_model1->get_range_psi_start(min_psi_start, max_psi_start);
    geom_model1->get_range_phi_start(min_phi_start, max_phi_start);

    double range_psi = angle_padding_ratio * (max_psi_start - min_psi_start);
    double mean_psi = (max_psi_start+min_psi_start) / 2;

    double range_phi0 = angle_padding_ratio * (max_phi_start - min_phi_start);
    double mean_phi0 = (max_phi_start + min_phi_start) / 2;

    grid_p1 = grid_root;
    grid_p1.phi1_.clear();
    grid_p1.set_psi_vec(mean_psi-range_psi/2, mean_psi+range_psi/2, grid_root.step_psi_);
    grid_p1.set_phi0_vec(mean_phi0 - range_phi0/2, mean_phi0 + range_phi0/2, grid_root.step_phi0_);
  }


  // Optimize child edge 1
  grid2d_float opt_cost_p1; 
  grid2d_int   opt_child_p1;
  this->optimize_child_node_given_parent_node(child_edge1, grid_p1, max_acceptable_subtree1_cost, opt_cost_p1, opt_child_p1);
  {
    // Compute min cost of the subtree
    float min_cost_p1 = this->compute_min_value(opt_cost_p1);
    vcl_cout << "\nMin cost for for subtree (xv= "<< xv_root->id() << ", xe= " << child_edge1->id() << ") is: " << min_cost_p1 << "\n";
  }



  // construct a grid for the parent node wrt child edge 2
  dbsks_xnode_grid grid_p2;
  {
    double angle_padding_ratio = 1.5;

    double min_psi_start, max_psi_start, min_phi_start, max_phi_start;
    geom_model2->get_range_psi_start(min_psi_start, max_psi_start);
    geom_model2->get_range_phi_start(min_phi_start, max_phi_start);

    double range_psi = angle_padding_ratio * (max_psi_start - min_psi_start);
    double mean_psi = (max_psi_start+min_psi_start) / 2;

    double range_phi0 = angle_padding_ratio * (max_phi_start - min_phi_start);
    double mean_phi0 = (max_phi_start + min_phi_start) / 2;

    grid_p2 = grid_root;
    grid_p2.phi1_.clear();
    grid_p2.set_psi_vec(mean_psi-range_psi/2, mean_psi+range_psi/2, grid_root.step_psi_);
    grid_p2.set_phi0_vec(mean_phi0 - range_phi0/2, mean_phi0 + range_phi0/2, grid_root.step_phi0_);
  }

  // Optimize child edge 2
  grid2d_float opt_cost_p2;
  grid2d_int opt_child_p2;
  this->optimize_child_node_given_parent_node(child_edge2, grid_p2, max_acceptable_subtree2_cost, opt_cost_p2, opt_child_p2);
  {
    // Compute min cost of the subtree
    float min_cost_p2 = this->compute_min_value(opt_cost_p2);
    vcl_cout << "\nMin cost for for subtree (xv= "<< xv_root->id() << ", xe= " << child_edge2->id() << ") is: " << min_cost_p2 << "\n";
  }




  //////////////////////////////////////////////////////////////////////////////
  // Number of child states to check for each state of the parent node 
  int num_samples_c = this->num_samples_c_;
  ////////////////////////////////////////////////////////////////////////////// 

  // Allocate memory for the to-be-computed grids
  {
    int num_x = grid_root.x_.size();
    int num_y = grid_root.y_.size();
    int size_per_cell = grid_root.num_states_per_cell();
    
    this->allocate_grid2d(opt_cost_p, num_x, num_y, size_per_cell, vnl_numeric_traits<float >::maxval);
    this->allocate_grid2d(opt_phi1, num_x, num_y, size_per_cell, -1);
    this->allocate_grid2d(opt_child1, num_x, num_y, size_per_cell, -1);
    this->allocate_grid2d(opt_child2, num_x, num_y, size_per_cell, -1);
    this->allocate_grid2d(opt_child0, num_x, num_y, size_per_cell, -1);
  }


  // index vectors
  vcl_vector<int > index_x_vec, index_y_vec, index_psi_vec, index_phi0_vec, index_r_vec, index_phi1_vec;
  vcl_vector<double > x_vec, y_vec, psi_vec, phi0_vec, r_vec, phi1_vec;
  {
    // we cover every point spatially
    int num_x_backward = grid_root.x_.size()/2;
    int num_x_forward = grid_root.x_.size()/2;
    grid_root.sample_x(grid_root.x_[grid_root.x_.size()/2], num_x_backward, num_x_forward, x_vec, index_x_vec); // everything
    
    int num_y_backward = grid_root.y_.size()/2;
    int num_y_forward = grid_root.y_.size()/2;
    grid_root.sample_y(grid_root.y_[grid_root.y_.size()/2], num_y_backward, num_y_forward, y_vec, index_y_vec); // everything

    int num_psi_backward = grid_root.psi_.size() / 2;
    int num_psi_forward = grid_root.psi_.size() / 2;
    grid_root.sample_psi(grid_root.psi_[grid_root.psi_.size()/2], num_psi_backward, num_psi_forward, psi_vec, index_psi_vec); // everything

    int num_phi0_backward = grid_root.phi0_.size() / 2;
    int num_phi0_forward = grid_root.phi0_.size() / 2;
    grid_root.sample_phi0(grid_root.phi0_[grid_root.phi0_.size()/2], num_phi0_backward, num_phi0_forward, phi0_vec, index_phi0_vec); // everything

    int num_r_backward = grid_root.r_.size()/2;
    int num_r_forward = grid_root.r_.size()/2;
    grid_root.sample_r(grid_root.r_[grid_root.r_.size()/2], num_r_backward, num_r_forward, r_vec, index_r_vec); // everything

    int num_phi1_backward = grid_root.phi1_.size() / 2;
    int num_phi1_forward = grid_root.phi1_.size() / 2;
    grid_root.sample_phi1(grid_root.phi1_[grid_root.phi1_.size()/2], num_phi1_backward, num_phi1_forward, phi1_vec, index_phi1_vec); // everything
  }
  

  // THE BIG LOOP
  unsigned size_x_vec = index_x_vec.size();
  unsigned size_y_vec = index_y_vec.size();
  unsigned size_psi_vec = index_psi_vec.size();
  unsigned size_phi0_vec = index_phi0_vec.size();
  unsigned size_r_vec = index_r_vec.size();
  unsigned size_phi1_vec = index_phi1_vec.size();

  for (unsigned k1 =0; k1 < size_x_vec; ++k1)
  {
    int ip_x = index_x_vec[k1];
    double x_p = x_vec[k1];
    for (unsigned k2 =0; k2 < size_y_vec; ++k2)
    {
      int ip_y = index_y_vec[k2];
      double y_p = y_vec[k2];
      
      vnl_vector<float >& opt_cost = opt_cost_p(ip_x, ip_y);
      for (unsigned k3 =0; k3 < size_psi_vec; ++k3)
      {
        int ip_psi = index_psi_vec[k3];
        double psi_p = psi_vec[k3];
        for (unsigned k4 =0; k4 < size_phi0_vec; ++k4)
        {
          int ip_phi0 = index_phi0_vec[k4];
          double phi0_p = phi0_vec[k4];
          for (unsigned k5 =0; k5 < size_r_vec; ++k5)
          {
            int ip_r = index_r_vec[k5];
            double r_p = r_vec[k5];

            // descriptor index at the parent node
            int ip_desc = grid_root.cell_grid_to_linear(ip_psi, ip_phi0, ip_r);

            // branch c0
            float min_cost0 = vnl_numeric_traits<float >::maxval;
            int min_cost0_idx = -1;
            {
              // descriptor at the parent node coverting both c1 and c2
              dbsksp_xshock_node_descriptor xd_p12 = grid_root.xdesc(ip_x, ip_y, ip_psi, ip_phi0, ip_r);

              // descriptor at the root node facing c0 (pseudo_parent): opposite of xd_p12
              dbsksp_xshock_node_descriptor xd_p0 = xd_p12.opposite_xnode();

              this->find_optimal_child_node_given_parent_state(child_edge0->id(), xd_p0, 
                xv_c0->degree(), geom_model0, this->num_samples_c_, grid_c0, opt_cost_c0,
                min_cost0, min_cost0_idx, max_acceptable_subtree0_cost);
            }
            

            // branch c1+c2
            float min_cost12 = vnl_numeric_traits<float >::maxval;
            int min_cost12_idx1 = -1;
            int min_cost12_idx2 = -1;
            int min_cost12_idx_phi1 = -1;

            // iterate thru phi1 and find the minimum cost
            for (unsigned k6 =0; k6 < size_phi1_vec; ++k6)
            {
              int ip_phi1 = index_phi1_vec[k6];
              double phi1_p = phi1_vec[k6];

              // we now step by step construct xdesc_p1 and xdesc_p2, which are
              // node descriptors at the root node xv_p for the child edges c1 and c2

              // first we compute the angle phi's for each descriptor
              // phi1 is the difference between the phi angles of xdesc_p1 and xdesc_p2
              // phi0 is the sum of the phi angles of xdesc_p1 and xdesc_p2
              double phi_p1 = (phi0_p + phi1_p) / 2;
              double phi_p2 = (phi0_p - phi1_p) / 2;

              if (phi_p1 < 0 || phi_p2 < 0) continue; // illegal case

              // we then compute shock tangents of the two descriptors
              // psi_p is the tangent of the "umbrella" descriptor xdesc_p, which covers xdesc_p1 and xdesc_p2
              double psi_p1 = psi_p - phi0_p + phi_p1;  // algebra on the angles
              double psi_p2 = psi_p + phi0_p - phi_p2;
              
              // construct the descriptors at the parent node for the two child edges
              dbsksp_xshock_node_descriptor xd_p1(x_p, y_p, psi_p1, phi_p1, r_p);
              dbsksp_xshock_node_descriptor xd_p2(x_p, y_p, psi_p2, phi_p2, r_p);

              // optimize the child on each side separately
              // optimize branch 1
              float min_cost1 = vnl_numeric_traits<float >::maxval;
              int min_cost1_idx = -1;

              ////
              //this->find_optimal_child_node_given_parent_state(child_edge1->id(), xd_p1, 
              //  xv_c1->degree(), geom_model1, num_samples_c1, grid_c1, opt_cost_c1, 
              //  min_cost1, min_cost1_idx, loose_max_acceptable_subtree1_cost-min_cost0);

              // retrieve min cost from computed cost grid
              int i_x_p1, i_y_p1, i_psi_p1, i_phi0_p1, i_r_p1;
              if (!grid_p1.xdesc_to_grid(xd_p1, i_x_p1, i_y_p1, i_psi_p1, i_phi0_p1, i_r_p1))
                continue;
              
              int i_desc_p1 = grid_p1.cell_grid_to_linear(i_psi_p1, i_phi0_p1, i_r_p1);
              min_cost1     = opt_cost_p1 (i_x_p1, i_y_p1)[i_desc_p1];
              min_cost1_idx = opt_child_p1(i_x_p1, i_y_p1)[i_desc_p1];


              //---------------------------------------------------------------------
              // optimize branch 2
              float min_cost2 = vnl_numeric_traits<float >::maxval;
              int min_cost2_idx = -1;

              ////
              //this->find_optimal_child_node_given_parent_state(child_edge2->id(), xd_p2,
              //  xv_c2->degree(), geom_model2, num_samples_c2, grid_c2, opt_cost_c2,
              //  min_cost2, min_cost2_idx, this->max_acceptable_xgraph_cost_-min_cost0-min_cost1);

              // retrieve min cost from computed cost grid
              int i_x_p2, i_y_p2, i_psi_p2, i_phi0_p2, i_r_p2;
              if (! grid_p2.xdesc_to_grid(xd_p2, i_x_p2, i_y_p2, i_psi_p2, i_phi0_p2, i_r_p2))
                continue;
              int i_desc_p2 = grid_p2.cell_grid_to_linear(i_psi_p2, i_phi0_p2, i_r_p2);
              min_cost2     = opt_cost_p2 (i_x_p2, i_y_p2)[i_desc_p2];
              min_cost2_idx = opt_child_p2(i_x_p2, i_y_p2)[i_desc_p2];

              // now sum up the two min_cost's and compare with the current minimum
              if (min_cost12 > (min_cost1 + min_cost2))
              {
                min_cost12 = min_cost1 + min_cost2;
                min_cost12_idx1 = min_cost1_idx;
                min_cost12_idx2 = min_cost2_idx;
                min_cost12_idx_phi1 = ip_phi1;
              }
            } // k6

            // record the optimal value
            opt_cost_p(ip_x, ip_y)[ip_desc] = min_cost0 + min_cost12;
            opt_child1(ip_x, ip_y)[ip_desc] = min_cost12_idx1;
            opt_child2(ip_x, ip_y)[ip_desc] = min_cost12_idx2;
            opt_child0(ip_x, ip_y)[ip_desc] = min_cost0_idx;
            opt_phi1(ip_x, ip_y)[ip_desc] = min_cost12_idx_phi1;
          } // k5
        } // k4
      } // k3
    } // k2
  } // k1

  // release memory of the optimal cost grids of child1 and child2 since
  // they will never be used again
  opt_cost_c1.clear();
  opt_cost_c2.clear();
  opt_cost_c0.clear();


  // update the cost map
  float min_cost_p = this->compute_min_value(opt_cost_p);
  this->map_node_to_min_cost_.erase(xv_c0->id());
  this->map_node_to_min_cost_.erase(xv_c1->id());
  this->map_node_to_min_cost_.erase(xv_c2->id());
  this->map_node_to_min_cost_[xv_root->id()] = min_cost_p;

  //// modify map of min cost matrix
  //this->map_node_to_min_cost_matrix_.erase(xv_c0->id());
  //this->map_node_to_min_cost_matrix_.erase(xv_c1->id());
  //this->map_node_to_min_cost_matrix_.erase(xv_c2->id());
  //this->compute_min_value_matrix(opt_cost_p, this->map_node_to_min_cost_matrix_[xv_root->id()]);

  return true;
}






bool dbsks_xshock_dp_new::
update_parent_node_cost_given_child_state(unsigned edge_id,
                                            const dbsksp_xshock_node_descriptor& xd_c,
                                            int degree_child_node,
                                            const dbsks_xfrag_geom_model_sptr& geom_model,
                                            int num_samples_c,
                                            const dbsks_xnode_grid& grid_p,
                                            float subtree_cost,
											int ic_idx,
                                            float max_acceptable_cost,
                                            grid2d_float& opt_cost_p,
											grid2d_int& opt_child)
{
  //>> place holder to store sample states for the parent node
  static vcl_vector<dbsksp_xshock_node_descriptor > edesc_list;

  // Strategy:
  // for child with degree==1, there is only 1 possible state. this case is dealt in 'find_optimal_child_node_given_parent_state'
  // for child with degree==2 or 3, we sample parent given the child state and update the reachable parent state 


  if (degree_child_node == 1) // this should not be useful right now. 
  {
	vcl_cout << "unexpected use of this function to update parent node states\n";
	return true;
    // for a terminal fragment, the only difference between the parent
    // xdesc and the child xdesc is the angle phi.
    dbsksp_xshock_node_descriptor xd_p = xd_c;
    xd_p.phi_ = 0;

    // form a fragment from which we will compute image cost (and deformation cost)
    dbsksp_xshock_fragment xfrag(xd_p, xd_c);

    // image cost
    //float fragment_cost = float(-this->xshock_likelihood()->loglike(edge_id, xfrag));
    float fragment_cost = this->fragment_cost(edge_id, xfrag);

  }
  //else if (geom_model->check_start_width_constraint(xd_p, this->graph_size()))
  else 
  {

    // sample more than necessary, but only check the first num_samples_c that satisfy constraints
    geom_model->sample_start_given_end_using_model_minmax_range(xd_c, this->graph_size(), num_samples_c*5, edesc_list);
    //geom_model->sample_new_legal_end_given_start_using_model_minmax_range(xd_p, this->graph_size(), num_samples_c*5, edesc_list);

    unsigned size_edesc_list = edesc_list.size();
    int count_legal_samples = 0; //> counter for the number of child xdesc that satisfy constraints

	if(edesc_list.size()==0) // is given child state can not sample parent, just do not update and return
	{
 		return true;
	}

    for (unsigned i_p =0; count_legal_samples < num_samples_c && i_p < size_edesc_list; ++i_p)
    {
	  
	  // consider the special case the parent node is of miner edge from root
	  if (edge_id == miner_child_eid)
	  {
		  dbsksp_xshock_node_descriptor xd_p = edesc_list[i_p].opposite_xnode();
	 
		  // get the grid index of this node descriptor instance
		  int ip_x, ip_y, ip_psi, ip_phi0, ip_r;
		  if (!grid_p.xdesc_to_grid(xd_p, ip_x, ip_y, ip_psi, ip_phi0, ip_r))
		    continue;
		  //vcl_cout << "xdesc to grid" << vcl_endl;



		  // if the subtree cost is already greater the max-acceptable cost then
		  // this branch will never make it to the final detection list
		  if (subtree_cost >= max_acceptable_cost)
		    continue;


		  int ip_desc = grid_p.cell_grid_to_linear(ip_psi, ip_phi0, ip_r);
		  /////////////////////////////////
		  xd_p = grid_p.xdesc(ip_x, ip_y, ip_psi, ip_phi0, ip_r);
		  ///////////////////////////////////

		  // form a fragment from which we compute geometric and image cost
		  dbsksp_xshock_fragment xfrag(xd_p.opposite_xnode(), xd_c);

		  //////
		
		  if (!geom_model->check_nkdiff_constraint_using_biarc_sampler_cache(xfrag) || !xfrag.is_legal())
		    continue;

		  ++count_legal_samples;

		  // Compute cost of this fragment
		  float fragment_cost = this->fragment_cost(edge_id, xfrag);
		  
		  float accum_cost = fragment_cost + subtree_cost;
		  // update the optimal cost and child for the reachable parent state
		  if (accum_cost < opt_cost_p(ip_x, ip_y)[ip_desc])
		  {
		    opt_cost_p(ip_x, ip_y)[ip_desc] = accum_cost;
		    opt_child(ip_x, ip_y)[ip_desc] = ic_idx;
		  }
	  }
	  else
	  { 
		  dbsksp_xshock_node_descriptor xd_p = edesc_list[i_p];
	 
		  // get the grid index of this node descriptor instance
		  int ip_x, ip_y, ip_psi, ip_phi0, ip_r;
		  if (!grid_p.xdesc_to_grid(xd_p, ip_x, ip_y, ip_psi, ip_phi0, ip_r))
		    continue;
		  //vcl_cout << "xdesc to grid" << vcl_endl;



		  // if the subtree cost is already greater the max-acceptable cost then
		  // this branch will never make it to the final detection list
		  if (subtree_cost >= max_acceptable_cost)
		    continue;


		  int ip_desc = grid_p.cell_grid_to_linear(ip_psi, ip_phi0, ip_r);
		  /////////////////////////////////
		  xd_p = grid_p.xdesc(ip_x, ip_y, ip_psi, ip_phi0, ip_r);
		  ///////////////////////////////////

		  // form a fragment from which we compute geometric and image cost
		  dbsksp_xshock_fragment xfrag(xd_p, xd_c);

		  //////
		
		  if (!geom_model->check_nkdiff_constraint_using_biarc_sampler_cache(xfrag) || !xfrag.is_legal())
		    continue;

		  ++count_legal_samples;

		  // Compute cost of this fragment
		  float fragment_cost = this->fragment_cost(edge_id, xfrag);
		  
		  float accum_cost = fragment_cost + subtree_cost;
		  // update the optimal cost and child for the reachable parent state
		  if (accum_cost < opt_cost_p(ip_x, ip_y)[ip_desc])
		  {
		    opt_cost_p(ip_x, ip_y)[ip_desc] = accum_cost;
		    opt_child(ip_x, ip_y)[ip_desc] = ic_idx;
		  }
	  }
    } // i_p
  } // if degree != 1

  return true;
}



// -----------------------------------------------------------------------------
//: Find the optimal child state given a specific state of the parent
bool dbsks_xshock_dp_new::
find_optimal_child_node_given_parent_state(unsigned edge_id,
                                            const dbsksp_xshock_node_descriptor& xd_p,
                                            int degree_child_node,
                                            const dbsks_xfrag_geom_model_sptr& geom_model,
                                            int num_samples_c,
                                            const dbsks_xnode_grid& grid_c,
                                            const grid2d_float& opt_cost_c,
                                            float& min_branch_cost,
                                            int& child_state_at_min_branch_cost,
                                            float max_acceptable_cost)
{
  //>> reset the output values
  min_branch_cost = vnl_numeric_traits<float >::maxval;
  child_state_at_min_branch_cost = -1; // non-existing or dependent (for degree-1 node)

  //>> place holder to store sample states for the child node
  static vcl_vector<dbsksp_xshock_node_descriptor > edesc_list;

  // Strategy:
  // for child with degree==1, there is only possible state
  // for child with degree==2 or 3, we sample a whole bunch and only optimize within these

  // initialize min_cost and its associated child state (min_cost_idx)
  float min_cost = vnl_numeric_traits<float >::maxval;
  int min_cost_idx = -1; //default is -1:  non-existing or dependent (for degree-1 node)

  if (degree_child_node == 1)
  {
    // for a terminal fragment, the only difference between the parent
    // xdesc and the child xdesc is the angle phi.
    dbsksp_xshock_node_descriptor xd_c = xd_p;
    xd_c.phi_ = 0;

    // form a fragment from which we will compute image cost (and deformation cost)
    dbsksp_xshock_fragment xfrag(xd_p, xd_c);

    // image cost
    //float fragment_cost = float(-this->xshock_likelihood()->loglike(edge_id, xfrag));
    float fragment_cost = this->fragment_cost(edge_id, xfrag);

    // we ignore deformation cost for now
    // \todo compute deformation cost

    // the child's state is unique, there is nothing to minimize
    min_cost = fragment_cost;
    min_cost_idx = -1;
  }
  //else if (geom_model->check_start_width_constraint(xd_p, this->graph_size()))
  else 
  {
    // sample a bunch of end-descriptors within the allowed range and compute the min
    //geom_model->sample_end_given_start(xd_p, num_samples_c, edesc_list, this->graph_size());

    // sample more than necessary, but only check the first num_samples_c that satisfy constraints
    geom_model->sample_end_given_start_using_model_minmax_range(xd_p, this->graph_size(), num_samples_c*5, edesc_list);
    //geom_model->sample_new_legal_end_given_start_using_model_minmax_range(xd_p, this->graph_size(), num_samples_c*5, edesc_list);

    unsigned size_edesc_list = edesc_list.size();
    int count_legal_samples = 0; //> counter for the number of child xdesc that satisfy constraints

	if(edesc_list.size()==0)
	{
	  min_branch_cost = min_cost;
	  child_state_at_min_branch_cost = min_cost_idx;
	  return true;
	}

    for (unsigned i_c =0; count_legal_samples < num_samples_c && i_c < size_edesc_list; ++i_c)
    {
      dbsksp_xshock_node_descriptor& xd_c = edesc_list[i_c];

      // get the grid index of this node descriptor instance
      int ic_x, ic_y, ic_psi, ic_phi0, ic_r;
      if (!grid_c.xdesc_to_grid(xd_c, ic_x, ic_y, ic_psi, ic_phi0, ic_r))
        continue;
	  //vcl_cout << "xdesc to grid" << vcl_endl;

      float subtree_cost = opt_cost_c(ic_x, ic_y)[grid_c.cell_grid_to_linear(ic_psi, ic_phi0, ic_r)];

      // if the subtree cost is already greater the max-acceptable cost then
      // this branch will never make it to the final detection list
      if (subtree_cost >= max_acceptable_cost)
        continue;

      /////////////////////////////////
      xd_c = grid_c.xdesc(ic_x, ic_y, ic_psi, ic_phi0, ic_r);
      ///////////////////////////////////

      // form a fragment from which we compute geometric and image cost
      dbsksp_xshock_fragment xfrag(xd_p, xd_c);

      // check geometric constraint after projecting to discrete space
      //if (!geom_model->check_constraints(xfrag) || !xfrag.is_legal())

      /////////
      if (!geom_model->check_nkdiff_constraint_using_biarc_sampler_cache(xfrag) || !xfrag.is_legal())
        continue;

	  //if (!geom_model->check_nkdiff_constraint_using_biarc_sampler_cache(xfrag))
		//continue;
	  //vcl_cout << "satisfy nkdiff constraint" << vcl_endl;

      ++count_legal_samples;

      // Compute cost of this fragment
      float fragment_cost = this->fragment_cost(edge_id, xfrag);
      
      float accum_cost = fragment_cost + subtree_cost;
      if (accum_cost < min_cost)
      {
        min_cost = accum_cost;
        min_cost_idx = grid_c.grid_to_linear(ic_x, ic_y, ic_psi, ic_phi0, ic_r);
      }
    } // i_c
  } // if degree != 1

  //>> reset the output values
  //vcl_cout << "min cost " << min_cost << vcl_endl;
  min_branch_cost = min_cost;
  child_state_at_min_branch_cost = min_cost_idx;

  return true;
}








//------------------------------------------------------------------------------
//: Compute cost of a fragment
float dbsks_xshock_dp_new::
fragment_cost(unsigned edge_id, const dbsksp_xshock_fragment& xfrag)
{
  return float(-this->xshock_likelihood()->loglike(edge_id, xfrag) + 
    this->xshock_likelihood()->loglike_upperbound(edge_id));
}











// -----------------------------------------------------------------------------
//: Compute te state of the root node corresponding to the global optimum
bool dbsks_xshock_dp_new::
compute_root_state_global_optimum_(unsigned root_vid,
                                   int& opt_root_state, float& opt_cost)
{
  grid2d_float& opt_cost_root = this->map_opt_cost[root_vid];
  dbsks_xnode_grid& grid_root = (*this->map_xnode_grid)[root_vid];

  // find sub-optimal state within each cell
  vnl_matrix<float > cell_opt_cost(opt_cost_root.rows(), opt_cost_root.cols());
  vnl_matrix<unsigned > cell_opt_idx(opt_cost_root.rows(), opt_cost_root.cols());
  for (unsigned i =0; i < opt_cost_root.rows(); ++i)
  {
    for (unsigned j =0; j < opt_cost_root.cols(); ++j)
    {
      vnl_vector<float >& x = opt_cost_root(i, j);
      float min_val = vnl_huge_val(float());
      unsigned min_idx = vnl_numeric_traits<unsigned >::maxval;
      for (unsigned k =0; k < x.size(); ++k)
      {
        if (x[k] < min_val)
        {
          min_val = x[k];
          min_idx = k;
        }
      }
      cell_opt_cost(i, j) = min_val;
      cell_opt_idx(i, j) = min_idx;
    }
  }

  // find global optimum
  opt_cost = cell_opt_cost.min_value();
  int min_idx_x = -1;
  int min_idx_y = -1;
  for (unsigned i =0; i < opt_cost_root.rows(); ++i)
  {
    for (unsigned j =0; j < opt_cost_root.cols(); ++j)
    {
      if (cell_opt_cost(i, j) <= opt_cost)
      {
        min_idx_x = i;
        min_idx_y = j;
        break;
      }
    }
  }

  vcl_cout << "Global min cost = " << opt_cost << vcl_endl;
  int i_psi, i_phi0, i_r;
  grid_root.cell_linear_to_grid(cell_opt_idx(min_idx_x, min_idx_y), i_psi, i_phi0, i_r);
  opt_root_state = grid_root.grid_to_linear(min_idx_x, min_idx_y, i_psi, i_phi0, i_r);
  return true;
}






// -----------------------------------------------------------------------------
//: Trace the states of the root node that correspond to optima with a spatial cell grid
bool dbsks_xshock_dp_new::
compute_root_state_local_optimum_(unsigned root_vid, 
                                 vcl_vector<int >& opt_state, 
                                 vcl_vector<float >& opt_cost)
{
  int radius = 1;

  // \todo Make radius an argument of the function
  grid2d_float& opt_cost_root = this->map_opt_cost[root_vid];
  dbsks_xnode_grid& grid_root = (*this->map_xnode_grid)[root_vid];

  // find sub-optimal state within each cell
  vnl_matrix<float > cell_opt_cost(opt_cost_root.rows(), opt_cost_root.cols());
  vnl_matrix<unsigned > cell_opt_idx(opt_cost_root.rows(), opt_cost_root.cols());
  for (unsigned i =0; i < opt_cost_root.rows(); ++i)
  {
    for (unsigned j =0; j < opt_cost_root.cols(); ++j)
    {
      vnl_vector<float >& x = opt_cost_root(i, j);
      float min_val = vnl_numeric_traits<float >::maxval;
      unsigned min_idx = vnl_numeric_traits<unsigned >::maxval;
      for (unsigned k =0; k < x.size(); ++k)
      {
        if (x[k] < min_val)
        {
          min_val = x[k];
          min_idx = k;
        }
      }
      cell_opt_cost(i, j) = min_val;
      cell_opt_idx(i, j) = min_idx;
    }
  }

  // find local optimum
  // for each position on the spatial grid, determine whether it is a local minimum
  // (consider a (2*radius+1)x(2*radius+1) window)
  opt_state.clear();
  opt_state.reserve(cell_opt_cost.size()/(4*radius*radius));
  opt_cost.clear();
  opt_cost.reserve(cell_opt_cost.size()/(4*radius*radius));

  // created a padded version of the opt_cost matrix
  vnl_matrix<float > padded(cell_opt_cost.rows()+2*radius, 
    cell_opt_cost.cols()+2*radius, vnl_numeric_traits<float >::maxval);
  padded.update(cell_opt_cost, radius, radius);

  // check each spatial position
  for (unsigned i =radius; i < padded.rows()-radius; ++i)
  {
    for (unsigned j =radius; j < padded.cols()-radius; ++j)
    {
      vnl_matrix<float > local_cost = padded.extract(2*radius+1, 2*radius+1, i-radius, j-radius);
      if (local_cost(radius, radius) < vnl_numeric_traits<float >::maxval && // non-trival local min
        local_cost(radius, radius) <= local_cost.min_value()) // center is local minimum
      {
        opt_cost.push_back(local_cost(radius, radius));
        int i_psi, i_phi0, i_r;
        grid_root.cell_linear_to_grid(cell_opt_idx(i-radius, j-radius), i_psi, i_phi0, i_r);
        opt_state.push_back(grid_root.grid_to_linear(i-radius, j-radius, i_psi, i_phi0, i_r));
      }
    }
  }
  return true;
}


//------------------------------------------------------------------------------
//: Find the root node's states that are local optimum wrt to all params of the root node
bool dbsks_xshock_dp_new::
compute_root_state_of_local_min_all_root_params(vcl_vector<int >& list_root_state, 
                                                vcl_vector<float >& list_graph_cost,
                                                int kernel_radius)
{
  list_root_state.clear();
  list_graph_cost.clear();

  grid2d_float& opt_cost_root = this->map_opt_cost[this->root_vid_];
  dbsks_xnode_grid& grid_root = (*this->map_xnode_grid)[this->root_vid_];

  // preliminary checks
  if (grid_root.size() <= 0) 
    return false;
  

  // find local min for all params
  int num_x = (int) grid_root.x_.size();
  int num_y = (int) grid_root.y_.size();
  int num_psi = (int) grid_root.psi_.size();
  int num_phi0 = (int) grid_root.phi0_.size();
  int num_r = (int) grid_root.r_.size();

  // x-
  for (int i_x =0; i_x < num_x; ++i_x)
  {
    int kernel_min_x = vnl_math::max(i_x - kernel_radius, 0);
    int kernel_max_x = vnl_math::min(i_x + kernel_radius, num_x-1);
    
    // y-
    for (int i_y =0; i_y < num_y; ++i_y)
    {
      int kernel_min_y = vnl_math::max(i_y - kernel_radius, 0);
      int kernel_max_y = vnl_math::min(i_y + kernel_radius, num_y-1);

      vnl_vector<float >& fz_vec = opt_cost_root(i_x, i_y);
      if (fz_vec.empty())
      {
        continue;
      }

      for (int i_psi =0; i_psi < num_psi; ++i_psi)
      {
        int kernel_min_psi = vnl_math::max(i_psi - kernel_radius, 0);
        int kernel_max_psi = vnl_math::min(i_psi + kernel_radius, num_psi-1);

        for (int i_phi0 =0; i_phi0 < (int) grid_root.phi0_.size(); ++i_phi0)
        {
          int kernel_min_phi0 = vnl_math::max(i_phi0 - kernel_radius, 0);
          int kernel_max_phi0 = vnl_math::min(i_phi0 + kernel_radius, num_phi0 -1);

          for (int i_r =0; i_r < (int) grid_root.r_.size(); ++i_r)
          {
            int kernel_min_r = vnl_math::max(i_r - kernel_radius, 0);
            int kernel_max_r = vnl_math::min(i_r + kernel_radius, num_r -1);

            // retrieve value of the objective function
            float fz = fz_vec[grid_root.cell_grid_to_linear(i_psi, i_phi0, i_r)];

            if (fz >= vnl_numeric_traits<float >::maxval)
              continue;

            // now compute the minimum cost in the kernel around this state
            float kernel_min_val = 0;
            if (!this->compute_kernel_min(opt_cost_root, grid_root, kernel_min_x, kernel_max_x,
              kernel_min_y, kernel_max_y, kernel_min_psi, kernel_max_psi,
              kernel_min_phi0, kernel_max_phi0, kernel_min_r, kernel_max_r,
              kernel_min_val))
            {
              continue;
            }

             
  

            // compare cost with the kernel min
            if (fz <= kernel_min_val)
            {
              list_graph_cost.push_back(fz);
              list_root_state.push_back(grid_root.grid_to_linear(i_x, i_y, i_psi, i_phi0, i_r));
            }
          }
        }
      }
    }
  }

  return true;
}





//-------------------------------------------------------------------------------
//: Trace the graph configuration given specific state of the root node
bool dbsks_xshock_dp_new::
trace_solution_from_root_state(int root_state, vcl_map<unsigned, int >& sol_node2state)
{
  sol_node2state.clear();


  // temporary storage for the optimap states of the nodes
  vcl_map<unsigned, int > map_node2state;
  map_node2state.clear();

  // preliminary check
  dbsksp_xshock_node_sptr xv_root = this->graph()->node_from_id(this->root_vid_);
  if (!xv_root)
    return false;

  // check validity of the root node's state
  dbsks_xnode_grid& root_grid = (*this->map_xnode_grid)[xv_root->id()];
  int i_x, i_y, i_psi, i_phi0, i_r;
  if (!root_grid.linear_to_grid(root_state, i_x, i_y, i_psi, i_phi0, i_r))
  {
    return false;
  }
  int i_desc = root_grid.cell_grid_to_linear(i_psi, i_phi0, i_r);

  // record state of the root
  if (xv_root->degree() == 2)
  {
    map_node2state.insert(vcl_make_pair(xv_root->id(), root_state));
  }
  else if (xv_root->degree() == 3)
  {
    int idx_within_cell = root_grid.cell_grid_to_linear(i_psi, i_phi0, i_r);

    // record state of the root
    grid2d_int& opt_phi1 = this->map_opt_phi1[xv_root->id()];
    int i_phi1 = opt_phi1(i_x, i_y)[idx_within_cell];
    int global_state = root_grid.grid_to_linear(i_x, i_y, i_psi, i_phi0, i_r, i_phi1);
    
    // now we know the root state is legal. Append the node's state
    map_node2state.insert(vcl_make_pair(xv_root->id(), global_state));  
  }

  // recursively trace the solution for each child node of root
  for (dbsksp_xshock_node::edge_iterator eit = xv_root->edges_begin(); eit != 
    xv_root->edges_end(); ++eit)
  {
    dbsksp_xshock_edge_sptr xe_c = *eit;
    dbsksp_xshock_node_sptr xv_c = xe_c->opposite(xv_root);

    // degree-1 is completely dependent on its adjacent node. No state to trace
    if (xv_c->degree() == 1)
      continue;

    // for degree-2 and -3 nodes, trace back using the opt_child grids
    grid2d_int& opt_child = this->map_opt_child_of_root[xv_c->id()];
    int opt_child_state = opt_child(i_x, i_y)[i_desc];

    // repeat the same process with the child nodes as root
    if (!this->trace_optimal_child_state_recursive(xv_c, xe_c, opt_child_state, map_node2state))
    {
      return false;
    };
  }

  // if all go well, we have a complete solution
  sol_node2state = map_node2state;
  return true;
}





// -----------------------------------------------------------------------------
//: Recursively trace out the optimal states of the nodes in a branch given 
// the state of the branch's root node
// xv_root is the root node of the branch
// xe_parent is the parent edge of the root node
// root_state is the state of the root node
// map_node2state is where optimal nodes of this branch will be APPENDED to
bool dbsks_xshock_dp_new::
trace_optimal_child_state_recursive(const dbsksp_xshock_node_sptr& xv_root,
                                    const dbsksp_xshock_edge_sptr& xe_parent, int root_state,
                                    vcl_map<unsigned, int >& map_node2state)
{
  if (!xv_root)
    return false;

  if (xv_root->degree() == 1)
    return true;

  // recursively do the same thing for the root's children
  if (xv_root->degree() == 2)
  {
    // parse the state index
    dbsks_xnode_grid& root_grid = (*this->map_xnode_grid)[xv_root->id()];
    int i_x, i_y, i_psi, i_phi0, i_r;

    // only record the given root state if it is a valid state
    if (!root_grid.linear_to_grid(root_state, i_x, i_y, i_psi, i_phi0, i_r))
    {
      return false;
    }

    // record state of the root
    map_node2state.insert(vcl_make_pair(xv_root->id(), root_state));

    // identify the child edge and child node
    dbsksp_xshock_edge_sptr xe_c = this->graph()->cyclic_adj_succ(xe_parent, xv_root);
    dbsksp_xshock_node_sptr xv_c = xe_c->opposite(xv_root);

    if (xv_c->degree() == 1)
      return true;

    // retrieve the optimal child node
    grid2d_int& opt_child = this->map_opt_child[xv_root->id()];
    int opt_child_state = opt_child(i_x, i_y)[root_grid.cell_grid_to_linear(i_psi, i_phi0, i_r)];

    // repeat the same process with the child node as root
    return this->trace_optimal_child_state_recursive(xv_c, xe_c, opt_child_state, map_node2state);
  }
  else if (xv_root->degree() == 3) // note: assume that this is not THE global root
  {
    // parse state information of the root
    dbsks_xnode_grid& root_grid = (*this->map_xnode_grid)[xv_root->id()];
    
    int i_x, i_y, i_psi, i_phi0, i_r;
    if (!root_grid.linear_to_grid(root_state, i_x, i_y, i_psi, i_phi0, i_r))
    {
      return false;
    }
    int idx_within_cell = root_grid.cell_grid_to_linear(i_psi, i_phi0, i_r);

    // record state of the root
    grid2d_int& opt_phi1 = this->map_opt_phi1[xv_root->id()];
    int i_phi1 = opt_phi1(i_x, i_y)[idx_within_cell];
    int global_state = root_grid.grid_to_linear(i_x, i_y, i_psi, i_phi0, i_r, i_phi1);
    
    // now we know the root state is legal. Append the node's state
    map_node2state.insert(vcl_make_pair(xv_root->id(), global_state));

    // identify the two children
    dbsksp_xshock_edge_sptr child_edge1, child_edge2; // counter-clockwise ordering
    child_edge1 = this->graph()->cyclic_adj_succ(xe_parent, xv_root);
    child_edge2 = this->graph()->cyclic_adj_succ(child_edge1, xv_root);

    dbsksp_xshock_node_sptr child1 = child_edge1->opposite(xv_root);
    dbsksp_xshock_node_sptr child2 = child_edge2->opposite(xv_root);

    // identify optimal states of the two children
    grid2d_int& opt_child1 = this->map_opt_child1[xv_root->id()];
    grid2d_int& opt_child2 = this->map_opt_child2[xv_root->id()];
    
    int opt_child1_state = opt_child1(i_x, i_y)[idx_within_cell];
    int opt_child2_state = opt_child2(i_x, i_y)[idx_within_cell];

    // recurvively find optimal states for each sub-tree
    return this->trace_optimal_child_state_recursive(child1, child_edge1, opt_child1_state, map_node2state) &&
      this->trace_optimal_child_state_recursive(child2, child_edge2, opt_child2_state, map_node2state);
  }

  return false;
}




//------------------------------------------------------------------------------
//: Compute min value in a window of a grid2d_float function
bool dbsks_xshock_dp_new::
compute_kernel_min(const grid2d_float& F,
                 const dbsks_xnode_grid& grid,
                 int min_i_x, int max_i_x,
                 int min_i_y, int max_i_y,
                 int min_i_psi, int max_i_psi,
                 int min_i_phi0, int max_i_phi0,
                 int min_i_r, int max_i_r,
                 float& min_val) const
{
  min_val = vnl_numeric_traits<float >::maxval;

  // preliminary checks
  if (F.size() == 0)
    return false;

  // now we use the convention: 1st coordinate is x- and 2nd coordinate is y-
  // therefore for a vbl_array_2d, num_rows = number of x values
  // and num_cols = number of y values
  if (F.rows() != grid.x_.size() || F.cols() != grid.y_.size())
    return false;

  float fmin = vnl_numeric_traits<float >::maxval;
  for (int i_x = min_i_x; i_x <= max_i_x; ++i_x)
  {
    for (int i_y = min_i_y; i_y <= max_i_y; ++i_y)
    {
      const vnl_vector<float >& fz_vec = F(i_x, i_y);
      if (fz_vec.empty())
        continue;

      for (int i_psi = min_i_psi; i_psi <= max_i_psi; ++i_psi)
      {
        for (int i_phi0 = min_i_phi0; i_phi0 <= max_i_phi0; ++i_phi0)
        {
          for (int i_r = min_i_r; i_r <= max_i_r; ++i_r)
          {
            int cell_state = grid.cell_grid_to_linear(i_psi, i_phi0, i_r);
            fmin = vnl_math::min(fmin, fz_vec[cell_state]);
          }
        }
      }
    }
  }

  min_val = fmin;
  return true;
}






//------------------------------------------------------------------------------
//: Compute min value of a grid2d_float function
float dbsks_xshock_dp_new::
compute_min_value(const grid2d_float& F) const
{
  // initialization to the largest possible
  float min_val = vnl_numeric_traits<float >::maxval;

  // iterate thru every element of the grid
  for (grid2d_float::const_iterator iter = F.begin(); iter != F.end(); ++iter)
  {
    if (iter->empty())
      continue;
    min_val = vnl_math::min(min_val, iter->min_value());
  }
  return min_val;
}





//------------------------------------------------------------------------------
//: Compute min value for position of a grid2d_float function
void dbsks_xshock_dp_new::
compute_min_value_matrix(const grid2d_float& F, vnl_matrix<float >& min_value) const
{
  min_value.set_size(F.rows(), F.cols());
  min_value.fill(vnl_numeric_traits<float >::maxval);
  for (unsigned r =0; r < F.rows(); ++r)
  {
    for (unsigned c =0; c < F.cols(); ++c)
    {
      if (F(r, c).empty())
        continue;
      min_value(r, c) = F(r, c).min_value();
    }
  }
  return;
}



//------------------------------------------------------------------------------
//: Compute maximum acceptable cost for a sub-tree based on the min-costs
// of other sub-trees that have been computed.
// except_vid1, except_vid2, and except_vid3 are id's of the nodes whose subtrees
// we want to ignore during the calculation
// Typically, for an A12 subtree, we only have except_vid0. For an A13 node,
// we have "except_vid0" and "except_vid1". "except_vid2" is there for any future use
float dbsks_xshock_dp_new::
compute_max_acceptable_subtree_cost(unsigned except_vid0, 
                                    unsigned except_vid1,
                                    unsigned except_vid2)
{
  // Constrain: the sum of current sub-tree's cost and the costs of sub-trees
  // that have been computed must be less than the global max_acceptable_xgraph_cost_
  float max_cost = this->max_acceptable_xgraph_cost_;
  for (vcl_map<unsigned, float >::iterator iter = this->map_node_to_min_cost_.begin();
    iter != this->map_node_to_min_cost_.end(); ++iter)
  {
    unsigned vid = iter->first;
    if (vid == except_vid0 || vid == except_vid1 || vid == except_vid2)
      continue;
    max_cost -= iter->second;
  }

  return max_cost;
}






















// -----------------------------------------------------------------------------
//: compute globally optimal graph
bool dbsks_xshock_dp_new::
find_xgraph_state_global_optimum(unsigned root_vid, 
                                 vcl_map<unsigned, int>& opt_xgraph_state, 
                                 float& opt_cost)
{
  // state of the root node corresponding to the global optimal cost
  int opt_root_state;
  this->compute_root_state_global_optimum_(root_vid, opt_root_state, opt_cost);

  // trace out the states of every node for this optimal cost
  this->trace_solution_from_root_state(opt_root_state, opt_xgraph_state);
  
  return true;
}



  


// -----------------------------------------------------------------------------
//: Compute locally optimal xgraphs
bool dbsks_xshock_dp_new::
find_xgraph_state_local_optimum(unsigned root_vid, 
                                vcl_vector<vcl_map<unsigned, int> >& list_xgraph_state,
                                vcl_vector<float >& list_cost)
{
  // state of the root node corresponding to local optimal costs
  vcl_vector<int > opt_state;
  vcl_vector<float > opt_cost;
  this->compute_root_state_local_optimum_(root_vid, opt_state, opt_cost);

  // trace out the states of every node corresponding to these solutions
  unsigned num_sols = opt_state.size();

  // prepare space
  list_xgraph_state.clear();
  list_xgraph_state.reserve(num_sols);

  list_cost.clear();
  list_cost.reserve(num_sols);

  // we only accept a solution if we can successfully trace back the states of all of its nodes
  for (unsigned i =0; i < num_sols; ++i)
  {
    vcl_map<unsigned, int> map_node2state;    
    if (this->trace_solution_from_root_state(opt_state[i], map_node2state))
    {
      list_xgraph_state.push_back(map_node2state);
      list_cost.push_back(opt_cost[i]);
    }
  }
  return true;
}





////: Propagate min-cost from children to parent to determine the lower bound cost
//// of a parent node before the optimization
//void dbsks_xshock_dp_new::
//propagate_min_cost_from_children_to_parent_node(vnl_matrix<float >& min_cost,
//                                                unsigned parent_id)
//{
//  //vcl_map<unsigned, vnl_matrix<float > >::iterator iter = this->map_node_to_min_cost_matrix_.find(xv_c->id());
//
//  //// if there is not cost info for the child (probably degree-1 node) then there is no restriction on parent
//  //if (iter == this->map_node_to_min_cost_matrix_.end())
//  //{
//  //  min_cost_xy_p.fill(0.0f);
//  //}
//  //else
//  //{
//  //  vnl_matrix<float > min_cost_xy_c = iter->second;
//
//  //  // To propagate, we first compute which position of the parent node will be
//  //  // affected by the value of the child node at index (0, 0)
//
//  //  double x0_c = grid_c.x_[0];
//  //  double y0_c = grid_c.y_[0];
//  //  vgl_point_2d<double > pt0_c(x0_c, y0_c);
//
//  //  double min_chord, max_chord;
//  //  geom_model->get_range_chord(min_chord, max_chord);
//
//  //  double min_psi_start, max_psi_start;
//  //  geom_model->get_range_psi_start(min_psi_start, max_psi_start);
//
//  //  double min_alpha_start, max_alpha_start;
//  //  geom_model->get_range_alpha_start(min_alpha_start, max_alpha_start);
//
//  //  // chord angle
//  //  double min_theta = min_psi_start - max_alpha_start;
//  //  double max_theta = max_psi_start - min_alpha_start;
//  //  vgl_vector_2d<double > min_theta_vec(vcl_cos(min_theta), vcl_sin(min_theta));
//  //  vgl_vector_2d<double > max_theta_vec(vcl_cos(max_theta), vcl_sin(max_theta));
//
//  //  double x0_p = grid_p.x_[0];
//  //  double y0_p = grid_p.y_[0];
//
//  //  // assuming the two grid have similar sampling rate
//  //  assert(grid_c.step_x_ = grid_p.step_x_);
//  //  assert(grid_c.step_y_ = grid_p.step_y_);
//  //  double step_x = grid_p.step_x_;
//  //  double step_y = grid_p.step_y_;
//
//  //  int max_x_idx = vnl_math_ceil( (x0_c+max_chord - x0_p) / step_x);
//  //  int min_x_idx = vnl_math_floor( (x0_c-max_chord - x0_p) / step_x);
//
//  //  int max_y_idx = vnl_math_ceil( (y0_c+max_chord - y0_p) / step_y);
//  //  int min_y_idx = vnl_math_floor( (y0_c-max_chord - y0_p) / step_y);
//
//  //  // Determine which points of parent grid affected by child point at index (0, 0)
//  //  // i_x and i_y are running indices for the points in the parent's grid
//
//  //  // x- and y- indices of parent node (grid_p) which will be affected
//  //  int max_npts_affected = (max_x_idx-min_x_idx+1) * (max_y_idx-min_x_idx+1);
//  //  vcl_vector<int > x_idx_p, y_idx_p;
//  //  x_idx_p.reserve(max_npts_affected);
//  //  y_idx_p.reserve(max_npts_affected);
//
//  //  for (int i_x = min_x_idx; i_x <= max_x_idx; ++i_x)
//  //  {
//  //    for (int i_y = min_y_idx; i_y <= max_y_idx; ++i_y)
//  //    {
//  //      vgl_point_2d<double > pt_p(x0_p + i_x*step_x, y0_p + i_y*step_y);
//  //      vgl_vector_2d<double > v = pt0_c - pt_p; // note order, vector from parent to node
//
//  //      // verify distance is within bounds
//  //      double len = v.length();
//  //      if (len < min_chord || len > max_chord)
//  //        continue;
//
//  //      // verify angle is within bounds
//  //      
//  //      // angle between v and the min_theta
//  //      double angle = signed_angle(min_theta_vec, v);
//  //      
//  //      // turn angle to [0, 2pi]
//  //      if (angle <0) angle += 2*vnl_math::pi;
//  //      
//  //      // check
//  //      if (angle > (max_theta - min_theta))
//  //        continue;
//
//  //      x_idx_p.push_back(i_x);
//  //      y_idx_p.push_back(i_y);
//  //    }
//  //  }
//
//  //  unsigned num_x_c = grid_c.x_.size();
//  //  unsigned num_y_c = grid_c.y_.size();
//  //  unsigned num_x_p = grid_p.x_.size();
//  //  unsigned num_y_p = grid_p.y_.size();
//
//  //  float large_value = vnl_numeric_traits<float >::maxval/2;
//
//  //  // Now update the cost matrix of the parent node
//  //  for (unsigned i_x_c =0; i_x_c < num_x_c; ++i_x_c)
//  //  {
//  //    for (unsigned i_y_c =0; i_y_c < num_y_c; ++i_y_c)
//  //    {
//  //      float val = min_cost_xy_c(i_x_c, i_y_c);
//
//  //      if (val > large_value)
//  //        continue;
//
//  //      // iterate through the parent locations that this child affects
//  //      for (unsigned k =0; k < x_idx_p.size(); ++k)
//  //      {
//  //        // translate the parent with the same amount as the child
//  //        int i_x = x_idx_p[k] + i_x_c; 
//  //        int i_y = y_idx_p[k] + i_y_c;
//
//  //        if (i_x < 0 || i_x >= int(num_x_p))
//  //          continue;
//
//  //        if (i_y < 0 || i_y >= int(num_y_p))
//  //          continue;
//
//  //        // update cost matrix of parent with the best it can have
//  //        min_cost_xy_p(i_x, i_y) = vnl_math::min(min_cost_xy_p(i_x, i_y), val);        
//  //      }
//  //    }
//  //  }
//  //}
//
//
//  return;
//}

