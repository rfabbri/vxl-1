// This is file shp/dbsksp/dbsksp_shock_model.cxx

//:
// \file

#include "dbsksp_shock_model.h"

#include <vnl/vnl_math.h>
#include <dbsksp/dbsksp_shock_graph.h>




// =====================================================================
//  dbsksp_shock_model
// =====================================================================



// ----------------------------------------------------------------------------
//: Constructor
dbsksp_shock_model::
dbsksp_shock_model(const dbsksp_shock_graph_sptr& shock_graph):
shock_graph_(shock_graph), num_params_(0)
{
};

// ----------------------------------------------------------------------------
//: Set the shock graph of the model
void dbsksp_shock_model::
set_shock_graph(const dbsksp_shock_graph_sptr& shock_graph)
{
  this->shock_graph_ = shock_graph;
  this->update_num_params();
}

// ----------------------------------------------------------------------------
//; Compute the number of free parameters of the model
// By default, the function compute the total degree of freedom of the shape
void dbsksp_shock_model::
update_num_params()
{
  // num_vertices |V|= (2N1 + N2 + N3)
  // degree of freedom = 2L + N1 + N2 + 2N3 + 4
  // but L = (N1+N2+N3-1)
  // and N1 = N3 + 2
  // so DOF = 3*|V| - 2 * N1
  int N1 = 0;
  int num_sinks_and_sources = 0;
  for (dbsksp_shock_graph::vertex_iterator vit = 
    this->shock_graph()->vertices_begin(); 
    vit != this->shock_graph()->vertices_end(); ++vit)
  {
    if ((*vit)->degree()==1)
    {
      N1 += 1;
      continue;
    }

    if ((*vit)->degree()==2)
    {
      if ((*vit)->descriptor_list().front()->shock_flow_dir == 0)
      {
        num_sinks_and_sources += 1;
      }
    }
    
  }
  int dof = 3*this->shock_graph()->number_of_vertices() - 2*N1 - num_sinks_and_sources;
  this->set_num_params(dof);
}


// =====================================================================
//  dbsksp_phi_m_L_shock_model
// =====================================================================


// ----------------------------------------------------------------------------
// CONSTRUCTORS / DESTRUCTORS
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//: constructor
dbsksp_phi_m_L_shock_model::
dbsksp_phi_m_L_shock_model( const dbsksp_shock_graph_sptr& shock_graph):
dbsksp_shock_model(shock_graph)
{
  this->update_num_params();
}


// ----------------------------------------------------------------------------
// UTILITY
// ----------------------------------------------------------------------------


//; Update (compute) the number of free parameters of the model
// This number is typically dependent on the shock graph, among others
// By default, the function compute the total degree of freedom of the shape
void dbsksp_phi_m_L_shock_model::
update_num_params()
{
  // counting explicitly
  int degree_one_nodes = 0;
  int dof = 0; // total degree of freedom of the graph

  // intrinsic DOF on the vertices
  for (dbsksp_shock_graph::vertex_iterator vit = this->shock_graph()->vertices_begin(); 
    vit != this->shock_graph()->vertices_end(); ++vit)
  {
    dbsksp_shock_node_sptr v = *vit;
    if (v->degree()==1)
    {
      degree_one_nodes += 1;
      continue;
    }
    dof += (v->degree()-1);
  }

  // intrinsic DOF on the edges
  dof += (this->shock_graph()->number_of_edges()-degree_one_nodes) * 2;

  // extrinsic DOF: origin(2), direction (1), radius at origin(1)
  dof += 4;
  
  this->set_num_params(dof);
}


