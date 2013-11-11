// This is file seg/dbsks/dbsks_local_match.cxx

//:
// \file

#include "dbsks_local_match.h"

#include <dbsks/dbsks_local_match_cost.h>
#include <vnl/algo/vnl_amoeba.h>
#include <vnl/algo/vnl_powell.h>
#include <vnl/algo/vnl_conjugate_gradient.h>
#include <dbsksp/dbsksp_shock_graph.h>
#include <dbsksp/dbsksp_shock_node.h>
#include <vcl_utility.h>
#include <dbsksp/dbsksp_shapelet.h>
#include <dbsks/dbsks_ocm_image_cost.h>
#include <vsol/vsol_box_2d.h>
#include <dbsks/dbsks_utils.h>


// ============================================================================
//  dbsks_local_match
// ============================================================================

// -----------------------------------------------------------------------------
void dbsks_local_match::
set_graph(const dbsksp_shock_graph_sptr& graph)
{
  this->graph_ = graph;
}

//: Set cost function parameters
void dbsks_local_match::
set_cost_params(const dbsks_ocm_image_cost_sptr& ocm, float ds, float lambda)
{
  this->ocm_ = ocm;
  // Reset tolerance in OCM
  this->ocm_->set_tolerance_near_zero(0.0f);
  this->ds_ = ds;
  this->lambda_ = lambda;
}






// -----------------------------------------------------------------------------------
//: Set initial states of xnodes using the shapelet results from DP engine
void dbsks_local_match::
set_xnode_states(const vcl_map<dbsksp_shock_edge_sptr, dbsksp_shapelet_sptr >& shapelet_map)
{

  this->cur_xnode_map_.clear();
  for (vcl_map<dbsksp_shock_edge_sptr, dbsksp_shapelet_sptr >::const_iterator it = 
    shapelet_map.begin(); it != shapelet_map.end(); ++it)
  {
    dbsksp_shock_edge_sptr e = it ->first;
    dbsksp_shapelet_sptr s_e = it->second;
    dbsksp_shock_node_sptr v_parent = e->parent_node();


    // xnode associated with the parent node
    dbsksp_xshock_node_descriptor xnode_parent(s_e->start(), s_e->bnd_start(0),
      s_e->bnd_start(1));
    this->cur_xnode_map_.insert(vcl_make_pair(v_parent, xnode_parent));

    // if the child node is connected to a terminal edge, then it is not
    // the parent node in any edge (for non-branching graphs). It xnode is thus
    // determined from the ``this'' edge
    if (e->child_node()->connected_to_terminal_edge())
    {
      dbsksp_shock_node_sptr v_child= e->child_node();
      dbsksp_xshock_node_descriptor xnode_child(s_e->end(), 
        s_e->bnd_end(0), s_e->bnd_end(1));
      this->cur_xnode_map_.insert(vcl_make_pair(v_child, xnode_child));
    }
  }

}











// -----------------------------------------------------------------------------
//: Get object boundary from the cur_xnode_map
vcl_vector<dbgl_circ_arc > dbsks_local_match::
get_cur_bnd_arc_list()
{
  return dbsks_bnd_arc_list(this->graph(), this->cur_xnode_map_);
}





// -----------------------------------------------------------------------------
//: Set initial states (fragments) of the edges
void dbsks_local_match::
set_init_states_of_edges(
  const vcl_map<dbsksp_shock_edge_sptr, dbsksp_shapelet_sptr >& shapelet_map)
{
  this->shapelet_map_ = shapelet_map;
  return;
}




// -----------------------------------------------------------------------------
//: Set ref_shapelet_map
void dbsks_local_match::
set_ref_shapelet_map(
  const vcl_map<dbsksp_shock_edge_sptr, dbsksp_shapelet_sptr >& ref_shapelet_map)
{
  this->ref_shapelet_map_ = ref_shapelet_map;
}



