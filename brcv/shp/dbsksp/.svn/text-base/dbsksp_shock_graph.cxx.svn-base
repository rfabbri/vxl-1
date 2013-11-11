// This is file shp/dbsksp/dbsksp_shock_graph.cxx

//:
// \file

#include "dbsksp_shock_graph.h"

#include <vnl/vnl_math.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_box_2d.h>
#include <dbsksp/dbsksp_shock_node_sptr.h>
#include <dbsksp/dbsksp_shock_edge_sptr.h>
#include <dbsksp/dbsksp_shock_fragment.h>
#include <vgl/vgl_distance.h>
#include <dbgl/algo/dbgl_closest_point.h>

const double dbsksp_epsilon = 1e-12;

// ----------------------------------------------------------------------------
//: Constructor
dbsksp_shock_graph::
dbsksp_shock_graph(): 
dbgrl_graph<dbsksp_shock_node, dbsksp_shock_edge>(), 
i_traverse_flag_(0), 
next_available_id_(0),
ref_node_(0),
ref_edge_(0),
ref_origin_(vgl_point_2d<double >(0, 0)),
ref_direction_(0, 0),
ref_node_radius_(-1)
{

};




// ----------------------------------------------------------------------------
//: Copy constructor
// Need to fix this constructor so that it also works when the argument is
// "const dbsksp_shock_graph&"
dbsksp_shock_graph::
dbsksp_shock_graph(dbsksp_shock_graph& that):
dbgrl_graph<dbsksp_shock_node, dbsksp_shock_edge>(), 
i_traverse_flag_(0)
{
  // 1. Create the nodes
  // iterate thru the nodes in the node list and create the nodes
  vcl_map<unsigned int, dbsksp_shock_node_sptr > node_list;
  for(dbsksp_shock_graph::vertex_iterator itr = that.vertices_begin();
    itr != that.vertices_end(); ++itr)
  {
    unsigned int id = (*itr)->id();
    
    // create the node
    dbsksp_shock_node_sptr v = new dbsksp_shock_node(id);
    node_list.insert(vcl_make_pair(id, v));
  }

  // 2. Create the edges
  // iterate thru the edges in the edge list and create the edges
  vcl_map<unsigned int, dbsksp_shock_edge_sptr > edge_list;
  for(dbsksp_shock_graph::edge_iterator itr = that.edges_begin();
    itr != that.edges_end();  ++itr)
  {
    dbsksp_shock_edge_sptr that_e = *itr;

    // find id of the edge
    int id = that_e->id();

    // Absolutely necessary properties
    int source_id = that_e->source()->id();
    int target_id = that_e->target()->id();


    vcl_map<unsigned int, dbsksp_shock_node_sptr >::iterator vit = node_list.find(source_id);
    if (vit == node_list.end())
    {
      vcl_cerr << "ERROR: cannot find source of edge, source_id=" << source_id 
        << vcl_endl;
      assert(false);
    }
    dbsksp_shock_node_sptr source_sptr = vit->second;

    vit = node_list.find(target_id);
    if (vit == node_list.end())
    {
      vcl_cerr << "ERROR: cannot find target of edge, target_id=" << target_id 
        << vcl_endl;
      assert(false);
    }
    dbsksp_shock_node_sptr target_sptr = vit->second;

    // create the edge
    dbsksp_shock_edge_sptr this_e = new dbsksp_shock_edge(source_sptr, target_sptr, id);
    edge_list.insert(vcl_make_pair(id, this_e));

    // free parameters
    this_e->set_param_m(that_e->param_m());
    this_e->set_chord_length(that_e->chord_length());

    // dependent parameters
    this_e->set_chord_dir(that_e->chord_dir());
    this_e->set_radius_increment(that_e->radius_increment());
  }


  // At this point al the edges are done. The only things left are the info
  // of the nodes, including the descriptor list

  // 3. Add edges to the nodes and fill in info for shock node descriptors
  for(dbsksp_shock_graph::vertex_iterator itr = that.vertices_begin();
    itr != that.vertices_end(); ++itr)
  {
    dbsksp_shock_node_sptr that_v = *itr;

    // i. find id of the node and retrieve the pointer to the node
    dbsksp_shock_node_sptr this_v = node_list.find(that_v->id())->second;

    // ii. Point coordinate
    this_v->set_pt(that_v->pt());    
    
    // iii. radius
    this_v->set_radius(that_v->radius());


    // iv. Descriptor list
    vcl_list<dbsksp_shock_node_descriptor_sptr > desc_list = that_v->descriptor_list();

    // parse the descriptor list
    for(vcl_list<dbsksp_shock_node_descriptor_sptr >::iterator itr = 
      desc_list.begin(); itr != desc_list.end(); ++itr)
    {
      dbsksp_shock_node_descriptor_sptr that_descriptor = *itr;
      
      // a. Edge id
      int edge_id = that_descriptor->edge->id();

      // retrieve edge from edge id
      dbsksp_shock_edge_sptr this_e = edge_list.find(edge_id)->second;
      this_v->add_edge(this_e);
      dbsksp_shock_node_descriptor_sptr this_descriptor = this_v->descriptor(this_e);

      // b. Angle alpha (optional)
      this_descriptor->alpha = that_descriptor->alpha;
      
      // c. Angle phi (required)
      this_descriptor->phi = that_descriptor->phi;

      // d. Rotation angle to successor edge (optional)
      this_descriptor->rot_angle_to_succ_edge = that_descriptor->rot_angle_to_succ_edge;
      
      // e. Shock direction
      this_descriptor->shock_flow_dir = that_descriptor->shock_flow_dir;
    }
  }

  // insert all the nodes and edges to the shock graph
  for (vcl_map<unsigned int, dbsksp_shock_node_sptr >::iterator itr = node_list.begin();
    itr != node_list.end(); ++itr)
  {
    this->add_vertex(itr->second);
  }

  for (vcl_map<unsigned int, dbsksp_shock_edge_sptr >::iterator itr = edge_list.begin();
    itr != edge_list.end(); ++itr)
  {
    this->add_edge(itr->second);
  }



  // 4. Parse the reference node and reference edge info

  // i. Reference node

  // a. retrieve ref_node from id
  unsigned int ref_node_id = that.ref_node()->id();
  this->set_ref_node(node_list.find(ref_node_id)->second);

  // b. coordinate of reference node
  this->set_ref_origin(that.ref_node()->pt() );

  // c. radius at reference node
  this->set_ref_node_radius(that.ref_node_radius());


  // ii. Reference edge

  // a. id of the reference edge
  int ref_edge_id = that.ref_edge()->id();
  this->set_ref_edge(edge_list.find(ref_edge_id)->second);

  // b. reference edge direction
  this->set_ref_direction(that.ref_direction());


  // 5. Last thing: next_available_id_
  this->set_next_available_id(that.next_available_id_);
  return;
}





