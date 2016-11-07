// This is file shp/dbsksp/algo/dbsksp_xgraph_model.cxx

//:
// \file

#include "dbsksp_xgraph_model.h"


#include <dbsksp/dbsksp_xshock_graph.h>
#include <dbsksp/dbsksp_xshock_edge.h>
#include <dbsksp/dbsksp_xshock_node.h>
#include <dbsksp/dbsksp_xshock_fragment.h>
#include <vnl/vnl_math.h>


//==============================================================================
// dbsksp_xgraph_model_using_L_alpha_phi_radius
//==============================================================================



//------------------------------------------------------------------------------
//: Constructor
dbsksp_xgraph_model_using_L_alpha_phi_radius::
dbsksp_xgraph_model_using_L_alpha_phi_radius(const dbsksp_xshock_graph_sptr& xgraph,
                                             unsigned root_vid, unsigned pseudo_parent_eid):
dbsksp_xgraph_model(xgraph)
{
  this->xgraph()->compute_vertex_depths(root_vid);
  this->root_node_ = this->xgraph()->node_from_id(root_vid);
  this->pseudo_parent_edge_ = this->xgraph()->edge_from_id(pseudo_parent_eid);

  assert(this->root_node_);
  assert(this->pseudo_parent_edge_);
  assert(this->pseudo_parent_edge_->is_vertex(this->root_node_));

  this->update_param_maps_from_xgraph();

  // compute dimension of the state vector
  // 3 for each edge
  // 2 for each degree-2 node
  // 3 for each degree-3 node
  {
    unsigned sum = 0;
    for (vcl_map<dbsksp_xshock_node_sptr, node_params >::iterator iter =
      this->map_xv_to_params_.begin(); iter != this->map_xv_to_params_.end(); ++iter)
    {
      dbsksp_xshock_node_sptr xv = iter->first;
      assert(xv->degree() < 4);
      if (xv->degree() == 2)
      {
        sum += 2;
      }
      else if (xv->degree() == 3)
      {
        sum += 3;
      }
    }

    // every edge has the same number of params
    sum += 3 * this->map_xe_to_params_.size();

    this->dim_ = sum;
  }
  return;
}


//------------------------------------------------------------------------------
//: Current `state' of the shock graph
void dbsksp_xgraph_model_using_L_alpha_phi_radius::
get_xgraph_state(vnl_vector<double >& x)
{
  x.set_size(this->dim());
  vnl_vector<double >::iterator ix = x.begin();

  // update the parameter map
  this->update_param_maps_from_xgraph();

  // copy parameter values to x

  // a) paremters at the nodes
  for (vcl_map<dbsksp_xshock_node_sptr, node_params >::iterator iter = 
    this->map_xv_to_params_.begin(); iter != this->map_xv_to_params_.end(); ++iter)
  {
    node_params& p = iter->second;
    *(ix++) = p.radius;
    *(ix++) = p.phi_parent;
    if (p.degree == 3)
    {
      *(ix++) = p.delta_phi;
    }
  }

  // b) parameters at the edges
  for (vcl_map<dbsksp_xshock_edge_sptr, edge_params >::iterator iter = 
    this->map_xe_to_params_.begin(); iter != this->map_xe_to_params_.end(); ++iter)
  {
    edge_params& p = iter->second;
    *(ix++) = p.chord_len;
    *(ix++) = p.alpha_parent;
    *(ix++) = p.alpha_child;
  }
  return;
}



