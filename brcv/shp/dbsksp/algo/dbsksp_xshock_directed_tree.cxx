#include "dbsksp_xshock_directed_tree.h"

#include <vcl_algorithm.h> 
#include <vcl_iostream.h>


#include <dbskr/dbskr_scurve.h>
#include <dbskr/dbskr_sc_pair.h>
#include <dbsksp/algo/dbsksp_compute_scurve.h>
//#include <vsol/vsol_polygon_2d.h>


#include <dbsksp/dbsksp_xshock_graph.h>

//==============================================================================
// dbsksp_xshock_directed_tree
//==============================================================================

//------------------------------------------------------------------------------
//: Constructor
dbsksp_xshock_directed_tree::
dbsksp_xshock_directed_tree()
{
  this->interpolate_ds_ = 3.0f;  // made this into a parameter as well, default is 0.5
  this->elastic_splice_cost_ = false;

  // important parameters
  this->set_scurve_sample_ds(3.0f);
  this->set_scurve_matching_R(6.0f);
}





//------------------------------------------------------------------------------
//: Reset all internal variables
void dbsksp_xshock_directed_tree::
clear(void) 
{
  dart_paths_.clear();
  this->xgraph_ = 0;

  dart_path_scurve_map_.clear();
  starting_nodes_.clear();
  shock_edges_.clear();
}

//------------------------------------------------------------------------------
//: acquire tree from the given shock graph
// \todo
bool dbsksp_xshock_directed_tree::
acquire(const dbsksp_xshock_graph_sptr& xgraph)
{
  bool ok = this->acquire_tree_topology(xgraph);

  if (!ok)
    return false;

  this->compute_delete_and_contract_costs();
  return true;
}







//------------------------------------------------------------------------------
//: acquire tree topology from a shock graph
bool dbsksp_xshock_directed_tree::
acquire_tree_topology(const dbsksp_xshock_graph_sptr& xgraph)
{
  this->set_xgraph(xgraph);

  //  elastic_splice_cost_ = elastic_splice_cost;
  vcl_vector<dbsksp_xshock_node_sptr> nodes_to_retain;
  
  //: make a map of nodes_to_retain list for fast access
  // first: id of the node in the shock graph
  // second: index of the node in the nodes_to_retain vector
  vcl_map<int, int> nodes_to_retain_map;
  for (dbsksp_xshock_graph::vertex_iterator v_it = this->xgraph()->vertices_begin();
    v_it != this->xgraph()->vertices_end(); ++v_it)
  {
    dbsksp_xshock_node_sptr cur_node = (*v_it);    
    int numbr_adj = cur_node->degree();
    if (numbr_adj == 1 || numbr_adj >= 3) 
    {
      nodes_to_retain.insert(nodes_to_retain.begin(), cur_node);
    }
  }

  for (unsigned int i = 0; i<nodes_to_retain.size(); i++) 
  {
    nodes_to_retain_map[nodes_to_retain[i]->id()] = i;
  }



  // find each retained nodes neighbors and save the edge information

  // storing list of shock links associated with each edge/dart of the tree
  // first: pair of ids of the shock nodes (as in the shock graph) corresponding to
  // the two ends of the tree edge. The pair is ordered: start-node --> end-node.
  // second: list of shock links
  vcl_map<vcl_pair<int, int>, vcl_vector<dbsksp_xshock_edge_sptr> > ids_to_edge_vector_map;


  // Storage for the neighrboring nodes and costs of neighboring edges around each node
  // Each member of the outer vector corresponds to one node in the tree, arranged
  // in the same order as in "nodes_to_retain".
  // Each member of the inner vector corresponds to an edge incident at the node,
  // arranged counter-clockwise
  vcl_vector< vcl_vector<vcl_pair<int, dbskr_edge_info> > > nodes;

  for (unsigned int i = 0; i<nodes_to_retain.size(); i++) 
  {
    vcl_vector<vcl_pair<int, dbskr_edge_info> > current_node; 
    dbsksp_xshock_node_sptr cur_node = nodes_to_retain[i];

    // start with its first adjacent edge (there is an order in esf file!)
    dbsksp_xshock_edge_sptr e = this->xgraph()->first_adj_edge(cur_node);
    int e_id_saved = e->id();

    // add each of its neighbors
    do 
    {   
      dbsksp_xshock_node_sptr adj_node = e->opposite(cur_node);

      // List of edges of the current chain
      vcl_vector<dbsksp_xshock_edge_sptr> tmp;
      tmp.push_back(e);

      // see if this node is in "nodes_to_retain" list
      vcl_map<int, int>::iterator iter = nodes_to_retain_map.find(adj_node->id());
      
      // advance until you hit a node which is in nodes_to_retain_map
      int prev_id = cur_node->id();
      while (iter == nodes_to_retain_map.end()) // starts the chain of edges to the next tree node
      {  
        for (dbsksp_xshock_node::edge_iterator e_it2 = adj_node->edges_begin();
          e_it2 != adj_node->edges_end(); ++e_it2 )
        { 
          dbsksp_xshock_node_sptr adj_adj_node = (*e_it2)->opposite(adj_node);
          if (prev_id == adj_adj_node->id()) // edge pointing backward
            continue;
          tmp.push_back(*e_it2);
          iter = nodes_to_retain_map.find(adj_adj_node->id());
          prev_id = adj_node->id();
          adj_node = adj_adj_node;    
          break;
        }
      }
          
      // we're done this is the neighbor
      int tree_node_id = iter->second; // index in the node vector


      // CONSIDER makeing this function more effecient later on

      vcl_pair<int, int> ids;
      ids.first = cur_node->id();
      ids.second = nodes_to_retain[tree_node_id]->id();
      ids_to_edge_vector_map[ids] = tmp;
      
      vcl_pair<int, dbskr_edge_info> p;
      p.first = tree_node_id;
      // costs are unknown for now
      (p.second).first = -1.0f;   // contract cost
      (p.second).second = -1.0f;  // delete cost
      current_node.push_back(p);

      //: go to next adjacent node
      e = this->xgraph()->cyclic_adj_succ(e, cur_node);
    } 
    while (e->id() != e_id_saved);

    nodes.push_back(current_node);
  }


  // initialize the trees from the list of node descriptors
  bool ok = dbskr_directed_tree::acquire(nodes);

  if (ok) 
  {      
    // fill in the shock_edges_ and starting_nodes_ arrays
    for (unsigned int i = 0; i<dart_cnt_; i++) 
    {
      // dart info
      int head = head_[i];
      int tail = tail_[i];
      
      // ids of shock nodes at two ends of dart
      vcl_pair<int, int> ids;
      ids.first = nodes_to_retain[tail]->id();
      ids.second = nodes_to_retain[head]->id();

      // retrieve list edges corresponding to the dart
      vcl_vector<dbsksp_xshock_edge_sptr> tmp = ids_to_edge_vector_map[ids];
      shock_edges_.push_back(tmp);
      starting_nodes_.push_back(nodes_to_retain[tail]);
    }
  }

  return ok;
}