// ----------------------------------------------------------------------------
//: Initialization
bool dbsksp_shock_graph::
init(vsol_polyline_2d_sptr polyline)
{
  // create a list of nodes associated with vertices of the polyline
  vcl_vector<dbsksp_shock_node_sptr > node_list;
  node_list.push_back(new dbsksp_shock_node(this->next_available_id()));
  for (unsigned int i=0; i<polyline->size(); ++i)
  {
    node_list.push_back(new dbsksp_shock_node(this->next_available_id()));
  }
  node_list.push_back(new dbsksp_shock_node(this->next_available_id()));

  // list of edges
  vcl_vector<dbsksp_shock_edge_sptr > edge_list;

  for (unsigned int i=0; i<node_list.size()-1; ++i)
  {
    dbsksp_shock_edge_sptr e;

    // special treatment for the first edge (make sure the A-infty node is
    // the target
    if (i==0)
    {
      e = new dbsksp_shock_edge(node_list[1], 
      node_list[0], this->next_available_id());
    }
    else
    {
      e = new dbsksp_shock_edge(node_list[i], 
      node_list[i+1], this->next_available_id());
    }
    
    
    node_list[i]->add_edge(e);
    node_list[i+1]->add_edge(e);
    edge_list.push_back(e);
  }

  // Now assign initial free variables to the nodes and edges

  // >> nodes
  unsigned int sink_node_order = (node_list.size()/2);

  // node [0]
  node_list[0]->descriptor(edge_list.front())->phi = vnl_math::pi;
  node_list[0]->descriptor(edge_list.front())->alpha = 0;
  node_list[0]->descriptor(edge_list.front())->shock_flow_dir = 0;

  // node [1..N-2]
  for (unsigned int i=1; i<node_list.size()-1; ++i)
  {
    dbsksp_shock_node_sptr node = node_list[i];
    node->descriptor(edge_list[i-1])->phi = vnl_math::pi /2;
    node->descriptor(edge_list[i])->phi = vnl_math::pi /2;

    // shock direction
    if (i<sink_node_order)
    {
      node->descriptor(edge_list[i-1])->shock_flow_dir = -1;
      node->descriptor(edge_list[i])->shock_flow_dir = 1;
    }
    else if (i>sink_node_order)
    {
      node->descriptor(edge_list[i-1])->shock_flow_dir = 1;
      node->descriptor(edge_list[i])->shock_flow_dir = -1;
    }
    else
    {
      node->descriptor(edge_list[i-1])->shock_flow_dir = 0;
      node->descriptor(edge_list[i])->shock_flow_dir = 0;
    }

  }

  node_list.back()->descriptor(edge_list.back())->phi = vnl_math::pi;
  node_list.back()->descriptor(edge_list.back())->alpha = 0;

  // >> edges
  edge_list[0]->set_chord_length(0);
  edge_list[0]->set_param_m(0);

  double alpha0 = 0;
  double alpha2;

  for (unsigned int i=1; i<edge_list.size()-1; ++i)
  {
    dbsksp_shock_edge_sptr edge = edge_list[i];

    // length
    
    vsol_point_2d_sptr v1 = polyline->vertex(i-1);
    vsol_point_2d_sptr v2 = polyline->vertex(i);

    edge->set_chord_length(v1->distance(v2));

    // param m
    vgl_vector_2d<double > t2 = normalized(v2->get_p() - v1->get_p());
    vgl_vector_2d<double > t0 = -t2;
    
    // only apply for the second segment and later
    if (i>1)
    {
      vsol_point_2d_sptr v0 = polyline->vertex(i-2);
      t0 = normalized(v0->get_p() - v1->get_p());
    }
    
    double theta = vcl_atan2(cross_product(t0, t2), dot_product(t0, t2));

    // solve for alpha2
    // we have
    // angle (t0, t2) = pi = -alpha0 + theta +  alpha2, therefore
    alpha2 = alpha0 - theta + vnl_math::pi;
    edge->set_param_m(vcl_sin(alpha2), node_list[i]);

    // update alpha0 for next edge
    alpha0 = -alpha2;
  }
  edge_list.back()->set_chord_length(0);
  edge_list.back()->set_param_m(0);
  

  // Now insert everything to the graph
  for (unsigned int i=0; i<node_list.size(); ++i)
  {
    this->add_vertex(node_list[i]);
  }

  for (unsigned int i=0; i<edge_list.size(); ++i)
  {
    this->add_edge(edge_list[i]);
  }

  // now set the reference node and edge
  this->set_ref_node(node_list[1]);
  this->set_ref_edge(edge_list[1]);
  this->set_ref_origin(polyline->vertex(0)->get_p());
  this->set_ref_direction(polyline->vertex(1)->get_p()-polyline->vertex(0)->get_p());
  this->set_ref_node_radius(20);

  return true;
}

// ----------------------------------------------------------------------------
//: Init the graph with three branches meeting at an A_1^3 node
void dbsksp_shock_graph::
init_simple_a13_graph()
{
  // create a list of nodes
  // A_1^3 node
  dbsksp_shock_node_sptr a13_node = new dbsksp_shock_node(this->next_available_id());

  // A_1^2 nodes
  dbsksp_shock_node_sptr a12_nodes[3];
  for (int i=0; i<3; ++i)
  {
    a12_nodes[i] = new dbsksp_shock_node(this->next_available_id());
  }

  // A_\infty nodes
  dbsksp_shock_node_sptr a3_nodes[3];
  for (int i=0; i<3; ++i)
  {
    a3_nodes[i] = new dbsksp_shock_node(this->next_available_id());
  }

  // create a list of edges
  // 3 edges connecting to the A_1^3 node
  dbsksp_shock_edge_sptr a12_edges[3];
  for (int i=0; i<3; ++i)
  {
    a12_edges[i] = new dbsksp_shock_edge(a12_nodes[i], a13_node, 
      this->next_available_id());
    a12_nodes[i]->add_edge(a12_edges[i]);
    a13_node->add_edge(a12_edges[i]);
  } 

  // 3 degenerate edges corresponding to the A_infty fragments
  dbsksp_shock_edge_sptr a3_edges[3];
  for (int i=0; i<3; ++i)
  {
    a3_edges[i] = new dbsksp_shock_edge(a3_nodes[i], a12_nodes[i], 
      this->next_available_id());
    a3_nodes[i]->add_edge(a3_edges[i]);
    a12_nodes[i]->add_edge(a3_edges[i]);
  } 


  // Now assign initial free variables to the nodes and edges
  // At the nodes

  // a13_node
  for (int i=0; i<3; ++i)
  {
    a13_node->descriptor(a12_edges[i])->phi = vnl_math::pi/3;
    a13_node->descriptor(a12_edges[i])->alpha = 0;
    a13_node->descriptor(a12_edges[i])->shock_flow_dir = -1;
    a13_node->descriptor(a12_edges[i])->rot_angle_to_succ_edge = vnl_math::pi*2/3;
  }

  // a12 nodes
  for (int i=0; i<3; ++i)
  {
    a12_nodes[i]->descriptor(a12_edges[i])->phi = vnl_math::pi * 7.0/12;
    a12_nodes[i]->descriptor(a12_edges[i])->alpha = 0;
    a12_nodes[i]->descriptor(a12_edges[i])->shock_flow_dir = 1;
    a12_nodes[i]->descriptor(a12_edges[i])->rot_angle_to_succ_edge = vnl_math::pi;

    a12_nodes[i]->descriptor(a3_edges[i])->phi = vnl_math::pi * 5.0/12;
    a12_nodes[i]->descriptor(a3_edges[i])->alpha = 0;
    a12_nodes[i]->descriptor(a3_edges[i])->shock_flow_dir = -1;
    a12_nodes[i]->descriptor(a3_edges[i])->rot_angle_to_succ_edge = vnl_math::pi;

  }

  // a3 nodes
  for (int i=0; i<3; ++i)
  {
    a3_nodes[i]->descriptor(a3_edges[i])->phi = vnl_math::pi;
    a3_nodes[i]->descriptor(a3_edges[i])->alpha = 0;
    a3_nodes[i]->descriptor(a3_edges[i])->shock_flow_dir = 1;
    a3_nodes[i]->descriptor(a3_edges[i])->rot_angle_to_succ_edge = 2*vnl_math::pi;
  }

  // At the edges
  // a12 edges
  for (int i=0; i<3; ++i)
  {
    a12_edges[i]->set_chord_length(100);
    a12_edges[i]->set_param_m(0);
  }

  for (int i=0; i<3; ++i)
  {
    a3_edges[i]->set_chord_length(0);
    a3_edges[i]->set_param_m(0);
  }

  // Now insert everything to the graph
  this->clear();

  // a13_node
  this->add_vertex(a13_node);

  // a12 nodes
  for (int i=0; i<3; ++i)
  {
    this->add_vertex(a12_nodes[i]);
  }

  // a3 nodes
  for (int i=0; i<3; ++i)
  {
    this->add_vertex(a3_nodes[i]);
  }

  // the edges
  for (int i=0; i<3; ++i)
  {
    this->add_edge(a12_edges[i]);
  }

  for (int i=0; i<3; ++i)
  {
    this->add_edge(a3_edges[i]);
  }

  // now set the reference node and edge
  
  // Reference coordinates
  vgl_point_2d<double > origin(200, 200);
  vgl_vector_2d<double > chord_dir(1, 0);

  this->set_ref_node(a13_node);
  this->set_ref_edge(a12_edges[0]);
  this->set_ref_origin(vgl_point_2d<double >(200, 200));
  this->set_ref_direction(vgl_vector_2d<double >(1, 0));
  this->set_ref_node_radius(60);
    
  return;
}



