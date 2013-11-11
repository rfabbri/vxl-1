// This is file shp/dbsksp/dbsksp_morph_shock_graph_same_topology.cxx

//:
// \file

#include "dbsksp_morph_shock_graph_same_topology.h"
#include <dbsksp/dbsksp_shock_graph.h>
#include <dbsksp/dbsksp_shock_edge.h>
#include <dbsksp/dbsksp_shock_node.h>
#include <dbsksp/dbsksp_shock_model.h>
#include <vcl_utility.h>


//: Establish correspondences between two shock graph using the ref_node's and
// ref_edge's as the starting point and perform euler tours on both graphs.
// Correspondence are in terms of <edge_id edge_id>
bool dbsksp_morph_shock_graph_same_topology::
compute_correspondence()
{
  // simple checks
  if (this->source_graph()->number_of_edges() != this->target_graph()->number_of_edges())
  {
    return false;
  }

  // clean up existing correspondence map
  this->edge_map_.clear();
  this->node_map_.clear();

  // Start with ref_node and ref_edge, do euler tours around both graphs
  // We need two iterators for each graph: node_itr and edge_itr

  // for the source graph
  dbsksp_shock_node_sptr source_node = this->source_graph()->ref_node();
  dbsksp_shock_edge_sptr source_edge = this->source_graph()->ref_edge();
  assert(source_edge->is_vertex(source_node));

  // for the target graph
  dbsksp_shock_node_sptr target_node = this->target_graph()->ref_node();
  dbsksp_shock_edge_sptr target_edge = this->target_graph()->ref_edge();
  assert(target_edge->is_vertex(target_node));

  // iterate thru all the edges via Euler tour
  do
  {
    // locate the current edge in the map
    vcl_map<dbsksp_shock_edge_sptr, dbsksp_shock_edge_sptr>::iterator itr = 
      this->edge_map_.find(source_edge);
    // if this edge has not been visited, add the pair
    if (itr == this->edge_map_.end())
    {
      this->edge_map_.insert(vcl_make_pair(source_edge, target_edge));
      this->node_map_.insert(vcl_make_pair(source_node, target_node));
    }
    // if it's been visited, compare the correspondence with the one found previously
    else
    {
      // if the two correspondences don't match, the two shock graph have different
      // topology
      if (itr->second != target_edge)
      {
        return false;
      }
    }

    // move on to the next node and edge on the Euler tour
    source_node = source_edge->opposite(source_node);
    source_edge = this->source_graph()->cyclic_adj_succ(source_edge, source_node);

    target_node = target_edge->opposite(target_node);
    target_edge = this->target_graph()->cyclic_adj_succ(target_edge, target_node);
  }
  while (source_node != this->source_graph()->ref_node() || 
    source_edge != this->source_graph()->ref_edge());

  return true;
}



// ----------------------------------------------------------------------------
//: Compute all the parameters necessary to produce intermediate shapes
bool dbsksp_morph_shock_graph_same_topology::
morph()
{
  if (!this->compute_correspondence())
  {
    vcl_cerr << "ERROR: could not establish correspondence between two shapes\n";
    return false;
  }

  // To get intermediate shock graphs, we will linearly interpolate the parameters

  // Create a copy of the source shock graph
  this->morphing_graph_ = new dbsksp_shock_graph(*this->source_graph());

  // get free parameter vector from the source and target shock graphs
  dbsksp_phi_m_L_shock_model model(this->source_graph_);
  
  // source
  model.compute_params_from_shock_graph(this->source_params_);
  
  // target
  model.get_free_params(this->target_graph(), this->target_params_, 
    this->edge_map_, this->node_map_);

  // special care for the reference angle to resolve the [0, 2pi] problem
  // making the rotating angle is minimal
  double source_ref_psi = this->source_params_[model.param_index_of_ref_dir()];
  double target_ref_psi = this->target_params_[model.param_index_of_ref_dir()];

  vgl_vector_2d<double > source_ref_dir(vcl_cos(source_ref_psi), vcl_sin(source_ref_psi));
  vgl_vector_2d<double > target_ref_dir(vcl_cos(target_ref_psi), vcl_sin(target_ref_psi));

  // reset the angles
  this->source_params_[model.param_index_of_ref_dir()] = 
    vcl_atan2(source_ref_dir.y(), source_ref_dir.x());

  this->target_params_[model.param_index_of_ref_dir()] = 
    this->source_params_[model.param_index_of_ref_dir()] + 
    signed_angle(source_ref_dir, target_ref_dir);


  return true;
}


// ----------------------------------------------------------------------------
//: Get an intermediate graph given a time `t', 0<= t <= 1
dbsksp_shock_graph_sptr dbsksp_morph_shock_graph_same_topology::
get_intermediate_graph(double t)
{
  // 
  dbsksp_phi_m_L_shock_model model(this->morphing_graph_);
  vnl_vector<double > params_t = (1-t) * this->source_params_ + t * this->target_params_;
  model.update_shock_graph(params_t);
  
  // new shock graph to return
  dbsksp_shock_graph_sptr graph = new dbsksp_shock_graph(*model.shock_graph());
  graph->compute_all_dependent_params();
  return graph;
}





// ----------------------------------------------------------------------------
//: print info of the morpher to an output stream
vcl_ostream& dbsksp_morph_shock_graph_same_topology::
print(vcl_ostream & os) const
{
  // print the correspondence maps
  os << "Edge correspondence map:\n";
  for (vcl_map<dbsksp_shock_edge_sptr, dbsksp_shock_edge_sptr>::const_iterator itr =
    this->edge_map_.begin(); itr != this->edge_map_.end(); ++itr)
  {
    os << "( " << itr->first->id() << " , " << itr->second->id() << " ) ";
  }
  os << "\n";


  os << "Node correspondence map:\n";
  for (vcl_map<dbsksp_shock_node_sptr, dbsksp_shock_node_sptr>::const_iterator itr =
    this->node_map_.begin(); itr != this->node_map_.end(); ++itr)
  {
    os << "( " << itr->first->id() << " , " << itr->second->id() << " ) ";
  }
  os << "\n";

  return os;
}









