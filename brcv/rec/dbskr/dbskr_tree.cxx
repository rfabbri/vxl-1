#include "dbskr_tree.h"

#include <vcl_algorithm.h> 
#include <vcl_iostream.h>

#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbskr/dbskr_scurve.h>
#include <dbskr/dbskr_sc_pair.h>
#include <dbskr/dbskr_compute_scurve.h>
#include <vsol/vsol_polygon_2d.h>

//#define SAMPLE_SIZE   (1.0)

//------------------------------------------------------------------------------
//: Default constructor
dbskr_tree::
dbskr_tree(float scurve_sample_ds, float interpolate_ds, float matching_R):
scurve_sample_ds_(scurve_sample_ds), interpolate_ds_(interpolate_ds), 
scurve_matching_R(matching_R),mirror_(false),scale_ratio_(1.0),area_(1.0),
width_(0.0)
{
}

//------------------------------------------------------------------------------
//: Default constructor
dbskr_tree::
dbskr_tree(dbsk2d_shock_graph_sptr sg,bool mirror,double width,
float scurve_sample_ds, float interpolate_ds, float matching_R):
    sg_(sg),
    scurve_sample_ds_(scurve_sample_ds), 
    interpolate_ds_(interpolate_ds), 
    scurve_matching_R(matching_R),
    mirror_(mirror),
    scale_ratio_(1.0),
    area_(1.0),
    width_(width)
{
}


//------------------------------------------------------------------------------
//: Reset all internal variables
void dbskr_tree::clear(void) 
{
  dart_paths_.clear();
  dart_path_scurve_map_.clear();
  starting_nodes_.clear();
  shock_edges_.clear();
  sg_ = 0;
}


//------------------------------------------------------------------------------
//: acquire tree from the given input file
// the euler tour traverses the tree in a clock wise manner
// the tour begins from the leaf node with smallest id
// each out going edge of a node is in order in .shg file
// initially the leaf node that the tour begins becomes the root of this tree
// if we were to draw this tree we would put the child which is traversed first in the tour
// as the right most child of the node. So once root is known, each node's children are in order
// from left to right
bool dbskr_tree::
acquire(vcl_string filename)
{
  vcl_ifstream tf(filename.c_str());
 
  if (!tf) 
  {
    vcl_cout << "Unable to open tree file " << filename << vcl_endl;
    return false;
  }

  // each node's outgoing edges are on consequtive lines
  // an empty line seperates nodes 
  vcl_vector< vcl_vector<vcl_pair<int, dbskr_edge_info> > > nodes;

  char buf[10000];
  
  // first line is comment
  tf.getline(buf, 10000);

  if (tf.eof()) 
  {
      vcl_cout << "Input file is empty!\n";
      return false;
  }

  vcl_vector<vcl_pair<int, dbskr_edge_info> > current_node;
  char dummy;
  do // read each edge and check if next node 
  {  
    dummy = tf.get();
    if (dummy >= '0' && dummy <= '9' && !tf.eof()) 
    {
      tf.putback(dummy);
      vcl_pair<int, dbskr_edge_info> p;
      tf >> p.first;
      // read contract cost
      tf >> (p.second).first;
      // read delete cost
      tf >> (p.second).second;
      
      current_node.push_back(p);
      tf.getline(buf, 10000);
    } 
    else 
    {
      if (current_node.size() > 0) 
      {
        vcl_vector<vcl_pair<int, dbskr_edge_info> > tmp(current_node);
        nodes.push_back(tmp);
        current_node.clear();
      }
    }
  } 
  while (!tf.eof());

  bool ok = dbskr_directed_tree::acquire(nodes);

  if (ok) 
  {
    find_subtree_delete_costs();
    return true;
  } 
  else 
    return false;
}