// ----------------------------------------------------------------------------
bool dbsksp_shock_graph::
init_default()
{
  // create a list of nodes associated with vertices of the polyline
  vcl_vector<dbsksp_shock_node_sptr > node_list;
  for (unsigned int i=0; i<3; ++i)
  {
    node_list.push_back(new dbsksp_shock_node(this->next_available_id()));
  }

  // list of edges
  // end-nodes have source the same as target
  vcl_vector<dbsksp_shock_edge_sptr > edge_list;
  edge_list.push_back(new dbsksp_shock_edge(node_list[0], node_list[0], this->next_available_id()));
  edge_list.push_back(new dbsksp_shock_edge(node_list[0], node_list[1], this->next_available_id()));
  edge_list.push_back(new dbsksp_shock_edge(node_list[1], node_list[2], this->next_available_id()));
  edge_list.push_back(new dbsksp_shock_edge(node_list[2], node_list[2], this->next_available_id()));

  // Now add the edges to the nodes
  for (unsigned int i=0; i<node_list.size(); ++i)
  {
    dbsksp_shock_node_sptr node = node_list[i];
    node->add_edge(edge_list[i]);
    node->add_edge(edge_list[i+1]);
  }

  // Now assign initial free variables to the nodes and edges
  // First at the nodes
  
  // node [0]
  node_list[0]->descriptor(edge_list[0])->phi = vnl_math::pi *1/6;
  node_list[0]->descriptor(edge_list[1])->phi = vnl_math::pi *5/6;

  node_list[1]->descriptor(edge_list[1])->phi = vnl_math::pi /2;
  node_list[1]->descriptor(edge_list[2])->phi = vnl_math::pi /2;

  node_list.back()->descriptor(edge_list[edge_list.size()-2])->phi = vnl_math::pi *5/6;  
  node_list.back()->descriptor(edge_list[edge_list.size()-1])->phi = vnl_math::pi *1/6;

  // Second, the edges
  edge_list[0]->set_chord_length(0);
  edge_list[0]->set_param_m(0);

  edge_list[1]->set_chord_length(100);
  edge_list[1]->set_param_m(0);

  edge_list[2]->set_chord_length(100);
  edge_list[2]->set_param_m(0);

  edge_list[3]->set_chord_length(0);
  edge_list[3]->set_param_m(0);

  // Now insert everything to the graph
  for (unsigned int i=0; i<node_list.size(); ++i)
  {
    this->add_vertex(node_list[i]);
  }

  for (unsigned int i=0; i<edge_list.size(); ++i)
  {
    this->add_edge(edge_list[i]);
  }

  // now set the reference node and edge
  this->set_ref_node(node_list[0]);
  this->set_ref_edge(edge_list[1]);
  this->set_ref_origin(vgl_point_2d<double >(20, 20));
  this->set_ref_direction(vgl_vector_2d<double >(1, 0));
  this->set_ref_node_radius(20);

  return true;
}



// ----------------------------------------------------------------------------
// DATA ACCESS
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//: Return the edge with the given id
dbsksp_shock_edge_sptr dbsksp_shock_graph::
edge_from_id(unsigned id)
{
  for (dbsksp_shock_graph::edge_iterator eit = this->edges_begin();
    eit != this->edges_end(); ++eit)
  {
    if ((*eit)->id() == id)
    {
      return (*eit);
    }
  }
  return 0;
}



// ----------------------------------------------------------------------------
//: Return the node with the given id
dbsksp_shock_node_sptr dbsksp_shock_graph::
node_from_id(unsigned id)
{
  for (dbsksp_shock_graph::vertex_iterator vit = this->vertices_begin();
    vit != this->vertices_end(); ++vit)
  {
    if ((*vit)->id() == id)
    {
      return (*vit);
    }
  }
  return 0;
}


// ----------------------------------------------------------------------------
//: Set reference node
void dbsksp_shock_graph::
set_ref_node(const dbsksp_shock_node_sptr& ref_node)
{ 
  if (this->ref_node_ == ref_node) return;
  this->ref_node_ = ref_node;
  this->set_ref_edge(*ref_node->edges_begin());

  return;
}


// ----------------------------------------------------------------------------
//: Set reference edge
bool dbsksp_shock_graph::
set_ref_edge( const dbsksp_shock_edge_sptr& ref_edge)
{
  if (this->ref_node() != ref_edge->source() && this->ref_node() != ref_edge->target())
    return false;

  this->ref_edge_ = ref_edge;
  return true;
}



// ----------------------------------------------------------------------------
//: Set all the reference parameters from an edge and a node
void dbsksp_shock_graph::
set_all_ref(const dbsksp_shock_node_sptr& ref_node,
            const dbsksp_shock_edge_sptr& ref_edge)
{
  this->set_ref_node(ref_node);
  this->set_ref_edge(ref_edge);
  
  // geometry
  this->set_ref_origin(this->ref_node()->pt());
  this->set_ref_node_radius(this->ref_node()->radius());
  this->set_ref_direction(this->ref_edge()->chord_dir(this->ref_node()));
}




// ----------------------------------------------------------------------------
// SHOCK GRAPH PROPERTIES
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//: Approximate the bounding box of the graph
// Assume all extrinsic properties have been computed.
// Run compute_all_dependent_params() if not sure.
vsol_box_2d_sptr dbsksp_shock_graph::
compute_approx_bounding_box()
{
  vsol_box_2d_sptr box = new vsol_box_2d();
  
  // iterate thru the vertices and grow the bounding box to include the circles
  // around each vertex
  for (dbsksp_shock_graph::vertex_iterator vit = this->vertices_begin();
    vit != this->vertices_end(); ++vit)
  {
    dbsksp_shock_node_sptr v = *vit;
    vgl_point_2d<double > pt = v->pt();
    double r = v->radius();

    box->add_point(pt.x()-r, pt.y()-r);
    box->add_point(pt.x()+r, pt.y()+r);
  }

  return box;
}





// ----------------------------------------------------------------------------
//: Enlarge the graph by a given scale
// scale > 1, the resulting shapelet is bigger
// scale < 1, the resulting shapelet is smaller
void dbsksp_shock_graph::
scale_up(double scale)
{
  assert(scale > 0);

  // only lengths and ref_radius are affected, all others params are not.
  this->set_ref_node_radius(scale * this->ref_node_radius());
  for (dbsksp_shock_graph::edge_iterator eit = this->edges_begin(); 
    eit != this->edges_end(); ++eit)
  {
    dbsksp_shock_edge_sptr e = *eit;
    if (e->is_terminal_edge())
      continue;
    e->set_chord_length(scale * e->chord_length());
  }
  this->compute_all_dependent_params();
  return;
}





// --------------------------------------------------------------------------
// BASIC GRAPH FUNCTIONS
// --------------------------------------------------------------------------

//: Remove a shock edge from the graph
bool dbsksp_shock_graph::
remove_shock_edge(const E_sptr& e)
{
  if (!e) return false;

  for (vcl_list<E_sptr >::iterator itr = this->edges_begin(); 
      itr != this->edges_end(); ++itr)
  {
    if ((*itr) == e)
    {
      // edge found
      e->source()->remove_shock_edge(e);
      e->target()->remove_shock_edge(e);
      itr = this->edges_.erase(itr);
      return true;
    }
  }
  return false;
}





