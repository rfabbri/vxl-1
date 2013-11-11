// This is file shp/dbsksp/dbsksp_xshock_graph.cxx

//:
// \file

#include "dbsksp_xshock_graph.h"

#include <dbsksp/dbsksp_xshock_fragment.h>
#include <dbgl/algo/dbgl_biarc.h>
#include <vnl/vnl_math.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vsol/vsol_box_2d.h>
#include <vcl_utility.h>


// -----------------------------------------------------------------------------
//: Constructor
dbsksp_xshock_graph::
dbsksp_xshock_graph(): dbgrl_graph<dbsksp_xshock_node, dbsksp_xshock_edge>(), 
next_available_id_(0)
{
}


// -----------------------------------------------------------------------------
//: Copy constructor
dbsksp_xshock_graph::
dbsksp_xshock_graph(dbsksp_xshock_graph& that)
{
  // 1. Create the nodes
  
  // iterate thru the nodes in the node list and create the nodes
  vcl_map<unsigned int, dbsksp_xshock_node_sptr > node_list;
  for(dbsksp_xshock_graph::vertex_iterator itr = that.vertices_begin(); itr != 
    that.vertices_end(); ++itr)
  {
    unsigned int id = (*itr)->id();
    
    // create the node
    dbsksp_xshock_node_sptr v = new dbsksp_xshock_node(id);
    node_list.insert(vcl_make_pair(id, v));
  }

  // 2. Create the edges
  
  // iterate thru the edges in the edge list and create the edges
  vcl_map<unsigned int, dbsksp_xshock_edge_sptr > edge_list;
  for(dbsksp_xshock_graph::edge_iterator itr = that.edges_begin(); itr != 
    that.edges_end();  ++itr)
  {
    dbsksp_xshock_edge_sptr that_e = *itr;

    // find id of the edge
    unsigned id = that_e->id();

    // Absolutely necessary properties
    unsigned source_id = that_e->source()->id();
    unsigned target_id = that_e->target()->id();

    dbsksp_xshock_node_sptr source_sptr = node_list[source_id];
    dbsksp_xshock_node_sptr target_sptr = node_list[target_id]; 

    // create the edge
    dbsksp_xshock_edge_sptr this_e = new dbsksp_xshock_edge(source_sptr, target_sptr, id);
    edge_list.insert(vcl_make_pair(id, this_e));
  }

  // 3. Add edges to the nodes and fill in info for shock node descriptors
  for(dbsksp_xshock_graph::vertex_iterator itr = that.vertices_begin(); itr != 
    that.vertices_end(); ++itr)
  {
    dbsksp_xshock_node_sptr that_v = *itr;

    // i. find id of the node and retrieve the pointer to the node
    dbsksp_xshock_node_sptr this_v = node_list[that_v->id()];

    // ii. Descriptor list
    //vcl_list<dbsksp_shock_node_descriptor_sptr > desc_list = that_v->descriptor_list();

    for (dbsksp_xshock_node::edge_iterator eit = that_v->edges_begin(); eit != 
      that_v->edges_end(); ++eit)
    {
      dbsksp_xshock_edge_sptr that_e = *eit;
      dbsksp_xshock_node_descriptor* that_descriptor = that_v->descriptor(that_e);

      // retrieve edge from edge id
      dbsksp_xshock_edge_sptr this_e = edge_list[that_e->id()];
      dbsksp_xshock_node_descriptor* this_descriptor = this_v->insert_shock_edge(this_e, 0);

      // equate the descriptors
      *this_descriptor = *that_descriptor;
    }

    // iii. Point coordinate
    this_v->set_pt(that_v->pt());    
    
    // iv. radius
    this_v->set_radius(that_v->radius());
  }

  // insert all the nodes and edges to the shock graph
  for (vcl_map<unsigned int, dbsksp_xshock_node_sptr >::iterator itr = node_list.begin();
    itr != node_list.end(); ++itr)
  {
    this->add_vertex(itr->second);
  }

  for (vcl_map<unsigned int, dbsksp_xshock_edge_sptr >::iterator itr = edge_list.begin();
    itr != edge_list.end(); ++itr)
  {
    this->add_edge(itr->second);
  }

  // 5. Last thing: next_available_id_
  this->set_next_available_id(that.next_available_id_);
  return;
}


  
// -----------------------------------------------------------------------------
//: Return the edge with the given id
dbsksp_xshock_edge_sptr dbsksp_xshock_graph::
edge_from_id(unsigned id)
{
  for (dbsksp_xshock_graph::edge_iterator eit = this->edges_begin();
    eit != this->edges_end(); ++eit)
  {
    if ((*eit)->id() == id)
    {
      return (*eit);
    }
  }
  return 0;
}

