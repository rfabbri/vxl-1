
#include <dbskfg/dbskfg_composite_graph.h>
#include <dbskfg/algo/dbskfg_cgraph_directed_tree.h>
#include <dbskfg/algo/dbskfg_compute_scurve.h>
#include <dbskfg/dbskfg_composite_node.h>
#include <dbskfg/dbskfg_shock_link.h>
#include <dbskfg/dbskfg_shock_node.h>
#include <dbskfg/dbskfg_utilities.h>
#include <dbskr/dbskr_scurve.h>
#include <dbskr/dbskr_sc_pair.h>

#include <vcl_algorithm.h> 
#include <vcl_fstream.h>
#include <vcl_cstdio.h>
#include <vcl_cassert.h>

//#include <dbsksp/algo/dbsksp_compute_scurve.h>
//#include <vsol/vsol_polygon_2d.h>


//#include <dbsksp/dbskfg_cgraph_graph.h>

//==============================================================================
// dbskfg_cgraph_directed_tree
//==============================================================================

//------------------------------------------------------------------------------
//: Constructor
dbskfg_cgraph_directed_tree::
dbskfg_cgraph_directed_tree(
    float scurve_sample_ds,float interpolate_ds,float matching_R)
    :scurve_sample_ds_(scurve_sample_ds),
     interpolate_ds_(interpolate_ds),
     scurve_matching_R_(matching_R),
     scale_ratio_(1.0),
     root_node_radius_(0.0)
{ 
}

//------------------------------------------------------------------------------
//: Constructor
dbskfg_cgraph_directed_tree::
~dbskfg_cgraph_directed_tree()
{
    this->clear();
}


//------------------------------------------------------------------------------
//: Reset all internal variables
void dbskfg_cgraph_directed_tree::
clear(void) 
{
  dart_paths_.clear();
  this->composite_graph_ = 0;

  dart_path_scurve_map_.clear();
  starting_nodes_.clear();
  shock_edges_.clear();
}

//------------------------------------------------------------------------------
//: acquire tree from the given shock graph
// \todo
bool dbskfg_cgraph_directed_tree::
acquire(const dbskfg_composite_graph_sptr& composite_graph,
        bool elastic_splice_cost,bool construct_circular_ends,
        bool dpmatch_combined)
{
  bool ok = this->acquire_tree_topology(composite_graph);

  if (!ok)
  {
    return false;
  }

  this->compute_delete_and_contract_costs(elastic_splice_cost,
                                          construct_circular_ends,
                                          dpmatch_combined);
  return true;
}

//------------------------------------------------------------------------------
//: acquire tree from the given shock graph
// \todo
bool dbskfg_cgraph_directed_tree::
acquire(const dbskfg_composite_graph_sptr& composite_graph,
        bool elastic_splice_cost,bool construct_circular_ends,
        bool dpmatch_combined,float cost)
{
  bool ok = this->acquire_tree_topology(composite_graph);

  if (!ok)
  {
    return false;
  }

  this->set_delete_and_contract_costs(elastic_splice_cost,
                                      construct_circular_ends,
                                      dpmatch_combined,
                                      cost);

  return true;
}


