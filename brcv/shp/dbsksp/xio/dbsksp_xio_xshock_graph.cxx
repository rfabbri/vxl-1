// This is dbsksp/xio/dbsksp_xio_xshock_graph.cxx


#include "dbsksp_xio_xshock_graph.h"

#include <bxml/bxml_read.h>
#include <bxml/bxml_write.h>
#include <bxml/bxml_find.h>

#include <dbsksp/dbsksp_xshock_graph.h>
#include <dbsksp/dbsksp_xshock_node.h>
#include <dbsksp/dbsksp_xshock_edge.h>

#include <vnl/vnl_math.h>
#include <vcl_utility.h>
#include <vcl_fstream.h>




//: Convert an angle to [-pi, pi] range
double dbsksp_xio_angle_to_npi2pi(double angle)
{
  double a = vcl_fmod(angle + vnl_math::pi, vnl_math::pi*2);
  return (a>0) ? (a - vnl_math::pi) : (a + vnl_math::pi);
}



//: enumerate possible tags in a xshock graph
enum dbsksp_xio_xshock_tag_index
{
  VERSION,
  SHOCK_GRAPH,
  SHOCK_NODE_LIST,
  SHOCK_NODE,
  ID,
  X,
  Y,
  RADIUS,
  SHOCK_NODE_DESCRIPTOR_LIST,
  SHOCK_NODE_DESCRIPTOR,
  PHI,
  SHOCK_EDGE_LIST,
  SHOCK_EDGE,
  SOURCE_ID,
  TARGET_ID,
  EDGE_ID,
  SHOCK_TANGENT_ANGLE
};


//: look-up table for all the tag labels
class dbsksp_xio_xshock_tag_list : public vcl_map<dbsksp_xio_xshock_tag_index, vcl_string >
{
public:
  dbsksp_xio_xshock_tag_list()
  {
    (*this)[VERSION] = "version";
    (*this)[SHOCK_GRAPH] = "sksp_xshock_graph";
    (*this)[SHOCK_NODE_LIST] = "shock_node_list";
    (*this)[SHOCK_NODE] = "shock_node";
    (*this)[ID] = "id",
    (*this)[X] = "x";
    (*this)[Y] = "y";
    (*this)[RADIUS] = "radius";
    (*this)[SHOCK_NODE_DESCRIPTOR_LIST] = "shock_node_descriptor_list";
    (*this)[SHOCK_NODE_DESCRIPTOR] = "shock_node_descriptor";
    (*this)[PHI] = "phi";
    (*this)[SHOCK_EDGE_LIST] = "shock_edge_list";
    (*this)[SHOCK_EDGE] = "shock_edge";
    (*this)[SOURCE_ID] = "source_id";
    (*this)[TARGET_ID] = "target_id";
    (*this)[EDGE_ID] = "edge_id";
    (*this)[SHOCK_TANGENT_ANGLE] = "shock_tangent_angle";
  }
};

static dbsksp_xio_xshock_tag_list dbsksp_xio_xshock_tag;



// ============================================================================
// IMPLEMENTATION OF X_WRITE and X_READ
// ============================================================================


// ----------------------------------------------------------------------------
//: write a shock graph to an xml file
bool x_write(const vcl_string& filepath, const dbsksp_xshock_graph_sptr& graph)
{
  vcl_ofstream file(filepath.c_str());
  if (x_write(file, graph))
  {
    file.close();
    return true;
  }
  else
  {
    file.close();
    return false;
  }
}