//------------------------------------------------------------------------------
//: Compute delete and contract costs for individual dart. Only call this function
// after acquire_tree_topology has been called and succeeded.
void dbsksp_xshock_directed_tree::
compute_delete_and_contract_costs()
{
  bool dpmatch_combined = false;
  bool construct_circular_ends = false;

  // find contract and delete costs for each dart
  for (unsigned int i = 0; i<dart_cnt_; i++) 
  {
    // non-leaf darts
    if (!leaf(i) && !leaf(mate_[i])) 
    {
      // Compute costs via a dbskr_scurve
      dbskr_scurve_sptr sc = this->get_curve(i, i, construct_circular_ends);
      info_[i].first = (float)(sc->contract_cost());

      //: if it is a non-leaf dart compute the splice cost ALWAYS with the circular completions
      //  because this is the intermediate step of the subtree removal operation
      info_[i].second = (float)sc->splice_cost(this->scurve_matching_R(), // R
        this->elastic_splice_cost_, 
        true, // construct_circular ends
        dpmatch_combined, 
        false); //> is leaf_dart or not

    }
    // for leaf darts, only compute for one direction
    else 
    {  
      // contract cost 
      info_[i].first = 1000.0f;  
      if (parent_dart(i) == static_cast<int>(i)) 
      {
        dbskr_scurve_sptr sc = this->get_curve(i, i, construct_circular_ends);

        //: for leaf darts, compute the splice cost with or without the circular completions
        //  depending on the construct_circular_ends flag!!

        info_[i].second = (float)sc->splice_cost(this->scurve_matching_R(), 
          this->elastic_splice_cost_, 
          construct_circular_ends, dpmatch_combined, true);
        /*if (construct_circular_ends)
        vcl_cout << "constructing WITH circular ends at the leaf dart, splice cost: " << info_[i].second << vcl_endl;
        else {
        vcl_cout << "constructing WITHOUT circular ends at the leaf dart, splice cost: " << info_[i].second << vcl_endl;
        vcl_cout << "cost WITH circular end would be: " << sc->splice_cost(elastic_splice_cost, true, dpmatch_combined) << vcl_endl;
        }*/
        info_[mate_[i]].second = info_[i].second;
      } 
    }
  }
  this->find_subtree_delete_costs();
  return;
}