// --------------------------------------------------------------------------
// HIGH-LEVEL GRAPH FUNCTIONS
// --------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//: Add a node in the middle of an edge without modifying the boundary geometry
// position of the new node is indicated by t, t \in [0,1]
// with t = 0: source node
// t = 1: target node
dbsksp_shock_node_sptr  dbsksp_shock_graph::
insert_shock_node(const dbsksp_shock_edge_sptr& e, double t)
{
  // special treatment for terminal edge
  if (e->is_terminal_edge()) 
    return this->insert_shock_node_at_terminal_edge(e);

  // new : one node + two edges
  // remove: one edge
  dbsksp_shock_node_sptr v0 = e->source();
  dbsksp_shock_node_sptr v2 = e->target();
  dbsksp_shock_node_sptr v1 = new dbsksp_shock_node(this->next_available_id());
  dbsksp_shock_edge_sptr e1 = new dbsksp_shock_edge(v0, v1, this->next_available_id());
  dbsksp_shock_edge_sptr e2 = new dbsksp_shock_edge(v1, v2, this->next_available_id());
  

  // set the intrinsic properties for the new edges and nodes
  if (!e->fragment()) e->form_fragment();
  dbgl_conic_arc conic = e->fragment()->shock_geom();
  v1->set_pt(conic.point_at(t));
  e1->set_chord_length(vgl_distance(v0->pt(), v1->pt()));
  vgl_vector_2d<double > chord_dir1 = v1->pt() - v0->pt();
  chord_dir1 = (chord_dir1.length() < dbsksp_epsilon) ? e->chord_dir() : chord_dir1;
  e1->set_chord_dir(chord_dir1, v0);

  e2->set_chord_length(vgl_distance(v1->pt(), v2->pt()));
  vgl_vector_2d<double > chord_dir2 = v2->pt() - v1->pt();
  chord_dir2 = (chord_dir2.length() < dbsksp_epsilon) ? e->chord_dir() : chord_dir2;
  e2->set_chord_dir(chord_dir2, v1);

  // param_m for e1
  vgl_vector_2d<double > shock_dir_v0 = rotated(e->chord_dir(v0), v0->descriptor(e)->alpha);
  double sin_alpha0 = cross_product(e1->chord_dir(v0), shock_dir_v0);
  e1->set_param_m(sin_alpha0 / vcl_sin(v0->descriptor(e)->phi), v0);

  // param_m for e2
  vgl_vector_2d<double > shock_dir_v2 = rotated(e->chord_dir(v2), v2->descriptor(e)->alpha);
  double sin_alpha2 = cross_product(e2->chord_dir(v2), shock_dir_v2);
  e2->set_param_m(sin_alpha2 / vcl_sin(v2->descriptor(e)->phi), v2);

  // phi for v1
  // There need to be a better place to do this
  dbgl_circ_arc left_bnd = e->fragment()->bnd(0)->arc();
  dbgl_circ_arc right_bnd = e->fragment()->bnd(1)->arc();

  // contact shock
  double left_ratio = -1;
  dbgl_closest_point::point_to_circular_arc(v1->pt(), 
    left_bnd.point1(), left_bnd.point2(), left_bnd.k(), left_ratio);

  // tangent of at v1
  vgl_vector_2d<double > shock_dir_v1 = conic.tangent_at(t);

  vgl_vector_2d<double > contact_shock = left_bnd.point_at(left_ratio)-v1->pt();
  double phi_v1 = angle(shock_dir_v1, contact_shock);

  // flow_dir = 1: e1 to e2, -1: e2 to e1
  int flow_dir = vnl_math_sgn(phi_v1-vnl_math::pi_over_2);


  v1->add_edge(e1);
  v1->add_edge(e2);

  v1->descriptor(e1)->phi = vnl_math::pi - phi_v1;
  v1->descriptor(e1)->shock_flow_dir = -flow_dir;
  
  v1->descriptor(e2)->phi = phi_v1;
  v1->descriptor(e2)->shock_flow_dir = flow_dir;

  // replace the old edge with the new edges
  // two places to replace: edge list and node descriptor list
  v0->descriptor(e)->edge = e1;
  dbsksp_shock_node::edge_iterator eit0 = v0->edges_begin();
  for (; *eit0 != e; ++eit0);
  *eit0 = e1;

  v2->descriptor(e)->edge = e2;
  dbsksp_shock_node::edge_iterator eit2 = v2->edges_begin();
  for (; *eit2 != e; ++eit2);
  *eit2 = e2;

  // final steps to change the graph
  this->add_vertex(v1);
  this->remove_edge(e);
  this->add_edge(e1);
  this->add_edge(e2);

  // take care of reference edge
  if (this->ref_edge()==e)
  {
    this->set_ref_edge(this->ref_node()==v0 ? e1 : e2);
    assert (this->ref_node() == this->ref_edge()->source() || 
      this->ref_node() == this->ref_edge()->target());
    this->set_ref_direction(this->ref_edge()->chord_dir(this->ref_node()));
  }

  return v1;
}


// ----------------------------------------------------------------------------
//: insert a shock node to a terminal edge
dbsksp_shock_node_sptr  dbsksp_shock_graph::
insert_shock_node_at_terminal_edge(const dbsksp_shock_edge_sptr& e)
{
  assert(e->is_terminal_edge());
  // new : one node + two edges
  // remove: one edge
  dbsksp_shock_node_sptr v0 = (e->source()->degree()==1) ? e->source() : e->target();
  dbsksp_shock_node_sptr v2 = e->opposite(v0);
  dbsksp_shock_node_sptr v1 = new dbsksp_shock_node(this->next_available_id());
  dbsksp_shock_edge_sptr e1 = new dbsksp_shock_edge(v0, v1, this->next_available_id());
  dbsksp_shock_edge_sptr e2 = new dbsksp_shock_edge(v1, v2, this->next_available_id());
  

  // set the intrinsic properties for the new edges and nodes
  if (!e->fragment()) e->form_fragment();
  //dbgl_conic_arc conic = e->fragment()->shock_geom();
  v1->set_pt(v2->pt());
  e1->set_chord_length(0);
  e1->set_chord_dir(e->chord_dir(v0), v0);

  e2->set_chord_length(0);
  e2->set_chord_dir(e->chord_dir(v0), v1);

  // param_m for e1
  e1->set_param_m(0);

  // param_m for e2
  e2->set_param_m(0);

  // phi for v1
  double phi_v1 = vnl_math::pi - v2->descriptor(e)->phi;

  // topology
  v1->add_edge(e1);
  v1->add_edge(e2);

  v1->descriptor(e1)->phi = vnl_math::pi - phi_v1;
  v1->descriptor(e1)->shock_flow_dir = -1;
  
  v1->descriptor(e2)->phi = phi_v1;
  v1->descriptor(e2)->shock_flow_dir = 1;

  // replace the old edge with the new edges
  // two places to replace: edge list and node descriptor list
  v0->descriptor(e)->edge = e1;
  dbsksp_shock_node::edge_iterator eit0 = v0->edges_begin();
  for (; *eit0 != e; ++eit0);
  *eit0 = e1;

  v2->descriptor(e)->edge = e2;
  dbsksp_shock_node::edge_iterator eit2 = v2->edges_begin();
  for (; *eit2 != e; ++eit2);
  *eit2 = e2;

  // final steps to change the graph
  this->add_vertex(v1);
  this->remove_edge(e);
  this->add_edge(e1);
  this->add_edge(e2);

  // take care of reference edge
  if (this->ref_edge()==e)
  {
    this->set_ref_edge(this->ref_node()==v0 ? e1 : e2);
    assert (this->ref_node() == this->ref_edge()->source() || 
      this->ref_node() == this->ref_edge()->target());
    this->set_ref_direction(this->ref_edge()->chord_dir(this->ref_node()));
  }
  return v1;

}




// ----------------------------------------------------------------------------
//: Insert a shock edge to the end of a given edge
// The new edge preserves the angle phi at the node
// TODO: take care of the case when `e' is the ref_edge
dbsksp_shock_edge_sptr dbsksp_shock_graph::
insert_shock_edge(dbsksp_shock_edge_sptr& e, 
                    double chord_length,
                    bool at_source,
                    double param_m)
{
  // for now: ignore when e is a reference edge
  if (e == this->ref_edge() ) return 0;

  // addition: one shock node and one shock edge
  dbsksp_shock_node_sptr v0 = (at_source ? e->source() : e->target());
  dbsksp_shock_node_sptr v1 = e->opposite(v0);
  dbsksp_shock_node_sptr new_v = this->insert_shock_node(e, (at_source ? 0 : 1));
  if (!new_v) return 0;

  assert(new_v->degree() == 2);
  // find the newly added edge
  dbsksp_shock_edge_sptr new_e = 0;
  for (dbsksp_shock_node::edge_iterator eit = new_v->edges_begin();
    eit != new_v->edges_end(); ++eit)
  {
    if (v0==(*eit)->source() || v0 == (*eit)->target())
    {
      new_e = *eit;
    }
    else
    {
      // the old edge has been deleted in the "insert_shock_node" function.
      // Update it with a proper one.
      e = *eit;
    }
  }
 
  assert(new_e);

  new_e->set_chord_length(chord_length);
  new_e->set_param_m(param_m, new_v);
  return new_e;
}