// ----------------------------------------------------------------------------
//: write a shock graph to a stream
bool x_write(vcl_ostream& os, const dbsksp_xshock_graph_sptr& graph)
{
  // set the accuracy to double-precision
  //int current_precision = os.precision();
  

  bxml_document doc;
  bxml_element *root = new bxml_element(dbsksp_xio_xshock_tag[SHOCK_GRAPH]); // "sksp_xshock_graph"
  doc.set_root_element(root);

  // Version
  root->set_attribute(dbsksp_xio_xshock_tag[VERSION], "1");

  // I. Node list
  bxml_element *node_list_elm = new bxml_element(dbsksp_xio_xshock_tag[SHOCK_NODE_LIST]); // "shock_node_list"
  root->append_data(node_list_elm);

  for (dbsksp_xshock_graph::vertex_iterator vit = graph->vertices_begin(); 
    vit != graph->vertices_end(); ++vit)
  {
    dbsksp_xshock_node_sptr v = *vit;
    bxml_element* node_elm = new bxml_element(dbsksp_xio_xshock_tag[SHOCK_NODE]); // "shock_node"
    node_list_elm->append_data(node_elm);

    // 1. id as attribute
    node_elm->set_attribute(dbsksp_xio_xshock_tag[ID], v->id()); // "id"
    
    // 2. point as attribute

    // x-coordinate
    node_elm->set_attribute(dbsksp_xio_xshock_tag[X], v->pt().x()); // "x"
    
    // y-coordinate
    node_elm->set_attribute(dbsksp_xio_xshock_tag[Y], v->pt().y()); // "y"

    // 3. radius
    node_elm->set_attribute(dbsksp_xio_xshock_tag[RADIUS], v->radius()); // "radius"

    // 4. Descriptor list
    bxml_element* descriptor_list_elm = new bxml_element(
      dbsksp_xio_xshock_tag[SHOCK_NODE_DESCRIPTOR_LIST]); // "shock_node_descriptor_list"
    node_elm->append_data(descriptor_list_elm);
    
    for (dbsksp_xshock_node::edge_iterator eit = v->edges_begin(); eit != 
      v->edges_end(); ++eit)
    {
      dbsksp_xshock_edge_sptr xe = *eit;
      dbsksp_xshock_node_descriptor* xdesc = v->descriptor(xe);

      bxml_element* desc_elm = new 
        bxml_element(dbsksp_xio_xshock_tag[SHOCK_NODE_DESCRIPTOR]); // "shock_node_descriptor"
      descriptor_list_elm->append_data(desc_elm);

      // elements inside a descriptor

      // a. Edge id
      desc_elm->set_attribute(dbsksp_xio_xshock_tag[EDGE_ID], xe->id()); // "edge_id"

      // b. Tangent angle
      desc_elm->set_attribute(dbsksp_xio_xshock_tag[SHOCK_TANGENT_ANGLE], 
        dbsksp_xio_angle_to_npi2pi(xdesc->psi_));

      // c. Angle phi
      desc_elm->set_attribute(dbsksp_xio_xshock_tag[PHI], xdesc->phi_); // "phi"
    }    
  }

  // II. Edge list
  bxml_element *shock_edge_list = new bxml_element( 
    dbsksp_xio_xshock_tag[SHOCK_EDGE_LIST]); // "shock_edge_list"
  root->append_data(shock_edge_list);

  for (dbsksp_xshock_graph::edge_iterator eit = graph->edges_begin(); eit != 
    graph->edges_end(); ++eit)
  {
    dbsksp_xshock_edge_sptr xe = *eit;

    bxml_element* edge_elm = new bxml_element(dbsksp_xio_xshock_tag[SHOCK_EDGE]); // "shock_edge"
    shock_edge_list->append_data(edge_elm);
    
    // a. Edge id
    edge_elm->set_attribute(dbsksp_xio_xshock_tag[ID], xe->id()); // "id"

    // b. Source id
    edge_elm->set_attribute(dbsksp_xio_xshock_tag[SOURCE_ID], xe->source()->id() ); // "source_id"

    // b. Target id
    edge_elm->set_attribute(dbsksp_xio_xshock_tag[TARGET_ID], xe->target()->id() ); // "target_id"
  }

  bxml_write(os, doc);
  //os.precision(current_precision);
  return true;
}


// ============================================================================
// READ
// ============================================================================

