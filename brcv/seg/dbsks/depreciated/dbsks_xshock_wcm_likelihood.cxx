// This is file seg/dbsks/dbsks_xshock_wcm_likelihood.cxx

//:
// \file


#include "dbsks_xshock_wcm_likelihood.h"

#include <dbsksp/dbsksp_xshock_graph.h>
#include <dbsks/dbsks_biarc_sampler.h>
#include <dbsks/dbsks_wcm.h>
#include <dbsks/dbsks_xgraph_ccm_model.h>
#include <dbsksp/algo/dbsksp_trace_boundary.h>
#include <vcl_map.h>
#include <vnl/vnl_math.h>
#include <dbnl/dbnl_angle.h>


// ============================================================================
// dbsks_xshock_wcm_likelihood
// ============================================================================

// -----------------------------------------------------------------------------
//: Set
void dbsks_xshock_wcm_likelihood::
set(const dbsks_xgraph_ccm_model_sptr& xgraph_ccm, dbsks_wcm* wcm, 
    dbsks_biarc_sampler* biarc_sampler)
{
  this->xgraph_ccm_ = xgraph_ccm;
  this->wcm_ = wcm;
  this->biarc_sampler_ = biarc_sampler;
  this->active_edge_id_ = 0;
  this->active_xfrag_ccm_ = 0;
  return;
}



// -----------------------------------------------------------------------------
//: Likelihood of a xshock fragment
double dbsks_xshock_wcm_likelihood::
loglike(unsigned edge_id, const dbsksp_xshock_fragment& xfrag) const
{
  dbsks_xfrag_ccm_model_sptr xfrag_ccm = 0;
  if (edge_id == this->active_edge_id_)
  {
    xfrag_ccm = this->active_xfrag_ccm_;
  }
  else
  {
    vcl_map<unsigned, dbsks_xfrag_ccm_model_sptr >::iterator iter = 
      this->xgraph_ccm_->map_edge2ccm().find(edge_id);
    assert(iter != this->xgraph_ccm_->map_edge2ccm().end());
   
    // update cache edge id and xfrag ccm model
    xfrag_ccm = iter->second;
    this->active_edge_id_ = edge_id;
    this->active_xfrag_ccm_ = xfrag_ccm;
  }

  // compute cost
  float cost_left, cost_right;

  static vcl_vector<int > x_vec, y_vec, angle_vec;
  dbsksp_xshock_node_descriptor start = xfrag.start();
  dbsksp_xshock_node_descriptor end = xfrag.end();

  // place holders for sample points
  double angle_step;

  // left boundary
  this->biarc_sampler_->compute_samples(start.bnd_pt_left(), start.bnd_tangent_left(),
    end.bnd_pt_left(), end.bnd_tangent_left(), x_vec, y_vec, angle_vec, angle_step);
  
  if (x_vec.empty())
  {
    cost_left = 1.0f;
  }
  else
  {
    cost_left = this->wcm_->f(x_vec, y_vec, angle_vec);
  }
  
  // right boundary
  this->biarc_sampler_->compute_samples(start.bnd_pt_right(), start.bnd_tangent_right(), 
    end.bnd_pt_right(), end.bnd_tangent_right(),
    x_vec, y_vec, angle_vec, angle_step);
  if (x_vec.empty())
  {
    cost_right = 1.0f;
  }
  else
  {
    cost_right = this->wcm_->f(x_vec, y_vec, angle_vec);
  }

  
  // overall cost of the fragment
  float sum_like = float(xfrag_ccm->log_likelihood(0, cost_left) + 
    xfrag_ccm->log_likelihood(1, cost_right));
  
  return sum_like;
}