////: Optimize energy of shock graph
//void dbsks_local_match::
//optimize()
//{
//
//  // setup the cost function
//  this->lm_cost()->set_graph(this->graph());
//  this->lm_cost()->oriented_chamfer_ = this->ocm_;
//  this->lm_cost()->ds_shapelet_ = this->ds_;
//  this->lm_cost()->lambda_ = this->lambda_;
//
//  vnl_vector<double > x =
//    this->lm_cost()->convert_to_graph_params(this->shapelet_map_);  
//  
//  double init_cost = this->lm_cost()->f(x);
//  vcl_cout << "Start minimization ... \n";
//    vcl_cout << "x_init = " << x << "\n";
//
//
//  // amoeba
//  vnl_amoeba amoeba(*this->lm_cost());
//  amoeba.default_verbose = true;
//  amoeba.verbose = 1;
//  //amoeba.set_max_iterations(100);
//  amoeba.minimize(x);
//
//  vcl_cout << "Final x = " << x << "\n";
//
//  double final_cost1 = this->lm_cost()->f(x);
//  double final_cost2 = this->lm_cost()->f(this->lm_cost()->xmin_);
//
//  this->final_x_ = this->lm_cost()->xmin_;
//
//  vcl_cout << "\nInit cost = " << init_cost << "\n"
//    << "Final cost 1 = " << final_cost1 << "\n"
//    << "Final cost 2= " << final_cost2 << "\n";
//}



