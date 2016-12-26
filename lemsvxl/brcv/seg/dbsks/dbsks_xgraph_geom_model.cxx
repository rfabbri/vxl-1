// This is file shp/dbsks/dbsks_xgraph_geom_model.cxx

//:
// \file

#include "dbsks_xgraph_geom_model.h"

#include <dbsks/dbsks_xfrag_geom_model.h>
#include <dbsks/dbsks_xnode_geom.h>
#include <dbsks/dbsks_xnode_geom_model.h>
#include <dbsks/dbsks_xshock_utils.h>
#include <dbsks/dbsks_biarc_sampler.h>

#include <dbsksp/dbsksp_xshock_graph.h>
#include <dbsksp/dbsksp_xshock_node_descriptor.h>
#include <dbsksp/dbsksp_xshock_fragment.h>

#include <dbnl/dbnl_angle.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_vector.h>
#include <vcl_utility.h>
#include <vcl_algorithm.h>
#include <container/vnl_vector_to_vcl_vector.h>


//==============================================================================
// dbsks_xgraph_geom_model
//==============================================================================



//------------------------------------------------------------------------------
//: destructor
dbsks_xgraph_geom_model::
~dbsks_xgraph_geom_model()
{
  if (this->biarc_sampler_)
  {
    delete (this->biarc_sampler_);
    this->biarc_sampler_ = 0;
  }
  return;
};
  

//------------------------------------------------------------------------------
//: Build the model
bool dbsks_xgraph_geom_model::
build(const dbsksp_xshock_graph_sptr& xgraph,
          const vcl_map<unsigned, vcl_vector<dbsksp_xshock_fragment_sptr > >& map_edge2xfrag, 
          const vcl_map<unsigned, vcl_vector<dbsks_xnode_geom_sptr > >& map_node2xgeom,
          unsigned root_vid, 
          unsigned major_child_eid,
          double norm_graph_size)
{
  // root node and major child
  this->root_vid_ = root_vid;

  // major child
  this->major_child_eid_ = major_child_eid;

  // normalized graph size
  this->set_graph_size(norm_graph_size);

  // clean up
  this->map_edge2geom_.clear();
  this->map_node2geom_.clear();

  // compute geometric attributes
  this->compute_edge_geom_attr_from_data(xgraph, norm_graph_size, map_edge2xfrag);
  this->compute_vertex_geom_attr_from_data(xgraph, norm_graph_size, map_node2xgeom);

  // Build models from attribute data
  return (this->build_edge_geom_models() && this->build_vertex_geom_models());
}









