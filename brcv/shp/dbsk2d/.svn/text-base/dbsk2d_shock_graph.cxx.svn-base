// This is brcv/shp/dbsk2d/dbsk2d_shock_graph.cxx

//:
// \file

#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbsk2d/dbsk2d_shock_edge.h>
#include <dbsk2d/dbsk2d_shock_grouped_ishock_edge.h>
#include <dbsk2d/dbsk2d_shock_grouped_ishock_edge_sptr.h>

//: clear all the nodes and edges of this graph
//  and also the other objects defined on the nodes and edges
void dbsk2d_shock_graph::clear()
{
  //delete the shock fragments before deleting the node and edges
  delete_shock_fragments();

  dbgrl_graph<dbsk2d_shock_node , dbsk2d_shock_edge>::clear();
}

//: delete the shock fragments defined on this shock graph
void dbsk2d_shock_graph::delete_shock_fragments()
{
  for ( edge_iterator e_itr = edges_begin();
        e_itr != edges_end(); ++e_itr )
  {
    //delete the shock fragment defined on this edge
    (*e_itr)->clear_shock_fragment();
  }

  for ( vertex_iterator v_itr = vertices_begin();
        v_itr != vertices_end(); ++v_itr )
  {
    //delete the shock fragment defined on this shock node
    (*v_itr)->clear_shock_fragment();
  }
}


dbsk2d_shock_node_sptr 
dbsk2d_shock_graph::get_other_end_merging_degree_twos(dbsk2d_shock_node_sptr n, dbsk2d_shock_edge_sptr e)
{
  if (!n || !e)
    return 0;

  dbsk2d_shock_node_sptr current_node = n;
  dbsk2d_shock_edge_sptr current_edge = e;  
  dbsk2d_shock_node_sptr next_node = current_edge->opposite(current_node);
  while (next_node && next_node->degree() == 2) { 
    current_node = next_node;
    current_edge = cyclic_adj_succ(current_edge, current_node); 
    next_node = current_edge->opposite(current_node);
  }

  if (next_node) 
    return next_node;
  else 
    return 0;
}

dbsk2d_shock_node_sptr 
dbsk2d_shock_graph::get_other_end_merging_degree_twos(dbsk2d_shock_node_sptr n, dbsk2d_shock_edge_sptr e, vcl_vector<dbsk2d_shock_edge_sptr>& edges)
{
  if (!n || !e)
    return 0;

  dbsk2d_shock_node_sptr current_node = n;
  dbsk2d_shock_edge_sptr current_edge = e;  
  edges.push_back(current_edge);
  dbsk2d_shock_node_sptr next_node = current_edge->opposite(current_node);
  while (next_node && next_node->degree() == 2) { 
    current_node = next_node;
    current_edge = cyclic_adj_succ(current_edge, current_node); 
    edges.push_back(current_edge);
    next_node = current_edge->opposite(current_node);
  }

  if (next_node) 
    return next_node;
  else 
    return 0;
}

dbsk2d_shock_node_sptr 
dbsk2d_shock_graph::get_node(int id)
{
  dbsk2d_shock_node_sptr current_node;
  for (vertex_iterator v_itr = vertices_begin(); v_itr != vertices_end(); v_itr++)
  { 
    current_node = *v_itr;
    if (current_node->id() == id)
      return current_node;
  }
   
  return 0;
}