// ----------------------------------------------------------------------------
//: Add an A_infty branch to an existing A_1^2 node and convert it to A_1^3
dbsksp_shock_edge_sptr dbsksp_shock_graph::
insert_A_infty_branch(const dbsksp_shock_node_sptr& node,
                   const dbsksp_shock_edge_sptr& ref_edge)
{
  // preliminary check
  if (!ref_edge->is_vertex(node)) return false;

  // only handle degree-2 node right now
  if (node->degree() != 2) 
  {
    vcl_cerr << 
      "ERROR: add_A_infty_branch currently can only handle degree-2 nodes.\n";
    return false;
  }

  // topology
  dbsksp_shock_node_sptr v0 = new dbsksp_shock_node(this->next_available_id());
  dbsksp_shock_edge_sptr e0 = 
    new dbsksp_shock_edge(v0, node, this->next_available_id());
  v0->add_edge(e0);


  // parameters for new node
  v0->descriptor(e0)->phi = vnl_math::pi;
  v0->descriptor(e0)->alpha = 0;
  v0->descriptor(e0)->shock_flow_dir = 1;
  v0->descriptor(e0)->rot_angle_to_succ_edge = 2*vnl_math::pi;

  // parameters for new edge
  e0->set_chord_length(0);
  e0->set_param_m(0);


  // parameter for ``node" wrt to e0
  dbsksp_shock_node_descriptor_sptr descriptor = 
    node->insert_shock_edge(e0, ref_edge);
  descriptor->alpha = 0;
  descriptor->phi = 0;
  descriptor->shock_flow_dir = -1;
  descriptor->rot_angle_to_succ_edge = node->descriptor(ref_edge)->phi;

  this->add_vertex(v0);
  this->add_edge(e0);
    
  return e0;
}




//: Remove an A_infty edge (with very small phi) from a node with 
// degree n >= 3
// Use when the edge is denegerate, i.e. the angle phi of the branch is very small, 
// the boundary is simply a point
// Return the opposite node to the terminal (degree-one) node
dbsksp_shock_node_sptr dbsksp_shock_graph::
remove_A_infty_edge(const dbsksp_shock_node_sptr& degree_one_node)
{
  if (! degree_one_node)
    return 0;
  
  if (degree_one_node->degree() != 1)
    return 0;

  // Collect basic info
  dbsksp_shock_edge_sptr e0 = *degree_one_node->edges_begin();
  dbsksp_shock_node_sptr v0 = e0->opposite(degree_one_node);

  assert(v0);

  dbsksp_shock_node_descriptor_sptr de0 = v0->descriptor(e0);

  // Get the edges right before and after e0 in the edge list of v0
  dbsksp_shock_edge_sptr e0_succ = this->cyclic_adj_succ(e0, v0);
  dbsksp_shock_edge_sptr e0_pred = this->cyclic_adj_pred(e0, v0);

  dbsksp_shock_node_descriptor_sptr de0_succ = v0->descriptor(e0_succ);
  dbsksp_shock_node_descriptor_sptr de0_pred = v0->descriptor(e0_pred);

  // Change connectivity
  this->remove_shock_edge(e0);
  this->remove_vertex(degree_one_node);

  // Modify the descriptors and edges adjacent to the A_infty edge
  de0_succ->phi += de0->phi/2;
  de0_pred->phi += de0->phi/2;

  return v0;
}







// ----------------------------------------------------------------------------
//: Squeeze a (short) shock edge to a point and convert it to two terminal edges
dbsksp_shock_node_sptr dbsksp_shock_graph::
squeeze_shock_edge(const dbsksp_shock_edge_sptr& e,
                   dbsksp_shock_edge_sptr& new_terminal_edge_left,
                   dbsksp_shock_edge_sptr& new_terminal_edge_right)
{
  dbsksp_shock_node_sptr source = e->source();
  dbsksp_shock_node_sptr target = e->target();

  // 1. Check for necessary conditions to squeeze the edge
  // The conditions are the contact shocks angle at target need to be "larger"
  // than those at source
  dbsksp_shock_node_descriptor_sptr source_e = source->descriptor(e);
  dbsksp_shock_node_descriptor_sptr target_e = target->descriptor(e);

  vgl_vector_2d<double > contact_source_left = rotated(e->chord_dir(source),
    source_e->alpha + source_e->phi);
  vgl_vector_2d<double > contact_source_right = rotated(e->chord_dir(source),
    source_e->alpha - source_e->phi);

  vgl_vector_2d<double > contact_target_left = rotated(e->chord_dir(target),
    target_e->alpha - target_e->phi);
  vgl_vector_2d<double > contact_target_right = rotated(e->chord_dir(target),
    target_e->alpha + target_e->phi);

  // This may cause problem in other functions that uses it. Ignore for now
  //// ignore if necessary conditions fail
  //if (signed_angle(contact_source_left, contact_target_left) > 0 ||
  //  signed_angle(contact_source_right, contact_target_right) < 0)
  //{
  //  return 0;
  //}

  // 2. Create two terminal edges, associated with the two gaps between the 
  // contact shocks

  // a. left terminal edge
  dbsksp_shock_node_sptr v_left = new dbsksp_shock_node(this->next_available_id());
  dbsksp_shock_edge_sptr e_left = 
    new dbsksp_shock_edge(v_left, source, this->next_available_id());
  v_left->add_edge(e_left);

  // parameters for new terminal node
  v_left->descriptor(e_left)->phi = vnl_math::pi;
  v_left->descriptor(e_left)->alpha = 0;
  v_left->descriptor(e_left)->shock_flow_dir = 1;
  v_left->descriptor(e_left)->rot_angle_to_succ_edge = 2*vnl_math::pi;

  // parameters for new edge
  e_left->set_chord_length(0);
  e_left->set_param_m(0);


  // b. right terminal edge
  dbsksp_shock_node_sptr v_right = new dbsksp_shock_node(this->next_available_id());
  dbsksp_shock_edge_sptr e_right = 
    new dbsksp_shock_edge(v_right, source, this->next_available_id());
  v_right->add_edge(e_right);

  // parameters for new terminal node
  v_right->descriptor(e_right)->phi = vnl_math::pi;
  v_right->descriptor(e_right)->alpha = 0;
  v_right->descriptor(e_right)->shock_flow_dir = 1;
  v_right->descriptor(e_right)->rot_angle_to_succ_edge = 2*vnl_math::pi;

  // parameters for new edge
  e_right->set_chord_length(0);
  e_right->set_param_m(0);


  // 2. Move everything from target to source and return source
  dbsksp_shock_edge_sptr front_edge = this->cyclic_adj_succ(e, source);
  source->remove_shock_edge(e);
  
  // left terminal edge
  if (!source->insert_shock_edge(e_left, front_edge))
  {
    vcl_cout << "ERROR: Could not insert left terminal edge.\n";
    return false;
  };
  
  dbsksp_shock_node_descriptor_sptr dleft = source->descriptor(e_left);
  // parameters for new terminal edge
  dleft->phi = signed_angle(contact_target_left, contact_source_left)/2;
  dleft->alpha = 0;
  dleft->shock_flow_dir = -1;
  dleft->rot_angle_to_succ_edge = dleft->phi + 
    source->descriptor(front_edge)->phi;
  
  // update front edge
  front_edge = e_left;
      
  // list of edges taken from target node which will be added to source node
  vcl_list<dbsksp_shock_edge_sptr > tomove_edges;
  dbsksp_shock_edge_sptr e2 = this->cyclic_adj_succ(e, target);
  while (e2 != e)
  {
    tomove_edges.push_back(e2);
    e2 = this->cyclic_adj_succ(e2, target);
  }

  // insert them to source
  for (vcl_list<dbsksp_shock_edge_sptr >::reverse_iterator itr = tomove_edges.rbegin();
    itr != tomove_edges.rend(); ++itr)
  {
    source->insert_shock_edge(*itr, front_edge);
    *(source->descriptor(*itr)) = *(target->descriptor(*itr));
    source->descriptor(*itr)->rot_angle_to_succ_edge = 
      source->descriptor(*itr)->phi + source->descriptor(front_edge)->phi;

    front_edge = *itr;
  }

  // right terminal edge
  source->insert_shock_edge(e_right, front_edge);

  // parameters for new terminal edge
  dbsksp_shock_node_descriptor_sptr dright = source->descriptor(e_right);
  dright->phi = signed_angle(contact_source_right, contact_target_right)/2;
  dright->alpha = 0;
  dright->shock_flow_dir = -1;
  dright->rot_angle_to_succ_edge = dright->phi + 
    source->descriptor(front_edge)->phi;

  front_edge = e_right;


  // replace 'target' with source in the edges
  for (vcl_list<dbsksp_shock_edge_sptr >::reverse_iterator itr = tomove_edges.rbegin();
    itr != tomove_edges.rend(); ++itr)
  {
    if (target == (*itr)->source())
    {
      (*itr)->set_source(source);
    }
    else
    {
      (*itr)->set_target(source);
    }
  }

  
  // Finally, modify the shock graph
  this->remove_shock_edge(e);
  // remove all the edges from target
  target->remove_all_shock_edges();
  
  this->remove_vertex(target);
  this->add_vertex(v_left);
  this->add_vertex(v_right);
  this->add_edge(e_left);
  this->add_edge(e_right);

  // return terminal edges and node
  new_terminal_edge_left = e_left;
  new_terminal_edge_right = e_right;

  return source;

}


