// This is file shp/dbsksp/dbsksp_morph_shock_graph_different_topology.cxx

//:
// \file

#include "dbsksp_morph_shock_graph_different_topology.h"
#include <dbsksp/dbsksp_shock_graph.h>
#include <dbsksp/dbsksp_shock_edge.h>
#include <dbsksp/dbsksp_shock_node.h>
#include <dbsksp/dbsksp_shock_model.h>
#include <vcl_utility.h>

#include <dbskr/dbskr_sm_cor.h>
#include <dbskr/dbskr_compute_scurve.h>
#include <dbskr/dbskr_scurve.h>
#include <dbsksp/algo/dbsksp_build_from_xshock_graph.h>
#include <dbsksp/algo/dbsksp_morph_shock_graph_same_topology.h>


//: Morphing the two shapes
// Building the data structure allowing for morphing
bool dbsksp_morph_shock_graph_different_topology::
morph()
{
  // Strategy: construct the intermediate shock graphs
  // Which can be morphed using the same topology shock graph morphing
  dbskr_tree_sptr tree1 = this->sm_cor()->get_tree1();
  dbskr_tree_sptr tree2 = this->sm_cor()->get_tree2();

  this->sm_cor()->recover_dart_ids_and_scurves();

  
  // Determine which darts will survive and which darts will be removed
  this->compute_surviving_darts();

  // Compute the number of samples that will be used to interpolate the darts in the tree
  this->compute_num_segments_to_interpolate_trees(this->shock_interp_sample_ds_);

  // Generate sequence of shock graphs to go trom tree 1 to the common tree

  // Get two reference darts from the two trees
  pathtable_key ref_key = this->sm_cor()->get_map()[2];
  int ref_dart_tree1 = ref_key.first.first;
  int ref_dart_tree2 = ref_key.second.first;

  // Generate transition table for each tree
  
  //// Generate transition table for each tree
  //this->generate_intermediate_graphs( tree1, 
  //  ref_dart_tree1,
  //  this->used_darts_tree1_,
  //  this->num_segments_tree1_,
  //  this->transitions_tree1_);
  //
  //// Generate transition table for each tree
  //this->generate_intermediate_graphs( tree2, 
  //  ref_dart_tree2,
  //  this->used_darts_tree2_,
  //  this->num_segments_tree2_,
  //  this->transitions_tree2_);


  //: compute final common graph
  generate_final_common_graph();

  return true;
} 




// Determine which darts will survive and which darts will be removed
void dbsksp_morph_shock_graph_different_topology::
compute_surviving_darts()
{
  // resize the survival list of the two trees to proper size
  this->used_darts_tree1_.resize(this->sm_cor()->get_tree1()->size(), false);
  this->used_darts_tree2_.resize(this->sm_cor()->get_tree2()->size(), false);

  // retrieve the final mapping between two trees
  vcl_vector<pathtable_key> dart_path_map = this->sm_cor()->get_map();

  // for convenenience
  dbskr_tree_sptr tree1 = this->sm_cor()->get_tree1();
  dbskr_tree_sptr tree2 = this->sm_cor()->get_tree2();

  for (unsigned int path_id = 0; path_id < dart_path_map.size(); path_id++) 
  {
    // make sure we understand what each term in the pathtable_key means
    pathtable_key key = dart_path_map[path_id];
    int tree1_start_dart = key.first.first;
    int tree1_end_dart = key.first.second;
    int tree2_start_dart = key.second.first;
    int tree2_end_dart = key.second.second;

    // get the dart list associated with this path
    // These are the surviving darts
    // Tree 1
    vcl_vector<int > sub_list1 = tree1->get_dart_path(tree1_start_dart, tree1_end_dart);
    for (unsigned i=0; i < sub_list1.size(); ++i)
    {
      this->used_darts_tree1_[sub_list1.at(i)] = true;
    }
    
    // Tree 2
    vcl_vector<int > sub_list2 = tree2->get_dart_path(tree2_start_dart, tree2_end_dart);
    for (unsigned i=0; i < sub_list2.size(); ++i)
    {
      this->used_darts_tree2_[sub_list2.at(i)] = true;
    }    
  }
}