//------------------------------------------------------------------------------
//: Update the shock graph with a `state' vector
void dbsksp_xgraph_model_using_L_alpha_phi_radius::
set_xgraph_state(const vnl_vector<double >& x)
{
  // update the param map from x
  vnl_vector<double >::const_iterator ix = x.begin();

  // a) paremters at the nodes
  for (vcl_map<dbsksp_xshock_node_sptr, node_params >::iterator iter = 
    this->map_xv_to_params_.begin(); iter != this->map_xv_to_params_.end(); ++iter)
  {
    node_params& p = iter->second;
    p.radius = *(ix++);
    p.phi_parent = *(ix++);
    if (p.degree == 3)
    {
      p.delta_phi = *(ix++);
    }
  }

  // b) parameters at the edges
  for (vcl_map<dbsksp_xshock_edge_sptr, edge_params >::iterator iter = 
    this->map_xe_to_params_.begin(); iter != this->map_xe_to_params_.end(); ++iter)
  {
    edge_params& p = iter->second;
    p.chord_len = *(ix++);
    p.alpha_parent = *(ix++);
    p.alpha_child = *(ix++);
  }

  this->update_xgraph_from_param_maps();
  return;
}


//------------------------------------------------------------------------------
//: Get intrinsic parameters for a given set of nodes and edges
// The parameters will be packed in 'x' using the order in 'active_nodes' followed
// by 'active_edges'
void dbsksp_xgraph_model_using_L_alpha_phi_radius::
get_intrinsic_params(const vcl_vector<dbsksp_xshock_node_sptr >& active_nodes,
                     const vcl_vector<dbsksp_xshock_edge_sptr >& active_edges,
                     vnl_vector<double >& x)
{
  vcl_vector<double* > x_ptr;
  this->get_intrinsic_params_ptr(active_nodes, active_edges, x_ptr);

  this->update_param_maps_from_xgraph();
  x.set_size(x_ptr.size());
  for (unsigned i =0; i < x.size(); ++i)
  {
    x[i] = *(x_ptr[i]);
  }
}


//------------------------------------------------------------------------------
//: Get pointers for direct access to intrinsic parameters of a set of nodes and edges
  // The parameters will be packed using the order of 'active_nodes' followed by 'active_edges'
void dbsksp_xgraph_model_using_L_alpha_phi_radius::
get_intrinsic_params_ptr(const vcl_vector<dbsksp_xshock_node_sptr >& active_nodes,
    const vcl_vector<dbsksp_xshock_edge_sptr >& active_edges,
    vcl_vector<double* >& x_ptr)
{
  x_ptr.resize(this->get_num_intrinsic_params(active_nodes, active_edges));

  // copy parameter pointers to x_ptr
  vcl_vector<double* >::iterator ix = x_ptr.begin();

  // a) paremters at the nodes
  for (vcl_vector<dbsksp_xshock_node_sptr >::const_iterator iter = active_nodes.begin();
    iter != active_nodes.end(); ++iter)
  {
    dbsksp_xshock_node_sptr xv = *iter;
    
    // no free params for degree-1 nodes
    if (xv->degree() == 1)
      continue;

    node_params& p = this->map_xv_to_params_[*iter];
    *(ix++) = &(p.radius);
    *(ix++) = &(p.phi_parent);
    if (p.degree == 3)
    {
      *(ix++) = &(p.delta_phi);
    }
  }

  // b) parameters at the edges
  for (vcl_vector<dbsksp_xshock_edge_sptr >::const_iterator iter = active_edges.begin(); 
    iter != active_edges.end(); ++iter)
  {
    dbsksp_xshock_edge_sptr xe = *iter;
   
    // no params for terminal edges
    if (xe->is_terminal_edge())
      continue;
    
    edge_params& p = this->map_xe_to_params_[*iter];
    *(ix++) = &(p.chord_len);
    *(ix++) = &(p.alpha_parent);
    *(ix++) = &(p.alpha_child);
  }

  assert(ix == x_ptr.end());
  return;
}


////------------------------------------------------------------------------------
////: Set intrinsic parameters for a given set of nodes and edges
//// The parameters will be retrieved from 'x' using the order in 'active_nodes' followed
//// by 'active_edges'
//void dbsksp_xgraph_model_using_L_alpha_phi_radius::
//set_intrinsic_params(const vcl_vector<dbsksp_xshock_node_sptr >& active_nodes,
//                     const vcl_vector<dbsksp_xshock_edge_sptr >& active_edges,
//                     vnl_vector<double >& x)
//{
//
//  return;
//}




