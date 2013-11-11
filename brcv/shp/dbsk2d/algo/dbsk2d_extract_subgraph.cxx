// This is brcv/shp/dbsk2d/algo/dbsk2d_compute_bounding_box.cxx

//:
// \file

#include <dbsk2d/algo/dbsk2d_compute_bounding_box.h>
#include <vsol/vsol_box_2d.h>
#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbsk2d/dbsk2d_xshock_edge_sptr.h>
#include <dbsk2d/dbsk2d_xshock_edge.h>
#include <dbsk2d/dbsk2d_xshock_node.h>

dbsk2d_shock_node_sptr add_node(dbsk2d_shock_graph_sptr xshock_graph, dbsk2d_shock_node_sptr node) 
{
  dbsk2d_shock_graph::vertex_iterator curN;
  for (curN = xshock_graph->vertices_begin(); curN != xshock_graph->vertices_end(); curN++)
    if ((*curN)->id() == node->id())
      return (*curN);

  dbsk2d_xshock_node* xnode = dynamic_cast<dbsk2d_xshock_node*>(node.ptr());
  dbsk2d_shock_node_sptr new_xnode;
  if (xnode) {
    /*vcl_vector<dbsk2d_xshock_sample_sptr > samples;
    for (int i = 0; i < xnode->num_samples(); i++)
      samples.push_back(new dbsk2d_xshock_sample(*(xnode->sample(i))));
    
    new_xnode = new dbsk2d_xshock_node(xnode->id(), samples, xnode->is_inside_shock());
    */
    new_xnode = new dbsk2d_xshock_node(*xnode);
  } else {
    new_xnode = new dbsk2d_xshock_node(node->id());
  }

  //compute extrinsic points for display purposes
  vcl_vector<vgl_point_2d<double> > & pts = node->ex_pts();
  for (unsigned i = 0; i < pts.size(); i++) {
    new_xnode->ex_pts().push_back(pts[i]);
  }
  //new_xnode->ex_pts().push_back(node->ex_pts().front());
  new_xnode->form_shock_fragments();

  //insert it into the extrinsic shock graph
  xshock_graph->add_vertex(new_xnode->cast_to_shock_node());

  return new_xnode;
}

dbsk2d_shock_edge_sptr add_edge(dbsk2d_shock_graph_sptr xshock_graph, dbsk2d_shock_edge_sptr edge, dbsk2d_shock_node_sptr s, dbsk2d_shock_node_sptr t) 
{
  dbsk2d_shock_graph::edge_iterator curE;
  for (curE = xshock_graph->edges_begin(); curE != xshock_graph->edges_end(); curE++)
    if ((*curE)->id() == edge->id())
      return (*curE);

  dbsk2d_xshock_edge* xedge = dynamic_cast<dbsk2d_xshock_edge*>(edge.ptr());
  dbsk2d_shock_edge_sptr new_xedge;
  if (xedge) {
    vcl_vector<dbsk2d_xshock_sample_sptr > samples;
    for (int i = 0; i < xedge->num_samples(); i++)
      samples.push_back(new dbsk2d_xshock_sample(*(xedge->sample(i))));
    
    new_xedge = new dbsk2d_xshock_edge(xedge->id(), s, t, samples, xedge->is_inside_shock());
  } else {
    new_xedge = new dbsk2d_xshock_edge(edge->id(), s, t);
  }

  //compute extrinsic points for display purposes
  vcl_vector<vgl_point_2d<double> > & pts = edge->ex_pts();
  for (unsigned i = 0; i < pts.size(); i++) {
    new_xedge->ex_pts().push_back(pts[i]);
  }

  //This is not the best place to do this (FIX ME!!!)
  new_xedge->form_shock_fragment();

  //insert it into the extrinsic shock graph
  xshock_graph->add_edge(new_xedge->cast_to_shock_edge());
  s->add_outgoing_edge(new_xedge);
  t->add_incoming_edge(new_xedge);

  return new_xedge;
}