//: ---------------------------------------------------------------------------
//: Optimize one node, keeping other node constant
void dbsks_local_match::
optimize(const dbsksp_shock_node_sptr& v)
{
  // only deal with degree-2 node for now
  if (v->degree() != 2)
    assert(false);

  dbsksp_xshock_node_descriptor new_xnode;

  // determine its adjacent nodes
  if (!v->connected_to_terminal_edge() )
  {
    dbsksp_shock_edge_sptr e_parent = v->parent_edge();
    dbsksp_shock_node_sptr v_parent = e_parent->parent_node();
    dbsksp_shock_edge_sptr e_child = this->graph()->cyclic_adj_succ(e_parent, v);
    dbsksp_shock_node_sptr v_child = e_child->child_node();

    dbsksp_xshock_node_descriptor xnode_parent = this->cur_xnode_map_.find(v_parent)->second;
    dbsksp_xshock_node_descriptor xnode_v = this->cur_xnode_map_.find(v)->second;
    dbsksp_xshock_node_descriptor xnode_child = this->cur_xnode_map_.find(v_child)->second;

    // retrieve the shapelets
    dbsksp_shapelet_sptr ref_shapelet_parent = 0;
    dbsksp_shapelet_sptr ref_shapelet_child = 0;
    if (!this->ref_shapelet_map_.empty())
    {
      ref_shapelet_parent = this->ref_shapelet_map_.find(e_parent)->second;
      ref_shapelet_child = this->ref_shapelet_map_.find(e_child)->second;
    }

    dbsks_a12_xnode_cost a12_xnode_cost;
    a12_xnode_cost.set(this->ocm_, this->lambda_, this->ds_, xnode_parent, xnode_child,
      ref_shapelet_parent, ref_shapelet_child);


    vnl_vector<double > x = dbsks_convert_to_vector(xnode_v);

    double f_init = a12_xnode_cost.f(x);


    
    
    vcl_cout << "\nInit cost = " << f_init << "\n"
        << "Cached accum_image_cost = " << a12_xnode_cost.accum_image_cost_ << "\n"
        << "Cached accum_length = " << a12_xnode_cost.accum_len_ << "\n"
        << "Cached accum_shape_cost = " << a12_xnode_cost.accum_shape_cost_ << "\n"
        << "Lambda = " << a12_xnode_cost.lambda() << "\n";

    // Run amoeba minimization
    vnl_amoeba::minimize(a12_xnode_cost, x, vnl_vector<double >(5, 4*this->ds_));


    double f_final = a12_xnode_cost.f(x);
    vcl_cout << "Final cost " << f_final << "\n"
      << "Cached accum_image_cost = " << a12_xnode_cost.accum_image_cost_ << "\n"
      << "Cached accum_length = " << a12_xnode_cost.accum_len_ << "\n"
      << "Cached accum_shape_cost = " << a12_xnode_cost.accum_shape_cost_ << "\n"
      << "Lambda = " << a12_xnode_cost.lambda() << "\n";

    // update the output xnode
    new_xnode = dbsks_convert_to_xnode(x);
  }
  else
  {
    if (v->depth() == 0) // root node
    {
      dbsksp_shock_edge_sptr e_child = *v->edges_begin();
      if (e_child->is_terminal_edge())
      {
        e_child = this->graph()->cyclic_adj_succ(e_child, v);
      }
      
      dbsksp_shock_node_sptr v_child = e_child->child_node();
      assert(v_child != v);

      dbsksp_xshock_node_descriptor xnode_child = this->cur_xnode_map_.find(v_child)->second;
      dbsksp_xshock_node_descriptor xnode_v = this->cur_xnode_map_.find(v)->second;


      // determine reference shapelet
      dbsksp_shapelet_sptr ref_shapelet = 0;
      if (!this->ref_shapelet_map_.empty())
      {
        ref_shapelet = this->ref_shapelet_map_.find(e_child)->second;
      }

      // local the reference shapelet for the edge
      dbsks_ainfty_xnode_cost ainfty_xnode_cost;
      ainfty_xnode_cost.set(this->ocm_, this->lambda_, this->ds_, xnode_child.opposite_xnode(), ref_shapelet->reversed_dir());

      // initial values for the shapelet
      vnl_vector<double > x = dbsks_convert_to_vector(xnode_v.opposite_xnode());

      // DEBUG - 
      double f_init = ainfty_xnode_cost.f(x);
      vcl_cout << "\nInit cost = " << f_init << "\n"
        << "Cached accum_image_cost = " << ainfty_xnode_cost.accum_image_cost_ << "\n"
        << "Cached accum_length = " << ainfty_xnode_cost.accum_len_ << "\n"
        << "Cached accum_shape_cost = " << ainfty_xnode_cost.accum_shape_cost_ << "\n"
        << "Lambda = " << ainfty_xnode_cost.lambda() << "\n";

      // Run amoeba minimization
      vnl_amoeba::minimize(ainfty_xnode_cost, x, vnl_vector<double >(5, 4*this->ds_));

      double f_final = ainfty_xnode_cost.f(x);

      vcl_cout << "\nInit cost = " << f_init << "\n"
        << "Final cost " << f_final << "\n"
        << "Cached accum_image_cost = " << ainfty_xnode_cost.accum_image_cost_ << "\n"
        << "Cached accum_length = " << ainfty_xnode_cost.accum_len_ << "\n"
        << "Cached accum_shape_cost = " << ainfty_xnode_cost.accum_shape_cost_ << "\n"
        << "Lambda = " << ainfty_xnode_cost.lambda() << "\n";

      // update the output xnode
      new_xnode = dbsks_convert_to_xnode(x).opposite_xnode();
    
    }
    else
    {
      dbsksp_shock_edge_sptr e_parent = v->parent_edge();
      dbsksp_shock_node_sptr v_parent = e_parent->parent_node();

      dbsksp_xshock_node_descriptor xnode_parent = 
        this->cur_xnode_map_.find(v_parent)->second;
      dbsksp_xshock_node_descriptor xnode_v = 
        this->cur_xnode_map_.find(v)->second;
      
      // determine reference shapelet
      dbsksp_shapelet_sptr ref_shapelet = 0;
      if (!this->ref_shapelet_map_.empty())
      {
        ref_shapelet = this->ref_shapelet_map_.find(e_parent)->second;
      }

      dbsks_ainfty_xnode_cost ainfty_xnode_cost;
      ainfty_xnode_cost.set(this->ocm_, this->lambda_, this->ds_, xnode_parent,
        ref_shapelet);

      // initial values
      vnl_vector<double > x = dbsks_convert_to_vector(xnode_v);
      
      
      double f_init = ainfty_xnode_cost.f(x);
      vcl_cout << "\nInit cost = " << f_init << "\n"
        << "Cached accum_image_cost = " << ainfty_xnode_cost.accum_image_cost_ << "\n"
        << "Cached accum_length = " << ainfty_xnode_cost.accum_len_ << "\n"
        << "Cached accum_shape_cost = " << ainfty_xnode_cost.accum_shape_cost_ << "\n"
        << "Lambda = " << ainfty_xnode_cost.lambda() << "\n";

      // Run amoeba minimization
      vnl_amoeba::minimize(ainfty_xnode_cost, x, vnl_vector<double >(5, 4*this->ds_));

      double f_final = ainfty_xnode_cost.f(x);

      vcl_cout << "Final cost " << f_final << "\n"
        << "Cached accum_image_cost = " << ainfty_xnode_cost.accum_image_cost_ << "\n"
        << "Cached accum_length = " << ainfty_xnode_cost.accum_len_ << "\n"
        << "Cached accum_shape_cost = " << ainfty_xnode_cost.accum_shape_cost_ << "\n"
        << "Lambda = " << ainfty_xnode_cost.lambda() << "\n";

      // update the output xnode
      new_xnode = dbsks_convert_to_xnode(x);
    }
  }
  vcl_cout << "Final xnode: \n"
      << "   pt = " << new_xnode.pt_ << "\n"
      << "   psi = " << new_xnode.psi_ << "\n"
      << "   phi = " << new_xnode.phi_ << "\n"
      << "   r = " << new_xnode.radius_ << "\n";

  this->cur_xnode_map_.find(v)->second = new_xnode;
}