//------------------------------------------------------------------------------
//: Count number of free parameters corresponding to a list of nodes and edges
unsigned dbsksp_xgraph_model_using_L_alpha_phi_radius::
get_num_intrinsic_params(const vcl_vector<dbsksp_xshock_node_sptr >& active_nodes,
                         const vcl_vector<dbsksp_xshock_edge_sptr >& active_edges)
{
  // determine dimension of the cost function;
  unsigned sum = 0;

  // free parameters on the nodes
  for (unsigned i =0; i < active_nodes.size(); ++i)
  {
    dbsksp_xshock_node_sptr xv = active_nodes[i];
    assert(xv->degree() < 4);
    switch (xv->degree())
    {
    case 2:
      sum += 2;
      break;
    case 3:
      sum += 3;
      break;
    }
  }

  // free parameters on the edges
  for (unsigned i =0; i < active_edges.size(); ++i)
  {
    dbsksp_xshock_edge_sptr xe = active_edges[i];
    if (xe->is_terminal_edge())
      continue;

    sum += 3;
  }

  return sum;
}




//------------------------------------------------------------------------------
//: Update the parameter map from the shock graph
void dbsksp_xgraph_model_using_L_alpha_phi_radius::
update_param_maps_from_xgraph()
{
  // First the nodes
  for (dbsksp_xshock_graph::vertex_iterator vit = this->xgraph()->vertices_begin();
    vit != this->xgraph()->vertices_end(); ++vit)
  {
    dbsksp_xshock_node_sptr xv = *vit;
    if (xv->degree() == 1)
      continue;

    dbsksp_xshock_edge_sptr parent_xe = this->parent_edge(xv);
    node_params& p = this->map_xv_to_params_[xv];
    p.degree = xv->degree();
    p.radius = xv->radius();
    p.phi_parent = xv->descriptor(parent_xe)->phi();
    
    // for degree-3 node, we need to compute dphi
    if (xv->degree() == 3)
    {
      dbsksp_xshock_edge_sptr major_child_edge = this->xgraph()->cyclic_adj_succ(parent_xe, xv);
      double phi1 = xv->descriptor(major_child_edge)->phi();
      double phi2 = vnl_math::pi - p.phi_parent - phi1;
      p.delta_phi = phi1 - phi2;
    }
  }

  // Next, the edges
  for (dbsksp_xshock_graph::edge_iterator eit = this->xgraph()->edges_begin();
    eit != this->xgraph()->edges_end(); ++eit)
  {
    dbsksp_xshock_edge_sptr xe = *eit;

    // there is no info at terminal edge
    if (xe->is_terminal_edge())
      continue;

    edge_params& p = this->map_xe_to_params_[xe];
    dbsksp_xshock_node_sptr xv_parent = xe->parent_node();
    dbsksp_xshock_node_sptr xv_child = xe->child_node();

    vgl_vector_2d<double > chord = xv_child->pt() - xv_parent->pt();
    

    // alpha angles are angle between the shock chord and the shock tangents
    vgl_vector_2d<double > shock_tangent_parent = xv_parent->descriptor(xe)->shock_tangent();
    vgl_vector_2d<double > shock_tangent_child = -xv_child->descriptor(xe)->shock_tangent();

    p.chord_len = chord.length();
    p.alpha_parent = signed_angle(chord, shock_tangent_parent);
    p.alpha_child = signed_angle(chord, shock_tangent_child);
  }
  return;
}