// -----------------------------------------------------------------------------
//: Return the node with the given id
dbsksp_xshock_node_sptr dbsksp_xshock_graph::
node_from_id(unsigned id)
{
  for (dbsksp_xshock_graph::vertex_iterator vit = this->vertices_begin();
    vit != this->vertices_end(); ++vit)
  {
    if ((*vit)->id() == id)
    {
      return (*vit);
    }
  }
  return 0;
}






// -----------------------------------------------------------------------------
//: similarity transform relative to a reference point
bool dbsksp_xshock_graph::
similarity_transform(const vgl_point_2d<double >& ref_pt, double dx, 
                     double dy, double dtheta, double scale)
{
  // compute transformation matrix
  vgl_h_matrix_2d<double > H;
  H.set_identity();
  H.set_translation(dx, dy);
  H.set_rotation(dtheta);
  H.set_scale(scale);
  

  for (dbsksp_xshock_graph::vertex_iterator vit = this->vertices_begin();
    vit != this->vertices_end(); ++vit)
  {
    dbsksp_xshock_node_sptr xv = *vit;

    // transform the vertex
    vgl_vector_2d<double > t(ref_pt.x(), ref_pt.y());
    vgl_homg_point_2d<double > old_hpt(xv->pt() - t); // relative to reference point
    vgl_homg_point_2d<double > new_hpt = H(old_hpt);
    vgl_point_2d<double > new_pt(new_hpt.x() / new_hpt.w(), new_hpt.y() / new_hpt.w());
    
    xv->set_pt(new_pt + t);
    xv->set_radius(xv->radius()*scale);

    // rotate the tangents
    for (dbsksp_xshock_node::edge_iterator eit = xv->edges_begin(); eit != 
      xv->edges_end(); ++eit)
    {
      dbsksp_xshock_node_descriptor* desc = xv->descriptor(*eit);
      desc->psi_ += dtheta;
    }
  }
  
  // reset bounding box
  this->bounding_box_ = 0;
  return true;
}

// -----------------------------------------------------------------------------
//: similarity transform relative to a reference point
//: (compatible with matlab's similarity transform)
bool dbsksp_xshock_graph::
similarity_transform2(const vgl_point_2d<double >& ref_pt, double dx,
                     double dy, double dtheta, double scale)
{
  // compute transformation matrix
  vgl_h_matrix_2d<double > H;
  H.set_similarity(scale, dtheta, dx, dy);


  for (dbsksp_xshock_graph::vertex_iterator vit = this->vertices_begin();
    vit != this->vertices_end(); ++vit)
  {
    dbsksp_xshock_node_sptr xv = *vit;

    // transform the vertex
    vgl_vector_2d<double > t(ref_pt.x(), ref_pt.y());
    vgl_homg_point_2d<double > old_hpt(xv->pt() - t); // relative to reference point
    vgl_homg_point_2d<double > new_hpt = H(old_hpt);
    vgl_point_2d<double > new_pt(new_hpt.x() / new_hpt.w(), new_hpt.y() / new_hpt.w());

    xv->set_pt(new_pt + t);
    xv->set_radius(xv->radius()*scale);

    // rotate the tangents
    for (dbsksp_xshock_node::edge_iterator eit = xv->edges_begin(); eit !=
      xv->edges_end(); ++eit)
    {
      dbsksp_xshock_node_descriptor* desc = xv->descriptor(*eit);
      desc->psi_ += dtheta;
    }
  }

  // reset bounding box
  this->bounding_box_ = 0;
  return true;
}

// -----------------------------------------------------------------------------
//: Translate the xshock graph
void dbsksp_xshock_graph::
translate(double dx, double dy)
{
  vgl_vector_2d<double > t(dx, dy);

  for (dbsksp_xshock_graph::vertex_iterator vit = this->vertices_begin();
    vit != this->vertices_end(); ++vit)
  {
    dbsksp_xshock_node_sptr xv = *vit;
    xv->set_pt(xv->pt() + t);
  }

  this->bounding_box_ = 0;
  return;
}