// -----------------------------------------------------------------------------
//: Optimize the nodes in sequences
void dbsks_local_match::
optimize_nodes_in_sequence(int num_node_visits)
{
  this->init_graph_cost_ = 
    this->f_graph_w_shock_edit_shape_cost(this->cur_xnode_map_, 
    this->init_graph_image_cost_,
    this->init_graph_shape_cost_,
    this->init_image_cost_map_,
    this->init_shape_cost_map_);
 

  // Euler tour of the nodes and optimize the nodes each time they are visited
  dbsksp_shock_node_sptr cur_node = this->graph()->root();


  dbsksp_shock_edge_sptr cur_edge = *cur_node->edges_begin();
  if (cur_edge->is_terminal_edge()) // we can the non-terminal one
    cur_edge = this->graph()->cyclic_adj_succ(cur_edge, cur_node);

  for (int i =0; i < num_node_visits; ++i)
  {
    vcl_cout << "Node id = " << cur_node->id() << "\n";
    this->optimize(cur_node);

    // select the next node
    if (!cur_edge->is_terminal_edge())
    {
      cur_node = cur_edge->opposite(cur_node);
      cur_edge = this->graph()->cyclic_adj_succ(cur_edge, cur_node);
    }
    else // skip touring the terminal edges
    {
      cur_edge = this->graph()->cyclic_adj_succ(cur_edge, cur_node);
      cur_node = cur_edge->opposite(cur_node);
      cur_edge = this->graph()->cyclic_adj_succ(cur_edge, cur_node);
    }
  }

  
  this->final_graph_cost_ = 
    this->f_graph_w_shock_edit_shape_cost(this->cur_xnode_map_,
    this->final_graph_image_cost_, this->final_graph_shape_cost_,
    this->final_image_cost_map_, this->final_shape_cost_map_);


  // Display final costs
  vcl_cout << "Initial graph cost = " << this->init_graph_cost_ << "\n"
    << "Init image cost = " << this->init_graph_image_cost_ << "\n"
    << "Init shape cost = " << this->init_graph_shape_cost_ << "\n";

  vcl_cout << "Final graph cost = " << this->final_graph_cost_ << "\n"
    << "Final image cost = " << this->final_graph_image_cost_ << "\n"
    << "Final shape cost = " << this->final_graph_shape_cost_ << "\n";
  
  return;
}




//: graph cost, using shock-edit distance cost as regularization
double dbsks_local_match::
f_graph_w_shock_edit_shape_cost(const vcl_map<dbsksp_shock_node_sptr, 
                                dbsksp_xshock_node_descriptor >& xnode_map,
                                double& image_cost,
                                double& shape_cost)
{
  vcl_map<dbsksp_shock_edge_sptr, double > image_cost_map;
  vcl_map<dbsksp_shock_edge_sptr, double > shape_cost_map;
  
  return this->f_graph_w_shock_edit_shape_cost(xnode_map,
    image_cost, shape_cost, image_cost_map, shape_cost_map);
}