//------------------------------------------------------------------------------
//: Update the xgraph using the parameter stored at param maps
void dbsksp_xgraph_model_using_L_alpha_phi_radius::
update_xgraph_from_param_maps()
{
  // First compute the phi angles and radius at the nodes
  for (vcl_map<dbsksp_xshock_node_sptr, node_params >::iterator iter =
    this->map_xv_to_params_.begin(); iter != this->map_xv_to_params_.end(); ++iter)
  {
    dbsksp_xshock_node_sptr xv = iter->first;
    if (xv->degree() == 1)
      continue;

    node_params& p = iter->second;
    
    dbsksp_xshock_edge_sptr parent_xe = this->parent_edge(xv);
    xv->descriptor(parent_xe)->set_phi(p.phi_parent);
    assert(xv->degree() == p.degree);

    if (xv->degree() == 2)
    {
      dbsksp_xshock_edge_sptr xe1 = this->xgraph()->cyclic_adj_succ(parent_xe, xv);
      xv->descriptor(xe1)->set_phi( vnl_math::pi-p.phi_parent );
    }
    else if (xv->degree() == 3)
    {
      dbsksp_xshock_edge_sptr xe1 = this->xgraph()->cyclic_adj_succ(parent_xe, xv);
      dbsksp_xshock_edge_sptr xe2 = this->xgraph()->cyclic_adj_succ(xe1, xv);

      xv->descriptor(xe1)->set_phi( (vnl_math::pi-p.phi_parent + p.delta_phi) / 2);
      xv->descriptor(xe2)->set_phi( (vnl_math::pi-p.phi_parent - p.delta_phi) / 2);
    }

    xv->set_radius(p.radius);
  }

  // Next, set position of the nodes and tangents of the shock edges
  vcl_vector<dbsksp_xshock_node_sptr > stack;
  stack.reserve(this->xgraph()->number_of_vertices());
  stack.push_back(this->root_node_);

  // for each node in the stack, assume its position and the shock tangennt of its parent edge has been computed,
  // compute the shock tangents for the remaining edges
  while (!stack.empty())
  {
    dbsksp_xshock_node_sptr xv = stack.back();
    stack.pop_back();

    if (xv->degree() == 1)
      continue;

    // use shock tangent at parent edge as reference, compute tangents at other nodes
    dbsksp_xshock_edge_sptr parent_xe = this->parent_edge(xv);
    dbsksp_xshock_edge_sptr prev_xe = parent_xe;
    dbsksp_xshock_node_descriptor* prev_xdesc = xv->descriptor(prev_xe);
    dbsksp_xshock_edge_sptr cur_xe = this->xgraph()->cyclic_adj_succ(prev_xe, xv);
    while (cur_xe != parent_xe)
    {
      dbsksp_xshock_node_descriptor* cur_xdesc = xv->descriptor(cur_xe);
      cur_xdesc->set_shock_tangent(prev_xdesc->psi() + prev_xdesc->phi() + cur_xdesc->phi());

      // update
      prev_xe = cur_xe;
      prev_xdesc = cur_xdesc;
      cur_xe = this->xgraph()->cyclic_adj_succ(prev_xe, xv);
    }

    // now use the parameters at surrounding edges to compute position and 
    // shock tangents for the child nodes
    for (dbsksp_xshock_node::edge_iterator eit = xv->edges_begin(); eit !=
      xv->edges_end(); ++eit)
    {
      dbsksp_xshock_edge_sptr xe = *eit;

      // there is no info in terminal edges
      if (xe->is_terminal_edge())
        continue;

      // ignore (real) parent edge. For root node, all edges will be considered.
      if (xv == xe->child_node())
        continue;

      dbsksp_xshock_node_sptr child_xv = xe->child_node();
      
      
      // Compute position of the child node
      dbsksp_xshock_node_descriptor* xdesc = xv->descriptor(xe);
      edge_params& p = this->map_xe_to_params_[xe];

      // \todo make p is actually found 

      vgl_vector_2d<double > chord_orient = rotated(xdesc->shock_tangent(), -p.alpha_parent);
      child_xv->set_pt(xv->pt() + p.chord_len * chord_orient);

      // Compute shock tangent of xe at child node
      // note the minus (-) in front of the rotated(...) function. This is correct.
      child_xv->descriptor(xe)->set_shock_tangent(-rotated(chord_orient, p.alpha_child));

      // Add this child node to stack
      stack.push_back(child_xv);
    }  
  }
  this->xgraph()->update_all_degree_1_nodes();
}



