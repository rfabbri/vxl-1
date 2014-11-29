// This is file shp/dbsksp/dbsksp_convert.cxx

//:
// \file

#include "dbsksp_convert.h"

#include <dbsksp/dbsksp_shock_graph.h>
#include <dbsksp/dbsksp_xshock_graph.h>
#include <vnl/vnl_math.h>
#include <vcl_utility.h>


//------------------------------------------------------------------------------
//: Convert an intrinsic generative shock graph to an extrinsic shock graph
dbsksp_xshock_graph_sptr dbsksp_convert_to_xshock_graph(const dbsksp_shock_graph_sptr& graph)
{
  // Preliminary check
  if (!graph) return 0;

  // Place holder for new graph
  dbsksp_xshock_graph_sptr xgraph = new dbsksp_xshock_graph();

  unsigned int max_id = 0;
  // create the extrinsic nodes
  vcl_map<dbsksp_shock_node_sptr, dbsksp_xshock_node_sptr > node_map;
  for (dbsksp_shock_graph::vertex_iterator vit = graph->vertices_begin(); 
    vit != graph->vertices_end(); ++vit)
  {
    dbsksp_shock_node_sptr v = *vit; 
    dbsksp_xshock_node_sptr xv = new dbsksp_xshock_node(v->id());
    node_map.insert(vcl_make_pair(v, xv));
    max_id = vnl_math::max(max_id, v->id());
  }

  // create the extrinsic edges
  vcl_map<dbsksp_shock_edge_sptr, dbsksp_xshock_edge_sptr > edge_map;
  for (dbsksp_shock_graph::edge_iterator eit = graph->edges_begin();
    eit != graph->edges_end(); ++eit)
  {
    dbsksp_shock_edge_sptr e = *eit;
    dbsksp_xshock_node_sptr xsource = node_map[e->source()];
    assert(xsource);
    dbsksp_xshock_node_sptr xtarget = node_map[e->target()];
    assert(xtarget);
    dbsksp_xshock_edge_sptr xe = new dbsksp_xshock_edge(xsource, xtarget, e->id());
    edge_map.insert(vcl_make_pair(e, xe));

    max_id = vnl_math::max(max_id, e->id());
  }
  
  // add the nodes and edges to the graph
  for (vcl_map<dbsksp_shock_node_sptr, dbsksp_xshock_node_sptr >::iterator it = 
    node_map.begin(); it != node_map.end(); ++it)
  {
    xgraph->add_vertex(it->second);
  }

  for (vcl_map<dbsksp_shock_edge_sptr, dbsksp_xshock_edge_sptr >::iterator it = 
    edge_map.begin(); it != edge_map.end(); ++it)
  {
    xgraph->add_edge(it->second);
  }
  xgraph->set_next_available_id(max_id);

  // add node-edge connectivity and properties for the new graph
  for (dbsksp_shock_graph::vertex_iterator vit = graph->vertices_begin();
    vit != graph->vertices_end(); ++vit)
  {
    dbsksp_shock_node_sptr v = *vit;
    dbsksp_xshock_node_sptr xv = node_map[v];

    for (dbsksp_shock_node::edge_iterator eit = v->edges_begin(); eit != 
      v->edges_end(); ++eit)
    {
      dbsksp_shock_edge_sptr e = *eit;
      dbsksp_xshock_edge_sptr xe = edge_map[e];
      dbsksp_xshock_node_descriptor* xdesc = xv->insert_shock_edge(xe, 0);
      assert(xdesc);

      // copy the properties
      dbsksp_shock_node_descriptor_sptr desc = v->descriptor(e);
      xdesc->phi_ = desc->phi;
      vgl_vector_2d<double > chord_v = e->chord_dir(v);
      vgl_vector_2d<double > tangent_v = rotated(chord_v, desc->alpha);
      xdesc->psi_ = vcl_atan2(tangent_v.y(), tangent_v.x());
    }

    // copy node properties
    xv->set_pt(v->pt());
    xv->set_radius(v->radius());
  }
  return xgraph;  
}