//------------------------------------------------------------------------------
//: Scale up the xshock graph around the center (cx, cy)
void dbsksp_xshock_graph::
scale_up(double cx, double cy, double scale_factor)
{
  vgl_point_2d<double > center(cx, cy);

  for (dbsksp_xshock_graph::vertex_iterator vit = this->vertices_begin();
    vit != this->vertices_end(); ++vit)
  {
    dbsksp_xshock_node_sptr xv = *vit;
    xv->set_pt(center + scale_factor * (xv->pt()-center));
    xv->set_radius(scale_factor * xv->radius());
  }
  this->update_all_degree_1_nodes();

  this->bounding_box_ = 0;
  return;
}



//------------------------------------------------------------------------------
//: Mirror an xgraph around a y-axis
void dbsksp_xshock_graph::
mirror_around_y_axis(double axis_x)
{
  // Things to change:
  // - node center
  // - psi-angles
  // - order of the edges around a vertex of degree-3 and above

  for (dbsksp_xshock_graph::vertex_iterator vit = this->vertices_begin(); vit !=
    this->vertices_end(); ++vit)
  {
    dbsksp_xshock_node_sptr xv = *vit;

    // change node center
    vgl_point_2d<double > pt = xv->pt();
    vgl_point_2d<double > new_pt(2*axis_x - pt.x(), pt.y());
    xv->set_pt(new_pt);

    // change psi-value for all edges
    // new_psi = pi - old_psi
    for (dbsksp_xshock_node::edge_iterator eit = xv->edges_begin(); eit != 
      xv->edges_end(); ++eit)
    {
      dbsksp_xshock_node_descriptor* xdesc = xv->descriptor(*eit);
      vgl_vector_2d<double > t = xdesc->shock_tangent();
      vgl_vector_2d<double > new_t(-t.x(), t.y());
      xdesc->set_shock_tangent(new_t);
    }

    // reverse edge order
    if (xv->degree() > 2)
    {
      xv->order_edge_list_by_shock_tangent();
    }
  }

  



 
}

// -----------------------------------------------------------------------------
//: area of the shock graph
double dbsksp_xshock_graph::
area()
{
  // compute area of the shock graph by summing up areas of individual fragments
  double sum_area = 0;
  for (dbsksp_xshock_graph::edge_iterator eit = this->edges_begin(); eit != 
    this->edges_end(); ++eit)
  {
    dbsksp_xshock_edge_sptr xe = *eit;
    
    // form a fragment for this edge
    dbsksp_xshock_node_descriptor* xdesc_source = xe->source()->descriptor(xe);
    dbsksp_xshock_node_descriptor* xdesc_target = xe->target()->descriptor(xe);
    
    dbsksp_xshock_fragment xfrag(*xdesc_source, xdesc_target->opposite_xnode());

    sum_area += xfrag.area();
  }
  return sum_area;
}






// -----------------------------------------------------------------------------
//: Return bounding box of the xgraph. Automatically compute if it has never been computed.
vsol_box_2d_sptr dbsksp_xshock_graph::
bounding_box()
{
  if (!this->bounding_box_)
  {
    this->update_bounding_box();
  }
  return this->bounding_box_;
}

// -----------------------------------------------------------------------------
//: Update bounding box of the xgraph with current graph property
void dbsksp_xshock_graph::
update_bounding_box()
{
  if (!this->bounding_box_)
  {
    this->bounding_box_ = new vsol_box_2d();
  }

  vgl_box_2d<double > bbox = this->compute_approx_bounding_box();
  this->bounding_box_->reset_bounds();
  this->bounding_box_->add_point(bbox.min_x(), bbox.min_y());
  this->bounding_box_->add_point(bbox.max_x(), bbox.max_y());
  return;
}



// -----------------------------------------------------------------------------
//: Compute bounding box of the shock graph
vgl_box_2d<double > dbsksp_xshock_graph::
compute_approx_bounding_box()
{
  vgl_box_2d<double > bbox;
  
  // iterate thru the vertices and grow the bounding box to include the circles
  // around each vertex
  for (dbsksp_xshock_graph::vertex_iterator vit = this->vertices_begin();
    vit != this->vertices_end(); ++vit)
  {
    dbsksp_xshock_node_sptr xv = *vit;
    vgl_point_2d<double > pt = xv->pt();
    double r = xv->radius();

    bbox.add(vgl_point_2d<double >(pt.x()-r, pt.y()-r));
    bbox.add(vgl_point_2d<double >(pt.x()+r, pt.y()+r));
  }

  return bbox;
}