//------------------------------------------------------------------------------
//: acquire tree from the given shock graph and prune the subtrees below the threshold
bool dbskr_tree::
acquire_and_prune(dbsk2d_shock_graph_sptr sg, double pruning_threshold, 
                  bool elastic_splice_cost, bool construct_circular_ends, bool dpmatch_combined) 
{
  sg_ = sg;
  dbskr_tree otree(scurve_sample_ds_);
  otree.acquire(sg, elastic_splice_cost, construct_circular_ends, dpmatch_combined);

  unsigned dart_cnt = otree.size();
  vcl_map< vcl_pair<double, unsigned>, bool> dart_splice_map;
  // sort the subtree delete costs
  for (unsigned i = 0; i<dart_cnt; i++) {
    vcl_pair<double, unsigned> p(otree.subtree_delete_cost(i), i);
    dart_splice_map[p] = true;
  }
  
  vcl_map< vcl_pair<double, unsigned>, bool>::iterator iter;
  double sum = 0.0f;
  vcl_vector<bool> dart_stays(dart_cnt, true);
  for (iter = dart_splice_map.begin(); iter != dart_splice_map.end(); iter++) {
    if ((iter->first).first > pruning_threshold)
      break;
    iter->second = false;
    sum += otree.delete_cost((iter->first).second);
    dart_stays[(iter->first).second] = false;
    dart_stays[otree.mate((iter->first).second)] = false;
  }
#if 0
  for (iter = dart_splice_map.begin(); iter != dart_splice_map.end(); iter++) {
    if (iter->second)
      vcl_cout << (iter->first).first << "\td: " << (iter->first).second << " stays\n";
    else
      vcl_cout << (iter->first).first << "\td: " << (iter->first).second << " goes\n";
  }
#endif
  //vcl_cout << "Total pruning: " << sum << vcl_endl;  
  
  // find nodes_to_retain list
  // if !processed --> node goes by default
  vcl_vector<vcl_pair<bool, bool> > nodes_to_retain(otree.node_size(), vcl_pair<bool, bool> (false, false));
  
  // if dart_status is true, node is retained only if one of its out dart status is true,
  for (unsigned i = 0; i<dart_cnt; i++) {
    if (!dart_stays[i]) continue;   // if !processed --> node goes irrelevant of second field
    int tail = otree.tail(i);
    if (!nodes_to_retain[tail].first) {  // do something if not traversed before
      vcl_vector<int>& outdarts = otree.out_darts(tail);
      int cnt = 0;
      for (unsigned k = 0; k < outdarts.size(); k++)
        if (dart_stays[outdarts[k]]) 
          cnt++;
      if (!(cnt == 2))   // this condition is irrelevant of number of outdarts, i.e. works for both degree 1, 3 and 4 nodes
        nodes_to_retain[tail].second = true;
      nodes_to_retain[tail].first = true;  // processed
    }
  }
#if 0
  int stays = 0, goes = 0;
  for (unsigned i = 0; i<nodes_to_retain.size(); i++) {
    vcl_cout << "node: " << i << " ";
    vcl_pair<bool, bool> p = nodes_to_retain[i];
    if (p.first) {
      vcl_cout << "processed ";
      if (p.second) {
        vcl_cout << " stays!...\n";
        stays++;
      }
      else {
        vcl_cout << " goes!...\n";
        goes++;
      }
    }
    else {
      vcl_cout << "!processed goes...\n";  // if !processed --> node goes irrelevant of second field
      goes++;
    }
  }
  vcl_cout << "number of darts: " << otree.node_size() << "# stays: " << stays << " # goes: " << goes << vcl_endl;
#endif 

  // construct node neighborhood among staying nodes 
  // CAUTION : starting from smallest id staying node, this might be a problem
  vcl_vector< vcl_vector<vcl_pair<int, dbskr_edge_info> > > nodes;
  //: save the edge information coming from original tree
  //        node id, node id in new tree, dart vector in old tree
  vcl_map<vcl_pair<int, int>, vcl_vector<int> > ids_to_edge_vector_map;
  
  vcl_vector<int> retained_node_ids(otree.node_size(), -1);
  int cnt = 0;
  for (unsigned i = 0; i < nodes_to_retain.size(); i++) {
    if (!nodes_to_retain[i].second) continue;
    retained_node_ids[i] = cnt;
    cnt++;
  }

  for (unsigned i = 0; i < nodes_to_retain.size(); i++) {
    if (!nodes_to_retain[i].second) continue;

    vcl_vector<vcl_pair<int, dbskr_edge_info> > current_node;
    
    vcl_vector<int>& outdarts = otree.out_darts(i);  // go along each outdart till you hit a retained node
    for (unsigned j = 0; j<outdarts.size(); j++) {  // add each of its neighbors
      
      int current_dart = outdarts[j];
      if (!dart_stays[current_dart]) continue; // oops wrong way

      vcl_vector<int> saved_otree_dart_path(1, current_dart);
      
      int neighbor_node = otree.head(current_dart);
      while (!nodes_to_retain[neighbor_node].second) {  // go along till you hit one
        vcl_vector<int>& children = otree.children(current_dart);
        for (unsigned k = 0; k < children.size(); k++) {
          current_dart = children[k];
          if (dart_stays[current_dart])   // the first staying dart HAS TO be the correct path, there should be only one such
            break;
        }
        saved_otree_dart_path.push_back(current_dart);
        neighbor_node = otree.head(current_dart);
      }
      vcl_pair<int, dbskr_edge_info> p;
      p.first = retained_node_ids[neighbor_node];
      //: costs are unknown for now
      (p.second).first = -1.0f;   // contract cost
      (p.second).second = -1.0f;  // delete cost
      current_node.push_back(p);
      // save dart vector of otree to get edge list and start node for this dart of new tree
      vcl_pair<int, int> node_p;
      node_p.first = retained_node_ids[i];
      node_p.second = retained_node_ids[neighbor_node];
      ids_to_edge_vector_map[node_p] = saved_otree_dart_path;
    }

    nodes.push_back(current_node);
  }

  bool ok = dbskr_directed_tree::acquire(nodes);

  if (ok) {  // fill in the shock_edges_ and starting_nodes_ arrays
    
    for (unsigned int i = 0; i<dart_cnt_; i++) {
      int head = head_[i];
      int tail = tail_[i];
      vcl_pair<int, int> ids(tail, head);
      vcl_vector<int> tmp = ids_to_edge_vector_map[ids];
      vcl_vector<dbsk2d_shock_edge_sptr> path;
      dbsk2d_shock_node_sptr start_node;
      otree.edge_list(tmp, start_node, path);
      shock_edges_.push_back(path);
      starting_nodes_.push_back(start_node);
    }

    //: find contract and delete costs for each dart
    for (unsigned int i = 0; i<dart_cnt_; i++) {
      if (!leaf(i) && !leaf(mate_[i])) {
        dbskr_scurve_sptr sc = get_curve(i, i, construct_circular_ends);
        info_[i].first = (float)sc->contract_cost();
        //: if it is a non-leaf dart compute the splice cost ALWAYS with the circular completions
        //  because this is the intermediate step of the subtree removal operation
        info_[i].second = (float)sc->splice_cost(scurve_matching_R, elastic_splice_cost, true, dpmatch_combined, false);
      }
      else {  // for leaf darts, only compute for one direction
        //: contract cost 
        info_[i].first = 1000.0f;  
        if (parent_dart(i) == static_cast<int>(i)) {
          dbskr_scurve_sptr sc = get_curve(i, i, construct_circular_ends);
          //: for leaf darts, compute the splice cost with or without the circular completions
          //  depending on the construct_circular_ends flag!!
          info_[i].second = (float)sc->splice_cost(scurve_matching_R, elastic_splice_cost, construct_circular_ends, dpmatch_combined, true);
          info_[mate_[i]].second = info_[i].second;
        } 
      }
    }

#if 0
  for (unsigned int i = 0; i<dart_cnt_; i++) {
    vcl_cout << "info[" << i << "]: " << info_[i].first << " " << info_[i].second << vcl_endl;
  }
#endif
 
    find_subtree_delete_costs();
    total_reconstructed_boundary_length_ = compute_total_reconstructed_boundary_length(construct_circular_ends);
    return true;
  } else 
    return false;
}