//------------------------------------------------------------------------------
//: Compute edge geometric attributes from training data
void dbsks_xgraph_geom_model::
compute_edge_geom_attr_from_data(const dbsksp_xshock_graph_sptr& xgraph,
                                 double norm_graph_size,
                                 const vcl_map<unsigned, vcl_vector<dbsksp_xshock_fragment_sptr > >& map_eid2xfrag)
{
  // Loop thru the edges
  for (vcl_map<unsigned, vcl_vector<dbsksp_xshock_fragment_sptr > >::const_iterator mit = 
    map_eid2xfrag.begin(); mit != map_eid2xfrag.end(); ++mit)
  {
    unsigned eid = mit->first;
    const vcl_vector<dbsksp_xshock_fragment_sptr >& xfrags = mit->second;

    int num_xfrags = xfrags.size();
    vcl_vector<double > psi_start(num_xfrags, 0);
    vcl_vector<double > r_start(num_xfrags, 0);
    vcl_vector<double > phi_start(num_xfrags, 0);
    
    vcl_vector<double > alpha_start(num_xfrags, 0);
    vcl_vector<double > chord(num_xfrags, 0);
    vcl_vector<double > dpsi(num_xfrags, 0);
    
    vcl_vector<double > phi_end(num_xfrags, 0);
    vcl_vector<double > r_end(num_xfrags, 0);

    double sum_log2_r_start = 0;
    for (unsigned i =0; i < xfrags.size(); ++i)
    {
      dbsksp_xshock_fragment_sptr xfrag = xfrags[i];
      dbsksp_xshock_node_descriptor start = xfrag->start();
      dbsksp_xshock_node_descriptor end = xfrag->end();
      
      // start
      psi_start[i] = start.psi_;
      r_start[i] = start.radius_; 
      phi_start[i] = start.phi_;

      // end relative to start
      alpha_start[i] = signed_angle(end.pt() - start.pt(), start.shock_tangent()); // angle(chord, shock tangent)
      chord[i] = (end.pt()-start.pt()).length();
      dpsi[i] = signed_angle(start.shock_tangent(), end.shock_tangent());

      // end
      r_end[i] = end.radius_;
      phi_end[i] = end.phi_;
    }


    // alpha_start needs special treatment
    // when the edge is a terminal edge, chord direction is non-deterministic
    if (xgraph->edge_from_id(eid)->is_terminal_edge())
    {
      alpha_start.resize(alpha_start.size(), 0);
    }
   
    // construct a xfrag geomtric model
    dbsks_xfrag_geom_model_sptr xfrag_geom = this->map_edge2geom_[eid];
    if (!xfrag_geom)
    {
      xfrag_geom = this->map_edge2geom_[eid] = new dbsks_xfrag_geom_model();
    }
    
    // add training data to model
    xfrag_geom->set_graph_size_for_attr_data(norm_graph_size);
    xfrag_geom->set_attr_data("list_psi_start", psi_start);
    xfrag_geom->set_attr_data("list_r_start", r_start);
    xfrag_geom->set_attr_data("list_r_end", r_end);
    xfrag_geom->set_attr_data("list_phi_start", phi_start);
    xfrag_geom->set_attr_data("list_phi_end", phi_end);
    xfrag_geom->set_attr_data("list_alpha_start", alpha_start);
    xfrag_geom->set_attr_data("list_chord", chord);
    xfrag_geom->set_attr_data("list_dpsi", dpsi);
  }

  // Additional attributes
  for (vcl_map<unsigned, vcl_vector<dbsksp_xshock_fragment_sptr > >::const_iterator mit = 
    map_eid2xfrag.begin(); mit != map_eid2xfrag.end(); ++mit)
  {
    unsigned eid = mit->first;
    const vcl_vector<dbsksp_xshock_fragment_sptr >& list_xfrag = mit->second;    

    unsigned num_xfrags = list_xfrag.size();
    vcl_vector<double > list_chord_length(num_xfrags, 0);
    vcl_vector<double > list_median_length(num_xfrags, 0);
    
    vcl_vector<double > list_start_radius(num_xfrags, 0);
    vcl_vector<double > list_start_width(num_xfrags, 0);
    
    vcl_vector<double > list_end_radius(num_xfrags, 0);
    vcl_vector<double > list_end_width(num_xfrags, 0);
    
    vcl_vector<double > list_average_width(num_xfrags, 0);

    vcl_vector<double > list_start_phi(num_xfrags, 0);
    vcl_vector<double > list_end_phi(num_xfrags, 0);
    
    vcl_vector<double > list_predicted_phi(num_xfrags, 0);

    vcl_vector<double > list_start_phi_diff(num_xfrags, 0);
    vcl_vector<double > list_end_phi_diff(num_xfrags, 0);

    vcl_vector<double > list_left_curvature_diff(num_xfrags, 0);
    vcl_vector<double > list_right_curvature_diff(num_xfrags, 0);

    vcl_vector<double > list_left_bnd_length(num_xfrags, 0);
    vcl_vector<double > list_right_bnd_length(num_xfrags, 0);

    vcl_vector<double > list_left_chord_length(num_xfrags, 0);
    vcl_vector<double > list_right_chord_length(num_xfrags, 0);

    for (unsigned i =0; i < num_xfrags; ++i)
    {
      dbsksp_xshock_fragment_sptr xfrag = list_xfrag[i];

      //> chord length
      list_chord_length[i] = (xfrag->end().pt() - xfrag->start().pt()).length();

      //> median length
      // median line is defined as the biarc connecting the starting and ending points
      vgl_point_2d<double > p0 = midpoint(xfrag->start().bnd_pt_left(), xfrag->start().bnd_pt_right());
      vgl_vector_2d<double > t0 = xfrag->start().shock_tangent();

      vgl_point_2d<double > p2 = midpoint(xfrag->end().bnd_pt_left(), xfrag->end().bnd_pt_right());
      vgl_vector_2d<double > t2 = xfrag->end().shock_tangent();

      // interpolate using a biarc
      dbgl_biarc median_curve(p0, t0, p2, t2);
      double median_length = median_curve.len();
      list_median_length[i] = median_length;

      //> start radius
      list_start_radius[i] = xfrag->start().radius_;

      //> end radius
      list_end_radius[i] = xfrag->end().radius_;

      //> start width
      double start_width = xfrag->start().radius()*vcl_sin(xfrag->start().phi());
      list_start_width[i] = start_width;

      //> end width
      double end_width = xfrag->end().radius()*vcl_sin(xfrag->end().phi());
      list_end_width[i] = end_width;

      //> average width
      list_average_width[i] = (start_width + end_width) / 2;

      //> start phi
      double start_phi = xfrag->start().phi_;
      list_start_phi[i] = start_phi;

      //> end phi
      double end_phi = xfrag->end().phi_;
      list_end_phi[i] = end_phi;

      //> predicted phi - using only 0th order info
      double temp = vcl_atan2((end_width - start_width), median_length); // <0: decreasing width
      double predicted_phi = temp + vnl_math::pi_over_2;
      list_predicted_phi[i] = predicted_phi;

      //> start_phi_diff - difference between predicated and real phi
      list_start_phi_diff[i] = start_phi - predicted_phi;

      //> end_phi_diff - difference between predicated and real phi
      list_end_phi_diff[i] = end_phi - predicted_phi;

      // curvature difference within curvature biarcs
      dbgl_biarc left_bnd = xfrag->bnd_left_as_biarc();
      dbgl_biarc right_bnd = xfrag->bnd_right_as_biarc();

      //> left_curvature_diff
      // curvature difference between arcs of left boundary 
      list_left_curvature_diff[i] = left_bnd.k1() - left_bnd.k2();

      //> right_curvature_diff
      list_right_curvature_diff[i] = right_bnd.k1() - right_bnd.k2();

      //> left_bnd_length
      list_left_bnd_length[i] = left_bnd.len();

      //> right_bnd_length
      list_right_bnd_length[i] = right_bnd.len();

      //> left_chord_length
      list_left_chord_length[i] = (left_bnd.end()-left_bnd.start()).length();

      //> right_chord_length
      list_right_chord_length[i] = (right_bnd.end()-right_bnd.start()).length();
    }

    // Add these properties to the attribute list
    dbsks_xfrag_geom_model_sptr xfrag_model = this->map_edge2geom_[eid];
    assert(xfrag_model);
  
    xfrag_model->set_attr_data("list_chord_length", list_chord_length);
    xfrag_model->set_attr_data("list_median_length", list_median_length);
    xfrag_model->set_attr_data("list_start_radius", list_start_radius);
    xfrag_model->set_attr_data("list_start_width", list_start_width);
    xfrag_model->set_attr_data("list_end_radius", list_end_radius);
    xfrag_model->set_attr_data("list_end_width", list_end_width);
    xfrag_model->set_attr_data("list_average_width", list_average_width);
    xfrag_model->set_attr_data("list_start_phi", list_start_phi);
    xfrag_model->set_attr_data("list_end_phi", list_end_phi);
    xfrag_model->set_attr_data("list_predicted_phi", list_predicted_phi);
    xfrag_model->set_attr_data("list_start_phi_diff", list_start_phi_diff);
    xfrag_model->set_attr_data("list_end_phi_diff", list_end_phi_diff);

    xfrag_model->set_attr_data("list_left_curvature_diff", list_left_curvature_diff);
    xfrag_model->set_attr_data("list_right_curvature_diff", list_right_curvature_diff);

    xfrag_model->set_attr_data("list_left_bnd_length", list_left_bnd_length);
    xfrag_model->set_attr_data("list_right_bnd_length", list_right_bnd_length);

    xfrag_model->set_attr_data("list_left_chord_length", list_left_chord_length);
    xfrag_model->set_attr_data("list_right_chord_length", list_right_chord_length);
  }
  return;
}



  






