// This is shp/dbsksp/algo/dbsksp_edit_distance.cxx


//:
// \file
// 



#include "dbsksp_edit_distance.h"


#include <dbsksp/algo/dbsksp_xshock_directed_tree.h>
#include <dbsksp/algo/dbsksp_xgraph_algos.h>
#include <dbskr/dbskr_dpmatch.h>
#include <dbskr/dbskr_sc_pair.h>
#include <dbsksp/dbsksp_xshock_graph.h>

#include <vnl/vnl_numeric_traits.h>
#include <vcl_set.h>

//==============================================================================
// dbsksp_edit_distance
//==============================================================================

//------------------------------------------------------------------------------
  //: Constructor
dbsksp_edit_distance::
dbsksp_edit_distance()
{
  this->scurve_matching_R_ = 6.0;
}


//------------------------------------------------------------------------------
//: Set tree 1
void dbsksp_edit_distance::
set_tree1(const dbsksp_xshock_directed_tree_sptr& tree1)
{
  this->tree1_ = tree1.ptr();
  return;
}


//------------------------------------------------------------------------------
//: Get tree1
dbsksp_xshock_directed_tree_sptr dbsksp_edit_distance::
tree1() const
{
  return static_cast<dbsksp_xshock_directed_tree* >(this->tree1_.ptr());;
}


//------------------------------------------------------------------------------
//: Set tree 2
void dbsksp_edit_distance::
set_tree2(const dbsksp_xshock_directed_tree_sptr& tree2)
{
  this->tree2_ = tree2.ptr();
  return;
}


//------------------------------------------------------------------------------
//: Get tree2
dbsksp_xshock_directed_tree_sptr dbsksp_edit_distance::
tree2() const
{
  return static_cast<dbsksp_xshock_directed_tree* >(this->tree2_.ptr());;;
}



//------------------------------------------------------------------------------
//: Set all params
void dbsksp_edit_distance::
set(const dbsksp_xshock_graph_sptr& xgraph1,
    const dbsksp_xshock_graph_sptr& xgraph2,
    float scurve_matching_R,
    float scurve_sample_ds)
{
  this->clear();

  // Convert xgraph to directed trees suitable for edit distance
  dbsksp_xshock_directed_tree_sptr tree1 = dbsksp_edit_distance::new_tree(xgraph1, scurve_matching_R, scurve_sample_ds);
  dbsksp_xshock_directed_tree_sptr tree2 = dbsksp_edit_distance::new_tree(xgraph2, scurve_matching_R, scurve_sample_ds);

  // Call edit distance
  this->set_tree1(tree1);
  this->set_tree2(tree2);
  this->set_scurve_matching_R(scurve_matching_R);
}



//------------------------------------------------------------------------------
//: find and cache the cost of matching curve pair induced by given dart paths
// \todo
float dbsksp_edit_distance::
get_cost(int td1, int d1, int td2, int d2)
{
  // form a key for this pair of shock branches
  pathtable_key key;
  key.first.first = tree1_->tail(td1);
  key.first.second = tree1_->head(d1);
  key.second.first = tree2_->tail(td2);
  key.second.second = tree2_->head(d2);

  pathtable_key key2;
  key2.first.first = key.first.second;
  key2.first.second = key.first.first;
  key2.second.first = key.second.second;
  key2.second.second = key.second.first;

  // Compute cost for this pair
  // If its cost has been cached, use it.
  // Otherwise, compute it using curve-matching style DP

  // place holder for the final cost
  float match_cost = vnl_numeric_traits<float>::maxval;
 
  vcl_map<pathtable_key, float>::iterator iter;
  iter = pathtable_.find(key);   // searching for either key or key2 is enough
  if (iter == pathtable_.end()) 
  {
    dbskr_sc_pair_sptr curve_pair1 = tree1_->get_curve_pair(td1, d1, true);
    dbskr_sc_pair_sptr curve_pair2 = tree2_->get_curve_pair(td2, d2, true);
    dbskr_scurve_sptr sc1 = curve_pair1->coarse;
    dbskr_scurve_sptr sc2 = curve_pair2->coarse;

    dbskr_dpmatch d(sc1, sc2);
    d.set_R(this->scurve_matching_R());

    // test if it is worth computing this match
    double approx_cost = d.approx_cost();



    d.Match();
    float init_dr = d.init_dr();
    float init_alp = d.init_phi();
    match_cost = float(d.finalCost() + init_dr + init_alp);

    vcl_vector<vcl_pair<int,int> > fmap=*(d.finalMap());    


    // Save the cost for this pair of shock curves

    // preparing the keys
    pathtable_key key_scm;
    key_scm.first.first = td1;
    key_scm.first.second = d1;
    key_scm.second.first = td2;
    key_scm.second.second = d2;

    pathtable_key key2_scm; // inverse of the key_scm
    key2_scm.first.first = key_scm.first.second;
    key2_scm.first.second = key_scm.first.first;
    key2_scm.second.first = key_scm.second.second;
    key2_scm.second.second = key_scm.second.first;

    // now we are ready to save the cost
    shock_curve_map_[key_scm] = fmap;
    shock_curve_map_[key2_scm] = fmap;

    pathtable_[key] = match_cost;
    pathtable_[key2] = match_cost;
  } 
  else 
  {
    match_cost = iter->second;
  } 
  return match_cost;
}


