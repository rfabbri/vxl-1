// This is file shp/dbsksp/dbsksp_shock_fragment.cxx

//:
// \file

#include "dbsksp_shock_fragment.h"
#include <dbsksp/dbsksp_shock_node.h>
#include <dbsksp/dbsksp_shock_node_descriptor.h>
#include <dbsksp/dbsksp_shock_edge.h>

#include <vnl/vnl_math.h>
#include <dbsksp/dbsksp_shapelet.h>


//: constructor
dbsksp_shock_fragment::
dbsksp_shock_fragment()
{
  this->bnd_[0] = new dbsksp_bnd_arc();
  this->bnd_[1] = new dbsksp_bnd_arc(); 
}



//: Build `this' shock fragment using info from a shock edge
void dbsksp_shock_fragment::
build_from(const dbsksp_shock_edge_sptr& edge)
{
  this->set_shock_edge(edge);
  this->build_from_regular_edge(edge);
}



//: Build this fragment from a regular edge (source != target)
void dbsksp_shock_fragment::
build_from_regular_edge(const dbsksp_shock_edge_sptr& edge)
{
  assert(edge->source() != edge->target());

  double x0, y0, theta0, r0, phi0, phi1, m0, len;
  x0 = edge->source()->pt().x();
  y0 = edge->source()->pt().y();
  theta0 = vcl_atan2(edge->chord_dir().y(), edge->chord_dir().x());
  r0 = edge->source()->radius();
  phi0 = edge->source()->descriptor(edge)->phi;
  phi1 = vnl_math::pi - edge->target()->descriptor(edge)->phi;
  m0 = edge->param_m();
  len = edge->chord_length();
  dbsksp_shapelet shapelet(x0, y0, theta0, r0, phi0, phi1, m0, len);

  this->left_bnd()->set_arc(shapelet.bnd_arc(0));
  this->right_bnd()->set_arc(shapelet.bnd_arc(1));
  this->set_shock_geom(shapelet.shock_geom());
  return;
}



//: Get the shapelet of this fragment
dbsksp_shapelet_sptr dbsksp_shock_fragment::
get_shapelet() const
{
  dbsksp_shock_edge_sptr edge = this->shock_edge();
  double x0, y0, theta0, r0, phi0, phi1, m0, len;
  x0 = edge->source()->pt().x();
  y0 = edge->source()->pt().y();
  theta0 = vcl_atan2(edge->chord_dir().y(), edge->chord_dir().x());
  r0 = edge->source()->radius();
  phi0 = edge->source()->descriptor(edge)->phi;
  phi1 = vnl_math::pi - edge->target()->descriptor(edge)->phi;
  m0 = edge->param_m();
  len = edge->chord_length();
  return new dbsksp_shapelet(x0, y0, theta0, r0, phi0, phi1, m0, len);
}



//: Update parameters on the edge and its nodes to match with those on the shapelet
void dbsksp_shock_fragment::
update_edge_with_shapelet(const dbsksp_shapelet_sptr& shapelet, 
                          const dbsksp_shock_node_sptr& starting_node,
                          bool maintain_phi_sum_equal_pi)
{
  dbsksp_shapelet_sptr s = shapelet;

  // reverse direction of the shapelet if necessary
  if (starting_node == this->shock_edge()->target())
  {
    s = shapelet->reversed_dir();
  }

  dbsksp_shock_node_sptr source = this->shock_edge()->source();
  dbsksp_shock_node_sptr target = this->shock_edge()->target();

  // the node coordinates
  this->shock_edge()->source()->set_pt(s->start());
  this->shock_edge()->target()->set_pt(s->end());

  // node phi
  if (maintain_phi_sum_equal_pi)
  {
    // start node
    // distribute the difference to other branches
    double phi_start_increment = s->phi_start()-source->descriptor(this->shock_edge())->phi;
    for (dbsksp_shock_node::edge_iterator eit = source->edges_begin();
      eit != source->edges_end(); ++eit)
    {
      source->descriptor(*eit)->phi -= phi_start_increment/(source->degree()-1);
    }
  
    // end node
    double phi_end_increment = (vnl_math::pi - s->phi_end())-target->descriptor(this->shock_edge())->phi;
    for (dbsksp_shock_node::edge_iterator eit = target->edges_begin();
      eit != target->edges_end(); ++eit)
    {
      target->descriptor(*eit)->phi -= phi_end_increment/(target->degree()-1);
    }
  }

  source->descriptor(this->shock_edge())->phi = s->phi_start();
  target->descriptor(this->shock_edge())->phi = vnl_math::pi - s->phi_end();
  

  // the edge: m and L
  this->shock_edge()->set_chord_length(s->chord_length());
  this->shock_edge()->set_param_m(s->m_start());


  return;
}


//: Build this fragment from a degenerate edge (source == target)
// TODO: remove this. No longer necessary with new convention on nodes and edges
// (no more degenerate edges)
void dbsksp_shock_fragment::
build_from_degenerate_edge(const dbsksp_shock_edge_sptr& edge)
{
  //
  assert(edge->source() == edge->target());
  dbsksp_shock_node_sptr node = edge->source();
  dbsksp_shock_node_descriptor_sptr desc = node->descriptor(edge);
  double phi = desc->phi;
  vgl_vector_2d<double > t = edge->chord_dir(node);
  vgl_point_2d<double > mid_point = node->pt() + node->radius() * t;
  vgl_point_2d<double > start = node->pt() + node->radius() * rotated(t, -phi);
  vgl_point_2d<double > end = node->pt() + node->radius() * rotated(t, phi);

  dbgl_circ_arc left_arc(start, mid_point, 1/node->radius());
  dbgl_circ_arc right_arc(end, mid_point, -1/node->radius());

  this->left_bnd()->set_arc(left_arc);
  this->right_bnd()->set_arc(right_arc);

  return;
}