//------------------------------------------------------------------------------
//: Group the vertices by their depths
// Example: vertex_bins[1] is a vector of all vertices with depth 1
bool dbsksp_xshock_graph::
group_vertices_by_depth(vcl_vector<vcl_vector<unsigned > >& vertex_bins)
{
  vertex_bins.clear();

  // make sure vertex depths have been computed
  dbsksp_xshock_node_sptr root_node = this->node_from_id(this->root_vertex_id());
  if (!root_node)
    return false;

  if (root_node->depth() != 0)
    return false;

  // find the maxium depth
  int max_depth = -1;
  for (dbsksp_xshock_graph::vertex_iterator vit = this->vertices_begin(); vit !=
    this->vertices_end(); ++vit)
  {
    int depth = (*vit)->depth();
    max_depth = (max_depth < depth) ? depth : max_depth;
  }

  // Collect the vertices be depth
  vertex_bins.clear();
  vertex_bins.resize(max_depth+1);

  for (dbsksp_xshock_graph::vertex_iterator vit = this->vertices_begin(); vit !=
    this->vertices_end(); ++vit)
  {
    int depth = (*vit)->depth();
    vertex_bins[depth].push_back((*vit)->id());
  }
  return true;
}



// -----------------------------------------------------------------------------
//: Update descriptors of degree-1 nodes to be compatiable with its adjacent 
// node (degree > 1)
// Require: degree of xv is 1.
bool dbsksp_xshock_graph::
update_degree_1_node(const dbsksp_xshock_node_sptr& xv)
{
  if (xv->degree() != 1) return false;
  dbsksp_xshock_edge_sptr xe = *xv->edges_begin();
  dbsksp_xshock_node_sptr xv2 = xe->opposite(xv);
  
  // the adjacent node should be non-terminal
  assert(xv2->degree() > 1);

  dbsksp_xshock_node_descriptor* xdesc = xv->descriptor(xe);
  dbsksp_xshock_node_descriptor* xdesc2 = xv2->descriptor(xe);
  
  // copy node properties
  xv->set_pt(xv2->pt());
  xv->set_radius(xv2->radius());
  xdesc->phi_ = vnl_math::pi;
  xdesc->psi_ = xdesc2->psi_ + vnl_math::pi;
  return true;
}


// -----------------------------------------------------------------------------
//: Update descriptors of degree-1 nodes around a particular node ('xv') so
// that they are compatible
bool dbsksp_xshock_graph::
update_degree_1_nodes_around(const dbsksp_xshock_node_sptr& xv)
{
  // check if any adjacent node is a terminal node
  for (dbsksp_xshock_node::edge_iterator eit = xv->edges_begin(); 
    eit != xv->edges_end(); ++eit)
  {
    dbsksp_xshock_node_sptr xv2 = (*eit)->opposite(xv);  
    if (xv2->degree() == 1)
    {
      this->update_degree_1_node(xv2);
    }
  }
  return true;
}


// -----------------------------------------------------------------------------
//: Update descriptors of all degree-1 nodes
bool dbsksp_xshock_graph::
update_all_degree_1_nodes()
{
  bool success = true;
  for (dbsksp_xshock_graph::vertex_iterator vit = this->vertices_begin();
    vit != this->vertices_end(); ++vit)
  {
    dbsksp_xshock_node_sptr xv = *vit;
    if (xv->degree() == 1)
    {
      success &= this->update_degree_1_node(xv);
    }
  }
  return success;
}




// -----------------------------------------------------------------------------
//: Insert a node in the middle of an edge without modifying the boundary 
// geometric position of the new node is indicated by t, t \in [0,1]
// with t = 0: source node, t = 1: target node
dbsksp_xshock_node_sptr dbsksp_xshock_graph::
insert_xshock_node(const dbsksp_xshock_edge_sptr& xe, double t)
{
  // we don't add node to a terminal edge
  if (xe->is_terminal_edge()) 
    return 0;
  
  // i. approximate the properties of the new node
  // \TODO This is arbitrary at the moment. Should improve.

  // source and target nodes of the old edge
  dbsksp_xshock_node_sptr xv0 = xe->source();
  dbsksp_xshock_node_sptr xv2 = xe->target();

  // store the node descriptor temporarily
  dbsksp_xshock_node_descriptor xdesc0 = (*xv0->descriptor(xe));
  dbsksp_xshock_node_descriptor xdesc2 = xv2->descriptor(xe)->opposite_xnode();

  // shock point
  dbgl_biarc shock_geom(xdesc0.pt(), xdesc0.shock_tangent(), xdesc2.pt(), xdesc2.shock_tangent());
  vgl_point_2d<double > xv1_pt = shock_geom.point_at(t * shock_geom.len());
  
  // shock tangent
  vgl_vector_2d<double > xv1_tangent = shock_geom.tangent_at(t * shock_geom.len());
  double xv1_psi = vcl_atan2(xv1_tangent.y(), xv1_tangent.x());

  // phi angle
  double xv1_phi = (1-t) * xdesc0.phi_ + t * xdesc2.phi_;

  // radius
  double xv1_radius = (1-t) * xdesc0.radius_ + t * xdesc2.radius_;

  // combine all to form a descriptor
  dbsksp_xshock_node_descriptor xv1_xdesc(xv1_pt.x(), xv1_pt.y(), xv1_psi, xv1_phi, xv1_radius);

  // Now, do the actual insertion
  return this->insert_xshock_node(xe, xv1_xdesc);
}