//: ----------------------------------------------------------------------------
//: graph cost, using shock-edit distance cost as regularization
double dbsks_local_match::
f_graph_w_shock_edit_shape_cost(const vcl_map<dbsksp_shock_node_sptr, 
                                dbsksp_xshock_node_descriptor >& xnode_map,
                                double& image_cost,
                                double& shape_cost,
                                vcl_map<dbsksp_shock_edge_sptr, double >& image_cost_map,
                                vcl_map<dbsksp_shock_edge_sptr, double >& shape_cost_map)
{
  // create place holder for the shape and image cost lists
  image_cost_map.clear();
  shape_cost_map.clear();

  for (dbsksp_shock_graph::edge_iterator eit = this->graph()->edges_begin();
    eit != this->graph()->edges_end(); ++eit)
  {
    dbsksp_shock_edge_sptr e = *eit;

    if (e->is_terminal_edge())
      continue;

    image_cost_map.insert(vcl_make_pair(e, 0));
    shape_cost_map.insert(vcl_make_pair(e, 0));
  }
  


  double final_cost = 0;
  double accum_image_cost = 0;
  double accum_shape_cost = 0;

  // extrinsic fragment cost
  dbsks_xfrag_cost xfrag_cost;
  xfrag_cost.set(this->ocm_, this->lambda_, this->ds_);

  // go thru the edges and compute the costs for each of them
  for (dbsksp_shock_graph::edge_iterator eit = this->graph()->edges_begin();
    eit != this->graph()->edges_end(); ++eit)
  {
    dbsksp_shock_edge_sptr e = *eit;
    if (!e->is_terminal_edge())
    {
      dbsksp_shock_node_sptr v_parent = e->parent_node();
      dbsksp_shock_node_sptr v_child = e->child_node();

      // retrieve the xnodes of the vertices
      dbsksp_xshock_node_descriptor xnode_parent = 
        this->cur_xnode_map_.find(v_parent)->second;
      dbsksp_xshock_node_descriptor xnode_child = 
        this->cur_xnode_map_.find(v_child)->second;

      // compute the cost
      double image_cost = xfrag_cost.f_image(xnode_parent, xnode_child);
      double shape_cost = xfrag_cost.f_shock_edit(xnode_parent, xnode_child,
        this->ref_shapelet_map_.find(e)->second);

      accum_image_cost += image_cost;
      accum_shape_cost += shape_cost;

      // save to cost lists
      image_cost_map.find(e)->second += image_cost;
      shape_cost_map.find(e)->second += shape_cost;
    }
    else
    {
      dbsksp_shock_node_sptr v_parent = e->parent_node();
      dbsksp_xshock_node_descriptor xnode_parent =
        this->cur_xnode_map_.find(v_parent)->second;
      double image_cost = 0;
      if (v_parent->depth() == 0) // root node
      {
        image_cost = xfrag_cost.f_image(xnode_parent.opposite_xnode());
      }
      else
      {
        final_cost += xfrag_cost.f_image(xnode_parent);
      }
      // there is no shape cost for a terminal edge
      accum_image_cost += image_cost;

      // save to cost map
      // find the edge adjacent to this terminal edge
      dbsksp_shock_edge_sptr next_e = this->graph()->cyclic_adj_succ(e, v_parent);
      image_cost_map.find(next_e)->second += image_cost;
    }
  }

  final_cost = accum_image_cost + this->lambda_ * accum_shape_cost;

  // save results
  image_cost = accum_image_cost;
  shape_cost = accum_shape_cost;
  return final_cost;
}






//: graph cost, using shock-edit distance cost as regularization
double dbsks_local_match::
f_graph_w_bnd_length_shape_cost(const vcl_map<dbsksp_shock_node_sptr, dbsksp_xshock_node_descriptor >& xnode_map)
{
  double final_cost = 0;
  // extrinsic fragment cost
  dbsks_xfrag_cost xfrag_cost;
  xfrag_cost.set(this->ocm_, this->lambda_, this->ds_);

  for (dbsksp_shock_graph::edge_iterator eit = this->graph()->edges_begin();
    eit != this->graph()->edges_end(); ++eit)
  {
    dbsksp_shock_edge_sptr e = *eit;
    if (!e->is_terminal_edge())
    {
      dbsksp_shock_node_sptr v_parent = e->parent_node();
      dbsksp_shock_node_sptr v_child = e->child_node();

      // retrieve the xnodes of the vertices
      dbsksp_xshock_node_descriptor xnode_parent = 
        this->cur_xnode_map_.find(v_parent)->second;
      dbsksp_xshock_node_descriptor xnode_child = 
        this->cur_xnode_map_.find(v_child)->second;

      // compute the cost
      final_cost += xfrag_cost.f_regularized_using_len(xnode_parent, xnode_child);
    }
    else
    {
      dbsksp_shock_node_sptr v_parent = e->parent_node();
      dbsksp_xshock_node_descriptor xnode_parent =
        this->cur_xnode_map_.find(v_parent)->second;
      if (v_parent->depth() == 0) // root node
      {
        final_cost += xfrag_cost.f_regularized_using_len(xnode_parent.opposite_xnode());
      }
      else
      {
        final_cost += xfrag_cost.f_regularized_using_len(xnode_parent);
      }
    }
  }

  return final_cost;
}