//------------------------------------------------------------------------------
//: acquire tree topology from a shock graph
bool dbskfg_cgraph_directed_tree::
acquire_tree_topology(const dbskfg_composite_graph_sptr& composite_graph)
{

  this->composite_graph_=composite_graph;

  // Grab all shock links from this node
  vcl_map<unsigned int,dbskfg_shock_link*> shock_links;

  // Grab all virtual nodes
  vcl_map<unsigned int,dbskfg_composite_node*> virtual_nodes;

  // Create a temp map to make sure nodes dont overlap
  // To get the nodes in a correct ordered list we will graph all degree one
  // and degree three nodes in two separate lists
  vcl_vector<dbskfg_composite_node_sptr> degree_1_nodes;
  vcl_vector<dbskfg_composite_node_sptr> degree_3_nodes;

  vcl_map<unsigned int,dbskfg_composite_node_sptr > temp_map;
  vcl_vector<vcl_pair<double,unsigned int> > node_pairs;
  vcl_vector<vcl_pair<double,unsigned int> > node_pairs_deg1;
  vcl_vector<vcl_pair<double,unsigned int> > node_pairs_deg3;
  
  // On the off chance that radius will be exact lets use multiple map
  // Grab all degree 1 nodes first
  for (dbskfg_composite_graph::edge_iterator eit =
           composite_graph->edges_begin();
       eit != composite_graph->edges_end(); ++eit)
  {
      dbskfg_composite_link_sptr link = *eit;
      if ( link->link_type() == dbskfg_composite_link::SHOCK_LINK )
      {
          
          dbskfg_shock_link* shock_link=
              dynamic_cast<dbskfg_shock_link*>(&(*link));

          if ( shock_link->shock_link_type() == dbskfg_shock_link::SHOCK_EDGE )
          {
              dbskfg_composite_node_sptr source_node = shock_link->source();
              dbskfg_composite_node_sptr target_node = shock_link->target();

              if ( source_node->node_type() == 
                   dbskfg_composite_node::CONTOUR_NODE )
              {
                  if ( temp_map.count(source_node->id()) == 0 )
                  {
                      temp_map[source_node->id()]=source_node;
                      degree_1_nodes.insert(degree_1_nodes.begin(),
                                            source_node);
                      node_pairs_deg1.push_back(vcl_make_pair(0,
                                                         source_node->id()));
                  }
              }
              else if ( source_node->get_composite_degree() == 1 )
              {
                  if ( temp_map.count(source_node->id()) == 0 )
                  {
                      temp_map[source_node->id()]=source_node;
                      degree_1_nodes.insert(degree_1_nodes.begin(),
                                            source_node);
                      dbskfg_shock_node* snode =
                          dynamic_cast<dbskfg_shock_node*>(&(*source_node));
                      node_pairs_deg1.push_back(vcl_make_pair
                                                (snode->get_radius(),
                                                 source_node->id()));
                      // Check if node is virtual node
                      if ( source_node->virtual_node())
                      {
                          virtual_nodes[source_node->id()]=source_node.ptr();
                      }
                  }
                  
              }
    
              if ( target_node->node_type() == 
                   dbskfg_composite_node::CONTOUR_NODE )
              {
                  if ( temp_map.count(target_node->id()) == 0 )
                  {
                      temp_map[target_node->id()]=target_node;
                      degree_1_nodes.insert(degree_1_nodes.begin(),
                                            target_node);
                      node_pairs_deg1.push_back(vcl_make_pair(0,
                                                         target_node->id()));

                  }
              }
              else if ( target_node->get_composite_degree() == 1 )
              {
                  if ( temp_map.count(target_node->id()) == 0 )
                  {
                      temp_map[target_node->id()]=target_node;
                      degree_1_nodes.insert(degree_1_nodes.begin(),
                                            target_node);

                      dbskfg_shock_node* snode =
                          dynamic_cast<dbskfg_shock_node*>(&(*target_node));
                      node_pairs_deg1.push_back(vcl_make_pair
                                                (snode->get_radius(),
                                                 target_node->id()));
               
                      // Check if node is virtual node
                      if ( target_node->virtual_node())
                      {
                          virtual_nodes[target_node->id()]=target_node.ptr();
                      }
                  }
                  
              }
    
          }
      }
  }

  // Grab all degree 3 nodes first
  for (dbskfg_composite_graph::vertex_iterator vit =
           composite_graph->vertices_begin();
      vit != composite_graph->vertices_end(); ++vit)
  {
      dbskfg_composite_node_sptr node = *vit;
      if ( node->node_type() == dbskfg_composite_node::SHOCK_NODE )
      {
            if ( node->get_composite_degree() >= 3 )
            {
                temp_map[node->id()]=node;
                degree_3_nodes.insert(degree_3_nodes.begin(),node);

                dbskfg_shock_node* snode =
                    dynamic_cast<dbskfg_shock_node*>(&(*node));
                node_pairs_deg3.push_back(vcl_make_pair(snode->get_radius(),
                                                   node->id()));
               
            }
      }
  }
  
  // Sort node pairs

  // vcl_sort(node_pairs_deg1.begin(),node_pairs_deg1.end(),
  //          this->compare_node_radius_pairs);
  // vcl_sort(node_pairs_deg3.begin(),node_pairs_deg3.end(),
  //          this->compare_node_radius_pairs);

 
  // Set up all nodes to retain
  vcl_vector<dbskfg_composite_node_sptr> nodes_to_retain;

  for ( unsigned int t=0; t < node_pairs_deg3.size() ; ++t)
  {
      node_pairs.push_back(node_pairs_deg3[t]);
  }

  for ( unsigned int t=0; t < node_pairs_deg1.size() ; ++t)
  {
      node_pairs.push_back(node_pairs_deg1[t]);
  }

  vcl_sort(node_pairs.begin(),node_pairs.end(),this->compare_node_radius_pairs);

  for ( unsigned int t=0; t < node_pairs.size() ; ++t)
  {
      nodes_to_retain.push_back(temp_map[node_pairs[t].second]);
  }

  root_node_radius_ = node_pairs[0].first;

  // if ( node_pairs_deg3.size())
  // {
  //     root_node_radius_ = node_pairs_deg3[0].first;
  // }
  // else
  // {
  //     root_node_radius_ = node_pairs_deg1[0].first;
  // }
  // // Fill up nodes to retain with first and 3 degree nodes
  // for ( unsigned int t = 0 ; t < degree_3_nodes.size() ; ++t)
  // {
  //     nodes_to_retain.push_back(degree_3_nodes[t]);
  // }

  // // Fill up nodes to retain with first and 3 degree nodes
  // for ( unsigned int f = 0 ; f < degree_1_nodes.size() ; ++f)
  // {
  //     nodes_to_retain.push_back(degree_1_nodes[f]);
  // }

  //: make a map of nodes_to_retain list for fast access
  // first: id of the node in the shock graph
  // second: index of the node in the nodes_to_retain vector
  vcl_map<unsigned int,unsigned int > nodes_to_retain_map;

  for (unsigned int v = 0; v<nodes_to_retain.size(); v++) 
  {
      nodes_to_retain_map[nodes_to_retain[v]->id()] = v;
  }

  // find each retained nodes neighbors and save the edge information
  // storing list of shock links associated with each edge/dart of the tree
  // first: pair of ids of the shock nodes (as in the shock graph) 
  // corresponding to the two ends of the tree edge. The pair is ordered: 
  // start-node --> end-node.
  // second: list of shock links
  vcl_map<vcl_pair<int, int>, vcl_vector<dbskfg_composite_link_sptr> > 
      ids_to_edge_vector_map;
  
  // Storage for the neighrboring nodes and costs of neighboring edges 
  // around each node 
  // Each member of the outer vector corresponds to one node in the tree, 
  // arranged in the same order as in "nodes_to_retain".
  // Each member of the inner vector corresponds to an edge incident at the 
  // node, arranged counter-clockwise
  vcl_vector< vcl_vector<vcl_pair<int, dbskr_edge_info> > > nodes;

  // Define iterator for map
  for ( unsigned int n=0 ; n < nodes_to_retain.size() ; ++n )
  {
      vcl_vector<vcl_pair<int, dbskr_edge_info> > current_node; 
      dbskfg_composite_node_sptr cur_node = nodes_to_retain[n];
      dbskfg_composite_link_sptr e = dbskfg_utilities::
          first_adj_shock(cur_node);
      unsigned int e_id_saved = e->id();

      bool ccw=false;
      if ( cur_node->get_composite_degree() == 3 ) 
      {
          vcl_vector<dbskfg_shock_link*>  cw_links=dbskfg_utilities::
              clockwise(cur_node);
          if ( cw_links.size() )
          {
              ccw=true;
          }
      }

      do
      {
          dbskfg_composite_node_sptr adj_node = e->opposite(cur_node);

          // List of edges of the current chain
          vcl_vector<dbskfg_composite_link_sptr> tmp;
          tmp.push_back(e);

          // See if this node is within nodes_to_retain_map
          vcl_map<unsigned int,unsigned int>::iterator find_it 
              = nodes_to_retain_map.find(adj_node->id());

          // advance until you hit a node which is nodes_to_retain_map
          unsigned int prev_id = cur_node->id();
          while ( find_it == nodes_to_retain_map.end() )
          {
              bool still = true;
              dbskfg_composite_node::edge_iterator srit;
              
              // Look at in edges first
              for ( srit  = adj_node->in_edges_begin() ; 
                    srit != adj_node->in_edges_end()   ; ++srit)
              {
                  if ((*srit)->link_type() == dbskfg_composite_link::
                      SHOCK_LINK)
                  {
                      dbskfg_shock_link* slink = 
                          dynamic_cast<dbskfg_shock_link*>(&(*(*srit)));
                      if ( slink->shock_link_type() == dbskfg_shock_link::
                           SHOCK_EDGE )
                      {
                          dbskfg_composite_node_sptr adj_adj_node =
                              (*srit)->opposite(adj_node);
                          if ( prev_id == adj_adj_node->id() )
                          {
                              continue;
                          }
                          tmp.push_back(*srit);
                          find_it = nodes_to_retain_map.
                              find(adj_adj_node->id());
                          prev_id = adj_node->id();
                          adj_node = adj_adj_node;
                          still=false;
                          break;
                      }
                  }
              }

              // In inner loop look at out edges
              if( still )
              {
                  // Look at out edges first
                  for ( srit  = adj_node->out_edges_begin() ; 
                        srit != adj_node->out_edges_end()   ; ++srit)
                  {
                      if ((*srit)->link_type() == dbskfg_composite_link::
                          SHOCK_LINK)
                      {
                          dbskfg_shock_link* slink = 
                              dynamic_cast<dbskfg_shock_link*>(&(*(*srit)));
                          if ( slink->shock_link_type() 
                               == dbskfg_shock_link::SHOCK_EDGE )
                          {
                 
                              dbskfg_composite_node_sptr adj_adj_node =
                                  (*srit)->opposite(adj_node);
                              
                              if ( prev_id == adj_adj_node->id() )
                              {
                                  continue;
                              }
                              tmp.push_back(*srit);
                              find_it = nodes_to_retain_map.
                                  find(adj_adj_node->id());
                              prev_id = adj_node->id();
                              adj_node = adj_adj_node;
                              break;

                          }
                      }

                  }    
              }
          }
               
          // we're done this is the neighbor
          unsigned int tree_node_id = find_it->second;

          // We are done with this neighbor
          vcl_pair<unsigned int,unsigned int> ids;
          ids.first  = cur_node->id();
          ids.second = nodes_to_retain[tree_node_id]->id(); 
          ids_to_edge_vector_map[ids]=tmp;

          // Create holder for contract and delete costs
          vcl_pair<unsigned int,dbskr_edge_info> p;
          p.first = tree_node_id;
          // costs are unknown for now
          (p.second).first = -1.0f;   // contract cost
          (p.second).second = -1.0f;  // delete cost
          current_node.push_back(p);
 
          // go to next adjacent node
          if ( ccw )
          {
              e = dbskfg_utilities::cyclic_adj_shock_succ(e,cur_node);
          }
          else
          {
              e = dbskfg_utilities::cyclic_adj_shock_pred(e,cur_node);
          }

      }while(e->id() != e_id_saved);
   
      nodes.push_back(current_node);
  }

  // initialize the trees from the list of node descriptors
  bool ok = dbskr_directed_tree::acquire(nodes);

  //assert(ok == true);
 
  if ( ok )
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
      vcl_vector<dbskfg_composite_link_sptr> tmp = ids_to_edge_vector_map[ids];
      shock_edges_.push_back(tmp);
      starting_nodes_.push_back(nodes_to_retain[tail]);

      // Hold darts that have virtual nodes
      vcl_map<unsigned int,dbskfg_composite_node*>::iterator bit;
      for ( bit=virtual_nodes.begin() ; bit != virtual_nodes.end() ; ++bit)
      {
          if ( dbskfg_utilities::is_node_in_set_of_links(tmp,(*bit).second))
          {
              darts_virtual_nodes_.push_back(i);
          }
      }
    }
  }

  return ok;
}


