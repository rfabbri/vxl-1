// This is file seg/dbsks/dbsks_compute_ocm_cost.cxx

//:
// \file



#include "dbsks_compute_ocm_cost.h"

#include <dbsksp/dbsksp_xshock_graph.h>
#include <dbsksp/dbsksp_xshock_edge.h>

#include <vnl/vnl_math.h>
#include <vul/vul_sprintf.h>
// ============================================================================
// dbsks_compute_ocm_cost
// ============================================================================



// -----------------------------------------------------------------------------
//: Compute ocm cost of a biarc
bool dbsks_compute_ocm_cost(const dbsks_ocm_image_cost& ocm, 
                            const dbgl_biarc& biarc, 
                            float& cost, 
                            double ds)
{
  // check validity of the curve
  if (!biarc.is_consistent())
  {
    cost = vnl_numeric_traits<float >::maxval;
    return false;
  }

  // Compute image cost by matching each of these arcs with one in the pre-computed grid
  double len = biarc.len();
  int half_num_pts = vnl_math_floor(len/(2*ds));
  vcl_vector<vgl_point_2d<double > > pts(2*half_num_pts+1);
  vcl_vector<vgl_vector_2d<double > > ts(2*half_num_pts+1);
  for (int k =-half_num_pts; k <= half_num_pts; ++k)
  {
    pts[k+half_num_pts] = biarc.point_at(len/2 + k*ds);
    ts [k+half_num_pts] = biarc.tangent_at(len/2 + k*ds);
  }
  cost = ocm.f(pts, ts);
  return true;
}




// -----------------------------------------------------------------------------
//: Compute cost of xfrag using a Chamfer Matching cost calculator
bool dbsks_compute_ocm_cost(const dbsks_ocm_image_cost& ocm,
                            const dbsksp_xshock_fragment& xfrag,
                            float& cost,
                            double ds)
{
  // Compute the four circular arcs of the fragment
  dbgl_biarc bnd_biarc[2];
  bnd_biarc[0] = xfrag.bnd_left_as_biarc();
  bnd_biarc[1] = xfrag.bnd_right_as_biarc();

  // check validity of the curve
  if (!bnd_biarc[0].is_consistent() || !bnd_biarc[1].is_consistent())
  {
    return false;
  }

  // Compute image cost by matching each of these arcs with one in the pre-computed grid
  double total_len = 0;
  double total_cost = 0;
  for (unsigned i =0; i < 2; ++i)
  {
    double len = bnd_biarc[i].len();

    int half_num_pts = vnl_math_floor(len/(2*ds));
    vcl_vector<vgl_point_2d<double > > pts(2*half_num_pts+1);
    vcl_vector<vgl_vector_2d<double > > ts(2*half_num_pts+1);
    for (int k =-half_num_pts; k <= half_num_pts; ++k)
    {
      pts[k+half_num_pts] = bnd_biarc[i].point_at(len/2 + k*ds);
      ts [k+half_num_pts] = bnd_biarc[i].tangent_at(len/2 + k*ds);
    }
    total_cost += ocm.f(pts, ts) * len;
    total_len += len;
  }

  cost = (float) (total_cost / total_len);
  return true;
}












// -----------------------------------------------------------------------------
//: Compute ocm cost using pre-computed biarc points
bool dbsks_compute_ocm_cost(const dbsks_ocm_image_cost& ocm,
                            dbsks_biarc_sampler* biarc_sampler,
                            const dbsksp_xshock_fragment& xfrag,
                            float& cost,
                            vcl_vector<vgl_point_2d<double > >& pts,
                            vcl_vector<vgl_vector_2d<double > >& tangents)
{
  dbsksp_xshock_node_descriptor start = xfrag.start();
  dbsksp_xshock_node_descriptor end = xfrag.end();

  // left boundary
  biarc_sampler->compute_samples(start.bnd_pt_left(), start.bnd_tangent_left(),
    end.bnd_pt_left(), end.bnd_tangent_left(), 
    pts, tangents);
   
  int npts_left = pts.size();
  float cost_left = ocm.f(pts, tangents);

  // right boundary
  biarc_sampler->compute_samples(start.bnd_pt_right(), start.bnd_tangent_right(), 
    end.bnd_pt_right(), end.bnd_tangent_right(),
    pts, tangents);
  int npts_right = pts.size();
  float cost_right = ocm.f(pts, tangents);

  // final cost
  if (npts_left + npts_right <= 0)
  {
    cost = ocm.max_cost();
  }
  else
  {
    cost = (npts_left * cost_left + npts_right * cost_right)/ (npts_left + npts_right);
  }
  return true;
}