//------------------------------------------------------------------------------
//: Get final correspondence after computing optimal edit distance
// Return a list of pathtable_key, which maps a pair of darts in tree1 to a 
// a pair of darts in tree2. Each pair of darts, in return, represents a path
// in the trees.
void dbsksp_edit_distance::
get_final_correspondence(vcl_vector<pathtable_key >& final_correspondence)
{
  final_correspondence.clear();

  // sanity check
  if (this->final_a1_ < 0)
  {
    vcl_cout << "\nERROR: Paths are not computed, call edit() first! \n";
    return;
  }

  // Optimal correspondence between the two trees
  final_correspondence = this->CP_[this->final_a1_];
}


//------------------------------------------------------------------------------
//: Compute sum of deformation costs
float dbsksp_edit_distance::
get_deform_cost(const vcl_vector<pathtable_key >& correspondence)
{
  float cost_sum = 0.0f;

  for (unsigned k =0; k < correspondence.size(); ++k)
  {
    cost_sum += this->get_deform_cost(correspondence[k]);
  }
  return cost_sum;
}


//------------------------------------------------------------------------------
//: Compute deformation cost for a path correspondence
float dbsksp_edit_distance::
get_deform_cost(const pathtable_key& key)
{
  int tree1_d1 = key.first.first;
  int tree1_d2 = key.first.second;

  int tree2_d1 = key.second.first;
  int tree2_d2 = key.second.second;

  return this->get_cost(tree1_d1, tree1_d2, tree2_d1, tree2_d2);
}







//------------------------------------------------------------------------------
//: Construct a tree from an xgraph
dbsksp_xshock_directed_tree_sptr dbsksp_edit_distance::
new_tree(const dbsksp_xshock_graph_sptr& xgraph,
    float scurve_matching_R,
    float scurve_sample_ds)
{
  dbsksp_xshock_directed_tree_sptr tree = new dbsksp_xshock_directed_tree();
  tree->set_scurve_matching_R(scurve_matching_R);
  tree->set_scurve_sample_ds(scurve_sample_ds);
  tree->acquire(xgraph);
  return tree;
}

//------------------------------------------------------------------------------
// Get xgraph1 trimmed to match the common (coarse) topology
// This tree can only computed after (optimal) edit distance has been computed
dbsksp_xshock_graph_sptr dbsksp_edit_distance::
edit_xgraph1_to_common_topology() const
{
  //return this->trim_source_tree_to_common_topology(true);
  return this->apply_optimal_splice_edits(true);
}



//------------------------------------------------------------------------------
// Get xgraph2 trimmed to match the common (coarse) topology
// This tree can only computed after (optimal) edit distance has been computed
dbsksp_xshock_graph_sptr dbsksp_edit_distance::
edit_xgraph2_to_common_topology() const
{
  //return this->trim_source_tree_to_common_topology(false);
  return this->apply_optimal_splice_edits(false);
}