//------------------------------------------------------------------------------
//: Compute delete and contract costs for individual dart. 
// Only call this function after acquire_tree_topology has been 
// called and succeeded.
void dbskfg_cgraph_directed_tree::
compute_delete_and_contract_costs(bool elastic_splice_cost,
                                  bool construct_circular_ends,
                                  bool dpmatch_combined)
{

  // find contract and delete costs for each dart
  for (unsigned int i = 0; i<dart_cnt_; i++) 
  {

      // non-leaf darts
      if (!leaf(i) && !leaf(mate_[i])) 
      {
          // Compute costs via a dbskr_scurve
          dbskr_scurve_sptr sc = this->get_curve(i, i, construct_circular_ends);
     
          //: if it is a non-leaf dart compute the splice cost ALWAYS 
          //  with the circular completions because this is the 
          //  intermediate step of the subtree removal operation
          info_[i].first = (double)(sc->contract_cost());
          info_[i].second =(double)(sc->splice_cost(
                                        scurve_matching_R_, 
                                        elastic_splice_cost, 
                                        true, // true for circular ends
                                        dpmatch_combined, false));
       
                        
      }
      // for leaf darts, only compute for one direction
      else
      {
          info_[i].first = 1000.0f; // contract cost

          if (parent_dart(i) == static_cast<int>(i)) 
          {
              vcl_vector<int>::iterator find_it = vcl_find(
                  darts_virtual_nodes_.begin(),
                  darts_virtual_nodes_.end(),
                  i);

              bool circ_flag = (find_it == darts_virtual_nodes_.end())
                  ? construct_circular_ends : false;

              // Compute costs via a dbskr_scurve
              dbskr_scurve_sptr sc = this->get_curve(i, i, 
                                                     circ_flag);

              //: for leaf darts, compute the splice cost with or without 
              //  the circular completions depending on the 
              //  construct_circular_ends flag!!          
              info_[i].second = (float)sc->splice_cost(
                  scurve_matching_R_, 
                  elastic_splice_cost, 
                  circ_flag,
                  dpmatch_combined, true);

              info_[mate_[i]].second = info_[i].second;
          }
      }

  }

  this->find_subtree_delete_costs();

}