// ------------------------------------------------------------------------------
//: Compute ocm cost using a biarc grid sampler
bool dbsks_compute_ocm_cost(dbsks_shotton_ocm* shotton_ocm,
                            dbsks_biarc_sampler* biarc_sampler,
                            const dbsksp_xshock_fragment& xfrag,
                            float& cost)
{
  // place holders for sample points
  static vcl_vector<int >x_vec, y_vec, angle_vec;
  dbsksp_xshock_node_descriptor start = xfrag.start();
  dbsksp_xshock_node_descriptor end = xfrag.end();
  
  double angle_step;

  // left boundary
  biarc_sampler->compute_samples(start.bnd_pt_left(), start.bnd_tangent_left(),
    end.bnd_pt_left(), end.bnd_tangent_left(), x_vec, y_vec, angle_vec, angle_step);
  int npts_left = x_vec.size();
  float cost_left = shotton_ocm->f(x_vec, y_vec, angle_vec);
  
  // right boundary
  biarc_sampler->compute_samples(start.bnd_pt_right(), start.bnd_tangent_right(), 
    end.bnd_pt_right(), end.bnd_tangent_right(),
    x_vec, y_vec, angle_vec, angle_step);
  int npts_right = x_vec.size();
  float cost_right = shotton_ocm->f(x_vec, y_vec, angle_vec);

  // final cost
  cost = (npts_left * cost_left + npts_right * cost_right) / (npts_left + npts_right);
  return true;
}




// -----------------------------------------------------------------------------
//: Compute gray OCM cost using biarc sampler
bool dbsks_compute_ocm_cost_biarc(dbsks_gray_ocm* gray_ocm,
                            dbsks_biarc_sampler* biarc_sampler,
                            const vgl_point_2d<double >& start_pt,
                            const vgl_vector_2d<double >& start_tangent,
                            const vgl_point_2d<double >& end_pt,
                            const vgl_vector_2d<double >& end_tangent,
                            float& cost)
{
  static vcl_vector<int > x_vec, y_vec, angle_vec;
  double angle_step;

  // boundary curve
  biarc_sampler->compute_samples(start_pt, start_tangent, end_pt, end_tangent, 
    x_vec, y_vec, angle_vec, angle_step);
  if (x_vec.empty())
  {
    cost = 1.0;
  }
  else
  {
    cost = gray_ocm->f(x_vec, y_vec, angle_vec);
  }
  return true;
}



// -----------------------------------------------------------------------------
//: Compute gray OCM cost using biarc sampler
bool dbsks_compute_ocm_cost(dbsks_gray_ocm* gray_ocm,
                            dbsks_biarc_sampler* biarc_sampler,
                            const dbsksp_xshock_fragment& xfrag,
                            float& cost)
{
  static vcl_vector<int > x_vec, y_vec, angle_vec;
  dbsksp_xshock_node_descriptor start = xfrag.start();
  dbsksp_xshock_node_descriptor end = xfrag.end();

  // place holders for sample points
  double angle_step;

  // left boundary
  biarc_sampler->compute_samples(start.bnd_pt_left(), start.bnd_tangent_left(),
    end.bnd_pt_left(), end.bnd_tangent_left(), x_vec, y_vec, angle_vec, angle_step);
  int npts_left = x_vec.size();
  float cost_left = gray_ocm->f(x_vec, y_vec, angle_vec);
  
  // right boundary
  biarc_sampler->compute_samples(start.bnd_pt_right(), start.bnd_tangent_right(), 
    end.bnd_pt_right(), end.bnd_tangent_right(),
    x_vec, y_vec, angle_vec, angle_step);
  int npts_right = x_vec.size();
  float cost_right = gray_ocm->f(x_vec, y_vec, angle_vec);

  // final cost
  if (npts_left <= 0 || npts_right <= 0)
  {
    cost = 1.0f;
  }
  else
  {
    //cost = (npts_left*cost_left + npts_right*cost_right) / (npts_left+npts_right);
    cost = (cost_left + cost_right) / 2;
  }
  return true;
}