//------------------------------------------------------------------------------
//: Trim either of the source tree to the common topology shared between the two
// if `use_tree1' is false, tree2 is used as the original tree.
// \note The current method does NOT work if there is a contract operation in order
// to go from the original tree (tree1 or tree2) to the common graph. This will
// result in a disconnected tree (per current method)
// \todo Take care of contract operation.
dbsksp_xshock_graph_sptr dbsksp_edit_distance::
trim_source_tree_to_common_topology(bool use_tree1) const
{
  // sanity check
  if (this->final_a1_ < 0)
  {
    vcl_cout << "\nERROR: Paths are not computed, call edit() first! \n";
    return 0;
  }

  dbsksp_xshock_directed_tree_sptr orig_tree = use_tree1 ? this->tree1() : this->tree2();

  // Optimal correspondence between the two trees
  vcl_vector<pathtable_key > optimal_correspondence = this->CP_[this->final_a1_];

  // Start from original tree, then delete darts/ branches that no longer exist
  dbsksp_xshock_graph_sptr common_xgraph = new dbsksp_xshock_graph(*orig_tree->xgraph());

  // Now find out the list of edges and nodes to retain from the correspondence map
  typedef unsigned xgraph_edge_id;
  vcl_set<xgraph_edge_id > edge_ids_to_keep;

  for (unsigned k =0; k < optimal_correspondence.size(); ++k)
  {
    pathtable_key key = optimal_correspondence[k];

    int d1, d2; // startind and ending darts for the corresponding path
    if (use_tree1)
    {
      d1 = key.first.first;
      d2 = key.first.second;
    }
    else
    {
      d1 = key.second.first;
      d2 = key.second.second;
    }

    // list of shock edges corresponding to this path
    dbsksp_xshock_node_sptr start_node = 0;
    vcl_vector<dbsksp_xshock_edge_sptr > list_shock_edge;
    vcl_vector<int >& dart_path = orig_tree->get_dart_path(d1, d2);
    orig_tree->get_edge_list(dart_path, start_node, list_shock_edge);
    for (unsigned i =0; i < list_shock_edge.size(); ++i)
    {
      edge_ids_to_keep.insert(list_shock_edge[i]->id());
    }
  }

  // Determine list of edges to remove in the new graph
  vcl_vector<dbsksp_xshock_edge_sptr > list_shock_edge_to_remove;
  list_shock_edge_to_remove.reserve(common_xgraph->number_of_edges());
  for (dbsksp_xshock_graph::edge_iterator eit = common_xgraph->edges_begin(); eit !=
    common_xgraph->edges_end(); ++eit)
  {
    dbsksp_xshock_edge_sptr xe = *eit;

    // an edge will be removed if it is not meant to be kept
    if (edge_ids_to_keep.find(xe->id()) == edge_ids_to_keep.end()) 
    {
      list_shock_edge_to_remove.push_back(xe);
    }
  }

  // Now actually remove the shock edges from the shock graph
  for (unsigned i =0; i < list_shock_edge_to_remove.size(); ++i)
  {
    dbsksp_xshock_edge_sptr xe = list_shock_edge_to_remove[i];

    // first disconnect it from its two end nodes
    dbsksp_xshock_node_sptr xv_source = xe->source();
    dbsksp_xshock_node_sptr xv_target = xe->target();
    xv_source->remove_shock_edge(xe);
    xv_target->remove_shock_edge(xe);

    // remove the edge from graph
    common_xgraph->remove_edge(xe);

    // if either of the end nodes becomes disconnected, remove it from the graph
    if (xv_source->degree() ==0)
    {
      common_xgraph->remove_vertex(xv_source);
    }

    if (xv_target->degree() ==0)
    {
      common_xgraph->remove_vertex(xv_target);
    }
  }

  // Verify that the remaining shock graph is still a connected tree
  
  
  if (common_xgraph->number_of_vertices() != (common_xgraph->number_of_edges() +1))
  {
    vcl_cerr << "\nERROR: in dbsksp_edit_distance::trim_source_tree_to_common_topology() - "
      << " the remaining (common) graph is not a tree. Return 0";
    return 0;
  }

  // Close all the gaps in the resulting tree
  dbsksp_xgraph_algos::close_angle_gaps_at_nodes(common_xgraph);

  return common_xgraph;
}