// ----------------------------------------------------------------------------
//: construct `this' vector from its shock graph
void dbsksp_phi_m_L_shock_model::
compute_params_from_shock_graph(vnl_vector<double >& params)
{
  params.set_size(this->num_params());

  // order of variables
  // Nodes (ignoring A-infinity (degree 1) nodes )
  // Node 0     : phi00 -- node 0 has degree k
  // Node 0     : phi01
  // ...
  // Node 0     : phi0(k-2)

  // Node 1     : phi10
  // ...
  // Node N-1     : phi(N-1)0
  
  // Edges
  // Only non-degenerate edges are encoded
  // Edge 0     : m0
  // Edge 0     : L0
  // Edge 1     : m1
  // Edge 1     : L1
  // ...
  // Edge P-1     : m(P-1)
  // Edge P-1     : L(P-1)
  // where P = |E| - (#Ainf nodes)

  // Global
  // Ref_node_x   : ref_x
  // Ref_node_y   : ref_y
  // Ref_edge_psi : ref_psi
  // Ref_node_radius
  
  int var_count = 0;
  
  // Nodes
  for (dbsksp_shock_graph::vertex_iterator vit = this->shock_graph()->vertices_begin();
    vit != this->shock_graph()->vertices_end(); ++vit)
  {
    dbsksp_shock_node_sptr v = (*vit);

    // ignore A-infinity nodes
    if (v->degree()==1) continue;
    
    // get the phi's associated with (degree(vertex)-1) first edges
    // assign all but the last phi
    vcl_list<dbsksp_shock_node_descriptor_sptr > desc_list = v->descriptor_list();
      
    for (vcl_list<dbsksp_shock_node_descriptor_sptr >::iterator it =
      desc_list.begin(); (*it) != desc_list.back(); ++it)
    {
      params.put(var_count, (*it)->phi);
      ++var_count;
    }
  } // nodes

  // Edges
  for (dbsksp_shock_graph::edge_iterator eit = this->shock_graph()->edges_begin();
    eit != this->shock_graph()->edges_end(); ++eit)
  {
    dbsksp_shock_edge_sptr e = (*eit);
    if (e->is_terminal_edge()) continue;
    
    // param_m
    params.put(var_count, e->param_m());
    ++var_count;
    params.put(var_count, e->chord_length());
    ++var_count;
  }

  // Global variables
  // reference node
  vgl_point_2d<double > pt = this->shock_graph()->ref_origin();
  params.put(var_count, pt.x());
  ++var_count;
  params.put(var_count, pt.y());
  ++var_count;

  // direction of reference edge
  vgl_vector_2d<double > ref_dir = this->shock_graph()->ref_direction();
  double ref_psi = vcl_atan2(ref_dir.y(), ref_dir.x());
  params.put(var_count, ref_psi);
  ++var_count;

  // Global radius
  params.put(var_count, this->shock_graph()->ref_node_radius());
  ++var_count;
  assert(var_count == params.size());
}


// ----------------------------------------------------------------------------
//: Update shock graph with `this' set of free params
void dbsksp_phi_m_L_shock_model::
update_shock_graph(const vnl_vector<double >& params )
{
  // size check
  assert(params.size() == this->num_params());

  int var_count = 0;
  
  // Nodes
  for (dbsksp_shock_graph::vertex_iterator vit = 
    this->shock_graph()->vertices_begin();
    vit != this->shock_graph()->vertices_end(); ++vit)
  {
    dbsksp_shock_node_sptr v = (*vit);

    // skip degree-1 nodes
    if (v->degree() == 1) continue;

    // first collect all the node descriptors
    vcl_list<dbsksp_shock_node_descriptor_sptr > desc_list = 
      v->descriptor_list();

    assert(desc_list.size() > 1);
   
    // assign all but the last phi
    double phi_sum = 0; // will be used to compute the last phi
    
    for (vcl_list<dbsksp_shock_node_descriptor_sptr >::iterator it =
      desc_list.begin(); (*it) != desc_list.back(); ++it)
    {
      (*it)->phi = params.get(var_count);
      ++var_count;

      // update the sum
      phi_sum += (*it)->phi;
    }
    // compute the last phi using the fact that sum of the phi is pi
    desc_list.back()->phi = vnl_math::pi - phi_sum;
  } // nodes

  // Edges
  for (dbsksp_shock_graph::edge_iterator eit = 
    this->shock_graph()->edges_begin(); 
    eit != this->shock_graph()->edges_end(); ++eit)
  {
    dbsksp_shock_edge_sptr e = (*eit);
    if (e->is_terminal_edge()) continue;
    
    // param_m
    e->set_param_m(params.get(var_count));
    ++var_count;
    e->set_chord_length(params.get(var_count));
    ++var_count;
  }

  // Global variables
  // reference node
  vgl_point_2d<double > pt(params.get(var_count), params.get(var_count+1));
  var_count = var_count + 2;
  this->shock_graph()->set_ref_origin(pt);

  // direction of reference edge
  double ref_angle = params.get(var_count);
  ++var_count;

  this->shock_graph()->set_ref_direction(
    vgl_vector_2d<double >(vcl_cos(ref_angle), vcl_sin(ref_angle)));
  
  // origin radius
  this->shock_graph()->set_ref_node_radius(params.get(var_count));
  ++var_count;
  
  assert(var_count == this->num_params());
  this->shock_graph()->compute_all_dependent_params();
  return;
}