//------------------------------------------------------------------------------
//: find and cache the shock curve for this pair of darts, if not already cached
// \todo write this
dbskr_scurve_sptr dbsksp_xshock_directed_tree::
get_curve(int start_dart, int end_dart, bool construct_circular_ends)
{
  dbskr_sc_pair_sptr sc_pair = this->get_curve_pair(start_dart, end_dart, construct_circular_ends);

  if (!sc_pair)
    return 0;
  else
    return sc_pair->coarse;
}


//------------------------------------------------------------------------------
//: returns both the coarse and dense version of shock curve
dbskr_sc_pair_sptr dbsksp_xshock_directed_tree::
get_curve_pair(int start_dart, int end_dart, bool construct_circular_ends)
{
  vcl_pair<int, int> p;
  p.first = start_dart;
  p.second = end_dart;
  vcl_map<vcl_pair<int, int>, dbskr_sc_pair_sptr>::iterator iter = dart_path_scurve_map_.find(p);
  if (iter == dart_path_scurve_map_.end()) // not found yet
  {    
    vcl_vector<int>& dart_list = this->get_dart_path(start_dart, end_dart);
    
    dbsksp_xshock_node_sptr start_node;  
    vcl_vector<dbsksp_xshock_edge_sptr> edges;

    // get the underlying shock graph edge list for this dart path on the tree
    this->get_edge_list(dart_list, start_node, edges);
    
    dbskr_scurve_sptr sc = dbsksp_compute_scurve(start_node, edges, this->scurve_sample_ds_);



    dbskr_sc_pair_sptr curve_pair = new dbskr_sc_pair();
    curve_pair->coarse = sc;
    
    // we never use the dense version, there is no need to construct it
    curve_pair->dense = 0;

    //: note: even if interpolate_ds_ is not properly set by the user vcl_min takes care of it
    //curve_pair->dense = new dbskr_scurve(*sc, curve_pair->c_d_map, vcl_min(scurve_sample_ds_, interpolate_ds_));

    dart_path_scurve_map_[p] = curve_pair;

    return curve_pair;
  } 
  else 
  {
    return iter->second;
  }
}


//------------------------------------------------------------------------------
//: return a vector of pointers to the edges in underlying shock graph for the given dart list
void dbsksp_xshock_directed_tree::
get_edge_list(const vcl_vector<int>& dart_list,  
              dbsksp_xshock_node_sptr& start_node,  
              vcl_vector<dbsksp_xshock_edge_sptr>& path) 
{
  
  int dart_id = dart_list[0];
  start_node = starting_nodes_[dart_id];
  path.clear();
  path = shock_edges_[dart_id];
  for (unsigned int i = 1; i<dart_list.size(); i++) 
  {
    dart_id = dart_list[i];
    for (unsigned int j = 0; j<shock_edges_[dart_id].size(); j++) 
    {
      path.push_back(shock_edges_[dart_id][j]);
    }
  }

}





////Amir added this function for debug
////: create and write .shg file to debug splice and contract costs
//bool dbsksp_xshock_directed_tree::create_shg(vcl_string fname)
//{
//  //create a file
//  vcl_ofstream outf(fname.c_str(), vcl_ios::out);
//  if (!outf){
//    vcl_cerr << "file could not be created";
//    return false;
//  }
//
//  //Header
//  outf << "Shock Graph v1.0 " << "0.0" << " " << this->total_splice_cost() << " " << this->node_size() << vcl_endl;
//
//  outf.precision(6);
//
//  //costs for the rest of the graph
//  for (int nn=0;nn<node_size();nn++)
//  {
//    vcl_vector<int>& odarts = out_darts(nn) ;
//
//    for (unsigned j=0; j<odarts.size(); j++)
//      outf << this->head(odarts[j]) << " "   << this->contract_cost(odarts[j]) << " " << this->delete_cost(odarts[j]) << vcl_endl;
//
//    outf << vcl_endl;
//  }
//
//  outf.close();
//  return true;
//}
//
//
////Amir added this function for debug
////: create and write .shgesg file to hold the correspondences between shock node ids and tree node ids
//bool dbsksp_xshock_directed_tree::create_shgesg(vcl_string fname)
//{
//  //create a file
//  vcl_ofstream outf(fname.c_str(), vcl_ios::out);
//  if (!outf){
//    vcl_cerr << "file could not be created";
//    return false;
//  }
//
//  //Header
//  outf << "Shock Graph v1.0 Correspondences" << vcl_endl;
//  outf << "Nodes" << vcl_endl;
//
//  //loop over all the nodes and find the corresponding shock node ids for them
//  for (int nn=0;nn<node_size();nn++){
//    vcl_vector<int>& odarts = out_darts(nn) ;
//    dbsk2d_shock_node_sptr sh_node = this->starting_nodes_[odarts.front()];
//
//    outf << nn << " " << sh_node->id() << vcl_endl;
//  }
//  outf << "Links" << vcl_endl;
//
//  outf.close();
//  return true;
//}
//