//------------------------------------------------------------------------------
//: Compute vertex geometric attributes from training data
void dbsks_xgraph_geom_model::
compute_vertex_geom_attr_from_data(const dbsksp_xshock_graph_sptr& xgraph,
                                   double norm_graph_size,
                                   const vcl_map<unsigned, vcl_vector<dbsks_xnode_geom_sptr > >& map_vid2xnode)
{
  for (vcl_map<unsigned, vcl_vector<dbsks_xnode_geom_sptr > >::const_iterator mit = 
    map_vid2xnode.begin(); mit != map_vid2xnode.end(); ++mit)
  {
    unsigned vid = mit->first;
    const vcl_vector<dbsks_xnode_geom_sptr >& xnode_geoms = mit->second;

    int num_xnodes = xnode_geoms.size();
    vcl_vector<double > psi(num_xnodes, 0);
    vcl_vector<double > phi(num_xnodes, 0);
    vcl_vector<double > radius(num_xnodes, 0);
    vcl_vector<double > phi_diff(num_xnodes, 0);

    for (unsigned i =0; i < xnode_geoms.size(); ++i)
    {
      dbsks_xnode_geom_sptr xnode_geom = xnode_geoms[i];
      psi[i] = xnode_geom->psi_;
      phi[i] = xnode_geom->phi_;
      radius[i] = xnode_geom->radius_;
      phi_diff[i] = xnode_geom->phi_diff_;
    }

    
    // construct a xfrag geomtric model
    dbsks_xnode_geom_model_sptr xnode_geom = this->map_node2geom_[vid];
    if (!xnode_geom)
    {
      xnode_geom = this->map_node2geom_[vid] = new dbsks_xnode_geom_model();
    }

    // add training data to model
    xnode_geom->set_graph_size_for_attr_data(norm_graph_size);
    xnode_geom->set_attr_data("list_psi", psi);
    xnode_geom->set_attr_data("list_phi", phi);
    xnode_geom->set_attr_data("list_radius", radius);
    xnode_geom->set_attr_data("list_phi_diff", phi_diff);
  }


  // Additional attributes
  for (vcl_map<unsigned, vcl_vector<dbsks_xnode_geom_sptr > >::const_iterator mit = 
    map_vid2xnode.begin(); mit != map_vid2xnode.end(); ++mit)
  {
    // vertex id
    unsigned vid = mit->first;

    // list exemplar xnodes
    const vcl_vector<dbsks_xnode_geom_sptr >& list_xnode = mit->second;

    unsigned num_xnodes = list_xnode.size();
    vcl_vector<double > list_parent_width(num_xnodes, 0);
    vcl_vector<double > list_child1_width(num_xnodes, 0);
    vcl_vector<double > list_child2_width(num_xnodes, 0);

    for (unsigned i =0; i < list_xnode.size(); ++i)
    {
      dbsks_xnode_geom_sptr xnode = list_xnode[i];

      double phi1 = (xnode->phi_ + xnode->phi_diff_)/2;
      double phi2 = (xnode->phi_ - xnode->phi_diff_)/2;

      list_parent_width[i] = xnode->radius_*vcl_sin(xnode->phi_);
      list_child1_width[i] = xnode->radius_ * vcl_sin(phi1);
      list_child2_width[i] = xnode->radius_ * vcl_sin(phi2);
    }

    // update these data to the model
    dbsks_xnode_geom_model_sptr xnode_model = this->map_node2geom_[vid];
    xnode_model->set_attr_data("list_parent_width", list_parent_width);
    xnode_model->set_attr_data("list_child1_width", list_child1_width);
    xnode_model->set_attr_data("list_child2_width", list_child2_width);
  }

  return;
}