// ----------------------------------------------------------------------------
//: Generate intermediate trees, given a tree and a survial list
// intermediate_graphs is a nx2 array
// n is the number of times the graph changes topology
// for each pair = intermediate_graphs[i];
// pair.first: starting graph
// pair.second: ending graph
// these two should exact have the same topology
// Between two consecutive pairs pair_i and pair_ip1,
// pair_i.second and pair_ip1.second should have exact same geometry
// i.e. a shock transition occurs between them
void dbsksp_morph_shock_graph_different_topology::
generate_intermediate_graphs(const dbskr_tree_sptr& tree,
                             int ref_dart,
                             const vcl_vector<bool > used_darts,
                             const vcl_vector<int >& num_segments_to_interpolate,
  vcl_vector<dbsksp_shock_graph_sptr > & transition_graphs)
{
  transition_graphs.clear();

  // >> Build the initial shock graph
  dbsksp_build_from_xshock_graph builder;
  builder.set_scurve_sample_ds(this->scurve_sample_ds_);
  builder.set_shock_interp_sample_ds(this->shock_interp_sample_ds_);

  //dbsksp_shock_graph_sptr graph = builder.build_from_skr_tree(tree);
  dbsksp_shock_graph_sptr graph = builder.build_from_skr_tree(tree,
    num_segments_to_interpolate);

  // set the reference node and edge properly
  vcl_vector<dbsksp_shock_edge_sptr > elist = builder.get_shock_edges_of_dart(ref_dart);
  if (!elist.empty())
  {
    dbsksp_shock_edge_sptr ref_edge = elist.front();
    dbsksp_shock_node_sptr ref_node = ref_edge->target();
    graph->set_all_ref(ref_node, ref_edge);
  }
  else
  {
    elist = builder.get_shock_edges_of_dart(tree->mate(ref_dart));
    assert(!elist.empty());

    dbsksp_shock_edge_sptr ref_edge = elist.back();
    dbsksp_shock_node_sptr ref_node = ref_edge->source();
    graph->set_all_ref(ref_node, ref_edge);
  }

  // save the initial graph
  dbsksp_shock_graph_sptr start_graph = new dbsksp_shock_graph(*graph);
  start_graph->compute_all_dependent_params();
  transition_graphs.push_back(start_graph);

  // Now deform it, i.e. take away piece by piece
  // 1) Construct generative shock graph equivalent to the tree 
  // iterate over the darts of this tree to reconstruct everything
  // this is actually an Euler tour because of the way the tree is constructed
  for (int dart=0; dart < tree->size(); dart++)
  {
    if (tree->mate(dart)<dart)
      continue; //already taken care of from the other side

    // only need to handle darts that won't survive
    if (used_darts[dart] || used_darts[tree->mate(dart)])
      continue;

    // Now we have to remove this dart from the shock graph
    

    // find out which edges need to be removed
    vcl_vector<dbsksp_shock_edge_sptr > edges_to_remove = 
      builder.get_shock_edges_of_dart(dart);
    vcl_vector<dbsksp_shock_node_sptr > nodes_to_remove = 
      builder.get_shock_nodes_of_dart(dart);

    if (edges_to_remove.empty())
    {
      vcl_cerr << "ERROR: No edges correspond to dart = " << dart << vcl_endl;  
    }

    assert(!edges_to_remove.empty());

    // check whether this is a leave-branch or an internal branch
    bool is_leaf_branch = false;
    // need_reverse_order = true if this is a leaf branch and 
    // the first node is not connected to a terminal edge
    bool need_reverse_order = false;  

    if (nodes_to_remove.front()->connected_to_terminal_edge())
    {
      is_leaf_branch = true;
      need_reverse_order = false;
    }
    else if (nodes_to_remove.back()->connected_to_terminal_edge())
    {
      is_leaf_branch = true;
      need_reverse_order = true;
    }
    else
    {
      is_leaf_branch = false;
      need_reverse_order = false;
    }

    // Compute final angle phi that all edges will have
    // At the end we will have
    // final_phi = nodes_to_remove(0)->descriptor(edges_to_remove(0).phi
    double final_phi = 0;
    dbsksp_shock_node_sptr start_node = nodes_to_remove.front();
    dbsksp_shock_edge_sptr start_edge = edges_to_remove.front();
    assert(start_edge->is_vertex(start_node));

    dbsksp_shock_node_sptr end_node = nodes_to_remove.back();
    dbsksp_shock_edge_sptr end_edge = edges_to_remove.back();
    assert(end_edge->is_vertex(end_node));
    if (is_leaf_branch)
    {
      if (need_reverse_order)
      {
        final_phi = start_node->descriptor(start_edge)->phi;
      }
      else
      {
        final_phi = vnl_math::pi - end_node->descriptor(end_edge)->phi;
      }
    }
    else
    {
      // compute final_phi
      final_phi = (start_node->descriptor(start_edge)->phi + 
        (vnl_math::pi - end_node->descriptor(end_edge)->phi)) / 2;
    }

    // Now adjust the phi's of the edges
    for (unsigned i=0; i<edges_to_remove.size(); ++i)
    {
      dbsksp_shock_node_sptr node = nodes_to_remove[i];
      dbsksp_shock_edge_sptr edge = edges_to_remove[i];
      node->change_phi_distribute_difference_uniformly(edge, final_phi);

      // adjust properties of edges
      edge->set_chord_length(this->len_epsilon);
      edge->set_param_m(0.0);
    }

    // handle the last node differently, the angle should be pi-final_phi
    end_node->change_phi_distribute_difference_uniformly(end_edge, vnl_math::pi - final_phi);  

    // Now the graph has been modified properply, insert it to the transition table
    dbsksp_shock_graph_sptr end_graph = new dbsksp_shock_graph(*graph);
    end_graph->compute_all_dependent_params();
    transition_graphs.push_back(end_graph);

    
    // >> Now we remove the edges to get a new shock graph, same geometry but different topology
    if (is_leaf_branch)
    {
      dbsksp_shock_node_sptr v0 = 0;
      if (need_reverse_order)
      {
        for (vcl_vector<dbsksp_shock_edge_sptr >::reverse_iterator itr = 
          edges_to_remove.rbegin(); itr != edges_to_remove.rend(); ++itr)
        {
          dbsksp_shock_edge_sptr e = *itr;
          v0 = graph->remove_leaf_A_1_2_edge(e);
          if (!v0)
          {
            vcl_cerr << "ERROR: could not remove leaf edge.\n";
          }
        }
      }
      else
      {
        for (unsigned i=0; i < edges_to_remove.size(); ++i)
        {
          dbsksp_shock_edge_sptr e = edges_to_remove[i];
          v0 = graph->remove_leaf_A_1_2_edge(e);
          if (!v0)
          {
            vcl_cerr << "ERROR: could not remove leaf edge.\n";
          }
        }
      }

      // find the terminal edge and remove it
      for (vcl_list<dbsksp_shock_edge_sptr >::const_iterator itr = v0->edge_list().begin();
        itr != v0->edge_list().end(); ++itr)
      {
        dbsksp_shock_edge_sptr e = *itr;
        if (e->is_terminal_edge())
        {
          v0->change_phi_distribute_difference_uniformly(e, 0);
          graph->remove_A_infty_edge(e->opposite(v0));
          break;
        }
      }
    
    }
    else
    {
      for (unsigned i=0; i < edges_to_remove.size(); ++i)
      {
        dbsksp_shock_edge_sptr e = edges_to_remove[i];
        bool ok = graph->remove_internal_edge(e);
        if (!ok)
        {
          vcl_cerr << "ERROR: could not remove internal edge.\n";
        }
      }
    }

    // Save the new shock graph
    dbsksp_shock_graph_sptr new_start_graph = new dbsksp_shock_graph(*graph);
    new_start_graph->compute_all_dependent_params();
    transition_graphs.push_back(new_start_graph);
    
  }
}