//------------------------------------------------------------------------------
//: Insert a node in the middle of an edge, given the descriptor of the new node
dbsksp_xshock_node_sptr dbsksp_xshock_graph::
insert_xshock_node(const dbsksp_xshock_edge_sptr& xe,
                   const dbsksp_xshock_node_descriptor& xdesc)
{
  dbsksp_xshock_node_sptr start_xv = xe->source();
  dbsksp_xshock_edge_sptr new_xe1 = 0;
  dbsksp_xshock_edge_sptr new_xe2 = 0;
  return this->insert_xshock_node(xe, start_xv, xdesc, new_xe1, new_xe2);
  //// we don't add node to a terminal edge
  //if (xe->is_terminal_edge()) 
  //  return 0;

  //// new : one node + two edges
  //// remove: one edge
  //dbsksp_xshock_node_sptr xv0 = xe->source();
  //dbsksp_xshock_node_sptr xv2 = xe->target();
  //dbsksp_xshock_node_sptr xv1 = new dbsksp_xshock_node(this->next_available_id());
  //dbsksp_xshock_edge_sptr xe0 = new dbsksp_xshock_edge(xv0, xv1, this->next_available_id());
  //dbsksp_xshock_edge_sptr xe2 = new dbsksp_xshock_edge(xv1, xv2, this->next_available_id());

  //// store the node descriptor temporarily
  //dbsksp_xshock_node_descriptor xdesc0 = (*xv0->descriptor(xe));
  //dbsksp_xshock_node_descriptor xdesc2 = xv2->descriptor(xe)->opposite_xnode();

  //// modify the graph, topologicaly
  //xv0->replace_shock_edge(xe, xe0);
  //xv2->replace_shock_edge(xe, xe2);
  //dbsksp_xshock_node_descriptor* xdesc10 = xv1->insert_shock_edge(xe0, 0);
  //dbsksp_xshock_node_descriptor* xdesc12 = xv1->insert_shock_edge(xe2, 0);

  //this->add_vertex(xv1);
  //this->remove_edge(xe);
  //this->add_edge(xe0);
  //this->add_edge(xe2);


  //// assign
  //xv1->set_pt(xdesc.pt());
  //xv1->set_radius(xdesc.radius());
  //
  //xdesc12->phi_ = xdesc.phi();
  //xdesc12->psi_ = xdesc.psi();

  //xdesc10->phi_ = vnl_math::pi - xdesc.phi();
  //xdesc10->psi_ = xdesc12->psi_ + vnl_math::pi;

  //return xv1;
}