//------------------------------------------------------------------------------
//: build geom models for edges from geometric attributes
bool dbsks_xgraph_geom_model::
build_edge_geom_models()
{
  bool success = true;
  for (vcl_map<unsigned, dbsks_xfrag_geom_model_sptr >::iterator iter = 
    this->map_edge2geom_.begin(); iter != this->map_edge2geom_.end(); ++iter)
  {
    success = success && iter->second->build_from_attr_data();
  }
  return success;
}

//------------------------------------------------------------------------------
//: build geom models for vertices from geometric attributes
bool dbsks_xgraph_geom_model::
build_vertex_geom_models()
{
  bool success = true;
  for (vcl_map<unsigned, dbsks_xnode_geom_model_sptr >::iterator iter = 
    this->map_node2geom_.begin(); iter != this->map_node2geom_.end(); ++iter)
  {
    success = success && iter->second->build_from_attr_data();
  }
  return success;
}











//------------------------------------------------------------------------------
//: Compute distributions of geometric attributes that can be used to constrain
// a shock graph
void dbsks_xgraph_geom_model::
compute_attribute_constraints()
{
  //> Construct a biarc sampler
  
  // Set parameters of biarc sampler
  dbsks_biarc_sampler_params bsp;
  bsp.step_dx = 1;
  bsp.num_dx = 21;
  bsp.min_dx = -10;

  // max dy = 32
  bsp.step_dy = 1; 
  bsp.num_dy = 21;
  bsp.min_dy = -10;

  bsp.step_alpha0 = vnl_math::pi/17;
  bsp.num_alpha0 = 13;
  bsp.min_alpha0 = -bsp.step_alpha0*(bsp.num_alpha0-1)/2;

  bsp.step_alpha2 = vnl_math::pi/17;
  bsp.num_alpha2 = 13;
  bsp.min_alpha2 = -bsp.step_alpha2*(bsp.num_alpha2-1)/2;

  // Reset the biarc sampler
  if (this->biarc_sampler_)
  {
    delete this->biarc_sampler_;
  }
  this->biarc_sampler_ = new dbsks_biarc_sampler;
  this->biarc_sampler_->set_grid(bsp);

  // we don't need to compute samples from this sampler
  this->biarc_sampler_->set_sampling_params(36, 1.5, 51);
  //biarc_sampler.compute_cache_sample_points();

  // But we need to computer other attributes such as nkdiff
  this->biarc_sampler_->compute_cache_nkdiff();

  for (vcl_map<unsigned, dbsks_xfrag_geom_model_sptr >::iterator iter = 
    this->map_edge2geom_.begin(); iter != this->map_edge2geom_.end(); ++iter)
  {
    unsigned eid = iter->first;
    dbsks_xfrag_geom_model_sptr xfrag_geom = iter->second;
    xfrag_geom->compute_constraints_from_attr_data();

    // every xfrag model share the same biarc sampler with the xgraph model
    xfrag_geom->set_biarc_sampler(this->biarc_sampler_);
  }
  return;
}