// ----------------------------------------------------------------------------
//: Generate intermediate trees, given a tree and a survial list
// intermediate_graphs is a nx2 array
// n is the number of times the graph changes topology
void dbsksp_morph_shock_graph_different_topology::
generate_morph_sequence(const dbskr_tree_sptr& tree,
  int ref_dart,
  const vcl_vector<bool > used_darts,
  const vcl_vector<int >& num_segments_to_interpolate,
  vcl_vector<dbsksp_morph_segment >& morph_sequence)
{
  

  return;




}















//: compute final common graph
void dbsksp_morph_shock_graph_different_topology::
generate_final_common_graph()
{
  dbskr_tree_sptr tree1 = this->sm_cor()->get_tree1();
  dbskr_tree_sptr tree2 = this->sm_cor()->get_tree2();

  // Get two reference darts from the two trees
  vcl_vector<pathtable_key > key_map = this->sm_cor()->get_map();

  // something in the middle should approximate the size better
  pathtable_key ref_key = key_map[key_map.size()/2];
  int ref_dart_tree1 = ref_key.first.first;
  int ref_dart_tree2 = ref_key.second.first;

  // Generate transition table for each tree
  dbsksp_shock_graph_sptr tree1_start1;
  dbsksp_shock_graph_sptr tree1_end1;
  vcl_map<dbsksp_shock_edge_sptr, dbsksp_shock_edge_sptr> tree1_end1_to_start1;
  
  this->generate_edited_graph( tree1, ref_dart_tree1, this->used_darts_tree1_,
    this->num_segments_tree1_,
    false,
    tree1_start1,
    tree1_end1,
    tree1_end1_to_start1);

  // Cache the initial graph
  this->tree1_init_graph_ = tree1_start1;

  
  dbsksp_shock_graph_sptr tree1_start2;
  dbsksp_shock_graph_sptr tree1_end2;
  vcl_map<dbsksp_shock_edge_sptr, dbsksp_shock_edge_sptr> tree1_end2_to_start2;
  
  this->generate_edited_graph( tree1, ref_dart_tree1, this->used_darts_tree1_,
    this->num_segments_tree1_,
    true,
    tree1_start2,
    tree1_end2,
    tree1_end2_to_start2);

  // We want to map edges of tree1_end_2 to edges of tree1_end_1;
  // To do this we establish a mapping between tree1_start_1 and tree1_end_1
  // using same_topology_morpher

  // Compute edge correspondence map between the two graphs
  dbsksp_morph_shock_graph_same_topology morpher;
  morpher.set_source_graph(tree1_start2);
  morpher.set_target_graph(tree1_start1);
  bool ok = morpher.compute_correspondence();
  assert(ok);
  morpher.morph();
  vcl_map<dbsksp_shock_edge_sptr, dbsksp_shock_edge_sptr> tree1_start2_to_start1;
  tree1_start2_to_start1 = morpher.edge_map();


  // We now compute start_1_to_end_1
  morpher.set_source_graph(tree1_start1);
  morpher.set_target_graph(tree1_end1);
  ok = morpher.compute_correspondence();
  assert(ok);
  vcl_map<dbsksp_shock_edge_sptr, dbsksp_shock_edge_sptr> tree1_start1_to_end1;
  tree1_start1_to_end1 = morpher.edge_map();

  // Now we are ready to compute end2_to_end1
  vcl_map<dbsksp_shock_edge_sptr, dbsksp_shock_edge_sptr> tree1_end2_to_end1;
  for (vcl_map<dbsksp_shock_edge_sptr, dbsksp_shock_edge_sptr>::iterator itr =
    tree1_end2_to_start2.begin(); itr != tree1_end2_to_start2.end(); ++itr)
  {
    dbsksp_shock_edge_sptr e0 = itr->first;
    dbsksp_shock_edge_sptr e1 = itr->second;
    dbsksp_shock_edge_sptr e2 = tree1_start2_to_start1.find(e1)->second;
    dbsksp_shock_edge_sptr e3 = tree1_start1_to_end1.find(e2)->second;
    tree1_end2_to_end1.insert(vcl_make_pair(e0, e3));  
  }




  // DO SIMILAR THING FOR TREE2
  // This is REALLY bad to repeat the node like this. But this hacking time ...


  // Generate transition table for each tree
  dbsksp_shock_graph_sptr tree2_start1;
  dbsksp_shock_graph_sptr tree2_end1;
  vcl_map<dbsksp_shock_edge_sptr, dbsksp_shock_edge_sptr> tree2_end1_to_start1;
  
  this->generate_edited_graph( tree2, ref_dart_tree2, this->used_darts_tree2_,
    this->num_segments_tree2_,
    false,
    tree2_start1,
    tree2_end1,
    tree2_end1_to_start1);

  // Cache the initial graph
  this->tree2_init_graph_ = tree2_start1;


  
  dbsksp_shock_graph_sptr tree2_start2;
  dbsksp_shock_graph_sptr tree2_end2;
  vcl_map<dbsksp_shock_edge_sptr, dbsksp_shock_edge_sptr> tree2_end2_to_start2;
  
  this->generate_edited_graph( tree2, ref_dart_tree2, this->used_darts_tree2_,
    this->num_segments_tree2_,
    true,
    tree2_start2,
    tree2_end2,
    tree2_end2_to_start2);

  // We want to map edges of tree2_end_2 to edges of tree2_end_1;
  // To do this we establish a mapping between tree2_start_1 and tree2_end_1
  // using same_topology_morpher

  // Compute edge correspondence map between the two graphs
  
  morpher.set_source_graph(tree2_start2);
  morpher.set_target_graph(tree2_start1);
  ok = morpher.compute_correspondence();
  assert(ok);
  morpher.morph();
  vcl_map<dbsksp_shock_edge_sptr, dbsksp_shock_edge_sptr> tree2_start2_to_start1;
  tree2_start2_to_start1 = morpher.edge_map();


  // We now compute start_1_to_end_1
  morpher.set_source_graph(tree2_start1);
  morpher.set_target_graph(tree2_end1);
  ok = morpher.compute_correspondence();
  assert(ok);
  vcl_map<dbsksp_shock_edge_sptr, dbsksp_shock_edge_sptr> tree2_start1_to_end1;
  tree2_start1_to_end1 = morpher.edge_map();

  // Now we are ready to compute end2_to_end1
  vcl_map<dbsksp_shock_edge_sptr, dbsksp_shock_edge_sptr> tree2_end2_to_end1;
  for (vcl_map<dbsksp_shock_edge_sptr, dbsksp_shock_edge_sptr>::iterator itr =
    tree2_end2_to_start2.begin(); itr != tree2_end2_to_start2.end(); ++itr)
  {
    dbsksp_shock_edge_sptr e0 = itr->first;
    dbsksp_shock_edge_sptr e1 = itr->second;
    dbsksp_shock_edge_sptr e2 = tree2_start2_to_start1.find(e1)->second;
    dbsksp_shock_edge_sptr e3 = tree2_start1_to_end1.find(e2)->second;
    tree2_end2_to_end1.insert(vcl_make_pair(e0, e3));  
  }



  // Now merge tree2_end2 and tree1_end2 to get a common graph for two sides
  morpher.set_source_graph(tree1_end2);
  morpher.set_target_graph(tree2_end2);
  ok = morpher.compute_correspondence();
  if (!ok)
  {
    vcl_cerr << "ERROR: end graphs do not have a common topology.\n";
    return;
  }

  morpher.morph();

  // mean graph - the common topology graph between the two trees
  dbsksp_shock_graph_sptr mean_graph_1 = morpher.get_intermediate_graph(0.5);


  // Now merge tree2_end2 and tree1_end2 to get a common graph for two sides
  morpher.set_source_graph(tree2_end2);
  morpher.set_target_graph(tree1_end2);
  ok = morpher.compute_correspondence();
  if (!ok)
  {
    vcl_cerr << "ERROR: end graphs do not have a common topology.\n";
    return;
  }

  morpher.morph();

  // mean graph - the common topology graph between the two trees
  dbsksp_shock_graph_sptr mean_graph_2 = morpher.get_intermediate_graph(0.5);


  this->common_topology_graph_ = mean_graph_1;

  
  // Now we need to propagate the data from mean_graph to tree1_end1 and tree2_end1
  // TREE 1
  // Find the mapping the match
  morpher.set_source_graph(mean_graph_1);
  morpher.set_target_graph(tree1_end2);
  ok = morpher.compute_correspondence();
  assert(ok);
  vcl_map<dbsksp_shock_edge_sptr, dbsksp_shock_edge_sptr> mean_to_tree1_end2;
  mean_to_tree1_end2 = morpher.edge_map();

  // compute map from mean graph to tree1_end1
  vcl_map<dbsksp_shock_edge_sptr, dbsksp_shock_edge_sptr> mean_to_tree1_end1;
  for (vcl_map<dbsksp_shock_edge_sptr, dbsksp_shock_edge_sptr>::iterator itr =
    mean_to_tree1_end2.begin(); itr != mean_to_tree1_end2.end(); ++itr)
  {
    dbsksp_shock_edge_sptr e0 = itr->first;
    dbsksp_shock_edge_sptr e1 = itr->second;
    dbsksp_shock_edge_sptr e2 = tree1_end2_to_end1.find(e1)->second;
    mean_to_tree1_end1.insert(vcl_make_pair(e0, e2));  
  }
  this->propagate_data(mean_graph_1, tree1_end1, mean_to_tree1_end1);
  tree1_end1->compute_all_dependent_params();
  this->tree1_mean_graph_ = tree1_end1;



  // TREE 2
  // Find the mapping the match
  morpher.set_source_graph(mean_graph_2);
  morpher.set_target_graph(tree2_end2);
  ok = morpher.compute_correspondence();
  assert(ok);
  vcl_map<dbsksp_shock_edge_sptr, dbsksp_shock_edge_sptr> mean_to_tree2_end2;
  mean_to_tree2_end2 = morpher.edge_map();

  // compute map from mean graph to tree1_end1
  vcl_map<dbsksp_shock_edge_sptr, dbsksp_shock_edge_sptr> mean_to_tree2_end1;
  for (vcl_map<dbsksp_shock_edge_sptr, dbsksp_shock_edge_sptr>::iterator itr =
    mean_to_tree2_end2.begin(); itr != mean_to_tree2_end2.end(); ++itr)
  {
    dbsksp_shock_edge_sptr e0 = itr->first;
    dbsksp_shock_edge_sptr e1 = itr->second;
    dbsksp_shock_edge_sptr e2 = tree2_end2_to_end1.find(e1)->second;
    mean_to_tree2_end1.insert(vcl_make_pair(e0, e2));  
  }

  this->propagate_data(mean_graph_2, tree2_end1, mean_to_tree2_end1);
  tree2_end1->compute_all_dependent_params();
  this->tree2_mean_graph_ = tree2_end1;




}