//------------------------------------------------------------------------------
//: Insert a node in the middle of an edge, given the descriptor of the new node
dbsksp_xshock_node_sptr dbsksp_xshock_graph::
insert_xshock_node(const dbsksp_xshock_edge_sptr& xe,
                   const dbsksp_xshock_node_sptr& start_xv,
                   const dbsksp_xshock_node_descriptor& xdesc,
                   dbsksp_xshock_edge_sptr& xe_from_start_to_new_xv, // connecting start_xv and new node
                   dbsksp_xshock_edge_sptr& xe_from_new_xv_to_end) // connectig new node and end_xv
{
  // sanitize old data
  xe_from_start_to_new_xv = 0;
  xe_from_new_xv_to_end = 0;

  // we don't add node to a terminal edge
  if (xe->is_terminal_edge()) 
    return 0;

  // sanity check
  if (!xe->is_vertex(start_xv))
    return 0;

  // new : one node + two edges
  // remove: one edge
  dbsksp_xshock_node_sptr xv0 = start_xv;
  dbsksp_xshock_node_sptr xv2 = xe->opposite(xv0);
  dbsksp_xshock_node_sptr xv1 = new dbsksp_xshock_node(this->next_available_id());
  dbsksp_xshock_edge_sptr xe0 = new dbsksp_xshock_edge(xv0, xv1, this->next_available_id());
  dbsksp_xshock_edge_sptr xe2 = new dbsksp_xshock_edge(xv1, xv2, this->next_available_id());

  // store the node descriptor temporarily
  dbsksp_xshock_node_descriptor xdesc0 = (*xv0->descriptor(xe));
  dbsksp_xshock_node_descriptor xdesc2 = xv2->descriptor(xe)->opposite_xnode();

  // modify the graph, topologicaly
  xv0->replace_shock_edge(xe, xe0);
  xv2->replace_shock_edge(xe, xe2);
  dbsksp_xshock_node_descriptor* xdesc10 = xv1->insert_shock_edge(xe0, 0);
  dbsksp_xshock_node_descriptor* xdesc12 = xv1->insert_shock_edge(xe2, 0);

  this->add_vertex(xv1);
  this->remove_edge(xe);
  this->add_edge(xe0);
  this->add_edge(xe2);


  // assign
  xv1->set_pt(xdesc.pt());
  xv1->set_radius(xdesc.radius());
  
  xdesc12->phi_ = xdesc.phi();
  xdesc12->psi_ = xdesc.psi();

  xdesc10->phi_ = vnl_math::pi - xdesc.phi();
  xdesc10->psi_ = xdesc12->psi_ + vnl_math::pi;

  // record new edges and node
  xe_from_start_to_new_xv = xe0;
  xe_from_new_xv_to_end = xe2;

  return xv1;
}















// -----------------------------------------------------------------------------
//: Delete an A12 shock node
// \todo : Finish coding this function
dbsksp_xshock_edge_sptr dbsksp_xshock_graph::
remove_A12_node(const dbsksp_xshock_node_sptr& xv)
{
  if (!xv || xv->degree() != 2)
    return 0;

  // get a hold of two adjacent edges and nodes
  dbsksp_xshock_edge_sptr xe0 = *xv->edges_begin();
  dbsksp_xshock_edge_sptr xe2 = *(++(xv->edges_begin()));

  dbsksp_xshock_node_sptr xv0 = xe0->opposite(xv);
  dbsksp_xshock_node_sptr xv2 = xe2->opposite(xv);

  // Create a new edge which will replace the two old edges, xe0 and xe1
  dbsksp_xshock_edge_sptr new_xe = new dbsksp_xshock_edge(xv0, xv2, this->next_available_id());
  dbsksp_xshock_node_descriptor* xdesc0 = xv0->replace_shock_edge(xe0, new_xe);
  assert(xdesc0);

  dbsksp_xshock_node_descriptor* xdesc2 = xv2->replace_shock_edge(xe2, new_xe);
  assert(xdesc2);

  // Remove the old node and the two old edges
  xv->remove_all_shock_edges();
  this->remove_edge(xe0);
  this->remove_edge(xe2);
  this->remove_vertex(xv);

  // Add the new edge
  this->add_edge(new_xe);

  // In case one of the old nodes has degree 1
  if (xv0->degree() == 1)
    this->update_degree_1_node(xv0);

  if (xv2->degree() == 1)
    this->update_degree_1_node(xv2);
  
  return new_xe;
}


// -----------------------------------------------------------------------------
//: Insert a node in the middle of a terminal edge (to extend the xshock branch)
// `shock_length' is the length of the new A12 edge
dbsksp_xshock_node_sptr dbsksp_xshock_graph::
insert_xshock_node_at_terminal_edge(const dbsksp_xshock_edge_sptr& xe, double shock_length)
{
  // we don't add node to a terminal edge
  if (!xe->is_terminal_edge()) 
    return 0;

  // new : one node + two edges
  // remove: one edge
  dbsksp_xshock_node_sptr xv0 = (xe->source()->degree()==1) ? xe->target() : xe->source();
  dbsksp_xshock_node_sptr xv2 = xe->opposite(xv0);
  dbsksp_xshock_node_sptr xv1 = new dbsksp_xshock_node(this->next_available_id());
  dbsksp_xshock_edge_sptr xe0 = new dbsksp_xshock_edge(xv0, xv1, this->next_available_id());
  dbsksp_xshock_edge_sptr xe2 = new dbsksp_xshock_edge(xv1, xv2, this->next_available_id());

  // store the node descriptor temporarily
  dbsksp_xshock_node_descriptor xdesc0 = (*xv0->descriptor(xe));
  dbsksp_xshock_node_descriptor xdesc2 = xv2->descriptor(xe)->opposite_xnode();

  // modify the graph, topologicaly
  xv0->replace_shock_edge(xe, xe0);
  xv2->replace_shock_edge(xe, xe2);
  dbsksp_xshock_node_descriptor* xdesc10 = xv1->insert_shock_edge(xe0, 0);
  dbsksp_xshock_node_descriptor* xdesc12 = xv1->insert_shock_edge(xe2, 0);

  this->add_vertex(xv1);
  this->remove_edge(xe);
  this->add_edge(xe0);
  this->add_edge(xe2);

  // determine the geometric properties of the new edges and node

  // i. approximate the properties of the new node
  vgl_point_2d<double > xv1_pt = xdesc0.pt() + shock_length * xdesc0.shock_tangent();
  vgl_vector_2d<double > xv1_tangent = xdesc0.shock_tangent();
  double xv1_phi = xdesc0.phi_;
  double xv1_radius = xdesc0.radius_ - shock_length * vcl_cos(xdesc0.phi_);

  // prevent the radius from going negative
  if (xv1_radius < 0)
    xv1_radius = 0;

  // ii. assign
  xv1->set_pt(xv1_pt);
  xv1->set_radius(xv1_radius);
  
  xdesc12->phi_ = xv1_phi;
  xdesc12->psi_ = vcl_atan2(xv1_tangent.y(), xv1_tangent.x());

  xdesc10->phi_ = vnl_math::pi - xv1_phi;
  xdesc10->psi_ = xdesc12->psi_ + vnl_math::pi;

  this->update_degree_1_node(xv2);
  return xv1;
}