//: Squeeze a (short) shock edge, but no interest in the new terminal edges created
dbsksp_shock_node_sptr dbsksp_shock_graph::
squeeze_shock_edge(const dbsksp_shock_edge_sptr& e)
{
  dbsksp_shock_edge_sptr e_left;
  dbsksp_shock_edge_sptr e_right;

  return this->squeeze_shock_edge(e, e_left, e_right);
}




//: Remove a leaf A_1^2 edge, the A_1^2 edge connected to only terminal edges
dbsksp_shock_node_sptr dbsksp_shock_graph::
remove_leaf_A_1_2_edge(const dbsksp_shock_edge_sptr& leaf_edge)
{
  // Algorithm: first we squeeze the edge down to a point, which creates two
  // terminal edges
  // Then we remove the two terminal edges

  // 1) Check if this is a leaf edge
  if (!leaf_edge->source()->connected_to_terminal_edge() && 
    !leaf_edge->target()->connected_to_terminal_edge())
  {
    // This is not a leaf edge
    return 0;
  }

  // 2) Adjust the phi's angle so that phi's at both sides of the edge are the same
  dbsksp_shock_node_sptr leaf_node = 0;
  dbsksp_shock_node_sptr non_leaf_node = 0;
  if (leaf_edge->source()->connected_to_terminal_edge())
  {
    leaf_node = leaf_edge->source();
    non_leaf_node = leaf_edge->target();
  }
  else
  {
    non_leaf_node = leaf_edge->source();
    leaf_node = leaf_edge->target();
  }

  double non_leaf_phi = non_leaf_node->descriptor(leaf_edge)->phi;
  leaf_node->change_phi_distribute_difference_uniformly(leaf_edge, 
    vnl_math::pi - non_leaf_phi + 1e-8);

  // 3) Squeeze the edge down to a point. This results in two degenerate terminal edge
  dbsksp_shock_edge_sptr new_terminal_edge_left;
  dbsksp_shock_edge_sptr new_terminal_edge_right;

  non_leaf_node = this->squeeze_shock_edge(leaf_edge, new_terminal_edge_left, new_terminal_edge_right);

  if (!non_leaf_node)
  {
    vcl_cout << "ERROR: in " __FILE__ " error squeezing the leaf edge.\n";
  }

  // 4) remove the degenerate terminal edge
  this->remove_A_infty_edge(new_terminal_edge_left->opposite(non_leaf_node));
  leaf_node = this->remove_A_infty_edge(new_terminal_edge_right->opposite(non_leaf_node));

  return leaf_node;
}



// ----------------------------------------------------------------------------
//: Remove internal edge
// The phi's at two sides will be averaged and distributed evenly
dbsksp_shock_node_sptr dbsksp_shock_graph::
remove_internal_edge(const dbsksp_shock_edge_sptr& e)
{
  // Algorithm: first we squeeze the edge down to a point, which creates two
  // terminal edges
  // Then we remove the two terminal edges

  // 1) Make sure this is not a leaf edge
  if (e->source()->connected_to_terminal_edge() ||
    e->target()->connected_to_terminal_edge())
  {
    // This is a leaf edge
    return 0;
  }

  // 2) Adjust the phi's angle so that phi's at both sides of the edge are the same
  double phi_source = e->source()->descriptor(e)->phi;
  double phi_target = vnl_math::pi - e->target()->descriptor(e)->phi;

  double mean_phi = (phi_source + phi_target) / 2;
  
  e->source()->change_phi_distribute_difference_uniformly(e, mean_phi);
  e->target()->change_phi_distribute_difference_uniformly(e, vnl_math::pi - mean_phi);

  // 3) Squeeze the edge down to a point. This results in two degenerate terminal edge
  dbsksp_shock_edge_sptr new_terminal_edge_left;
  dbsksp_shock_edge_sptr new_terminal_edge_right;

  dbsksp_shock_node_sptr node = 
    this->squeeze_shock_edge(e, new_terminal_edge_left, new_terminal_edge_right);

  if (!node)
  {
    vcl_cout << "ERROR: in " __FILE__ " error squeezing the leaf edge.\n";
  }

  // 4) remove the degenerate terminal edge
  this->remove_A_infty_edge(new_terminal_edge_left->opposite(node));
  return this->remove_A_infty_edge(new_terminal_edge_right->opposite(node));
}







//: Split an (degree > 3) node into two nodes and insert an edge between them
// Typical used in A_1^4 transitions
// The edges incident to the node will be divided into two groups, each
// stays with one node. The user gives and start and end edge of one group,
// inclusively.
// Return a pointer to the new edge
dbsksp_shock_edge_sptr dbsksp_shock_graph::
split_shock_node(const dbsksp_shock_node_sptr& node, 
                 double chord_length,
                 const dbsksp_shock_edge_sptr& start_edge, 
                 const dbsksp_shock_edge_sptr& end_edge)
{
  // Preliminary checks
  if (!node || !start_edge || !end_edge) return 0;
  
  if (node->degree() < 4)
    return 0;

  if (!start_edge->is_vertex(node) || !end_edge->is_vertex(node))
    return 0;

  // Determine the groups
  vcl_vector<dbsksp_shock_edge_sptr > edge_group_1;
  dbsksp_shock_edge_sptr start_group_2 = this->cyclic_adj_succ(end_edge, node);

  for ( dbsksp_shock_edge_sptr e = start_edge; e != start_group_2;
    e = this->cyclic_adj_succ(e, node) )
  {
    edge_group_1.push_back(e);
  }

  vcl_vector<dbsksp_shock_edge_sptr > edge_group_2;
  for ( dbsksp_shock_edge_sptr e = start_group_2; e != start_edge;
    e = this->cyclic_adj_succ(e, node) )
  {
    edge_group_2.push_back(e);
  }

  if (edge_group_1.size() < 2 || edge_group_2.size() < 2)
  {
    vcl_cerr << "One of the two groups has less than 2 edges.\n";
    return 0;
  }

  // Create a new vertex and a new edge
  dbsksp_shock_node_sptr new_node = new dbsksp_shock_node(this->next_available_id());
  dbsksp_shock_edge_sptr new_edge = new dbsksp_shock_edge(node, new_node, 
    this->next_available_id());
  new_edge->set_chord_length(chord_length);
  new_edge->set_param_m(0);

  // Edge group 1 will stay with ``node" and edge group 2 will be moved to ``new_node"

  // Move the edes in group 2 to new_node
  double phi_sum = 0;
  for (unsigned i=0; i<edge_group_2.size(); ++i)
  {
    dbsksp_shock_edge_sptr e = edge_group_2[i];
    
    // put e into new_node
    new_node->add_edge(e);
    *(new_node->descriptor(e)) = *(node->descriptor(e));
    phi_sum += new_node->descriptor(e)->phi;

    // put new_node into e
    if (node == e->source())
    {
      e->set_source(new_node);
    }
    else if (node == e->target())
    {
      e->set_target(new_node);
    }

    // disconnect e from node
    node->remove_shock_edge(e);
  }

  // add the new edge to both ``node" and ``new_node"

  // ``new node"
  new_node->add_edge(new_edge);
  new_node->descriptor(new_edge)->phi = vnl_math::pi - phi_sum;
  new_node->descriptor(new_edge)->shock_flow_dir = 
    (new_node->descriptor(new_edge)->phi > vnl_math::pi_over_2) ? 1 : -1;

  // ``node"
  node->add_edge(new_edge);
  node->descriptor(new_edge)->phi = 
    vnl_math::pi - new_node->descriptor(new_edge)->phi; 
  node->descriptor(new_edge)->shock_flow_dir = 
    (node->descriptor(new_edge)->phi > vnl_math::pi_over_2) ? 1 : -1;

  // lastly add the new_node and new_vertex to the graph
  this->add_vertex(new_node);
  this->add_edge(new_edge);

  return new_edge;
}

