// ----------------------------------------------------------------------------
//: Propagate data from source to target, given an edge map from source to target
void dbsksp_morph_shock_graph_different_topology::
propagate_data(const dbsksp_shock_graph_sptr& child,
               const dbsksp_shock_graph_sptr& parent,
  const vcl_map<dbsksp_shock_edge_sptr, dbsksp_shock_edge_sptr>& child_to_parent)
{
  // First propagate the edge properties
  for (vcl_map<dbsksp_shock_edge_sptr, dbsksp_shock_edge_sptr>::const_iterator itr =
    child_to_parent.begin(); itr != child_to_parent.end(); ++itr)
  {
    dbsksp_shock_edge_sptr e_child = itr->first;
    dbsksp_shock_edge_sptr e_parent = itr->second;

    // edge informatin
    if (!e_child->is_terminal_edge())
    {
      e_parent->set_chord_length(e_child->chord_length());
      e_parent->set_param_m(e_child->param_m());
    }

    // Node information

    // Assuming the source and target  still correspond
    dbsksp_shock_node_sptr child_source = e_child->source();
    dbsksp_shock_node_sptr child_target = e_child->target();

    dbsksp_shock_node_sptr parent_source = e_parent->source();
    dbsksp_shock_node_sptr parent_target = e_parent->target();

    // no interest in degree-one nodes
    if (child_source->degree() > 1)
    {
      assert(parent_source->degree() > 1);
      parent_source->descriptor(e_parent)->phi = child_source->descriptor(e_child)->phi;
    }
    else
    {
      assert (parent_source->degree() == 1);
    }

    if (child_target->degree() > 1)
    {
      assert(parent_target->degree() > 1);
      parent_target->descriptor(e_parent)->phi = child_target->descriptor(e_child)->phi;
    }
    else
    {
      assert(parent_target->degree() == 1);
    }
  }

  // Propagate reference information
  dbsksp_shock_edge_sptr child_ref_edge = child->ref_edge();
  dbsksp_shock_node_sptr child_ref_node = child->ref_node();


  dbsksp_shock_edge_sptr parent_ref_edge = child_to_parent.find(child_ref_edge)->second;
  dbsksp_shock_node_sptr parent_ref_node;
  if (child_ref_node == child_ref_edge->source())
  {
    parent_ref_node = parent_ref_edge->source();
  }
  else
  {
    parent_ref_node = parent_ref_edge->target();
  }

  parent->set_ref_node(parent_ref_node);
  parent->set_ref_edge(parent_ref_edge);
  parent->set_ref_origin(child->ref_origin());
  parent->set_ref_direction(child->ref_direction());
  parent->set_ref_node_radius(child->ref_node_radius());

  return;


}