// -----------------------------------------------------------------------------
//: Compute gray OCM cost of a graph
bool dbsks_compute_ocm_cost(dbsks_gray_ocm* gray_ocm,
                            dbsks_biarc_sampler* biarc_sampler,
                            const dbsksp_xshock_graph_sptr& xgraph,
                            float& cost,
                            const vcl_vector<unsigned >& ignored_edges,
                            bool verbose)
{
  float sum_cost = 0;
  for (dbsksp_xshock_graph::edge_iterator eit = xgraph->edges_begin(); eit !=
    xgraph->edges_end(); ++eit)
  {
    dbsksp_xshock_edge_sptr xe = *eit;

    unsigned eid = xe->id();
    bool is_ignored = false;
    for (unsigned i =0; i < ignored_edges.size(); ++i)
    {
      if (ignored_edges[i] == eid)
      {
        is_ignored = true;
        break;
      }
    }
    if (is_ignored)
      continue;

    dbsksp_xshock_node_sptr xv_p, xv_c;
    if (xe->source()->depth() < xe->target()->depth())
    {
      xv_p = xe->source();
      xv_c = xe->target();
    }
    else
    {
      xv_p = xe->target();
      xv_c = xe->source();
    }
    assert(xv_p->depth() < xv_c->depth());

    dbsksp_xshock_node_descriptor xd0 = *(xv_p->descriptor(xe));
    dbsksp_xshock_node_descriptor xd1 = xv_c->descriptor(xe)->opposite_xnode();
    dbsksp_xshock_fragment xfrag(xd0, xd1);


    // Fragment cost
    float fragment_cost = 0;
    if (!dbsks_compute_ocm_cost(gray_ocm, biarc_sampler, xfrag, fragment_cost))
    {
      fragment_cost = 1;
    }
    sum_cost += fragment_cost;

    if (verbose)
    {
      vcl_cout << "   Edge_id=" << (*eit)->id() << " image_cost= " << fragment_cost << "\n";
      vcl_cout << "     parent_vid=" << xv_p->id() << "\n      ";
      xv_p->descriptor(xe)->print(vcl_cout);
      vcl_cout << "     child_vid=" << xv_c->id() << "\n      ";
      xv_c->descriptor(xe)->opposite_xnode().print(vcl_cout);
    }
  }

  if (verbose)
  {
    vcl_cout << "   Total cost=" << sum_cost << vcl_endl;
  }

  // final result
  cost = sum_cost;

  return true;
}





// -----------------------------------------------------------------------------
//: Compute gray OCM cost using biarc sampler
bool dbsks_compute_ocm_cost_biarc(dbsks_ccm* ccm,
                            dbsks_biarc_sampler* biarc_sampler,
                            const vgl_point_2d<double >& start_pt,
                            const vgl_vector_2d<double >& start_tangent,
                            const vgl_point_2d<double >& end_pt,
                            const vgl_vector_2d<double >& end_tangent,
                            float& cost)
{
  static vcl_vector<int > x_vec, y_vec, angle_vec;
  double angle_step;

  // boundary curve
  biarc_sampler->compute_samples(start_pt, start_tangent, end_pt, end_tangent, 
    x_vec, y_vec, angle_vec, angle_step);  
  cost = ccm->f(x_vec, y_vec, angle_vec);
  return true;
}