// ============= LEGALITY CHECK ==========================================

//: Check the legality (generating a simple smooth contour) of the shock graph
bool dbsksp_shock_graph::
is_legal()
{
  // radius > 0
  bool min_radius_check = true;
  bool shock_flow_check = true;
  for (dbsksp_shock_graph::vertex_iterator vit = this->vertices_begin(); 
    vit != this->vertices_end(); ++vit)
  {
    min_radius_check = min_radius_check && ((*vit)->radius() > 0);
    vcl_list<dbsksp_shock_node_descriptor_sptr > desc_list = (*vit)->descriptor_list();
    for (vcl_list<dbsksp_shock_node_descriptor_sptr >::iterator it = desc_list.begin();
      it != desc_list.end(); ++it)
    {
      shock_flow_check = shock_flow_check &&
        (((*it)->shock_flow_dir * ((*it)->phi-vnl_math::pi_over_2)) >= 0);
    }
    
  }

  // lengths of shock chord >=0
  bool chord_length_check = true;
  bool sin_alpha_check = true;
  bool max_radius_check = true;
  

  for (dbsksp_shock_graph::edge_iterator eit = this->edges_begin();
    eit != this->edges_end(); ++eit)
  {
    dbsksp_shock_edge_sptr e = *eit;

    // L > = 0
    chord_length_check = chord_length_check && 
      (e->chord_length() >= 0);

    // m * sin(phi) >= 0
    sin_alpha_check = sin_alpha_check &&
      (vnl_math_abs(e->param_m()* vcl_sin(e->source()->descriptor(e)->phi)) <= 1) && 
      (vnl_math_abs(e->param_m()* vcl_sin(e->target()->descriptor(e)->phi)) <= 1);

    // max radius check
    dbsksp_shock_node_descriptor_sptr desc_start = e->source()->descriptor(e);
    dbsksp_shock_node_descriptor_sptr desc_end = e->target()->descriptor(e);

    double theta_start_left = desc_start->phi + desc_start->alpha;
    double theta_end_left = desc_end->phi -desc_end->alpha ;
    double theta_midpt_left = vnl_math::pi - theta_start_left - theta_end_left;
    
    // R1 * sin(pi - theta1 - theta2) < d * sin(theta2)
    bool left_side_check = (e->source()->radius()*vcl_sin(theta_midpt_left) <= 
      e->chord_length() * vcl_sin(theta_end_left));

    double theta_start_right = desc_start->phi - desc_start->alpha;
    double theta_end_right = desc_end->phi + desc_end->alpha;
    double theta_midpt_right = vnl_math::pi - theta_start_right - theta_end_right;
    
    bool right_side_check = e->source()->radius()*vcl_sin(theta_midpt_right) <=
      e->chord_length() * vcl_sin(theta_end_right);

    max_radius_check = max_radius_check && left_side_check && right_side_check;
  }
  
  return min_radius_check && 
    //shock_flow_check && 
    chord_length_check && 
    sin_alpha_check &&
    max_radius_check;
}


// ============= UTILITY ==================================================



// ----------------------------------------------------------------------------
//: Using the reference node and reference edge to compute the extrinsic
// coodinates of the nodes
void dbsksp_shock_graph::
compute_coordinates_for_all_nodes()
{
  // make sure ref_edge is incident to ref_node
  assert(this->ref_node()==this->ref_edge()->source() ||
    this->ref_node() == this->ref_edge()->target());

  // recursive algorithm
  this->ref_node()->set_pt(this->ref_origin());
  this->ref_edge()->set_chord_dir(this->ref_direction(), this->ref_node());

  // increase traversal flag
  this->init_traversal();

  // recursively propage ...
  this->propage_extrinsic_geometry_from(this->ref_edge(), 
    this->ref_node(),
    this->i_traverse_flag());

  return;
}



//: Compute dependent parameters at A12 nodes
void dbsksp_shock_graph::
compute_dependent_params_at_A12_nodes()
{
  for (dbsksp_shock_graph::vertex_iterator vit = this->vertices_begin();
    vit != this->vertices_end(); ++vit)
  {
    dbsksp_shock_node_sptr node = (*vit);
    assert(node);

    if (node->degree()==2)
    {
      dbsksp_shock_node_algos::compute_params_at_A12_node_given_2_m_and_1_phi(node);
    }
  }
}




// ----------------------------------------------------------------------------
//: Compute dependent parameters at A13 nodes
void dbsksp_shock_graph::
compute_dependent_params_at_A13_nodes()
{
  for (dbsksp_shock_graph::vertex_iterator vit = this->vertices_begin();
    vit != this->vertices_end(); ++vit)
  {
    dbsksp_shock_node_sptr node = (*vit);
    assert(node);

    if (node->degree()==3)
    {
      dbsksp_shock_node_algos::compute_params_at_A13_node_given_3_m_and_2_phi(node);
    }
  }
  return;
}


// ----------------------------------------------------------------------------
//: Compute dependent parameters at A1n, n>3, nodes
void dbsksp_shock_graph::
compute_dependent_params_at_A1nGT3_nodes()
{
  for (dbsksp_shock_graph::vertex_iterator vit = this->vertices_begin();
    vit != this->vertices_end(); ++vit)
  {
    dbsksp_shock_node_sptr node = (*vit);
    assert(node);

    if (node->degree()>3)
    {
      dbsksp_shock_node_algos::
        compute_params_at_A1nGT3_node_given_n_m_and_n_minus_1_phi(node);
    }
  }
  return;

}



// ----------------------------------------------------------------------------
//: Compute radius increment at all edges
void dbsksp_shock_graph::
compute_radius_increment_for_all_edges()
{
  for (dbsksp_shock_graph::edge_iterator eit = this->edges_begin();
    eit != this->edges_end(); ++eit)
  {
    dbsksp_shock_edge_sptr edge = (*eit);
    assert(edge);

    edge->compute_radius_increment_from_nodes();
  }
  return;
}



// ----------------------------------------------------------------------------
//: Using the reference node and reference edge to compute radius at all nodes
void dbsksp_shock_graph::
compute_radius_for_all_nodes()
{
  // recursive algorithm
  this->ref_node()->set_radius(this->ref_node_radius());
  
  // increase traversal flag
  this->init_traversal();

  // recursively propage ...
  this->propage_radius_from(this->ref_node(), this->i_traverse_flag());

  return;
}

// ----------------------------------------------------------------------------
//: Form visual fragments at all edges
void dbsksp_shock_graph::
form_fragment_for_all_edges()
{
  for (dbsksp_shock_graph::edge_iterator eit = this->edges_begin();
    eit != this->edges_end(); ++eit)
  {
    assert(*eit);
    (*eit)->form_fragment();
  }

}



// ----------------------------------------------------------------------------
//: Compute all dependent params
void dbsksp_shock_graph::
compute_all_dependent_params()
{
  this->compute_dependent_params_at_A12_nodes();
  this->compute_dependent_params_at_A13_nodes();
  this->compute_dependent_params_at_A1nGT3_nodes();
  this->compute_coordinates_for_all_nodes();
  this->compute_radius_increment_for_all_edges();
  this->compute_radius_for_all_nodes();
  this->form_fragment_for_all_edges();

  // rooted tree structure
  this->compute_vertex_depths();


  return;
}