//: Generate final common shock graph of the matching
void dbsksp_morph_shock_graph_different_topology::
generate_edited_graph(const dbskr_tree_sptr& tree,
  int ref_dart,
  const vcl_vector<bool > used_darts,
  const vcl_vector<int >& num_segments_to_interpolate,
  bool real_edge_deletion,
  dbsksp_shock_graph_sptr& start_graph,
  dbsksp_shock_graph_sptr& final_graph,
  vcl_map<dbsksp_shock_edge_sptr, dbsksp_shock_edge_sptr>& final_to_start_edge_map)
{
  // >> Build the initial shock graph
  dbsksp_build_from_xshock_graph builder;
  builder.set_scurve_sample_ds(this->scurve_sample_ds_);
  builder.set_shock_interp_sample_ds(this->shock_interp_sample_ds_);

  //dbsksp_shock_graph_sptr graph = builder.build_from_skr_tree(tree);
  dbsksp_shock_graph_sptr graph = builder.build_from_skr_tree(tree,
    num_segments_to_interpolate);

  // set the reference node and edge properly



  // set the reference node and edge properly
  vcl_vector<dbsksp_shock_edge_sptr > elist = builder.get_shock_edges_of_dart(ref_dart);
  if (!elist.empty())
  {
    dbsksp_shock_edge_sptr ref_edge = elist.front();
    dbsksp_shock_node_sptr ref_node = ref_edge->target();
    graph->set_all_ref(ref_node, ref_edge);
  }
  else
  {
    elist = builder.get_shock_edges_of_dart(tree->mate(ref_dart));
    assert(!elist.empty());

    dbsksp_shock_edge_sptr ref_edge = elist.back();
    dbsksp_shock_node_sptr ref_node = ref_edge->source();
    graph->set_all_ref(ref_node, ref_edge);
  }








  //dbsksp_shock_edge_sptr ref_edge = builder.get_shock_edges_of_dart(ref_dart).front();
  //dbsksp_shock_node_sptr ref_node = ref_edge->target();
  //graph->set_all_ref(ref_node, ref_edge);

  // save the initial graph
  start_graph = new dbsksp_shock_graph(*graph);
  start_graph->compute_all_dependent_params();

  // Compute edge correspondence map between the two graphs
  dbsksp_morph_shock_graph_same_topology morpher;
  morpher.set_source_graph(graph);
  morpher.set_target_graph(start_graph);
  bool ok = morpher.compute_correspondence();
  assert(ok);
  morpher.morph();
  final_to_start_edge_map = morpher.edge_map();


  // Now deform it, i.e. take away piece by piece
  // 1) Construct generative shock graph equivalent to the tree 
  // iterate over the darts of this tree to reconstruct everything
  // this is actually an Euler tour because of the way the tree is constructed
  for (int dart=0; dart < tree->size(); dart++)
  {
    if (tree->mate(dart)<dart)
      continue; //already taken care of from the other side

    // only need to handle darts that won't survive
    if (used_darts[dart] || used_darts[tree->mate(dart)])
      continue;

    // Now we have to remove this dart from the shock graph
    

    // find out which edges need to be removed
    vcl_vector<dbsksp_shock_edge_sptr > edges_to_remove = 
      builder.get_shock_edges_of_dart(dart);
    vcl_vector<dbsksp_shock_node_sptr > nodes_to_remove;
      //builder.get_shock_nodes_of_dart(dart);
    // get nodes_to_remove from the edges rather than from the builder
    // because some of the nodes may not exist
    dbsksp_shock_node_sptr cur_node = edges_to_remove[0]->opposite(
      edges_to_remove[0]->shared_vertex(*edges_to_remove[1]));
    nodes_to_remove.push_back(cur_node);
    for (unsigned i=0; i<edges_to_remove.size(); ++i)
    {
      dbsksp_shock_edge_sptr e = edges_to_remove[i];
      cur_node = e->opposite(cur_node);
      nodes_to_remove.push_back(cur_node);
    }

    if (edges_to_remove.empty())
    {
      vcl_cerr << "ERROR: No edges correspond to dart = " << dart << vcl_endl;  
    }

    assert(!edges_to_remove.empty());

    // check whether this is a leave-branch or an internal branch
    bool is_leaf_branch = false;
    // need_reverse_order = true if this is a leaf branch and 
    // the first node is not connected to a terminal edge
    bool need_reverse_order = false;  

    if (nodes_to_remove.front()->connected_to_terminal_edge())
    {
      is_leaf_branch = true;
      need_reverse_order = false;
    }
    else if (nodes_to_remove.back()->connected_to_terminal_edge())
    {
      is_leaf_branch = true;
      need_reverse_order = true;
    }
    else
    {
      is_leaf_branch = false;
      need_reverse_order = false;
    }

    // Compute final angle phi that all edges will have
    // At the end we will have
    // final_phi = nodes_to_remove(0)->descriptor(edges_to_remove(0).phi
    double final_phi = 0;
    dbsksp_shock_node_sptr start_node = nodes_to_remove.front();
    dbsksp_shock_edge_sptr start_edge = edges_to_remove.front();
    assert(start_edge->is_vertex(start_node));

    dbsksp_shock_node_sptr end_node = nodes_to_remove.back();
    dbsksp_shock_edge_sptr end_edge = edges_to_remove.back();
    assert(end_edge->is_vertex(end_node));
    if (is_leaf_branch)
    {
      if (need_reverse_order)
      {
        // final_phi = start_node->descriptor(start_edge)->phi;
        final_phi = 0.001;
      }
      else
      {
        // final_phi = vnl_math::pi - end_node->descriptor(end_edge)->phi;
        final_phi = vnl_math::pi - 0.001;
      }
    }
    else
    {
      // compute final_phi
      final_phi = (start_node->descriptor(start_edge)->phi + 
        (vnl_math::pi - end_node->descriptor(end_edge)->phi)) / 2;
    }

    // Now adjust the phi's of the edges
    for (unsigned i=0; i<edges_to_remove.size(); ++i)
    {
      dbsksp_shock_node_sptr node = nodes_to_remove[i];
      dbsksp_shock_edge_sptr edge = edges_to_remove[i];
      node->change_phi_distribute_difference_uniformly(edge, final_phi);

      // adjust properties of edges
      edge->set_chord_length(this->len_epsilon);
      edge->set_param_m(0.0);
    }

    // handle the last node differently, the angle should be pi-final_phi
    end_node->change_phi_distribute_difference_uniformly(end_edge, vnl_math::pi - final_phi);  
   
    // >> If requested, we  need to physically remove the edges
    if (real_edge_deletion)
    {
      if (is_leaf_branch)
      {
        dbsksp_shock_node_sptr v0 = 0;
        if (need_reverse_order)
        {
          for (vcl_vector<dbsksp_shock_edge_sptr >::reverse_iterator itr = 
            edges_to_remove.rbegin(); itr != edges_to_remove.rend(); ++itr)
          {
            dbsksp_shock_edge_sptr e = *itr;
            v0 = graph->remove_leaf_A_1_2_edge(e);
            if (!v0)
            {
              vcl_cerr << "ERROR: could not remove leaf edge.\n";
            }
          }
        }
        else
        {
          for (unsigned i=0; i < edges_to_remove.size(); ++i)
          {
            dbsksp_shock_edge_sptr e = edges_to_remove[i];
            v0 = graph->remove_leaf_A_1_2_edge(e);
            if (!v0)
            {
              vcl_cerr << "ERROR: could not remove leaf edge.\n";
            }
          }
        }

        // find the terminal edge and remove it
        for (vcl_list<dbsksp_shock_edge_sptr >::const_iterator itr = v0->edge_list().begin();
          itr != v0->edge_list().end(); ++itr)
        {
          dbsksp_shock_edge_sptr e = *itr;
          if (e->is_terminal_edge())
          {
            v0->change_phi_distribute_difference_uniformly(e, 0);
            graph->remove_A_infty_edge(e->opposite(v0));
            break;
          }
        }
      
      }
      else
      {
        for (unsigned i=0; i < edges_to_remove.size(); ++i)
        {
          dbsksp_shock_edge_sptr e = edges_to_remove[i];
          bool ok = graph->remove_internal_edge(e);
          if (!ok)
          {
            vcl_cerr << "ERROR: could not remove internal edge.\n";
          }
        }
      }
    }  
  }

  final_graph = graph;


  return;
}



