//------------------------------------------------------------------------------
//: acquire tree from the given shock graph
bool dbskr_tree::acquire(dbsk2d_shock_graph_sptr sg, bool elastic_splice_cost, 
                               bool construct_circular_ends, bool dpmatch_combined) 
{

  sg_ = sg;
  elastic_splice_cost_ = elastic_splice_cost;

  vcl_vector<dbsk2d_shock_node_sptr> nodes_to_retain;
  //: make a map of nodes_to_retain list for fast access
  vcl_map<int, int> nodes_to_retain_map;

  dbsk2d_shock_graph::vertex_iterator v_it = sg->vertices_begin();
  for (; v_it != sg->vertices_end(); ++v_it )
  {
    dbsk2d_shock_node_sptr cur_node = (*v_it);    
    int numbr_adj = cur_node->in_degree()+cur_node->out_degree();
    if (numbr_adj == 1 || numbr_adj >= 3) 
      nodes_to_retain.insert(nodes_to_retain.begin(), cur_node);
  }

  for (unsigned int i = 0; i<nodes_to_retain.size(); i++) {
    nodes_to_retain_map[nodes_to_retain[i]->id()] = i;
  }
#if 0
  vcl_cout <<"retained nodes:\n";
  for (unsigned int i = 0; i<nodes_to_retain.size(); i++) {
    vcl_cout << "id: " << nodes_to_retain[i]->id() << " ";
    vcl_cout << "i: " << nodes_to_retain_map[nodes_to_retain[i]->id()] << vcl_endl;
  }
#endif

  //: find each retained nodes neighbors and save the edge information
  vcl_map<vcl_pair<int, int>, vcl_vector<dbsk2d_shock_edge_sptr> > ids_to_edge_vector_map;
  vcl_vector< vcl_vector<vcl_pair<int, dbskr_edge_info> > > nodes;

  for (unsigned int i = 0; i<nodes_to_retain.size(); i++) {
    vcl_vector<vcl_pair<int, dbskr_edge_info> > current_node;
    
    dbsk2d_shock_node_sptr cur_node = nodes_to_retain[i];
    //: start with its first adjacent edge (there is an order in esf file!)
    dbsk2d_shock_edge_sptr e = sg_->first_adj_edge(cur_node);
    int e_id_saved = e->id();

    do {   // add each of its neighbors

      dbsk2d_shock_node_sptr adj_node = e->opposite(cur_node);
      vcl_vector<dbsk2d_shock_edge_sptr> tmp;
      tmp.push_back(e);

      //: see if this node is in "nodes_to_retain" list
      vcl_map<int, int>::iterator iter;
      iter = nodes_to_retain_map.find(adj_node->id());
      int tree_node_id;
      
      //: advance until you hit a node which is in nodes_to_retain_map
      int prev_id = cur_node->id();
      while (iter == nodes_to_retain_map.end()) {  // starts the chain of edges to the next tree node

        bool still = true;
        for (dbsk2d_shock_node::edge_iterator e_it2 = adj_node->in_edges_begin();
         e_it2 != adj_node->in_edges_end(); ++e_it2 )
        { 
           dbsk2d_shock_node_sptr adj_adj_node = (*e_it2)->opposite(adj_node);
           if (prev_id == adj_adj_node->id()) continue;
           tmp.push_back(*e_it2);
           iter = nodes_to_retain_map.find(adj_adj_node->id());
           prev_id = adj_node->id();
           adj_node = adj_adj_node;
           still = false;
           break;
         }
         if (still) {
          for (dbsk2d_shock_node::edge_iterator e_it2 = adj_node->out_edges_begin(); 
            e_it2 != adj_node->out_edges_end(); ++e_it2 )
          {
            dbsk2d_shock_node_sptr adj_adj_node = (*e_it2)->opposite(adj_node);
            if (prev_id == adj_adj_node->id()) continue;
            tmp.push_back(*e_it2);
            iter = nodes_to_retain_map.find(adj_adj_node->id());
            prev_id = adj_node->id();
            adj_node = adj_adj_node;
            break;
          }
         }
      }
      
      // we're done this is the neighbor
      tree_node_id = iter->second;
      vcl_pair<int, int> ids;
      ids.first = cur_node->id();
      ids.second = nodes_to_retain[tree_node_id]->id();
      ids_to_edge_vector_map[ids] = tmp;
      
      vcl_pair<int, dbskr_edge_info> p;
      p.first = tree_node_id;
      //: costs are unknown for now
      (p.second).first = -1.0f;   // contract cost
      (p.second).second = -1.0f;  // delete cost
      current_node.push_back(p);

      //: go to next adjacent node
      e = sg_->cyclic_adj_pred(e, cur_node);
    } while (e->id() != e_id_saved);

    nodes.push_back(current_node);
  } 

  bool ok = dbskr_directed_tree::acquire(nodes);

  if (ok) {  // fill in the shock_edges_ and starting_nodes_ arrays
    
    for (unsigned int i = 0; i<dart_cnt_; i++) {
      int head = head_[i];
      int tail = tail_[i];
      vcl_pair<int, int> ids;
      ids.first = nodes_to_retain[tail]->id();
      ids.second = nodes_to_retain[head]->id();
      vcl_vector<dbsk2d_shock_edge_sptr> tmp = ids_to_edge_vector_map[ids];
      shock_edges_.push_back(tmp);
      starting_nodes_.push_back(nodes_to_retain[tail]);
    }

    //: find contract and delete costs for each dart
    for (unsigned int i = 0; i<dart_cnt_; i++) {
      if (!leaf(i) && !leaf(mate_[i])) {
        dbskr_scurve_sptr sc = get_curve(i, i, construct_circular_ends);
        info_[i].first = (float)sc->contract_cost();
        //: if it is a non-leaf dart compute the splice cost ALWAYS with the circular completions
        //  because this is the intermediate step of the subtree removal operation
        info_[i].second = (float)sc->splice_cost(scurve_matching_R, elastic_splice_cost, true, dpmatch_combined, false);
      }
      else {  // for leaf darts, only compute for one direction
        //: contract cost 
        info_[i].first = 1000.0f;  
        if (parent_dart(i) == static_cast<int>(i)) {
          dbskr_scurve_sptr sc = get_curve(i, i, construct_circular_ends);
          //: for leaf darts, compute the splice cost with or without the circular completions
          //  depending on the construct_circular_ends flag!!
          info_[i].second = (float)sc->splice_cost(scurve_matching_R, elastic_splice_cost, construct_circular_ends, dpmatch_combined, true);
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

#if 0
  for (unsigned int i = 0; i<dart_cnt_; i++) {
    vcl_cout << "info[" << i << "]: " << info_[i].first << " " << info_[i].second << vcl_endl;
  }
#endif
 
    find_subtree_delete_costs();
    total_reconstructed_boundary_length_ = compute_total_reconstructed_boundary_length(construct_circular_ends);

#if 0   // write shg file for debugging purposes

  //vcl_FILE *of=vcl_fopen("tree.shg","w");
  double dummy = 0.0f;
  vcl_printf("\n SHG \n");
  //vcl_fprintf(of,"Shock Graph v1.0 %5.2f %7.2f %d %7.2f \n",dummy,total_splice_cost_,node_cnt_,dummy);
  
  for (unsigned int i = 0; i<nodes.size(); i++) {
    for (unsigned int j = 0; j<nodes[i].size(); j++) {
      int tail = i;
      int head = nodes[i][j].first;
      unsigned int k;
      for (k = 0; k<dart_cnt_; k++) {
        if (head_[k] == head && tail_[k] == tail)
          break;
      }
      //vcl_fprintf(of,"%d %7.2f  %7.2f \n",nodes[i][j].first,contract_cost(k),delete_cost(k));
      vcl_printf("%d %7.2f  %7.2f \n",nodes[i][j].first,contract_cost(k),delete_cost(k));
    }
    //vcl_fprintf(of, "\n");
    vcl_printf("\n");
  }

  //vcl_fclose(of);

#endif
    return true;

  } else 
    return false;
}


//------------------------------------------------------------------------------
//: acquire tree from the given shock graph
bool dbskr_tree::acquire_tree_topology()
{


  vcl_vector<dbsk2d_shock_node_sptr> nodes_to_retain;
  //: make a map of nodes_to_retain list for fast access
  vcl_map<int, int> nodes_to_retain_map;

  dbsk2d_shock_graph::vertex_iterator v_it = sg_->vertices_begin();
  for (; v_it != sg_->vertices_end(); ++v_it )
  {
    dbsk2d_shock_node_sptr cur_node = (*v_it);    
    int numbr_adj = cur_node->in_degree()+cur_node->out_degree();
    if (numbr_adj == 1 || numbr_adj >= 3) 
      nodes_to_retain.insert(nodes_to_retain.begin(), cur_node);
  }

  for (unsigned int i = 0; i<nodes_to_retain.size(); i++) {
    nodes_to_retain_map[nodes_to_retain[i]->id()] = i;
  }
#if 0
  vcl_cout <<"retained nodes:\n";
  for (unsigned int i = 0; i<nodes_to_retain.size(); i++) {
    vcl_cout << "id: " << nodes_to_retain[i]->id() << " ";
    vcl_cout << "i: " << nodes_to_retain_map[nodes_to_retain[i]->id()] << vcl_endl;
  }
#endif

  //: find each retained nodes neighbors and save the edge information
  vcl_map<vcl_pair<int, int>, vcl_vector<dbsk2d_shock_edge_sptr> > ids_to_edge_vector_map;
  vcl_vector< vcl_vector<vcl_pair<int, dbskr_edge_info> > > nodes;

  for (unsigned int i = 0; i<nodes_to_retain.size(); i++) {
    vcl_vector<vcl_pair<int, dbskr_edge_info> > current_node;
    
    dbsk2d_shock_node_sptr cur_node = nodes_to_retain[i];
    //: start with its first adjacent edge (there is an order in esf file!)
    dbsk2d_shock_edge_sptr e = sg_->first_adj_edge(cur_node);
    int e_id_saved = e->id();

    do {   // add each of its neighbors

      dbsk2d_shock_node_sptr adj_node = e->opposite(cur_node);
      vcl_vector<dbsk2d_shock_edge_sptr> tmp;
      tmp.push_back(e);

      //: see if this node is in "nodes_to_retain" list
      vcl_map<int, int>::iterator iter;
      iter = nodes_to_retain_map.find(adj_node->id());
      int tree_node_id;
      
      //: advance until you hit a node which is in nodes_to_retain_map
      int prev_id = cur_node->id();
      while (iter == nodes_to_retain_map.end()) {  // starts the chain of edges to the next tree node

        bool still = true;
        for (dbsk2d_shock_node::edge_iterator e_it2 = adj_node->in_edges_begin();
         e_it2 != adj_node->in_edges_end(); ++e_it2 )
        { 
           dbsk2d_shock_node_sptr adj_adj_node = (*e_it2)->opposite(adj_node);
           if (prev_id == adj_adj_node->id()) continue;
           tmp.push_back(*e_it2);
           iter = nodes_to_retain_map.find(adj_adj_node->id());
           prev_id = adj_node->id();
           adj_node = adj_adj_node;
           still = false;
           break;
         }
         if (still) {
          for (dbsk2d_shock_node::edge_iterator e_it2 = adj_node->out_edges_begin(); 
            e_it2 != adj_node->out_edges_end(); ++e_it2 )
          {
            dbsk2d_shock_node_sptr adj_adj_node = (*e_it2)->opposite(adj_node);
            if (prev_id == adj_adj_node->id()) continue;
            tmp.push_back(*e_it2);
            iter = nodes_to_retain_map.find(adj_adj_node->id());
            prev_id = adj_node->id();
            adj_node = adj_adj_node;
            break;
          }
         }
      }
      
      // we're done this is the neighbor
      tree_node_id = iter->second;
      vcl_pair<int, int> ids;
      ids.first = cur_node->id();
      ids.second = nodes_to_retain[tree_node_id]->id();
      ids_to_edge_vector_map[ids] = tmp;
      
      vcl_pair<int, dbskr_edge_info> p;
      p.first = tree_node_id;
      //: costs are unknown for now
      (p.second).first = -1.0f;   // contract cost
      (p.second).second = -1.0f;  // delete cost
      current_node.push_back(p);

      //: go to next adjacent node
      e = sg_->cyclic_adj_pred(e, cur_node);
    } while (e->id() != e_id_saved);

    nodes.push_back(current_node);
  } 

  bool ok = dbskr_directed_tree::acquire(nodes);

  if (ok) 
  {  // fill in the shock_edges_ and starting_nodes_ arrays
      
      for (unsigned int i = 0; i<dart_cnt_; i++)
      {
          int head = head_[i];
          int tail = tail_[i];
          vcl_pair<int, int> ids;
          ids.first = nodes_to_retain[tail]->id();
          ids.second = nodes_to_retain[head]->id();
          vcl_vector<dbsk2d_shock_edge_sptr> tmp = ids_to_edge_vector_map[ids];
          shock_edges_.push_back(tmp);
          starting_nodes_.push_back(nodes_to_retain[tail]);
      }
  }

  return ok;

}

//: compute delete contract costs
void dbskr_tree::compute_delete_and_contract_costs(bool elastic_splice_cost,
                                                   bool construct_circular_ends,
                                                   bool dpmatch_combined)
{


    elastic_splice_cost_=elastic_splice_cost;

    //: find contract and delete costs for each dart
    for (unsigned int i = 0; i<dart_cnt_; i++) {
      if (!leaf(i) && !leaf(mate_[i])) {
        dbskr_scurve_sptr sc = get_curve(i, i, construct_circular_ends);
        info_[i].first = (float)sc->contract_cost();
        //: if it is a non-leaf dart compute the splice cost ALWAYS 
        //  with the circular completions because this is the 
        // intermediate step of the subtree removal operation
        info_[i].second = (float)sc->splice_cost(
            scurve_matching_R, 
            elastic_splice_cost_, 
            true, 
            dpmatch_combined, 
            false);
      }
      else {  // for leaf darts, only compute for one direction
        //: contract cost 
        info_[i].first = 1000.0f;  
        if (parent_dart(i) == static_cast<int>(i)) {
          dbskr_scurve_sptr sc = get_curve(i, i, construct_circular_ends);
          //: for leaf darts, compute the splice cost with or 
          //  without the circular completions depending 
          // on the construct_circular_ends flag!!
          info_[i].second = (float)sc->splice_cost(
              scurve_matching_R, 
              elastic_splice_cost_, 
              construct_circular_ends, 
              dpmatch_combined, 
              true);
          info_[mate_[i]].second = info_[i].second;
        } 
      }
    }

 
    this->find_subtree_delete_costs();
}

//: return the total length of the reconstructed boundary with this tree
//  (trees from the same shock graph may return different values since they may be constructed with different parameters,
//   e.g. with our without circular completions at the leaves, etc.)
float 
dbskr_tree::compute_total_reconstructed_boundary_length(bool construct_circular_ends)
{
  //: find the set of darts to use
  vcl_vector<unsigned> to_use;
  for (unsigned i = 0; i < dart_cnt_; i++) {
    if (leaf_[i]) {
      to_use.push_back(i);
      continue;
    }

    if (leaf_[mate_[i]]) 
      continue;
    
    bool contain = false;
    for (unsigned j = 0; j < to_use.size(); j++) {
      if (to_use[j] == mate_[i]) {
        contain = true;
        break;
      }
    }

    if (!contain)
      to_use.push_back(i);
  }

  //: now add up the scurves lengths
  float sum = 0;
  for (unsigned j = 0; j < to_use.size(); j++) {
    unsigned i = to_use[j];
    dbskr_scurve_sptr sc = get_curve(i, i, construct_circular_ends);
    float total = (float)(sc->boundary_minus_length() + sc->boundary_plus_length());
    //vcl_cout << "i: " << i << " start node: ";
    //vcl_vector<int> dart_list(1, i);
    //dbsk2d_shock_node_sptr start_node;
    //vcl_vector<dbsk2d_shock_edge_sptr> path;
    //edge_list(dart_list, start_node, path);
    //vcl_cout << start_node->id() << " edge list: ";
    //for (unsigned k = 0; k < path.size(); k++)
    //  vcl_cout << path[k]->id() << " ";
    //vcl_cout << " total scurve length: " << total << vcl_endl;
    sum += total;
    
  }
  return sum;
}


//------------------------------------------------------------------------------
//: uses the already existing scurves, so if circular_ends = true while acquiring the tree then the outline will have circular completions
vsol_polygon_2d_sptr dbskr_tree::
compute_reconstructed_boundary_polygon(bool construct_circular_ends)
{
  //: find the set of darts to use
  vcl_vector<unsigned> to_use;
  for (unsigned i = 0; i < dart_cnt_; i++) {
    if (leaf_[i]) {
      to_use.push_back(i);
      continue;
    }

    if (leaf_[mate_[i]]) 
      continue;
    
    bool contain = false;
    for (unsigned j = 0; j < to_use.size(); j++) {
      if (to_use[j] == mate_[i]) {
        contain = true;
        break;
      }
    }

    if (!contain)
      to_use.push_back(i);
  }

  //: now concatanate the scurves  
  vcl_vector<vsol_point_2d_sptr> points;
  unsigned j = 0;
  for (unsigned i = 0; i < dart_cnt_; i++) {  // push the minus boundary for each dart
    if (i == to_use[j]) {
      dbskr_scurve_sptr sc = get_curve(i, i, construct_circular_ends);  // if the curve already exists returns the existing one
                                                                      // in that case construct_circular_ends is meaningless
      for (unsigned k = 0; k < sc->bdry_minus().size(); k++)
        points.push_back(new vsol_point_2d(sc->bdry_minus_pt(k).x(), sc->bdry_minus_pt(k).y()));
      j++;
    } else {  // then mate of i is in to_use
      dbskr_scurve_sptr sc = get_curve(mate_[i], mate_[i], construct_circular_ends);
      for (int k = int(sc->bdry_plus().size()-1); k >=0 ; k--)
        points.push_back(new vsol_point_2d(sc->bdry_plus_pt(k).x(), sc->bdry_plus_pt(k).y()));
    }
  }

  vsol_polygon_2d_sptr poly = new vsol_polygon_2d(points);
  return poly;
}

 
//: find and cache the shock curve for this pair of darts, if not already cached
dbskr_scurve_sptr 
dbskr_tree::get_curve(int start_dart, int end_dart, bool construct_circular_ends) {
  vcl_pair<int, int> p;
  p.first = start_dart;
  p.second = end_dart;
  vcl_map<vcl_pair<int, int>, dbskr_sc_pair_sptr>::iterator iter = dart_path_scurve_map_.find(p);
  if (iter == dart_path_scurve_map_.end()) {  // not found yet
    
    vcl_vector<int>& dart_list = get_dart_path(start_dart, end_dart);
    
    dbsk2d_shock_node_sptr start_node;  
    vcl_vector<dbsk2d_shock_edge_sptr> edges;
    //: get the underlying shock graph edge list for this dart path on the tree
    edge_list(dart_list, start_node, edges);
    
    dbskr_scurve_sptr sc;
    if (construct_circular_ends)
      //: note: even if interpolate_ds_ is not properly set by the user vcl_min takes care of it
        sc = dbskr_compute_scurve(start_node, edges, leaf_[end_dart], true, true, vcl_min((float)scurve_sample_ds_, interpolate_ds_), scurve_sample_ds_,scale_ratio_);
    else
        sc = dbskr_compute_scurve(start_node, edges, false, true, true, vcl_min((float)scurve_sample_ds_, interpolate_ds_), scurve_sample_ds_,scale_ratio_);

    dbskr_sc_pair_sptr curve_pair = new dbskr_sc_pair();
    curve_pair->coarse = sc;
    //: note: even if interpolate_ds_ is not properly set by the user vcl_min takes care of it
    curve_pair->dense = new dbskr_scurve(*(sc.ptr()), curve_pair->c_d_map, vcl_min((float)scurve_sample_ds_, interpolate_ds_));

    dart_path_scurve_map_[p] = curve_pair;
    return sc;
  } else {
    return (iter->second)->coarse;
  }
}

//------------------------------------------------------------------------------
//: returns the saved shock curve pair
dbskr_sc_pair_sptr dbskr_tree::
get_curve_pair(int start_dart, int end_dart, bool construct_circular_ends) {
  vcl_pair<int, int> p;
  p.first = start_dart;
  p.second = end_dart;
  vcl_map<vcl_pair<int, int>, dbskr_sc_pair_sptr>::iterator iter = dart_path_scurve_map_.find(p);
  if (iter == dart_path_scurve_map_.end()) {  // not found yet
    
    vcl_vector<int>& dart_list = get_dart_path(start_dart, end_dart);
    
    dbsk2d_shock_node_sptr start_node;  
    vcl_vector<dbsk2d_shock_edge_sptr> edges;
    //: get the underlying shock graph edge list for this dart path on the tree
    edge_list(dart_list, start_node, edges);
    dbskr_scurve_sptr sc;
    if (construct_circular_ends)
      //: note: even if interpolate_ds_ is not properly set by the user vcl_min takes care of it
        sc = dbskr_compute_scurve(start_node, edges, leaf_[end_dart], true, true, vcl_min((float)scurve_sample_ds_, interpolate_ds_), scurve_sample_ds_,scale_ratio_);
    else
        sc = dbskr_compute_scurve(start_node, edges, false, true, true, vcl_min((float)scurve_sample_ds_, interpolate_ds_), scurve_sample_ds_,scale_ratio_);

    dbskr_sc_pair_sptr curve_pair = new dbskr_sc_pair();
    curve_pair->coarse = sc;
    //: note: even if interpolate_ds_ is not properly set by the user vcl_min takes care of it
    curve_pair->dense = new dbskr_scurve(*(sc.ptr()), curve_pair->c_d_map, vcl_min((float)scurve_sample_ds_, interpolate_ds_));
    dart_path_scurve_map_[p] = curve_pair;
    return curve_pair;
  } else {
    return iter->second;
  }
}





//------------------------------------------------------------------------------
//: return a vector of pointers to the edges in underlying shock graph for the given dart list
void dbskr_tree::
edge_list(const vcl_vector<int>& dart_list,  
                      dbsk2d_shock_node_sptr& start_node,  
                      vcl_vector<dbsk2d_shock_edge_sptr>& path) {
  
  int dart_id = dart_list[0];
  start_node = starting_nodes_[dart_id];
  path.clear();
  path = shock_edges_[dart_id];
  for (unsigned int i = 1; i<dart_list.size(); i++) {
    dart_id = dart_list[i];
    for (unsigned int j = 0; j<shock_edges_[dart_id].size(); j++) 
      path.push_back(shock_edges_[dart_id][j]);
    
  }

}





//Amir added this function for debug
//: create and write .shg file to debug splice and contract costs
bool dbskr_tree::create_shg(vcl_string fname)
{
  //create a file
  vcl_ofstream outf(fname.c_str(), vcl_ios::out);
  if (!outf){
    vcl_cerr << "file could not be created";
    return false;
  }

  //Header
  outf << "Shock Graph v1.0 " << "0.0" << " " << this->total_splice_cost() << " " << this->node_size() << vcl_endl;

  outf.precision(6);

  //costs for the rest of the graph
  for (int nn=0;nn<node_size();nn++)
  {
    vcl_vector<int>& odarts = out_darts(nn) ;

    for (unsigned j=0; j<odarts.size(); j++)
      outf << this->head(odarts[j]) << " "   << this->contract_cost(odarts[j]) << " " << this->delete_cost(odarts[j]) << vcl_endl;

    outf << vcl_endl;
  }

  outf.close();
  return true;
}


//Amir added this function for debug
//: create and write .shgesg file to hold the correspondences between shock node ids and tree node ids
bool dbskr_tree::create_shgesg(vcl_string fname)
{
  //create a file
  vcl_ofstream outf(fname.c_str(), vcl_ios::out);
  if (!outf){
    vcl_cerr << "file could not be created";
    return false;
  }

  //Header
  outf << "Shock Graph v1.0 Correspondences" << vcl_endl;
  outf << "Nodes" << vcl_endl;

  //loop over all the nodes and find the corresponding shock node ids for them
  for (int nn=0;nn<node_size();nn++){
    vcl_vector<int>& odarts = out_darts(nn) ;
    dbsk2d_shock_node_sptr sh_node = this->starting_nodes_[odarts.front()];

    outf << nn << " " << sh_node->id() << vcl_endl;
  }
  outf << "Links" << vcl_endl;

  outf.close();
  return true;
}