// ----------------------------------------------------------------------------
//: Extract the free parameters for the internal shock graph given its vertex and
// edge correspondences with another shock graph of the same topology
void dbsksp_phi_m_L_shock_model::
get_free_params(const dbsksp_shock_graph_sptr& that_graph, vnl_vector<double >& params,
                const vcl_map<dbsksp_shock_edge_sptr, dbsksp_shock_edge_sptr>& edge_map,
                const vcl_map<dbsksp_shock_node_sptr, dbsksp_shock_node_sptr>& node_map)
{
  params.set_size(this->num_params());

  // order of variables - the same as in compute_free_params...
  // The order is with respect to 
  // Nodes (ignoring A-infinity (degree 1) nodes )
  // Node 0     : phi00 -- node 0 has degree k
  // Node 0     : phi01
  // ...
  // Node 0     : phi0(k-2)

  // Node 1     : phi10
  // ...
  // Node N-1     : phi(N-1)0
  
  // Edges
  // Only non-degenerate edges are encoded
  // Edge 0     : m0
  // Edge 0     : L0
  // Edge 1     : m1
  // Edge 1     : L1
  // ...
  // Edge P-1     : m(P-1)
  // Edge P-1     : L(P-1)
  // where P = |E| - (#Ainf nodes)

  // Global
  // Ref_node_x   : ref_x
  // Ref_node_y   : ref_y
  // Ref_edge_psi : ref_psi
  // Ref_node_radius
  
  int var_count = 0;
  
  // Nodes
  for (dbsksp_shock_graph::vertex_iterator vit = this->shock_graph()->vertices_begin();
    vit != this->shock_graph()->vertices_end(); ++vit)
  {
    dbsksp_shock_node_sptr this_v = (*vit);

    // ignore A-infinity nodes
    if (this_v->degree()==1) continue;

    dbsksp_shock_node_sptr that_v = node_map.find(this_v)->second;
    
    // get the phi's associated with (degree(vertex)-1) first edges
    // assign all but the last phi
    vcl_list<dbsksp_shock_node_descriptor_sptr > desc_list = this_v->descriptor_list(); 
    for (vcl_list<dbsksp_shock_node_descriptor_sptr >::iterator itr =
      desc_list.begin(); (*itr) != desc_list.back(); ++itr)
    {
      dbsksp_shock_edge_sptr this_e = (*itr)->edge;

      // find equivalent edge and node in ``that'' graph
      dbsksp_shock_edge_sptr that_e = edge_map.find(this_e)->second;
      dbsksp_shock_node_descriptor_sptr that_descriptor = that_v->descriptor(that_e);
      params.put(var_count, that_descriptor->phi);
      ++var_count;
    }
  } // nodes

  // Edges
  for (dbsksp_shock_graph::edge_iterator eit = this->shock_graph()->edges_begin();
    eit != this->shock_graph()->edges_end(); ++eit)
  {
    dbsksp_shock_edge_sptr this_e = (*eit);
    if (this_e->is_terminal_edge()) continue;
    
    dbsksp_shock_edge_sptr that_e = edge_map.find(this_e)->second;

    // param_m
    params.put(var_count, that_e->param_m());
    ++var_count;
    params.put(var_count, that_e->chord_length());
    ++var_count;
  }

  // Global variables
  // reference node
  vgl_point_2d<double > pt = that_graph->ref_origin();
  params.put(var_count, pt.x());
  ++var_count;
  params.put(var_count, pt.y());
  ++var_count;

  // direction of reference edge
  vgl_vector_2d<double > ref_dir = that_graph->ref_direction();
  double ref_psi = vcl_atan2(ref_dir.y(), ref_dir.x());
  params.put(var_count, ref_psi);
  ++var_count;

  // Global radius
  params.put(var_count, that_graph->ref_node_radius());
  ++var_count;
  assert(var_count == params.size());
}