// ----------------------------------------------------------------------------
//: load a shock graph from an xml file
bool x_read(const vcl_string& filepath, dbsksp_xshock_graph_sptr& xg)
{
  xg = 0;
  bxml_document doc = bxml_read(filepath);
  if (!doc.root_element())
  {
    xg = 0;
    return false;
  }


  dbsksp_xshock_graph_sptr graph = new dbsksp_xshock_graph();

  // Loading a graph will be done in two passes.
  // Pass 1: just load enough to create a graph topology, connectivity
  // Pass 2: put in the shock properties

  // 0. Find the root element
  bxml_element* root = static_cast<bxml_element* >(bxml_find_by_name(doc.root_element(), 
    bxml_element(dbsksp_xio_xshock_tag[SHOCK_GRAPH])).ptr());
  if (!root)
  {
    vcl_cerr << "ERROR: Unable to locate root element `sksp_xshock_graph'.\n";
    return false;
  }

  // check version
  vcl_string version = "";
  if (!root->get_attribute(dbsksp_xio_xshock_tag[VERSION], version) || version != "1")
  {
    vcl_cerr << "ERROR: Unknown xshock graph version.\n";
    return false;
  }

  // 1. Create the nodes
  bxml_element* node_list_elm = static_cast<bxml_element* >(
    bxml_find_by_name(root, bxml_element(dbsksp_xio_xshock_tag[SHOCK_NODE_LIST])).ptr());
  if (!node_list_elm)
  {
    vcl_cerr << "ERROR: Unable to locate element `" << dbsksp_xio_xshock_tag[SHOCK_NODE_LIST] << "'.\n";
    return false;
  }

  // iterate thru the nodes in the node list and create the nodes
  vcl_map<unsigned int, dbsksp_xshock_node_sptr > node_list;
  vcl_map<unsigned int, bxml_element* > node_elm_list;

  for(bxml_element::const_data_iterator itr = node_list_elm->data_begin();
    itr != node_list_elm->data_end();  ++itr)
  {
    if ( (*itr)->type() != bxml_data::ELEMENT )
      continue;

    bxml_element* node_elm = static_cast<bxml_element* >((*itr).ptr());
    if (node_elm->name() != dbsksp_xio_xshock_tag[SHOCK_NODE]) // "shock_node"
      continue;

    // retrieve properties of the node
    
    unsigned int xv_id = 0;
    double xv_radius = -1;
    double xv_x = 0;
    double xv_y = 0;
    if (!node_elm->get_attribute(dbsksp_xio_xshock_tag[ID], xv_id) || xv_id == 0 ||// "id"
      !node_elm->get_attribute(dbsksp_xio_xshock_tag[X], xv_x) ||    // "x"
      !node_elm->get_attribute(dbsksp_xio_xshock_tag[Y], xv_y) ||    // "y"
      !node_elm->get_attribute(dbsksp_xio_xshock_tag[RADIUS], xv_radius))      // "radius"
    {
      vcl_cerr << "ERROR : Unable to retrieve all node attributes.\n";
      return false;
    };
   
    // create the node
    dbsksp_xshock_node_sptr xv = new dbsksp_xshock_node(xv_id);
    xv->set_pt(vgl_point_2d<double >(xv_x, xv_y));
    xv->set_radius(xv_radius);

    node_list.insert(vcl_make_pair(xv_id, xv));
    node_elm_list.insert(vcl_make_pair(xv_id, node_elm));
  }


  // 2. Create the edges
  bxml_element* edge_list_elm = static_cast<bxml_element* >(
    bxml_find_by_name(root, bxml_element(dbsksp_xio_xshock_tag[SHOCK_EDGE_LIST])).ptr());
  if (!edge_list_elm)
  {
    vcl_cerr << "ERROR: Unable to locate element `" << dbsksp_xio_xshock_tag[SHOCK_EDGE_LIST] << "'.\n";
    return false;
  }

  // iterate thru the edges in the edge list and create the edges
  vcl_map<unsigned int, dbsksp_xshock_edge_sptr > edge_list;
  for(bxml_element::const_data_iterator itr = edge_list_elm->data_begin();
    itr != edge_list_elm->data_end();  ++itr)
  {
    if ( (*itr)->type() != bxml_data::ELEMENT )
      continue;

    bxml_element* edge_elm = static_cast<bxml_element* >((*itr).ptr());
    if (edge_elm->name() != dbsksp_xio_xshock_tag[SHOCK_EDGE]) // "shock_edge"
      continue;

    // retrieve properties of the edge
    unsigned int xe_id = 0;
    unsigned int xe_source_id = 0;
    unsigned int xe_target_id = 0;

    if (!edge_elm->get_attribute(dbsksp_xio_xshock_tag[ID], xe_id) || 
      xe_id == 0 || // "id"
      !edge_elm->get_attribute(dbsksp_xio_xshock_tag[SOURCE_ID], xe_source_id) || 
      xe_source_id == 0 || // "source_id"
      !edge_elm->get_attribute(dbsksp_xio_xshock_tag[TARGET_ID], xe_target_id) ||
      xe_target_id == 0  // "target_id"
      ) 
    {
      vcl_cerr << "ERROR : Unable to retrieve all attributes of element `" 
        << dbsksp_xio_xshock_tag[SHOCK_EDGE] << "'.\n";
      return false;
    };

    // retrieve pointers of source and target nodes from its id's
    vcl_map<unsigned int, dbsksp_xshock_node_sptr >::iterator source_vit, target_vit; 
    source_vit = node_list.find(xe_source_id);
    target_vit = node_list.find(xe_target_id);
    
    if (source_vit == node_list.end() || target_vit == node_list.end())
    {
      vcl_cerr << "ERROR: unable to find either source or target node of edge with id = " 
        << xe_id << ".\n";
      return false;
    }
    dbsksp_xshock_node_sptr source_sptr = source_vit->second;
    dbsksp_xshock_node_sptr target_sptr = target_vit->second;

    // create the edge
    dbsksp_xshock_edge_sptr xe = new dbsksp_xshock_edge(source_sptr, target_sptr, xe_id);
    edge_list.insert(vcl_make_pair(xe_id, xe));
  }

  // 3. Add edges to the nodes and fill in info for shock node descriptors

  // iterate thru the list of nodes
  for (vcl_map<unsigned int, dbsksp_xshock_node_sptr >::iterator nit = 
    node_list.begin(); nit != node_list.end(); ++nit)
  {
    // retrieve basic info of the node of interest
    unsigned int xv_id = nit->first;
    dbsksp_xshock_node_sptr xv = nit->second;
    bxml_element* node_elm = node_elm_list[xv_id];

    if (node_elm->name() != dbsksp_xio_xshock_tag[SHOCK_NODE])
      continue;

    // Read the descriptor list and assign to the node

    // i. locate the descriptor list
    bxml_element* descriptor_list_elm = static_cast<bxml_element* >( 
      bxml_find_by_name(node_elm, bxml_element( 
      dbsksp_xio_xshock_tag[SHOCK_NODE_DESCRIPTOR_LIST])).ptr());

    if (!descriptor_list_elm)
    {
      vcl_cerr << "ERROR: Unable to locate element `" 
        << dbsksp_xio_xshock_tag[SHOCK_NODE_DESCRIPTOR_LIST] << "'.\n";
      return false;
    }

    // ii. parse the descriptor list
    for(bxml_element::const_data_iterator itr = descriptor_list_elm->data_begin();
      itr != descriptor_list_elm->data_end();  ++itr)
    {
      // type check
      if ( (*itr)->type() != bxml_data::ELEMENT )
        continue;

      bxml_element* descriptor_elm = static_cast<bxml_element* >((*itr).ptr());
      if ( descriptor_elm->name() != dbsksp_xio_xshock_tag[SHOCK_NODE_DESCRIPTOR] )
        continue;

      // retrieve properties of the node descriptor
      unsigned int xe_id = 0;
      double xe_tangent_angle = 0;
      double xe_phi = 0;

      if (!descriptor_elm->get_attribute(dbsksp_xio_xshock_tag[EDGE_ID], xe_id) || 
        xe_id == 0 || // "edge_id" 
        !descriptor_elm->get_attribute(dbsksp_xio_xshock_tag[SHOCK_TANGENT_ANGLE], xe_tangent_angle) ||
        !descriptor_elm->get_attribute(dbsksp_xio_xshock_tag[PHI], xe_phi)
        )
      {
        vcl_cerr << "ERROR : Unable to retrieve all attributes of element `" 
          << dbsksp_xio_xshock_tag[SHOCK_NODE_DESCRIPTOR] << "'.\n";
        return false;
      }

      // retrieve edge from edge id
      vcl_map<unsigned int, dbsksp_xshock_edge_sptr >::iterator xe_itr = edge_list.find(xe_id);
      if (xe_itr == edge_list.end())
      {
        vcl_cerr << "ERROR: unable to find edge (id = " << xe_id 
          << " ) associated with a node (id = " << xv_id << ").\n";
        return false;
      }
      dbsksp_xshock_edge_sptr xe = xe_itr->second;

      // add edge to node
      dbsksp_xshock_node_descriptor* xdesc = xv->insert_shock_edge(xe, 0);
      xdesc->phi_ = xe_phi;
      xdesc->psi_ = xe_tangent_angle;
    }

    // set radius and node-coordinates associated with the newly-added descriptors
    xv->set_pt(xv->pt());
    xv->set_radius(xv->radius());
  }


  // insert all the nodes and edges to the shock graph
  for (vcl_map<unsigned int, dbsksp_xshock_node_sptr >::iterator itr = node_list.begin();
    itr != node_list.end(); ++itr)
  {
    graph->add_vertex(itr->second);
  }

  for (vcl_map<unsigned int, dbsksp_xshock_edge_sptr >::iterator itr = edge_list.begin();
    itr != edge_list.end(); ++itr)
  {
    graph->add_edge(itr->second);
  }

  // set the next available ID to be the max ID in the graph
  unsigned max_id = 0;
  for (dbsksp_xshock_graph::vertex_iterator vit = graph->vertices_begin();
    vit != graph->vertices_end(); ++vit)
  {
    max_id = vnl_math::max(max_id, (*vit)->id());
  }

  for (dbsksp_xshock_graph::edge_iterator eit = graph->edges_begin();
    eit != graph->edges_end(); ++eit)
  {
    max_id = vnl_math::max(max_id, (*eit)->id());
  }
  graph->set_next_available_id(max_id);

  // make sure terminal-nodes are consistent with their adjacent nodes
  graph->update_all_degree_1_nodes();

  // 4. Check consistency in the graph

  // i. Topology
  // Every edge `xe' of a node 'xv' has the node 'xv' as either its source or target.
  for (dbsksp_xshock_graph::vertex_iterator vit = graph->vertices_begin();
    vit != graph->vertices_end(); ++vit)
  {
    dbsksp_xshock_node_sptr xv = *vit;
    for (dbsksp_xshock_node::edge_iterator eit = xv->edges_begin(); eit != 
      xv->edges_end(); ++eit)
    {
      dbsksp_xshock_edge_sptr xe = *eit;
      if (!xe->is_vertex(xv))
      {
        vcl_cerr << "ERROR: graph structure is not consistent.\n";
        return false;
      }
    }
  }

  // ii. Geometry
  
  for (dbsksp_xshock_graph::vertex_iterator vit = graph->vertices_begin();
    vit != graph->vertices_end(); ++vit)
  {
    dbsksp_xshock_node_sptr xv = *vit;

    // sum of phi's around each node needs to be pi
    double sum_phi = 0;
    for (dbsksp_xshock_node::edge_iterator eit = xv->edges_begin(); eit != 
      xv->edges_end(); ++eit)
    {
      dbsksp_xshock_edge_sptr xe = *eit;
      dbsksp_xshock_node_descriptor* xdesc = xv->descriptor(xe);
      sum_phi += xdesc->phi_;
    }

    // distribute the errors uniformly
    double dphi = (sum_phi - vnl_math::pi) / xv->degree();

    for (dbsksp_xshock_node::edge_iterator eit = xv->edges_begin(); eit != 
      xv->edges_end(); ++eit)
    {
      dbsksp_xshock_edge_sptr xe = *eit;
      dbsksp_xshock_node_descriptor* xdesc = xv->descriptor(xe);
      xdesc->phi_ -= dphi;
    }

    // tangent angles must be consistent with phi angles
    // assuming the tangent of the first branch is accurate
    dbsksp_xshock_node::edge_iterator eit = xv->edges_begin();
    dbsksp_xshock_edge_sptr last_xe = *eit;
    dbsksp_xshock_node_descriptor* last_desc = xv->descriptor(last_xe);

    // compute others' tangent according to phi's and the first tangent
    ++eit;
    for (; eit != xv->edges_end(); ++eit)
    {
      dbsksp_xshock_edge_sptr cur_xe = *eit;
      dbsksp_xshock_node_descriptor* cur_desc = xv->descriptor(cur_xe);
      cur_desc->psi_ = last_desc->psi_ + last_desc->phi_ + cur_desc->phi_;

      // update for next iteration
      last_xe = cur_xe;
      last_desc = cur_desc;
    }

  }


  // assign the returned graph
  xg = graph;
  return true;
}