// -----------------------------------------------------------------------------
//: Add an A_infty branch to an existing A_1^2 node, right before a reference edge "xe"
// The A_1^2 node becomes an A_1^3 node
dbsksp_xshock_edge_sptr dbsksp_xshock_graph::
insert_A_infty_branch(const dbsksp_xshock_node_sptr& xv, 
                      const dbsksp_xshock_edge_sptr& xe)
{
  // preliminary check
  if (!xe->is_vertex(xv)) return 0;

  // only handle degree-2 node right now
  if (xv->degree() != 2) 
  {
    vcl_cerr << "ERROR: add_A_infty_branch currently only handles degree-2 nodes.\n";
    return 0;
  }

  // topology
  dbsksp_xshock_node_sptr xv0 = new dbsksp_xshock_node(this->next_available_id());
  dbsksp_xshock_edge_sptr xe0 = new dbsksp_xshock_edge(xv0, xv, this->next_available_id());
  xv0->insert_shock_edge(xe0, 0);
  dbsksp_xshock_node_descriptor* xdesc0 = xv->insert_shock_edge(xe0, xe);
  this->add_vertex(xv0);
  this->add_edge(xe0);

  // determine geometric properties
  dbsksp_xshock_node_descriptor* xdesc = xv->descriptor(xe);
  xdesc0->pt_ = xv->pt();
  xdesc0->radius_ = xv->radius();
  xdesc0->psi_ = xdesc->psi_ - xdesc->phi_;
  xdesc0->phi_ = 0;
  this->update_degree_1_node(xv0);
  return xe0;
}








// -----------------------------------------------------------------------------
//: Remove the A_infty branch at an A_1^2-A_infty node and convert the A_1^2-A_infty node
// (A_1^3 node) to an A_1^2 node. If the phi angle of the A_infty branch is not zero,
// it will distributed to the two adjacent branches. phi_ratio_succ (\in [0, 1]) is the
// proportion given to the successing branch, in CCW order, of the A_infty branch.
// The predessessing branch get (1-phi_ratio_succ) * phi
// Return the pointer to the branch which previously succeeded the A_infty branch
dbsksp_xshock_edge_sptr dbsksp_xshock_graph::
remove_A_infty_branch(const dbsksp_xshock_node_sptr& xv,
                      const dbsksp_xshock_edge_sptr& terminal_xe, double phi_ratio_succ)
{
  // preliminary checks
  if (!xv || xv->degree() != 3)
    return 0;
  if (!terminal_xe || !terminal_xe->is_vertex(xv) || !terminal_xe->is_terminal_edge())
    return 0;

  if (phi_ratio_succ <0 || phi_ratio_succ >1)
    return 0;

  // Collect all the related info
  dbsksp_xshock_edge_sptr pred_xe = this->cyclic_adj_pred(terminal_xe, xv);
  dbsksp_xshock_edge_sptr succ_xe = this->cyclic_adj_succ(terminal_xe, xv);
  dbsksp_xshock_node_sptr terminal_xv = terminal_xe->opposite(xv);
  assert(terminal_xv->degree() == 1);

  dbsksp_xshock_node_descriptor terminal_xdesc = *xv->descriptor(terminal_xe);

  // disconnect the terminal edge from its vertices
  terminal_xv->remove_shock_edge(terminal_xe);
  xv->remove_shock_edge(terminal_xe);

  // remove the terminal edge and terminal vertex from the graph
  this->remove_edge(terminal_xe);
  this->remove_vertex(terminal_xv);

  // modify the descriptors of predecessing and successing edges
  dbsksp_xshock_node_descriptor* pred_xdesc = xv->descriptor(pred_xe);
  dbsksp_xshock_node_descriptor* succ_xdesc = xv->descriptor(succ_xe);

  double pred_dphi = (1-phi_ratio_succ) * terminal_xdesc.phi();
  double succ_dphi = phi_ratio_succ * terminal_xdesc.phi();
  
  pred_xdesc->set_phi(pred_xdesc->phi() + pred_dphi);
  pred_xdesc->set_shock_tangent(rotated(pred_xdesc->shock_tangent(), pred_dphi));

  succ_xdesc->set_phi(succ_xdesc->phi() + succ_dphi);
  succ_xdesc->set_shock_tangent(rotated(succ_xdesc->shock_tangent(), -succ_dphi));

  return succ_xe;
}