//: index of reference direction variable in the parameter vector
int dbsksp_phi_m_L_shock_model::
param_index_of_ref_dir() const
{
  return this->num_params()-2;
}









// ============================================================================
//          dbsksp_phi_m_L_model_unified_dimension
// ============================================================================


//: constructor
dbsksp_phi_m_L_model_unified_dim::
dbsksp_phi_m_L_model_unified_dim(const dbsksp_shock_graph_sptr& shock_graph):
dbsksp_phi_m_L_shock_model(shock_graph)
{
  int num_degree_1_nodes =
    (3*this->shock_graph()->number_of_vertices() -this->num_params())/2;

  // # phi_params = N1 + N2 + 2*N3 = |V| -2
  int num_phi_params = this->shock_graph()->number_of_vertices() -2;
  this->node_phi_list_.set_size(num_phi_params);

  // # m params
  int num_m_params = 2*(this->shock_graph()->number_of_edges() - num_degree_1_nodes);
  this->edge_m_len_list_.set_size(2*num_m_params);
  
}

//: construct `this' vector from its shock graph
void dbsksp_phi_m_L_model_unified_dim::
compute_params_from_shock_graph(vnl_vector<double >& params)
{
  vnl_vector<double > x;
  dbsksp_phi_m_L_shock_model::compute_params_from_shock_graph(x);
  this->node_phi_list_ = x.extract(this->node_phi_list_.size(), 0);
  this->edge_m_len_list_ = x.extract(this->edge_m_len_list_.size(), 
    this->edge_m_len_list_.size());

  params.set_size(x.size());

  // TO CONTINUE ....................


}

//: Update shock graph with `this' set of free params
void dbsksp_phi_m_L_model_unified_dim::
update_shock_graph(const vnl_vector<double >& params )
{


}




// ============================================================================
// dbsksp_extrinsic_info_model
// ============================================================================


// ============= UTILITY =============================================

//: construct `this' vector from its shock graph
void dbsksp_extrinsic_info_model::
compute_params_from_shock_graph(vnl_vector<double >& params)
{
  // The params are
  // x0
  // y0
  // psi0
  // r0
  params.set_size(4);
  params[0] = this->shock_graph()->ref_origin().x();
  params[1] = this->shock_graph()->ref_origin().y();
  params[2] = vcl_atan2(this->shock_graph()->ref_direction().y(), 
    this->shock_graph()->ref_direction().x());
  params[3] = this->shock_graph()->ref_node_radius();

  return;
  
}

//: Update shock graph with `this' set of free params
void dbsksp_extrinsic_info_model::
update_shock_graph(const vnl_vector<double >& params )
{
  // reverse process of compute_params_from_shock_graph
  assert(params.size() == 4);
  this->shock_graph()->set_ref_origin(vgl_point_2d<double >(params[0], params[1]));
  this->shock_graph()->set_ref_direction(
    vgl_vector_2d<double >(vcl_cos(params[2]), vcl_sin(params[2])));
  this->shock_graph()->set_ref_node_radius(params[3]);

  this->shock_graph()->compute_all_dependent_params();
}



// ========================================================================
// dbsksp_intrinsic_phi_m_L_shock_model
// ========================================================================

// ============= UTILITY =============================================



//; Update (compute) the number of free parameters of the model
void dbsksp_intrinsic_phi_m_L_shock_model::
update_num_params()
{
  dbsksp_phi_m_L_shock_model::update_num_params();
  this->set_num_params(this->num_params()-4);
}


//: construct `this' vector from its shock graph
void dbsksp_intrinsic_phi_m_L_shock_model::
compute_params_from_shock_graph(vnl_vector<double >& params)
{
  vnl_vector<double > full_params;
  dbsksp_phi_m_L_shock_model::compute_params_from_shock_graph(full_params);
  params = full_params.extract(full_params.size()-4, 0);
  return;
}