//------------------------------------------------------------------------------
//: Parent edge of a node
dbsksp_xshock_edge_sptr dbsksp_xgraph_model_using_L_alpha_phi_radius::
parent_edge(const dbsksp_xshock_node_sptr xv)
{
  if (xv == this->root_node_)
    return this->pseudo_parent_edge_;
  else
    return xv->parent_edge();
}




//==============================================================================
// dbsksp_xgraph_model_using_x_y_r_psi_phi
//==============================================================================


//------------------------------------------------------------------------------
//: Constructor
dbsksp_xgraph_model_using_x_y_r_psi_phi::
dbsksp_xgraph_model_using_x_y_r_psi_phi(const dbsksp_xshock_graph_sptr& xgraph,
                                        unsigned root_vid, unsigned pseudo_parent_eid):
dbsksp_xgraph_model(xgraph)
{
  this->xgraph()->compute_vertex_depths(root_vid);
  this->root_node_ = this->xgraph()->node_from_id(root_vid);
  this->pseudo_parent_edge_ = this->xgraph()->edge_from_id(pseudo_parent_eid);

  assert(this->root_node_);
  assert(this->pseudo_parent_edge_);
  assert(this->pseudo_parent_edge_->is_vertex(this->root_node_));

  this->update_param_maps_from_xgraph();

  // compute dimension of the state vector
  // 5 for each degree-2 node
  // 6 for each degree-3 node
  {
    unsigned sum = 0;
    for (vcl_map<dbsksp_xshock_node_sptr, node_params >::iterator iter =
      this->map_xv_to_params_.begin(); iter != this->map_xv_to_params_.end(); ++iter)
    {
      dbsksp_xshock_node_sptr xv = iter->first;
      assert(xv->degree() < 4);
      if (xv->degree() == 2)
      {
        sum += 5;
      }
      else if (xv->degree() == 3)
      {
        sum += 6;
      }
    }
    this->dim_ = sum;
  }
  return;

}


//------------------------------------------------------------------------------
//: Current `state' of the shock graph
void dbsksp_xgraph_model_using_x_y_r_psi_phi::
get_xgraph_state(vnl_vector<double >& x)
{
  x.set_size(this->dim());
  vnl_vector<double >::iterator ix = x.begin();

  // update the parameter map
  this->update_param_maps_from_xgraph();

  // copy parameter values to x

  // paremters at the nodes
  for (vcl_map<dbsksp_xshock_node_sptr, node_params >::iterator iter = 
    this->map_xv_to_params_.begin(); iter != this->map_xv_to_params_.end(); ++iter)
  {
    node_params& p = iter->second;
    *(ix++) = p.x;
    *(ix++) = p.y;
    *(ix++) = p.radius;
    *(ix++) = p.phi_parent;
    *(ix++) = p.psi_parent;
    if (p.degree == 3)
    {
      *(ix++) = p.delta_phi;
    }
  }
  return;
}



//------------------------------------------------------------------------------
//: Update the shock graph with a `state' vector
void dbsksp_xgraph_model_using_x_y_r_psi_phi::
set_xgraph_state(const vnl_vector<double >& x)
{
  // update the param map from x
  vnl_vector<double >::const_iterator ix = x.begin();

  // a) paremters at the nodes
  for (vcl_map<dbsksp_xshock_node_sptr, node_params >::iterator iter = 
    this->map_xv_to_params_.begin(); iter != this->map_xv_to_params_.end(); ++iter)
  {
    node_params& p = iter->second;

    p.x = *(ix++);
    p.y = *(ix++);
    p.radius = *(ix++);
    p.phi_parent = *(ix++);
    p.psi_parent = *(ix++);
    if (p.degree == 3)
    {
      p.delta_phi = *(ix++);
    }
  }
  this->update_xgraph_from_param_maps();
  return;
}