// if next_node is a degree two it should have an other edge connected to it,
// otherwise next_node was a degree one
// replace next_node with the other node of the other edge if exists, replace current_edge with the new edge
//                    new edge
//                        |
//  --------------- * --------- * ----
//         |        |           |
//  current_edge next_node  other_node
// return true if other_node is the source of new edge
bool get_other_edge(dbsk2d_shock_node_sptr& next_node, dbsk2d_shock_edge_sptr& current_edge) 
{
  dbsk2d_shock_graph::edge_iterator curE;
  bool found_it = false;
  for (curE = next_node->in_edges_begin(); curE != next_node->in_edges_end(); curE++)
  {
    if (current_edge->id() == (*curE)->id())
      continue;
    current_edge = (*curE);
    next_node = current_edge->source();
    found_it = true;
    break;
  }
  if (found_it)
    return true;
  for (curE = next_node->out_edges_begin(); curE != next_node->out_edges_end(); curE++)
  {
    if (current_edge->id() == (*curE)->id())
      continue;
    current_edge = (*curE);
    next_node = current_edge->target();
    found_it = true;
    break;
  }
  if (!found_it) {
    next_node = 0;
    current_edge = 0;
    return false;
  } 
  return false;
}

//: take an "extrinsic" shock graph, and a pointer to one of its node, and return the subgraph at the given depth 
//  so that the given node is the root
void
dbsk2d_extract_subgraph (dbsk2d_shock_graph_sptr xshock_graph, dbsk2d_shock_edge_sptr parent_edge, dbsk2d_shock_node_sptr node, int depth)
{
  if (depth <= 0)
    return;
  //: add_node checks whether a node with this id already exists
  dbsk2d_shock_node_sptr root = add_node(xshock_graph, node);
  
  //: insert edges and degree 2 nodes till we hit a degree 3 node at a given depth
  dbsk2d_shock_graph::edge_iterator curE;
  for (curE = node->in_edges_begin(); curE != node->in_edges_end(); curE++)
  {
    if (parent_edge && (*curE)->id() == parent_edge->id())
      continue;
    dbsk2d_shock_node_sptr next_node = (*curE)->source();
    dbsk2d_shock_edge_sptr current_edge = (*curE);
    dbsk2d_shock_node_sptr current_node = root;
    bool source = true;
    while (next_node && next_node->degree() < 3) {
      dbsk2d_shock_node_sptr snode = add_node(xshock_graph, next_node);
      // add_edge checks whether an edge with this id already exists
      if (source)
        add_edge(xshock_graph, current_edge, snode, current_node); 
      else
        add_edge(xshock_graph, current_edge, current_node, snode); 
      current_node = snode;
      // if target is a degree two it should have an other edge connected to it,
      // otherwise target was a degree one
      // replace current_target with the new target if exists, replace current_edge with the new edge
      source = get_other_edge(next_node, current_edge);
    }
    if (next_node && current_edge && next_node->degree() >= 3) {
      dbsk2d_shock_node_sptr snode = add_node(xshock_graph, next_node);
      if (source)
        add_edge(xshock_graph, current_edge, snode, current_node); 
      else
        add_edge(xshock_graph, current_edge, current_node, snode); 
      dbsk2d_extract_subgraph(xshock_graph, current_edge, next_node, depth-1);
    }

  }
  for (curE = node->out_edges_begin(); curE != node->out_edges_end(); curE++)
  {
    if (parent_edge && (*curE)->id() == parent_edge->id())
      continue;
    dbsk2d_shock_node_sptr next_node = (*curE)->target();
    dbsk2d_shock_edge_sptr current_edge = (*curE);
    dbsk2d_shock_node_sptr current_node = root;
    bool source = false;
    while (next_node && next_node->degree() < 3) {
      dbsk2d_shock_node_sptr tnode = add_node(xshock_graph, next_node);
      if (source)
        add_edge(xshock_graph, current_edge, tnode, current_node); 
      else
        add_edge(xshock_graph, current_edge, current_node, tnode); 
      current_node = tnode;
      // if target is a degree two it should have an other edge connected to it,
      // otherwise target was a degree one
      // replace current_target with the new target if exists, replace current_edge with the new edge
      source = get_other_edge(next_node, current_edge);
    }
    if (next_node && current_edge && next_node->degree() >= 3) {
      dbsk2d_shock_node_sptr tnode = add_node(xshock_graph, next_node);
      if (source)
        add_edge(xshock_graph, current_edge, tnode, current_node); 
      else
        add_edge(xshock_graph, current_edge, current_node, tnode); 
      dbsk2d_extract_subgraph(xshock_graph, current_edge, next_node, depth-1);
    }
  }

  return;
}