//: Update shock graph with `this' set of free params
void dbsksp_intrinsic_phi_m_L_shock_model::
update_shock_graph(const vnl_vector<double >& params )
{
  vnl_vector<double > full_params(dbsksp_phi_m_L_shock_model::num_params());
  //params = full_params.extract(full_params.size()-4, 0);
  
  assert(params.size() == full_params.size()-4);

  // intrinsic part
  full_params.update(params, 0);

  // extrinsic
  vnl_vector<double > extrinsic_params(4);
  
  // reference node
  vgl_point_2d<double > pt = this->shock_graph()->ref_origin();
  extrinsic_params[0] = pt.x();
  extrinsic_params[1] = pt.y();

  // direction of reference edge
  vgl_vector_2d<double > ref_dir = this->shock_graph()->ref_direction();
  extrinsic_params[2] = vcl_atan2(ref_dir.y(), ref_dir.x());
  
  // global radius
  extrinsic_params[3] = this->shock_graph()->ref_node_radius();

  // update
  full_params.update(extrinsic_params, params.size());

  dbsksp_phi_m_L_shock_model::update_shock_graph(full_params);
  
  return;

}





// ============================================================================
// dbsksp_selective_phi_m_L_shock_model
// ============================================================================


// ============= UTILITY =============================================




//; Update (compute) the number of free parameters of the model
void dbsksp_selective_phi_m_L_shock_model::
update_num_params()
{
  int num_params = 0;
  vcl_map<dbsksp_shock_edge_sptr, vnl_int_4 > selection = this->params_selection();
  for (vcl_map<dbsksp_shock_edge_sptr, vnl_int_4 >::iterator sit = selection.begin();
    sit != selection.end(); ++sit)
  {
    for (int i=0; i<4; ++i)
    {
      num_params += sit->second[i];
    }
  }

  this->set_num_params(num_params);
  return;
}

//: construct `this' vector from its shock graph
void dbsksp_selective_phi_m_L_shock_model::
compute_params_from_shock_graph(vnl_vector<double >& params)
{
  params.set_size(this->num_params());
  vnl_vector<double >::iterator params_it = params.begin();

  vcl_map<dbsksp_shock_edge_sptr, vnl_int_4 > selection = this->params_selection();
  for (vcl_map<dbsksp_shock_edge_sptr, vnl_int_4 >::iterator sit = selection.begin();
    sit != selection.end(); ++sit)
  {
    dbsksp_shock_edge_sptr e = sit->first;
    // m
    if (sit->second[0]==1)
    {
      *params_it = e->param_m();
      ++params_it;
    }

    // L
    if (sit->second[1]==1)
    {
      *params_it =e->chord_length();
      ++params_it;
    }

    if (sit->second[2] == 1)
    {
      *params_it = e->source()->descriptor(e)->phi;
      ++params_it;
    }

    if (sit->second[3]==1)
    {
      *params_it = e->target()->descriptor(e)->phi;
      ++params_it;
    }
  }

  assert(params_it == params.end());
  return;
}

//: Update shock graph with `this' set of free params
void dbsksp_selective_phi_m_L_shock_model::
update_shock_graph(const vnl_vector<double >& params )
{
  assert(params.size() == this->num_params());
  vnl_vector<double >::const_iterator params_it = params.begin();
  
  vcl_map<dbsksp_shock_edge_sptr, vnl_int_4 > selection = this->params_selection();
  for (vcl_map<dbsksp_shock_edge_sptr, vnl_int_4 >::iterator sit = selection.begin();
    sit != selection.end(); ++sit)
  {
    dbsksp_shock_edge_sptr e = sit->first;
    // m
    if (sit->second[0]==1)
    {
      e->set_param_m(*params_it);
      ++params_it;
    }

    // L
    if (sit->second[1]==1)
    {
      e->set_chord_length(*params_it);
      ++params_it;
    }

    // A hack for now
    // Need to think thoroughly how to update all the anlges around a node
    if (sit->second[2] == 1)
    {
      // the current method only works for degree 2 node
      assert(e->source()->degree()==2);
      e->source()->descriptor(e)->phi = (*params_it);
      dbsksp_shock_edge_sptr e1 = this->shock_graph()->cyclic_adj_succ(e, e->source());
      e->source()->descriptor(e1)->phi = vnl_math::pi - (*params_it);
      ++params_it;
    }

    if (sit->second[3]==1)
    {
      // the current method only works for degree 2 node
      assert(e->target()->degree()==2);
      e->target()->descriptor(e)->phi = (*params_it);
      dbsksp_shock_edge_sptr e1 = this->shock_graph()->cyclic_adj_succ(e, e->target());
      e->target()->descriptor(e1)->phi = vnl_math::pi - (*params_it);
      ++params_it;
    }
  }

  assert(params_it == params.end());
  return;


}