// -----------------------------------------------------------------------------
//: Remove a terminal edge (a leaf edge) at an A_1^2 node
// Return pointer to the edge which preceded the terminal edge before the removal
dbsksp_xshock_edge_sptr dbsksp_xshock_graph::
remove_leaf_edge_at_A12_node(const dbsksp_xshock_edge_sptr& terminal_xe)
{
  if (!terminal_xe || !terminal_xe->is_terminal_edge())
    return 0;

  // the leaf node (degree-1 node)
  dbsksp_xshock_node_sptr terminal_xv = (terminal_xe->source()->degree()==1) ?
    terminal_xe->source() : terminal_xe->target();

  // the A12 node (soon to be a leaf node)
  dbsksp_xshock_node_sptr xv = terminal_xe->opposite(terminal_xv);

  // enforce that xv has degree-2 (important!!!)
  if (xv->degree() != 2)
    return 0;

  // The edge immediately preceded the terminal edge
  dbsksp_xshock_edge_sptr xe = this->cyclic_adj_succ(terminal_xe, xv);

  // disconnect the terminal edge from its vertices
  terminal_xv->remove_shock_edge(terminal_xe);
  xv->remove_shock_edge(terminal_xe);

  // remove the terminal edge and terminal vertex from the graph
  this->remove_edge(terminal_xe);
  this->remove_vertex(terminal_xv);

  // modify the descriptor of xe
  this->update_degree_1_node(xv);

  return xe;
}









// -----------------------------------------------------------------------------
//: Compute vertex depth
bool dbsksp_xshock_graph::
compute_vertex_depths(unsigned root_vertex_id)
{
  this->root_vertex_id_ = root_vertex_id;

  vcl_map<dbsksp_xshock_node_sptr, int > vertex_depth;
  
  // do an Euler tour of the graph (tree)
  dbsksp_xshock_node_sptr init_node = this->node_from_id(root_vertex_id);
  if (!init_node) return false;

  dbsksp_xshock_edge_sptr init_edge = *init_node->edges_begin();
  vertex_depth.insert(vcl_make_pair(init_node, 0));

  dbsksp_xshock_node_sptr cur_node = init_node;
  dbsksp_xshock_edge_sptr cur_edge = init_edge;
  int cur_depth = 0;

  do
  {
    vcl_map<dbsksp_xshock_node_sptr, int >::iterator it = vertex_depth.find(cur_node);
    if (it != vertex_depth.end()) // found it!
    {
      cur_depth = it->second;
    }
    else // unknown vertex, set depth for it
    {
      ++cur_depth;
      vertex_depth.insert(vcl_make_pair(cur_node, cur_depth));
    }

    cur_node = cur_edge->opposite(cur_node);
    cur_edge = this->cyclic_adj_succ(cur_edge, cur_node);
  }
  while (cur_node != init_node || cur_edge != init_edge);

  // set the depths
  for (vcl_map<dbsksp_xshock_node_sptr, int >::iterator it = vertex_depth.begin();
    it != vertex_depth.end(); ++it)
  {
    it->first->set_depth(it->second);
  }

  return true;
}

// -----------------------------------------------------------------------------
//: print info of the shock graph to an output stream
vcl_ostream& dbsksp_xshock_graph::
print(vcl_ostream & os) const
{
  return os;
}