//// -----------------------------------------------------------------------------
////: Return the final twoshapelets associated with the edges
//void dbsks_local_match::
//get_final_twoshapelets(
//  vcl_map<dbsksp_shock_edge_sptr, dbsksp_twoshapelet_sptr >& twoshapelet_map)
//{
//  this->lm_cost()->convert_to_twoshapelet_map(this->final_x_, twoshapelet_map);
//  return;
//}



//// -----------------------------------------------------------------------------
////: Return the final xnode associated with the vertices
//void dbsks_local_match::
//get_final_xnodes(
//  vcl_map<dbsksp_shock_node_sptr, dbsksp_xshock_node_descriptor >& xnode_map)
//{
//  this->lm_cost()->convert_to_xnode_map(this->final_x_, xnode_map);
//}






//// -----------------------------------------------------------------------------
////: Return the final list boundary arcs
//void dbsks_local_match::
//get_bnd_arc_list(vcl_vector<dbgl_circ_arc >& arc_list)
//{
//  this->lm_cost()->convert_to_circ_arc_list(this->final_x_, arc_list);
//  return;
//}





// UTILITY functions ---------------------------------------------------------

//: Print Optimization results to a stream
void dbsks_local_match::
print_summary(vcl_ostream& str) const
{
  // compute bounding box
  vsol_box_2d_sptr bbox = dbsks_compute_bounding_box(this->get_cur_xnode_map());

  str << "dbsks_local_match_summary 0\n"
    << "ds " << this->ds_ << "\n"
    << "lambda " << this->lambda_ << "\n"
    << "edge_threshold " << this->ocm_->edge_threshold_ << "\n"
    << "ocm_sigma_distance " << this->ocm_->sigma_distance_ << "\n"
    << "ocm_sigma_angle " << this->ocm_->sigma_angle_ << "\n"
    
    << "init_graph_cost " << this->init_graph_cost_ << "\n"
    << "init_graph_image_cost " << this->init_graph_image_cost_ << "\n"
    << "init_graph_shape_cost " << this->init_graph_shape_cost_ << "\n"

    << "final_graph_cost " << this->final_graph_cost_ << "\n"
    << "final_graph_image_cost " << this->final_graph_image_cost_ << "\n"
    << "final_graph_shape_cost " << this->final_graph_shape_cost_ << "\n"
    << "final_local_match_cost_break_down 0\n";

  for (dbsksp_shock_graph::edge_iterator eit = this->graph()->edges_begin();
    eit != this->graph()->edges_end(); ++eit)
  {
    if ((*eit)->is_terminal_edge())
      continue;

    dbsksp_shock_edge_sptr e = *eit;
    str << "edge_id " << e->id() << "\n"
      << "init_image_cost " << this->init_image_cost_map_.find(e)->second << "\n"
      << "init_shape_cost " << this->init_shape_cost_map_.find(e)->second << "\n"
      << "final_image_cost " << this->final_image_cost_map_.find(e)->second << "\n"
      << "final_shape_cost " << this->final_shape_cost_map_.find(e)->second << "\n";
  }


  str  << "bbox_xmin " << bbox->get_min_x() << "\n"
    << "bbox_ymin " << bbox->get_min_y() << "\n"
    << "bbox_xmax " << bbox->get_max_x() << "\n"
    << "bbox_ymax " << bbox->get_max_y() << "\n"
    << "final_xnode_map 0\n"
    << "number_of_nodes " << this->cur_xnode_map_.size() << "\n";
  for (vcl_map<dbsksp_shock_node_sptr, dbsksp_xshock_node_descriptor >::const_iterator
    it = this->cur_xnode_map_.begin(); it != this->cur_xnode_map_.end(); ++it)
  {
    dbsksp_shock_node_sptr v = it->first;
    dbsksp_xshock_node_descriptor xnode = it->second;
    str << v->id() << " "
      << xnode.pt().x() << " "
      << xnode.pt().y() << " "
      << xnode.psi_ << " "
      << xnode.phi_ << " "
      << xnode.radius_ << "\n";
  }
  return;
}