// -----------------------------------------------------------------------------
//: Compute contour OCM cost using biarc sampler
bool dbsks_compute_ocm_cost(dbsks_ccm* ccm,
                            dbsks_biarc_sampler* biarc_sampler,
                            const dbsksp_xshock_fragment& xfrag,
                            float& cost)
{
  static vcl_vector<int > x_vec, y_vec, angle_vec;
  dbsksp_xshock_node_descriptor start = xfrag.start();
  dbsksp_xshock_node_descriptor end = xfrag.end();

  // place holders for sample points
  double angle_step;

  // left boundary
  biarc_sampler->compute_samples(start.bnd_pt_left(), start.bnd_tangent_left(),
    end.bnd_pt_left(), end.bnd_tangent_left(), x_vec, y_vec, angle_vec, angle_step);
  int npts_left = x_vec.size();
  float cost_left = ccm->f(x_vec, y_vec, angle_vec);
  
  // right boundary
  biarc_sampler->compute_samples(start.bnd_pt_right(), start.bnd_tangent_right(), 
    end.bnd_pt_right(), end.bnd_tangent_right(),
    x_vec, y_vec, angle_vec, angle_step);
  int npts_right = x_vec.size();
  float cost_right = ccm->f(x_vec, y_vec, angle_vec);

  // final cost
  if (npts_left <= 0 || npts_right <= 0)
  {
    cost = 1.0f;
  }
  else
  {
    //cost = (npts_left*cost_left + npts_right*cost_right) / (npts_left+npts_right);
    cost = (cost_left + cost_right) / 2;
  }
  return true;
}





// -----------------------------------------------------------------------------
//: Compute contour OCM cost using biarc sampler
bool dbsks_compute_ocm_cost(dbsks_ccm* ccm,
                            dbsks_biarc_sampler* biarc_sampler,
                            const dbsksp_xshock_fragment& xfrag,
                            float& cost_left, float& cost_right)
{
  static vcl_vector<int > x_vec, y_vec, angle_vec;
  dbsksp_xshock_node_descriptor start = xfrag.start();
  dbsksp_xshock_node_descriptor end = xfrag.end();

  // place holders for sample points
  double angle_step;

  // left boundary
  biarc_sampler->compute_samples(start.bnd_pt_left(), start.bnd_tangent_left(),
    end.bnd_pt_left(), end.bnd_tangent_left(), x_vec, y_vec, angle_vec, angle_step);
  
  if (x_vec.empty())
  {
    cost_left = 1.0f;
  }
  else
  {
    cost_left = ccm->f(x_vec, y_vec, angle_vec);
  }
  
  // right boundary
  biarc_sampler->compute_samples(start.bnd_pt_right(), start.bnd_tangent_right(), 
    end.bnd_pt_right(), end.bnd_tangent_right(),
    x_vec, y_vec, angle_vec, angle_step);
  if (x_vec.empty())
  {
    cost_right = 1.0f;
  }
  else
  {
    cost_right = ccm->f(x_vec, y_vec, angle_vec);
  }
  return true;
}







// -----------------------------------------------------------------------------
//: Compute contour OCM cost of a graph
bool dbsks_compute_ocm_cost(dbsks_ccm* ccm,
                            dbsks_biarc_sampler* biarc_sampler,
                            const dbsksp_xshock_graph_sptr& xgraph,
                            float& cost,
                            const vcl_vector<unsigned >& ignored_edges,
                            bool verbose)
{
  float sum_cost = 0;
  for (dbsksp_xshock_graph::edge_iterator eit = xgraph->edges_begin(); eit !=
    xgraph->edges_end(); ++eit)
  {
    dbsksp_xshock_edge_sptr xe = *eit;

    unsigned eid = xe->id();
    bool is_ignored = false;
    for (unsigned i =0; i < ignored_edges.size(); ++i)
    {
      if (ignored_edges[i] == eid)
      {
        is_ignored = true;
        break;
      }
    }
    if (is_ignored)
      continue;

    dbsksp_xshock_node_sptr xv_p, xv_c;
    if (xe->source()->depth() < xe->target()->depth())
    {
      xv_p = xe->source();
      xv_c = xe->target();
    }
    else
    {
      xv_p = xe->target();
      xv_c = xe->source();
    }
    assert(xv_p->depth() < xv_c->depth());

    dbsksp_xshock_node_descriptor xd0 = *(xv_p->descriptor(xe));
    dbsksp_xshock_node_descriptor xd1 = xv_c->descriptor(xe)->opposite_xnode();
    dbsksp_xshock_fragment xfrag(xd0, xd1);


    // Fragment cost
    float fragment_cost = 0;
    if (!dbsks_compute_ocm_cost(ccm, biarc_sampler, xfrag, fragment_cost))
    {
      fragment_cost = 1;
    }
    sum_cost += fragment_cost;

    if (verbose)
    {
      vcl_cout << "   Edge_id=" << (*eit)->id() << " image_cost= " << fragment_cost << "\n";
      vcl_cout << "     parent_vid=" << xv_p->id() << "\n      ";
      xv_p->descriptor(xe)->print(vcl_cout);
      vcl_cout << "     child_vid=" << xv_c->id() << "\n      ";
      xv_c->descriptor(xe)->opposite_xnode().print(vcl_cout);
    }
  }

  if (verbose)
  {
    vcl_cout << "   Total cost=" << sum_cost << vcl_endl;
  }

  // final result
  cost = sum_cost;

  return true;
}