//------------------------------------------------------------------------------
//: Compute delete and contract costs for individual dart. 
// Only call this function after acquire_tree_topology has been 
// called and succeeded.
void dbskfg_cgraph_directed_tree::
set_delete_and_contract_costs(bool elastic_splice_cost,
                              bool construct_circular_ends,
                              bool dpmatch_combined,
                              float cost)
{

  // find contract and delete costs for each dart
  for (unsigned int i = 0; i<dart_cnt_; i++) 
  {

      // non-leaf darts
      if (!leaf(i) && !leaf(mate_[i])) 
      {
          // Compute costs via a dbskr_scurve
          dbskr_scurve_sptr sc = this->get_curve(i, i, construct_circular_ends);
     
          //: if it is a non-leaf dart compute the splice cost ALWAYS 
          //  with the circular completions because this is the 
          //  intermediate step of the subtree removal operation
          info_[i].first = (double)(sc->contract_cost());
          info_[i].second =(double)(sc->splice_cost(
                                        scurve_matching_R_, 
                                        elastic_splice_cost, 
                                        true, // true for circular ends
                                        dpmatch_combined, false));
       
                        
      }
      // for leaf darts, only compute for one direction
      else
      {
          info_[i].first = 1000.0f; // contract cost

          if (parent_dart(i) == static_cast<int>(i)) 
          {
              vcl_vector<int>::iterator find_it = vcl_find(
                  darts_virtual_nodes_.begin(),
                  darts_virtual_nodes_.end(),
                  i);

              bool circ_flag = (find_it == darts_virtual_nodes_.end())
                  ? construct_circular_ends : false;

              // Compute costs via a dbskr_scurve
              dbskr_scurve_sptr sc = this->get_curve(i, i, 
                                                     circ_flag);

              //: for leaf darts, compute the splice cost with or without 
              //  the circular completions depending on the 
              //  construct_circular_ends flag!!          
              info_[i].second = (float)sc->splice_cost(
                  scurve_matching_R_, 
                  elastic_splice_cost, 
                  circ_flag,
                  dpmatch_combined, true);

              info_[mate_[i]].second = info_[i].second;
          }
      }
      info_[i].first=cost;
      info_[i].second=cost;
  }

  this->find_subtree_delete_costs();

}