// -----------------------------------------------------------------------------
//: Likelihood of an xshock fragment given the whole shape's boundary
double dbsks_xshock_wcm_likelihood::
f_whole_contour(unsigned edge_id, const dbsksp_xshock_fragment& xfrag, const vgl_polygon<double >& boundary)
{
  dbsks_xfrag_ccm_model_sptr xfrag_ccm = 0;
  if (edge_id == this->active_edge_id_)
  {
    xfrag_ccm = this->active_xfrag_ccm_;
  }
  else
  {
    vcl_map<unsigned, dbsks_xfrag_ccm_model_sptr >::iterator iter = 
      this->xgraph_ccm_->map_edge2ccm().find(edge_id);
    assert(iter != this->xgraph_ccm_->map_edge2ccm().end());
   
    // update cache edge id and xfrag ccm model
    xfrag_ccm = iter->second;
    this->active_edge_id_ = edge_id;
    this->active_xfrag_ccm_ = xfrag_ccm;
  }

  // compute cost
  float cost_left, cost_right;

  static vcl_vector<int > x_vec, y_vec, angle_vec;
  dbsksp_xshock_node_descriptor start = xfrag.start();
  dbsksp_xshock_node_descriptor end = xfrag.end();

  // place holders for sample points
  double angle_step;

  // left boundary
  this->biarc_sampler_->compute_samples(start.bnd_pt_left(), start.bnd_tangent_left(),
    end.bnd_pt_left(), end.bnd_tangent_left(), x_vec, y_vec, angle_vec, angle_step);
  
  if (x_vec.empty())
  {
    cost_left = 1.0f;
  }
  else
  {
    dbgl_biarc frag_bnd(start.bnd_pt_left(), start.bnd_tangent_left(), 
      end.bnd_pt_left(), end.bnd_tangent_left());
    cost_left = this->wcm_->f_whole_contour(x_vec, y_vec, angle_vec, frag_bnd.len(), boundary);
  }
  
  // right boundary
  this->biarc_sampler_->compute_samples(start.bnd_pt_right(), start.bnd_tangent_right(), 
    end.bnd_pt_right(), end.bnd_tangent_right(),
    x_vec, y_vec, angle_vec, angle_step);
  if (x_vec.empty())
  {
    cost_right = 1.0f;
  }
  else
  {
    dbgl_biarc frag_bnd(start.bnd_pt_right(), start.bnd_tangent_right(), 
      end.bnd_pt_right(), end.bnd_tangent_right());
    cost_right = this->wcm_->f_whole_contour(x_vec, y_vec, angle_vec, frag_bnd.len(), boundary);
  }

  
  // overall cost of the fragment
  float final_cost = float(xfrag_ccm->log_likelihood(0, cost_left) +
    xfrag_ccm->log_likelihood(1, cost_right));
  
  return final_cost;
  
}





// -----------------------------------------------------------------------------
//: Likelihood of a xshock graph using whole contour matching
double dbsks_xshock_wcm_likelihood::
f_whole_contour(const dbsksp_xshock_graph_sptr& xgraph, 
                const vcl_vector<unsigned >& ignored_edges, bool verbose)
{
  // Compute boundary of the xgraph as a polygon
  double approx_ds = 2;
  vcl_vector<vgl_point_2d<double > > bnd_pts;
  dbsksp_trace_xgraph_boundary_as_polygon(xgraph, bnd_pts, approx_ds);
  vgl_polygon<double > boundary(bnd_pts);

  // compute cost using the computed polygon as shape boundary
  double sum_cost = 0;
  for (dbsksp_xshock_graph::edge_iterator eit = xgraph->edges_begin(); eit !=
    xgraph->edges_end(); ++eit)
  {
    dbsksp_xshock_edge_sptr xe = *eit;
    unsigned eid = xe->id();

    // Check whether this edge is "ignored"
    bool is_ignored = false;
    for (unsigned i =0; i < ignored_edges.size(); ++i)
    {
      if (ignored_edges[i] == eid)
      {
        is_ignored = true;
        break;
      }
    }

    // ignore the cost of this fragment if this edge is ignored.
    if (is_ignored)
      continue;

    // Compute the image cost of this fragment. The order of start and end
    // xnode descriptor follows the flow of the rooted tree.
    dbsksp_xshock_node_sptr xv_p = xe->parent_node();
    dbsksp_xshock_node_sptr xv_c = xe->child_node();
    assert(xv_p->depth() < xv_c->depth());

    // form an xshock fragment for the edge
    dbsksp_xshock_node_descriptor xd0 = *(xv_p->descriptor(xe));
    dbsksp_xshock_node_descriptor xd1 = xv_c->descriptor(xe)->opposite_xnode();
    dbsksp_xshock_fragment xfrag(xd0, xd1);

    // Compute fragment cost
    double fragment_cost = this->f_whole_contour(eid, xfrag, boundary);
    sum_cost += fragment_cost;

    if (verbose)
    {
      vcl_cout << "   Edge_id=" << (*eit)->id() << " image_cost= " << fragment_cost << "\n";
      vcl_cout << "     parent_vid=" << xv_p->id() << "\n";
      vcl_cout << "     child_vid=" << xv_c->id() << "\n";
    }
  }
  if (verbose)
  {
    vcl_cout << "   Total cost=" << sum_cost << vcl_endl;
  }
  return sum_cost;
}