// -----------------------------------------------------------------------------
//: Compute CCM cost for every boundary contour fragment of a graph
// Assumption: root node has been chosen and vertex depths have been computed
// Return a map from contour fragment labels to their ccm costs
// Format of contour fragment label: 13-L for left fragment of edge_id=13
bool dbsks_compute_ccm_of_xgraph(dbsks_ccm* ccm,
                            dbsks_biarc_sampler* biarc_sampler,
                            const dbsksp_xshock_graph_sptr& xgraph,
                            vcl_vector<vcl_string >& cfrag_labels, 
                            vcl_vector<float >& cfrag_ccm_costs)
{
  // clean-up
  cfrag_labels.clear();
  cfrag_ccm_costs.clear();

  // allocate memory
  cfrag_labels.reserve(xgraph->number_of_edges()*2);
  cfrag_ccm_costs.reserve(xgraph->number_of_edges()*2);

  // Loop thru the edges 
  for (dbsksp_xshock_graph::edge_iterator eit = xgraph->edges_begin(); eit !=
    xgraph->edges_end(); ++eit)
  {
    dbsksp_xshock_edge_sptr xe = *eit;
    unsigned eid = xe->id();
   
    cfrag_labels.push_back(vul_sprintf("%d-L", eid)); 
    cfrag_labels.push_back(vul_sprintf("%d-R", eid)); 


    dbsksp_xshock_node_sptr xv_p = xe->parent_node();
    dbsksp_xshock_node_sptr xv_c = xe->child_node();
    assert(xv_p->depth() < xv_c->depth());

    dbsksp_xshock_node_descriptor start = *(xv_p->descriptor(xe));
    dbsksp_xshock_node_descriptor end = xv_c->descriptor(xe)->opposite_xnode();

    // Compute CCM cost
    vcl_vector<int > x_vec, y_vec, angle_vec;
    double angle_step;

    // boundary curve
    biarc_sampler->compute_samples(start.bnd_pt_left(), start.bnd_tangent_left(), 
      end.bnd_pt_left(), end.bnd_tangent_left(), 
      x_vec, y_vec, angle_vec, angle_step);  

    // ensure that the angle resolution in the biarc_sampler and in "ccm" are the same.
    // \todo convert if they're not
    assert(ccm->nbins_0to2pi() == biarc_sampler->nbins_0to2pi());    
    float cost_left = ccm->f(x_vec, y_vec, angle_vec);

    biarc_sampler->compute_samples(start.bnd_pt_right(), start.bnd_tangent_right(), 
      end.bnd_pt_right(), end.bnd_tangent_right(),
      x_vec, y_vec, angle_vec, angle_step);  
    float cost_right = ccm->f(x_vec, y_vec, angle_vec);

    cfrag_ccm_costs.push_back(cost_left);
    cfrag_ccm_costs.push_back(cost_right);
  }

  return true;
}