//------------------------------------------------------------------------------
//: find and cache the shock curve for this pair of darts, if not already cached
// \todo write this
dbskr_scurve_sptr dbskfg_cgraph_directed_tree::
get_curve(int start_dart, int end_dart, bool construct_circular_ends)
{
  dbskr_sc_pair_sptr sc_pair = this->get_curve_pair(start_dart, 
                                                    end_dart, 
                                                    construct_circular_ends);
  if (!sc_pair)
    return 0;
  else
    return sc_pair->coarse;

}


//------------------------------------------------------------------------------
//: returns both the coarse and dense version of shock curve
dbskr_sc_pair_sptr dbskfg_cgraph_directed_tree::
get_curve_pair(int start_dart, int end_dart, bool construct_circular_ends)
{
  vcl_pair<int, int> p;
  p.first = start_dart;
  p.second = end_dart;
  vcl_map<vcl_pair<int, int>, dbskr_sc_pair_sptr>::iterator iter = 
      dart_path_scurve_map_.find(p);

  if (iter == dart_path_scurve_map_.end()) // not found yet
  {    
    vcl_vector<int>& dart_list = this->get_dart_path(start_dart, end_dart);
    
    dbskfg_composite_node_sptr start_node;  
    vcl_vector<dbskfg_composite_link_sptr> edges;

    // get the underlying shock graph edge list for this dart path on the tree
    this->get_edge_list(dart_list, start_node, edges);
    
    //Curve reconstruct object
    dbskfg_compute_scurve reconstructor;

    dbskr_scurve_sptr sc;

    if (construct_circular_ends)
    {
       
        sc = reconstructor.compute_curve(start_node, edges, 
                                         leaf_[end_dart], 
                                         true, true, 
                                         vcl_min((float)scurve_sample_ds_, 
                                                 interpolate_ds_), 
                                         scurve_sample_ds_,
                                         scale_ratio_);

    }
    else
    {
        sc = reconstructor.compute_curve(start_node, edges, 
                                         false, 
                                         true, true, 
                                         vcl_min((float)scurve_sample_ds_, 
                                                 interpolate_ds_), 
                                         scurve_sample_ds_,
                                         scale_ratio_);
    }

    dbskr_sc_pair_sptr curve_pair = new dbskr_sc_pair();
    curve_pair->coarse = sc;
    
    // we never use the dense version, there is no need to construct it
    curve_pair->dense = 0;

    //: note: even if interpolate_ds_ is not properly set by the user 
    //  vcl_min takes care of it
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
//: return a vector of pointers to the edges in underlying composite graph 
//   for the given dart list
void dbskfg_cgraph_directed_tree::
get_edge_list(const vcl_vector<int>& dart_list,  
              dbskfg_composite_node_sptr& start_node,  
              vcl_vector<dbskfg_composite_link_sptr>& path) 
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


//------------------------------------------------------------------------------
//: create and write .shg file to debug splice and contract costs
bool dbskfg_cgraph_directed_tree::create_shg(vcl_string fname)
{
  //create a file
  vcl_ofstream outf(fname.c_str(), vcl_ios::out);
  if (!outf){
    vcl_cerr << "file could not be created";
    return false;
  }

  //Header
  outf << "Shock Graph v1.0 " << "0.0" << " " << 
      this->total_splice_cost() << " " << this->node_size() << vcl_endl;

  outf.precision(6);

  //costs for the rest of the graph
  for (int nn=0;nn<node_size();nn++)
  {
    vcl_vector<int>& odarts = out_darts(nn) ;

    for (unsigned j=0; j<odarts.size(); j++)
      outf << this->head(odarts[j]) << " "   << 
          this->contract_cost(odarts[j]) << " " << 
          this->delete_cost(odarts[j]) << vcl_endl;

    outf << vcl_endl;
  }

  outf.close();
  return true;
}