// ============= MISCELLANEOUS =============================================

// ---------------------------------------------------------------------------- 
//: print info of the shock graph to an output stream
void dbsksp_shock_graph::
print(vcl_ostream & os)
{
  os << "Type< " << this->is_a() << ">\n"
    << "# nodes< " << this->number_of_vertices() << " >\n";
  for (dbsksp_shock_graph::vertex_iterator vit= this->vertices_begin();
    vit != this->vertices_end(); ++vit)
  {
    (*vit)->print(os);
  }

  os << "\n #edges< " << this->number_of_edges() << " >\n";
  for (dbsksp_shock_graph::edge_iterator eit = this->edges_begin();
    eit != this->edges_end(); ++eit)
  {
    (*eit)->print(os);
  }

  os << "\nGlobal info\n";
  os << "Reference node id < " << this->ref_node()->id() << " >\n"
    << "Reference node coord< (" << this->ref_origin().x() 
    << " , " << this->ref_origin().y() << ") >\n"
    << "Reference edge id < " << this->ref_edge()->id() << " >\n"
    << "Reference edge dir " << this->ref_direction() << "\n"
    << "Refeerence radius < " << this->ref_node_radius() << " >\n";

}




// ----------------------------------------------------------------------------
//: Trace out the boundary of the shock graph
// For now, represented as a vector of polyline, each polyline
// is a circular arc segment
vcl_vector<vsol_spatial_object_2d_sptr > dbsksp_shock_graph::
trace_boundary()
{
  vcl_vector<vsol_point_2d_sptr > pt_list;

  // Euler tour on the shock graph
  dbsksp_shock_node_sptr v0 = *(this->vertices_begin());
  dbsksp_shock_edge_sptr e0 = *(v0->edges_begin());

  dbsksp_shock_node_sptr cur_node = v0;
  dbsksp_shock_edge_sptr cur_edge = e0;
  do
  {
    assert (cur_edge->fragment());
    
    dbgl_circ_arc arc;
    
    // always take the right arc
    if (cur_node == cur_edge->source())
    {
      dbsksp_bnd_arc_sptr bnd = cur_edge->fragment()->bnd(1);
      arc = bnd->arc();
    }
    else
    {
      dbsksp_bnd_arc_sptr bnd = cur_edge->fragment()->bnd(0);
      arc = bnd->arc().reversed_arc();
    }

    // an arc as a 11-vertex polyline
    for (double t=0; t<1; t = t+0.1)
    {
      vgl_point_2d<double > pt = arc.point_at(t);
      pt_list.push_back(new vsol_point_2d(pt));
    }

    // move to the next edge
    cur_node = cur_edge->opposite(cur_node);
    cur_edge = this->cyclic_adj_succ(cur_edge, cur_node);
  }
  while (cur_node != v0 || cur_edge != e0);

  vcl_vector<vsol_spatial_object_2d_sptr > vsol_list;
  vsol_list.push_back(new vsol_polygon_2d(pt_list));
  return vsol_list;
}



// ----------------------------------------------------------------------------
//: Recursive function to propagate extrinsic geometry from a node and an edge
// Requirement: edge is incident to node
// Note: run this->init_traversal() before this function
// and then set i_traverse_flag to this->i_traverse_flag();
void dbsksp_shock_graph::
propage_extrinsic_geometry_from(const dbsksp_shock_edge_sptr& ref_edge,
                                const dbsksp_shock_node_sptr& ref_node,
                                unsigned int i_traverse_flag)
{
  // Algorithm: starting with e0, v0
  // - Mark e0 and v0 as visited
  // - Use e0 to compute directions of all adjacent edges of v0
  // - For each adjacent edge e1 of v0 (including e0)
  //      + determine opposite node v1 of v0
  //      + skip visited nodes
  //      + for un-visited nodes,
  //         ++ compute v1's coordinates
  //         ++ propapgate using (e1, v1)
  
  // This algorithm should work well for a tree
  // The problem is over-constrained for a graph with loop

  // ////////////////////////////////////////
  ref_edge->set_i_visited(i_traverse_flag);
  ref_node->set_i_visited(i_traverse_flag);
  // ////////////////////////////////////////
  

  dbsksp_shock_node_sptr cur_node = ref_node;
  dbsksp_shock_edge_sptr cur_edge = ref_edge;
  dbsksp_shock_edge_sptr next_edge = 0;

  // cycle edges adjacent to the node to set their directions
  while ((next_edge = this->cyclic_adj_succ(cur_edge, ref_node)) != ref_edge)
  {
    // terminate loop when hitting visited edges (happens at A1-Ainfinity or when loop occurs)
    if (next_edge->is_visited(i_traverse_flag)) continue;

    

    // mark next_edge as visisted
    next_edge->set_i_visited(i_traverse_flag);

    // direction of cur_edge
    vgl_vector_2d<double > v0 = cur_edge->chord_dir(cur_node);
    double rot_angle = cur_node->descriptor(cur_edge)->rot_angle_to_succ_edge;


    // rotatate v0 to get direction of next edge's chord
    next_edge->set_chord_dir(rotated(v0, rot_angle), cur_node);

    // move on to the next adjacent edge
    cur_edge = next_edge;
  }

  // Now we can recursively use edges incident to cur_node as reference to propagate the geometry
  cur_edge = ref_edge;
  do
  {
    dbsksp_shock_node_sptr next_node = cur_edge->opposite(ref_node);
    
    // skip visited nodes
    if (!next_node->is_visited(i_traverse_flag))
    {

      // for others, compute its geometry and propagate
      next_node->set_pt(ref_node->pt() + 
        cur_edge->chord_dir(ref_node) * cur_edge->chord_length());

      this->propage_extrinsic_geometry_from(cur_edge, next_node, i_traverse_flag);
    }

    cur_edge = this->cyclic_adj_succ(cur_edge, ref_node);
  }
  while (cur_edge != ref_edge);
  return;
}



// ----------------------------------------------------------------------------

//: propagate radius from a node
void dbsksp_shock_graph::
propage_radius_from(const dbsksp_shock_node_sptr& ref_node,
  unsigned int i_traverse_flag)
{
// Algorithm: starting with v0
  // - Mark v0 as visited
  // - Use v0 to compute radius at all adjacent nodes of v0 (skip visited nodes)
  // - For each unvisited adjacent node v2 of v0
  //         ++ propapgate using v2
  
  // This algorithm should work well for a tree
  // The problem is over-constrained for a graph with loop

  // ////////////////////////////////////////
  ref_node->set_i_visited(i_traverse_flag);
  // ////////////////////////////////////////

  // compute radius of nodes adjacent to ref_node
  for (dbsksp_shock_node::edge_iterator eit = ref_node->edges_begin();
    eit != ref_node->edges_end(); ++eit)
  {
    dbsksp_shock_edge_sptr edge = (*eit);
    dbsksp_shock_node_sptr node = edge->opposite(ref_node);

    if (node->is_visited(i_traverse_flag)) continue;

    double delta_r = edge->radius_increment()*(ref_node == edge->source() ? 1:-1);
    node->set_radius(ref_node->radius() + delta_r);

    // recursively propage radius from the adjacent nodes of ref_node
    this->propage_radius_from(node, i_traverse_flag);
  }
  return;
}



//: Compute depths and parent-child relationship
void dbsksp_shock_graph::
compute_vertex_depths()
{
  this->root()->set_parent_edge(0);
  this->compute_subtree_vertex_depths(this->root(), 0);
  return;
}



//: Assign vertex depths for a subtree whose root is root_depth
void dbsksp_shock_graph::
compute_subtree_vertex_depths(const dbsksp_shock_node_sptr& root, int root_depth)
{
  root->set_depth(root_depth);

  for (edge_iterator eit = root->edges_begin(); eit != root->edges_end(); ++eit)
  {
    E_sptr e = *eit;
    
    // we only want to consider the child vertices
    if (e == root->parent_edge())
      continue;

    V_sptr v = e->opposite(root);
    v->set_parent_edge(e);
    this->compute_subtree_vertex_depths(v, root_depth+1);
  }
  
  return;
}