//: Generate the numbers of segments used to interpolate each dart of each tree
// so that the common shock graphs between the two trees have EXACT same topology
// This will allow us to do averaging between two shapes
// sampling_ds is the approximate sampling rate
void dbsksp_morph_shock_graph_different_topology::
compute_num_segments_to_interpolate_trees(double sampling_ds)
{

  // for convenenience
  dbskr_tree_sptr tree1 = this->sm_cor()->get_tree1();
  dbskr_tree_sptr tree2 = this->sm_cor()->get_tree2();

  // first compute the default ones
  // the number of segments is simply the total arclength divided by
  // the sampling_ds

  dbsksp_build_from_xshock_graph::compute_num_segments_for_darts(tree1, 
    sampling_ds, this->num_segments_tree1_);

  dbsksp_build_from_xshock_graph::compute_num_segments_for_darts(tree2, 
    sampling_ds, this->num_segments_tree2_);

  // Now we check for the matching darts, make sure we get the same number of segments
  // for each pair of matching dart paths

  // retrieve the final mapping between two trees
  vcl_vector<pathtable_key> dart_path_map = this->sm_cor()->get_map();

  
  for (unsigned int path_id = 0; path_id < dart_path_map.size(); path_id++) 
  {
    // make sure we understand what each term in the pathtable_key means
    pathtable_key key = dart_path_map[path_id];
    int tree1_start_dart = key.first.first;
    int tree1_end_dart = key.first.second;
    int tree2_start_dart = key.second.first;
    int tree2_end_dart = key.second.second;

    // Retrieve the darts involved in this matching
    // Tree 1
    vcl_vector<int > dart_list_1 = tree1->get_dart_path(tree1_start_dart, tree1_end_dart);
      
    // Tree 2
    vcl_vector<int > dart_list_2 = tree2->get_dart_path(tree2_start_dart, tree2_end_dart);

    // Now we compute the arclength associated with each path
    vcl_vector<double > dart_lengths_1;
    vcl_vector<double > dart_lengths_2;

    this->compute_dart_lengths(tree1, dart_list_1, dart_lengths_1);
    this->compute_dart_lengths(tree2, dart_list_2, dart_lengths_2);

    // compute total length for each dart path
    double total_length_1 = 0;
    for (unsigned i=0; i< dart_lengths_1.size(); ++i)
    {
      total_length_1 += dart_lengths_1[i];
    }

    double total_length_2 = 0;
    for (unsigned i=0; i< dart_lengths_2.size(); ++i)
    {
      total_length_2 += dart_lengths_2[i];
    }

    // compute the number of segments associated with each path, take the max
    int num_segments_path_1 = int(vnl_math_rnd(total_length_1/ sampling_ds));
    int num_segments_path_2 = int(vnl_math_rnd(total_length_2/ sampling_ds));
    //int num_segments_path = vnl_math_max(num_segments_path_1, num_segments_path_2);
    double mean_length = (total_length_1 + total_length_2) / 2;
    double max_length = vnl_math_max(total_length_1 , total_length_2);
    int num_segments_path = int(vnl_math_rnd(mean_length/ sampling_ds)) + 1;

    // Make sure we have at least one segment for each dart path
    num_segments_path = vnl_math_max(num_segments_path, 1);

    // Now we need to distribute this total to the darts by their length ratio

    // DART PATH 1
    int running_segment_sum_path_1 = 0;
    for (unsigned i=0; (i+1) < dart_lengths_1.size(); ++i)
    {
      int dart = dart_list_1[i];
      double len = dart_lengths_1[i];
      int n = (int)vcl_floor(num_segments_path * len / total_length_1);
      n = vnl_math_max(n, 1);
      num_segments_tree1_[dart] = n;
      num_segments_tree1_[tree1->mate(dart)] = n;
      running_segment_sum_path_1 += n;
    }
    int num_segments_remaining_1 = num_segments_path - running_segment_sum_path_1;
    num_segments_tree1_[dart_list_1.back()] = num_segments_remaining_1 ;
    num_segments_tree1_[tree1->mate(dart_list_1.back())] = num_segments_remaining_1;

    assert(num_segments_remaining_1 > 0);


    // Now we need to distribute this total to the darts by their length ratio
    // DART PATH 2
    int running_segment_sum_path_2 = 0;
    for (unsigned i=0; (i+1) < dart_lengths_2.size(); ++i)
    {
      int dart = dart_list_2[i];
      double len = dart_lengths_2[i];
      int n = (int)vcl_floor(num_segments_path * len / total_length_2);
      n = vnl_math_max(n, 1);
      num_segments_tree2_[dart] = n;
      num_segments_tree2_[tree2->mate(dart)] = n;
      running_segment_sum_path_2 += n;
    }
    int num_segments_remaining_2 = num_segments_path - running_segment_sum_path_2;
    num_segments_tree2_[dart_list_2.back()] = num_segments_remaining_2 ;
    num_segments_tree2_[tree2->mate(dart_list_2.back())] = num_segments_remaining_2;

    assert(num_segments_remaining_2 > 0);  
  }

  return;
}
  

// ----------------------------------------------------------------------------
//: Compute the lengths of the darts for a tree
void dbsksp_morph_shock_graph_different_topology::
compute_dart_lengths(const dbskr_tree_sptr& tree,
                     const vcl_vector<int >& query_dart_list,
                     vcl_vector<double >& dart_lengths)
{
  dart_lengths.resize(query_dart_list.size(), 0);
  for (unsigned i=0; i < query_dart_list.size(); ++i)
  {
    int dart = query_dart_list[i];

    //temp data structures
    dbsk2d_shock_node_sptr start_node;
    vcl_vector<dbsk2d_shock_edge_sptr> edges;
    vcl_vector<int> dart_list;

    //get shock edge list from this path
    dart_list.clear();
    dart_list.push_back(dart);

    edges.clear();
    tree->edge_list(dart_list, start_node, edges); 

    double sampling_ds = 1.0;
    dbskr_scurve_sptr sc = 
      dbskr_compute_scurve(start_node, edges, false, true, true, 1.0, sampling_ds);
    dart_lengths[i] = sc->arclength(sc->num_points()-1);
  }

  return;
}

