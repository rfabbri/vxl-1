// This is file seg/dbsks/dbsks_dp_match_utils.cxx

//:
// \file

#include "dbsks_dp_match_utils.h"
#include <dbsksp/dbsksp_shock_graph.h>
#include <vcl_utility.h>
#include <dbsksp/dbsksp_shapelet.h>
#include <dbsksp/algo/dbsksp_interp_two_xnodes.h>
#include <dbsksp/dbsksp_shock_fragment.h>
#include <vnl/vnl_math.h>

//: Construct a graph from a list of states for each of its edges
dbsksp_shock_graph_sptr dbsks_construct_graph(
  dbsks_dp_match_sptr dp_engine,
  vcl_map<unsigned int, vgl_point_2d<int > >& graph_i_state_map)
{
  dbsksp_shock_graph_sptr model_graph = dp_engine->graph();
  

  // First construct the shapelets corresponding to the states of the graph
  vcl_map<dbsksp_shock_edge_sptr, dbsksp_shapelet_sptr > shapelet_map;
  for (vcl_map<unsigned int, vgl_point_2d<int > >::const_iterator it =
    graph_i_state_map.begin(); it != graph_i_state_map.end(); ++it)
  {
    dbsksp_shock_edge_sptr e = model_graph->edge_from_id(it->first);
    int i_xy = it->second.x();
    int i_plane = it->second.y();
    dbsks_shapelet_grid& grid = dp_engine->shapelet_grid_map_.find(e)->second;

    // compute the shapelet associated with this state
    dbsksp_shapelet_sptr s_e = grid.shapelet(i_xy, i_plane);
    shapelet_map.insert(vcl_make_pair(e, s_e));
  }


  vcl_map<dbsksp_shock_edge_sptr, dbsksp_twoshapelet_sptr > twoshapelet_map;
  for (vcl_map<dbsksp_shock_edge_sptr, dbsksp_shapelet_sptr >::iterator it = 
    shapelet_map.begin(); it != shapelet_map.end(); ++it)
  {
    dbsksp_shock_edge_sptr e = it->first;
    dbsksp_shapelet_sptr s_e = it->second;
    dbsks_shapelet_grid& grid_e = dp_engine->shapelet_grid_map_.find(e)->second;

    // left and right
    dbsksp_shock_node_sptr v_child = e->child_node();
    dbsksp_shock_node_sptr v_parent = e->parent_node();

    // interpolate to get smooth boundary
    if (v_child->degree() == 2) 
    {
      if (!grid_e.has_rear_arc_ )
      {
        // compute two shapelets from two extrinsic nodes
        vgl_point_2d<double > left_start_e = s_e->bnd_start(0);
        vgl_point_2d<double > right_start_e = s_e->bnd_start(1);
        vgl_point_2d<double > start_e = s_e->start();

        dbsksp_xshock_node_descriptor start_xnode(start_e, left_start_e, right_start_e);

        // the rear shapelet should come from the children
        dbsksp_shock_edge_sptr e1 = model_graph->cyclic_adj_succ(e, v_child);
        dbsksp_shapelet_sptr s_e1 = shapelet_map.find(e1)->second;
        vgl_point_2d<double > left_start_e1 = s_e1->bnd_start(0);
        vgl_point_2d<double > right_start_e1 = s_e1->bnd_start(1);
        vgl_point_2d<double > start_e1 = s_e1->start();
        dbsksp_xshock_node_descriptor end_xnode(start_e1, left_start_e1, right_start_e1);

        dbsksp_twoshapelet_sptr ss_e = 0;
        dbsksp_optimal_interp_two_xnodes interpolator(start_xnode, end_xnode);
        ss_e = interpolator.optimize();
        dbsksp_shapelet_sptr s0_e = ss_e->shapelet_start();
        dbsksp_shapelet_sptr s1_e = ss_e->shapelet_end();
        twoshapelet_map.insert(vcl_make_pair(e, ss_e));
      }
      else
      {
        // break the original shapelet into two pieces

        //// compute two shapelets from two extrinsic nodes
        //vgl_point_2d<double > left_start_e = s_e->bnd_start(0);
        //vgl_point_2d<double > right_start_e = s_e->bnd_start(1);
        //vgl_point_2d<double > start_e = s_e->start();

        //dbsksp_xshock_node_descriptor start_xnode(start_e, left_start_e, right_start_e);

        //// the rear shapelet also come from the parent
        //vgl_point_2d<double > left_end_e = s_e->bnd_end(0);
        //vgl_point_2d<double > right_end_e = s_e->bnd_end(1);
        //vgl_point_2d<double > end_e = s_e->end();
        //dbsksp_xshock_node_descriptor end_xnode(end_e, left_end_e, right_end_e);

        //dbsksp_twoshapelet_sptr ss_e = 0;
        //dbsksp_optimal_interp_two_xnodes interpolator(start_xnode, end_xnode);
        //ss_e = interpolator.optimize();

        dbsksp_twoshapelet_sptr ss_e = new dbsksp_twoshapelet(s_e, 0.5);

        dbsksp_shapelet_sptr s0_e = ss_e->shapelet_start();
        dbsksp_shapelet_sptr s1_e = ss_e->shapelet_end();
        twoshapelet_map.insert(vcl_make_pair(e, ss_e));
      
      }
    }
    else
    {
      vcl_cout << "ERROR: in function dbsks_construct_graph :"
        << " current codes cannot handle shock graph with degree-3 nodes."
        << " quit function now.\n";
      return 0;
    }
  }

  // Now construct a new graph by replacing each edge of the old graph with two
  // new edges in order to accomodate the twoshapelets
  dbsksp_shock_graph_sptr g = new dbsksp_shock_graph(*model_graph);
  g->compute_all_dependent_params();

  // 1. Modify the non-terminal edges
  for (dbsksp_shock_graph::edge_iterator eit = model_graph->edges_begin(); 
    eit != model_graph->edges_end(); ++eit)
  {
    dbsksp_shock_edge_sptr e_model = *eit;
    if (e_model->is_terminal_edge())
      continue;
    unsigned int id_v_child = e_model->child_node()->id();
    unsigned int id_v_parent = e_model->parent_node()->id();

    dbsksp_twoshapelet_sptr ss = twoshapelet_map.find(e_model)->second;
    dbsksp_shock_edge_sptr e_g = g->edge_from_id(e_model->id());

    dbsksp_shock_node_sptr new_v_g = g->insert_shock_node(e_g, 0.5);
    dbsksp_shock_node_sptr v_child_g = g->node_from_id(id_v_child);
    dbsksp_shock_node_sptr v_parent_g = g->node_from_id(id_v_parent);

    // Determine the two new edges corresponding to the parent and child vertices
    dbsksp_shock_edge_sptr e_child_g = *new_v_g->edges_begin();
    dbsksp_shock_edge_sptr e_parent_g = 0;
    if (e_child_g->is_vertex(v_child_g))
    {
      e_parent_g = g->cyclic_adj_succ(e_child_g, new_v_g);
    }
    else
    {
      e_parent_g = e_child_g;
      e_child_g = g->cyclic_adj_succ(e_parent_g, new_v_g);
    }

    e_parent_g->form_fragment();
    e_parent_g->fragment()->update_edge_with_shapelet(ss->shapelet_start(), 
      v_parent_g, false);

    e_child_g->form_fragment();
    e_child_g->fragment()->update_edge_with_shapelet(ss->shapelet_end(), 
      new_v_g, false);    

    v_parent_g->set_radius(ss->shapelet_start()->radius_start());
    v_child_g->set_radius(ss->shapelet_end()->radius_end());
  }

  // 2. Modify the phi's of the terminal nodes
  for (dbsksp_shock_graph::edge_iterator eit = g->edges_begin(); 
    eit != g->edges_end(); ++eit)
  {
    dbsksp_shock_edge_sptr e0 = *eit;
    if (!e0->is_terminal_edge())
      continue;

    // find the non-degenerate node
    dbsksp_shock_node_sptr v0 = (e0->source()->degree() == 1) ? e0->target() : e0->source();
    dbsksp_shock_edge_sptr e1 = g->cyclic_adj_succ(e0, v0);

    // compute the angle
    v0->descriptor(e0)->phi = vnl_math::pi - v0->descriptor(e1)->phi;
  }

  // 3. Modify the reference nodes and edges
  // Find the current ref_node and edges
  dbsksp_shock_node_sptr ref_node = g->ref_node();
  dbsksp_shock_edge_sptr ref_edge = *ref_node->edges_begin();
  if (ref_edge->is_terminal_edge())
    ref_edge = g->cyclic_adj_succ(ref_edge, ref_node);

  g->set_all_ref(ref_node, ref_edge);









  // TODO
  // WHY the result graph has only 9 instead of 10 fragments (5 x 2)
  // SEEMS like ref_radius is not updated properly. CHECK this.

  return g;
}








//: 
dbsksp_shock_graph_sptr 
dbsks_construct_graph(dbsks_dp_match_sptr dp_engine, 
                      vcl_map<dbsksp_shock_edge_sptr, vgl_point_2d<int > >& graph_i_state_map)
{
  vcl_map<unsigned int, vgl_point_2d<int > > state_map;
  for (vcl_map<dbsksp_shock_edge_sptr, vgl_point_2d<int > >::iterator it = 
    graph_i_state_map.begin(); it != graph_i_state_map.end(); ++it)
  {
    unsigned int id = it->first->id();
    vgl_point_2d<int > state = it->second;
    state_map.insert(vcl_make_pair(id, state));
  }

  return dbsks_construct_graph(dp_engine, state_map);
}