//------------------------------------------------------------------------------
//: Apply splice edits (after edit distance have been computed) on source tree
// if `use_tree1' is false, tree2 is used as the original tree.
dbsksp_xshock_graph_sptr dbsksp_edit_distance::
apply_optimal_splice_edits(bool use_tree1) const
{
  // sanity check
  if (this->final_a1_ < 0)
  {
    vcl_cout << "\nERROR: Paths are not computed, call edit() first! \n";
    return 0;
  }

  dbsksp_xshock_directed_tree_sptr orig_tree = use_tree1 ? this->tree1() : this->tree2();

  // Optimal correspondence between the two trees
  vcl_vector<pathtable_key > optimal_correspondence = this->CP_[this->final_a1_];

  // Start from original tree, then delete darts/ branches that no longer exist
  dbsksp_xshock_graph_sptr common_xgraph = new dbsksp_xshock_graph(*orig_tree->xgraph());

  // Now find out the list of edges and nodes to retain from the correspondence map
  typedef unsigned xgraph_edge_id;
  vcl_set<xgraph_edge_id > edge_ids_to_keep;

  for (unsigned k =0; k < optimal_correspondence.size(); ++k)
  {
    pathtable_key key = optimal_correspondence[k];

    int d1, d2; // startind and ending darts for the corresponding path
    if (use_tree1)
    {
      d1 = key.first.first;
      d2 = key.first.second;
    }
    else
    {
      d1 = key.second.first;
      d2 = key.second.second;
    }

    // list of shock edges corresponding to this path
    dbsksp_xshock_node_sptr start_node = 0;
    vcl_vector<dbsksp_xshock_edge_sptr > list_shock_edge;
    vcl_vector<int >& dart_path = orig_tree->get_dart_path(d1, d2);
    orig_tree->get_edge_list(dart_path, start_node, list_shock_edge);
    for (unsigned i =0; i < list_shock_edge.size(); ++i)
    {
      edge_ids_to_keep.insert(list_shock_edge[i]->id());
    }
  }

  // Determine list of edges to remove in the new graph
  vcl_list<dbsksp_xshock_edge_sptr > list_shock_edge_to_remove;
  for (dbsksp_xshock_graph::edge_iterator eit = common_xgraph->edges_begin(); eit !=
    common_xgraph->edges_end(); ++eit)
  {
    dbsksp_xshock_edge_sptr xe = *eit;

    // an edge will be removed if it is not meant to be kept
    if (edge_ids_to_keep.find(xe->id()) == edge_ids_to_keep.end()) 
    {
      list_shock_edge_to_remove.push_back(xe);
    }
  }

  // Now actually remove the shock edges from the shock graph
  // Since splice operations only apply to leaf edges, we only remove an edge
  // when it is a terminal (leaf) edge

  while (!list_shock_edge_to_remove.empty())
  {
    // Find a terminal edge 
    vcl_list<dbsksp_xshock_edge_sptr >::iterator eit = list_shock_edge_to_remove.begin();
    for (; eit != list_shock_edge_to_remove.end() && !(*eit)->is_terminal_edge(); ++eit);

    // if there is no remaining terminal edge (leaf edge), we are done
    if (eit == list_shock_edge_to_remove.end())
      break;

    dbsksp_xshock_edge_sptr xe = *eit;
    list_shock_edge_to_remove.erase(eit);
    
    // Find the non-leaf node
    dbsksp_xshock_node_sptr xv = (xe->source()->degree()==1) ? 
                                 xe->target() : xe->source();

    // Remove terminal edge from the graph
    // There are two cases: degree-2 and degree-3 nodes
    switch (xv->degree())
    {
    case 2:
      common_xgraph->remove_leaf_edge_at_A12_node(xe);
      break;
    case 3:
      common_xgraph->remove_A_infty_branch(xv, xe, 0.5);
      break;
    default:
      vcl_cout << "\nERROR: can't handle non-leaf node with degree = " << xv->degree() << "\n";
      break;
    }
  }

  // Verify that the remaining shock graph is still a connected tree  
  if (common_xgraph->number_of_vertices() != (common_xgraph->number_of_edges() +1))
  {
    vcl_cerr << "\nERROR: in dbsksp_edit_distance::apply_optimal_splice_edits() - "
      << " the remaining (common) graph is not a tree. Return 0";
    return 0;
  }
  return common_xgraph;
}





