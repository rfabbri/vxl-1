// This is file seg/dbsks/dbsks_xshock_likelihood.cxx

//:
// \file

#include "dbsks_xshock_likelihood.h"
#include <dbsksp/dbsksp_xshock_graph.h>
#include <vnl/vnl_numeric_traits.h>

// =============================================================================
// dbsks_xshock_likelihood
// =============================================================================



//------------------------------------------------------------------------------
//: Compute lower bounds of the likelihood function
double dbsks_xshock_likelihood::
loglike_lowerbound(unsigned edge_id) const
{
  return -vnl_numeric_traits<double >::maxval;
}



//------------------------------------------------------------------------------
//: Computer upper bound of the likelihood function
double dbsks_xshock_likelihood::
loglike_upperbound(unsigned edge_id) const
{
  return vnl_numeric_traits<double >::maxval;
}


//------------------------------------------------------------------------------
//: Likelihood of a xshock graph
double dbsks_xshock_likelihood::
loglike_xgraph(const dbsksp_xshock_graph_sptr& xgraph, 
               const vcl_vector<unsigned >& ignored_edges,
               bool verbose) const
{
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


    // form an xshock fragment for the edge
    dbsksp_xshock_node_descriptor xd0 = *(xv_p->descriptor(xe));
    dbsksp_xshock_node_descriptor xd1 = xv_c->descriptor(xe)->opposite_xnode();
    dbsksp_xshock_fragment xfrag(xd0, xd1);

    // Compute fragment cost
    double fragment_cost = this->loglike(eid, xfrag);
    sum_cost += fragment_cost;

    if (verbose)
    {
      vcl_cout << "   Edge_id=" << (*eit)->id() << " image_cost= " << fragment_cost << "\n";
      vcl_cout << "     parent_vid=" << xv_p->id() << "\n";
      //xv_p->descriptor(xe)->print(vcl_cout);
      vcl_cout << "     child_vid=" << xv_c->id() << "\n";
      //xv_c->descriptor(xe)->opposite_xnode().print(vcl_cout);
    }
  }
  if (verbose)
  {
    vcl_cout << "   Total cost=" << sum_cost << vcl_endl;
  }
  return sum_cost;
}