//---------------------------------------------------------------------------
//: Return true if this model is compatible with a given shock graph topology
bool dbsks_xgraph_geom_model::
is_compatible(const dbsksp_xshock_graph_sptr& xgraph)
{
  if (!xgraph)
    return false;
  
  // Check if there is a geometric model for all EDGES in the xgraph
  bool has_all_edges = true;
  for (dbsksp_xshock_graph::edge_iterator eit = xgraph->edges_begin(); eit != 
    xgraph->edges_end(); ++eit)
  {
    vcl_map<unsigned, dbsks_xfrag_geom_model_sptr >::iterator iter = 
      this->map_edge2geom_.find((*eit)->id());
    if (iter == this->map_edge2geom_.end())
    {
      has_all_edges = false;
      break;
    }
  }

  // Check if there is a geometric model for all NODES with degree > 1 in the xgraph
  bool has_all_nodes = true;
  for (dbsksp_xshock_graph::vertex_iterator vit = xgraph->vertices_begin(); 
    vit != xgraph->vertices_end(); ++vit)
  {
    if ((*vit)->degree() <= 1)
      continue;

    vcl_map<unsigned, dbsks_xnode_geom_model_sptr >::iterator iter = 
      this->map_node2geom_.find((*vit)->id());

    if (iter == this->map_node2geom_.end())
    {
      has_all_nodes = false;
      break;
    }
  }

  return (has_all_edges && has_all_nodes);
}















