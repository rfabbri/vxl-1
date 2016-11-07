// This is dbsksp/xio/dbsksp_xio_shock_graph.cxx


#include "dbsksp_xio_shock_graph.h"
#include <bxml/bxml_read.h>
#include <bxml/bxml_write.h>
#include <bxml/bxml_find.h>

#include <dbsksp/dbsksp_shock_graph.h>
#include <dbsksp/dbsksp_shock_node.h>
#include <dbsksp/dbsksp_shock_edge.h>

#include <vcl_utility.h>
#include <vcl_fstream.h>

enum dbsksp_xio_tag_index{
  SHOCK_GRAPH = 0,
  SHOCK_NODE_LIST = 1,
  SHOCK_NODE = 2,
  ID = 3,
  POINT = 4,
  X = 5,
  Y = 6,
  RADIUS = 7,
  SHOCK_NODE_DESCRIPTOR_LIST = 8,
  SHOCK_NODE_DESCRIPTOR = 9,
  SHOCK_EDGE_ID = 10,
  ALPHA = 11,
  PHI = 12,
  ROT_ANGLE_TO_SUCC_EDGE = 13,
  SHOCK_FLOW_DIR = 14,
  SHOCK_EDGE_LIST = 15,
  SHOCK_EDGE = 16,
  SOURCE_ID = 17,
  TARGET_ID = 18,
  CHORD_LENGTH = 19,
  CHORD_DIR = 20,
  PARAM_M = 21,
  RADIUS_INCREMENT = 22,
  REFERENCE_NODE = 23,
  REFERENCE_EDGE = 24,
  EDGE_ID = 25
};
vcl_string dbsksp_xio_tag[] = {
  "shock_graph",                            // 0
  "shock_node_list",                        // 1
  "shock_node",                             // 2      
  "id",                                     // 3  
  "point",                                  // 4      
  "x",                                      // 5  
  "y",                                      // 6    
  "radius",                                 // 7
  "shock_node_descriptor_list",             // 8    
  "shock_node_descriptor",                  // 9  
  "shock_edge_id",                          // 10
  "alpha",                                  // 11
  "phi",                                    // 12
  "rot_angle_to_succ_edge",                 // 13
  "shock_flow_dir",                         // 14
  "shock_edge_list",                        // 15
  "shock_edge",                             // 16  
  "source_id",                              // 17  
  "target_id",                              // 18    
  "chord_length",                           // 19
  "chord_dir",                              // 20
  "param_m",                                // 21  
  "radius_increment",                       // 22  
  "reference_node",                         // 23      
  "reference_edge",                         // 24
  "edge_id"                                 // 25
};

// ============================================================================
// SUPPORT FUNCTIONS
// ============================================================================

// ----------------------------------------------------------------------------
// READ
// ----------------------------------------------------------------------------

//: Create and append a simple element( element with only text data) 
// to an existing element
template<class T>
bxml_element* 
append_simple_elm(bxml_element* parent_elm, 
                                 const vcl_string& elm_name, 
                                 T elm_value)
{
  vcl_stringstream s;
  s.str("");
  s << elm_value;
  
  bxml_element* elm = new bxml_element(elm_name);
  parent_elm->append_data(elm);
  elm->append_text(s.str());

  return elm;
}


// ----------------------------------------------------------------------------
// WRITE
// ----------------------------------------------------------------------------

//: A place holder for functions to parse a shock graph xml file
class dbsksp_xio_shock_graph_xml_parser
{
public:
  ~dbsksp_xio_shock_graph_xml_parser();

  //: find a property of an element saved as a simple child element
  // (an element with only one text as child data)
  template<class T> 
  static bool get_data_from_simple_child_element(bxml_element* elm, 
    const vcl_string& elm_name,
    T& return_value);

  ////: Parse an descriptor list element and return a list of descriptors
  //// is ready to be added to a node
  //bool parse_descriptor_list( bxml_element* descriptor_list_elm,
  //  const vcl_map<int, dbsksp_shock_edge_sptr >& edge_map,
  //  vcl_vector<dbsksp_shock_node_descriptor_sptr >& descriptor_list);

  //: Find a (immedicate) child element
  static bxml_element* find_child_element(const bxml_element* parent_elem, 
    const bxml_element& query);

  
protected:


private:
  // Disable constructors, only static functions
  dbsksp_xio_shock_graph_xml_parser();

};

// ----------------------------------------------------------------------------

//: find a property of an element saved as a simple child element
// (an element with only one text as child data)
template<class T> bool
dbsksp_xio_shock_graph_xml_parser::
get_data_from_simple_child_element(bxml_element* parent_elm, 
                                   const vcl_string& child_elm_name,
                                   T& return_value)
{
  // iterate thru all the child data
  for (bxml_element::const_data_iterator itr = parent_elm->data_begin();
    itr != parent_elm->data_end(); ++itr)
  {
    if ((*itr)->type() != bxml_data::ELEMENT)
      continue;

    bxml_element* elm = static_cast<bxml_element* >((*itr).ptr());
    if (elm->name() == child_elm_name)
    {
      if (elm->num_data() != 1)
        continue;

      bxml_data_sptr data = *(elm->data_begin());
      if (data->type() != bxml_data::TEXT)
        continue;
      
      bxml_text* text_data = static_cast<bxml_text*>(data.ptr());
      vcl_stringstream s(text_data->data());

      s >> return_value;
      return true;
    }
  }
  return false;
}

// ----------------------------------------------------------------------------
//: Find a (immedicate) child element
bxml_element* dbsksp_xio_shock_graph_xml_parser::
find_child_element(const bxml_element* parent_elm, 
                   const bxml_element& query)
{
  if ( !parent_elm ) return 0;

  bxml_element* child_elm = 0;

  // find the node list element
  for (bxml_element::const_data_iterator itr = 
    parent_elm->data_begin(); itr != parent_elm->data_end(); ++itr)
  {
    if ((*itr)->type() != bxml_data::ELEMENT)
      continue;

    bxml_element* elm = static_cast<bxml_element* >((*itr).ptr());
    if ( bxml_matches(*elm, query) )
    {
      return elm;
    }
  }

  return 0;

}




// ============================================================================
// IMPLEMENTATION OF X_WRITE and X_READ
// ============================================================================


// ----------------------------------------------------------------------------
//: write a shock graph to an xml file
bool x_write(const vcl_string& filepath, const dbsksp_shock_graph_sptr& graph)
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
bool x_write(vcl_ostream& os, const dbsksp_shock_graph_sptr& graph)
{
  bxml_document doc;
  bxml_element *root = new bxml_element(dbsksp_xio_tag[SHOCK_GRAPH]); // "shock_graph"
  doc.set_root_element(root);

  // I. Node list
  bxml_element *node_list_elm = new bxml_element(dbsksp_xio_tag[SHOCK_NODE_LIST]); // "shock_node_list"
  root->append_data(node_list_elm);

  for (dbsksp_shock_graph::vertex_iterator vit = graph->vertices_begin(); 
    vit != graph->vertices_end(); ++vit)
  {
    dbsksp_shock_node_sptr v = *vit;
    bxml_element* node_elm = new bxml_element(dbsksp_xio_tag[SHOCK_NODE]); // "shock_node"
    node_list_elm->append_data(node_elm);

    // 1. id as attribute
    append_simple_elm(node_elm, dbsksp_xio_tag[ID], v->id()); // "id"
    node_elm->set_attribute(dbsksp_xio_tag[ID], v->id()); // "id"
    
    // 2. point coordinate
    bxml_element* point_elm = new bxml_element(dbsksp_xio_tag[POINT]); // "point"
    node_elm->append_data(point_elm);

    // x-coordinate
    append_simple_elm(point_elm, dbsksp_xio_tag[X], v->pt().x()); // "x"
    
    // y-coordinate
    append_simple_elm(point_elm, dbsksp_xio_tag[Y], v->pt().y());  // "y"
    
    // 3. radius
    append_simple_elm(node_elm, dbsksp_xio_tag[RADIUS], v->radius()); // "radius"

    // 4. Descriptor list
    bxml_element* descriptor_list_elm = new bxml_element(
      dbsksp_xio_tag[SHOCK_NODE_DESCRIPTOR_LIST]); // "shock_node_descriptor_list"
    node_elm->append_data(descriptor_list_elm);

    vcl_list<dbsksp_shock_node_descriptor_sptr > dlist = v->descriptor_list();
    for (vcl_list<dbsksp_shock_node_descriptor_sptr >::iterator itr = 
      dlist.begin(); itr != dlist.end(); ++itr)
    {
      dbsksp_shock_node_descriptor_sptr desc = *itr;
      
      bxml_element* desc_elm = new bxml_element(dbsksp_xio_tag[SHOCK_NODE_DESCRIPTOR]); // "shock_node_descriptor"
      descriptor_list_elm->append_data(desc_elm);

      // elements inside a descriptor

      // a. Edge id
      append_simple_elm(desc_elm, dbsksp_xio_tag[EDGE_ID], desc->edge->id()); // "edge_id"

      // b. Angle alpha
      append_simple_elm(desc_elm, dbsksp_xio_tag[ALPHA], desc->alpha); // "alpha"

      // c. Angle phi
      append_simple_elm(desc_elm, dbsksp_xio_tag[PHI], desc->phi); // "phi"

      // d. Rotation angle to successor edge
      append_simple_elm(desc_elm, dbsksp_xio_tag[ROT_ANGLE_TO_SUCC_EDGE], 
        desc->rot_angle_to_succ_edge); // "rot_angle_to_succ_edge"

      // e. Shock direction
      append_simple_elm(desc_elm, dbsksp_xio_tag[SHOCK_FLOW_DIR], desc->shock_flow_dir); // "shock_flow_dir"
    }    
  }

  // II. Edge list

  bxml_element *shock_edge_list = new bxml_element(dbsksp_xio_tag[SHOCK_EDGE_LIST]); // "shock_edge_list"
  root->append_data(shock_edge_list);

  for (dbsksp_shock_graph::edge_iterator eit = graph->edges_begin();
    eit != graph->edges_end(); ++eit)
  {
    dbsksp_shock_edge_sptr e = *eit;

    bxml_element* edge_elm = new bxml_element(dbsksp_xio_tag[SHOCK_EDGE]); // "shock_edge"
    shock_edge_list->append_data(edge_elm);
    edge_elm->set_attribute(dbsksp_xio_tag[ID], e->id()); // "id"

    // 1. Graph structure
    // a. Source
    append_simple_elm(edge_elm, dbsksp_xio_tag[SOURCE_ID], e->source()->id()); // "source_id"
    
    // b. Target
    append_simple_elm(edge_elm, dbsksp_xio_tag[TARGET_ID], e->target()->id()); // "target_id"

    // 2. Shock properties
    // a. Chord length
    append_simple_elm(edge_elm, dbsksp_xio_tag[CHORD_LENGTH], e->chord_length()); // "chord_length"
    
    // b. Chord direction
    bxml_element* chord_dir_elm = new bxml_element(dbsksp_xio_tag[CHORD_DIR]); // "chord_dir"
    edge_elm->append_data(chord_dir_elm);

    // b.i. x
    append_simple_elm(chord_dir_elm, dbsksp_xio_tag[X], e->chord_dir().x()); // "x"

    // b.ii. y
    append_simple_elm(chord_dir_elm, dbsksp_xio_tag[Y] , e->chord_dir().y()); // "y"

    // c. m-parameter
    append_simple_elm(edge_elm, dbsksp_xio_tag[PARAM_M], e->param_m()); // "param_m"
    
    // d. delta_r
    append_simple_elm(edge_elm, dbsksp_xio_tag[RADIUS_INCREMENT], e->radius_increment()); // "radius_increment"

  }

  // III. Reference node
  bxml_element* ref_node_elm = new bxml_element(dbsksp_xio_tag[REFERENCE_NODE]); // "reference_node"
  root->append_data(ref_node_elm);

  // 1. id of reference node
  append_simple_elm(ref_node_elm, dbsksp_xio_tag[ID], graph->ref_node()->id()); // "id"

  // 2. coordinate of reference node
  bxml_element* ref_node_point_elm = new bxml_element(dbsksp_xio_tag[POINT]); // "point"
  ref_node_elm->append_data(ref_node_point_elm);

  // x
  append_simple_elm(ref_node_point_elm, dbsksp_xio_tag[X], graph->ref_origin().x()); // "x"

  // y
  append_simple_elm(ref_node_point_elm, dbsksp_xio_tag[Y], graph->ref_origin().y()); // "y"

  // 3. radius at reference node
  append_simple_elm(ref_node_elm, dbsksp_xio_tag[RADIUS], graph->ref_node_radius()); // "radius"

  // IV. Reference edge
  bxml_element* ref_edge_elm = new bxml_element(dbsksp_xio_tag[REFERENCE_EDGE]); // "reference_edge"
  root->append_data(ref_edge_elm);

  // 1. id of the reference edge
  append_simple_elm(ref_edge_elm, dbsksp_xio_tag[ID], graph->ref_edge()->id()); // "id"

  // 2. reference edge direction
  bxml_element* ref_edge_dir_elm = new bxml_element(dbsksp_xio_tag[CHORD_DIR]); // "chord_dir"
  ref_edge_elm->append_data(ref_edge_dir_elm);

  // x
  append_simple_elm(ref_edge_dir_elm, dbsksp_xio_tag[X], graph->ref_direction().x()); // "x"

  // y
  append_simple_elm(ref_edge_dir_elm, dbsksp_xio_tag[Y], graph->ref_direction().y()); // "y"

  bxml_write(os, doc);
  return true;
}


// ============================================================================
// READ
// ============================================================================

// ----------------------------------------------------------------------------
//: load a shock graph from an xml file
bool x_read(const vcl_string& filepath, dbsksp_shock_graph_sptr& graph)
{
  bxml_document doc = bxml_read(filepath);

  // Loading a graph will be done in two passes.
  // Pass 1: just load enough to create a graph topology, connectivity
  // Pass 2: put in the shock properties

  if (graph)
    graph->clear();
  else
    graph = new dbsksp_shock_graph();
  
  // 1. Create the nodes
  bxml_element* root = static_cast<bxml_element* >(doc.root_element().ptr());
  bxml_element* node_list_elm = 
    dbsksp_xio_shock_graph_xml_parser::find_child_element(root, 
    bxml_element(dbsksp_xio_tag[SHOCK_NODE_LIST]));

  if (!node_list_elm)
  {
    vcl_cerr << "ERROR: No shock node list found.\n";
    return false;
  }

  // iterate thru the nodes in the node list and create the nodes
  vcl_map<int, dbsksp_shock_node_sptr > node_list;
  for(bxml_element::const_data_iterator itr = node_list_elm->data_begin();
    itr != node_list_elm->data_end();  ++itr)
  {
    if ( (*itr)->type() != bxml_data::ELEMENT )
      continue;

    bxml_element* node_elm = static_cast<bxml_element* >((*itr).ptr());
    if (node_elm->name() != dbsksp_xio_tag[SHOCK_NODE]) // "shock_node"
      continue;

    // find id of the node
    int id = 0;
    if (!node_elm->get_attribute(dbsksp_xio_tag[ID], id)) // "id"
    {
      vcl_cerr << "ERROR : found node with no ID.\n";
      return false;
    };

    // create the node
    dbsksp_shock_node_sptr v = new dbsksp_shock_node(id);
    node_list.insert(vcl_make_pair(id, v));
  }


  // 2. Create the edges
  
  bxml_element* edge_list_elm = 
    dbsksp_xio_shock_graph_xml_parser::find_child_element(root, 
    bxml_element(dbsksp_xio_tag[SHOCK_EDGE_LIST]));

  if (!edge_list_elm)
  {
    vcl_cerr << "ERROR: No shock edge list found.\n";
    return false;
  }

  // iterate thru the edges in the edge list and create the edges
  vcl_map<int, dbsksp_shock_edge_sptr > edge_list;
  for(bxml_element::const_data_iterator itr = edge_list_elm->data_begin();
    itr != edge_list_elm->data_end();  ++itr)
  {
    if ( (*itr)->type() != bxml_data::ELEMENT )
      continue;

    bxml_element* edge_elm = static_cast<bxml_element* >((*itr).ptr());
    if (edge_elm->name() != dbsksp_xio_tag[SHOCK_EDGE]) // "shock_edge"
      continue;

    // find id of the edge
    int id = 0;
    if (!edge_elm->get_attribute(dbsksp_xio_tag[ID], id)) // "id"
    {
      vcl_cerr << "ERROR : found edge with no ID.\n";
      return false;
    };

    // find other properties of the edge

    // Absolutely necessary properties
    int source_id = -1;
    dbsksp_xio_shock_graph_xml_parser::get_data_from_simple_child_element(
      edge_elm, dbsksp_xio_tag[SOURCE_ID], source_id); // "source_id"

    int target_id = -1;
    dbsksp_xio_shock_graph_xml_parser::get_data_from_simple_child_element(
      edge_elm, dbsksp_xio_tag[TARGET_ID], target_id); // "target_id"

    

    // find the pointers to the source and target
    if (source_id == -1 || target_id == -1)
    {
      vcl_cerr << "ERROR: either source or target of edge is not specified.\n";
      return false;
    }

    vcl_map<int, dbsksp_shock_node_sptr >::iterator vit = node_list.find(source_id);
    if (vit == node_list.end())
    {
      vcl_cerr << "ERROR: cannot find source of edge, source_id=" << source_id 
        << vcl_endl;
      return false;
    }
    dbsksp_shock_node_sptr source_sptr = vit->second;

    vit = node_list.find(target_id);
    if (vit == node_list.end())
    {
      vcl_cerr << "ERROR: cannot find target of edge, target_id=" << target_id 
        << vcl_endl;
      return false;
    }
    dbsksp_shock_node_sptr target_sptr = vit->second;

    // create the edge
    dbsksp_shock_edge_sptr e = new dbsksp_shock_edge(source_sptr, target_sptr, id);
    edge_list.insert(vcl_make_pair(id, e));

    double param_m = 0;
    if ( !dbsksp_xio_shock_graph_xml_parser::get_data_from_simple_child_element(
      edge_elm, dbsksp_xio_tag[PARAM_M], param_m) ) // "param_m"
    {
      vcl_cerr << "ERROR: param_m of edge (id= " << id << ") not specified.\n";
      return false;
    };
    e->set_param_m(param_m);

    double chord_length = 0;
    if ( !dbsksp_xio_shock_graph_xml_parser::get_data_from_simple_child_element(
      edge_elm, dbsksp_xio_tag[CHORD_LENGTH], chord_length) ) // "chord_length"
    {
      vcl_cerr << "ERROR: chord length of edge (id= " << id << ") not specified.\n";
      return false;
    };
    e->set_chord_length(chord_length);

    // Optional properties, may not exist
    vgl_vector_2d<double > chord_dir (1, 0);
    bxml_element* chord_dir_elm = static_cast<bxml_element* >(
      bxml_find(edge_elm, bxml_element(dbsksp_xio_tag[CHORD_DIR])).ptr()); // "chord_dir"

    if (chord_dir_elm)
    {
      double vx = 0;
      double vy = 0;
      
      if (dbsksp_xio_shock_graph_xml_parser::get_data_from_simple_child_element(
      chord_dir_elm, dbsksp_xio_tag[X], vx) && // "x"
        dbsksp_xio_shock_graph_xml_parser::get_data_from_simple_child_element(
      chord_dir_elm, dbsksp_xio_tag[Y], vy) // "y"
      )
      {
        chord_dir.set(vx, vy);
        e->set_chord_dir(chord_dir);
      }
    }

    double delta_r = 0;
    if ( dbsksp_xio_shock_graph_xml_parser::get_data_from_simple_child_element(
      edge_elm, dbsksp_xio_tag[RADIUS_INCREMENT], delta_r) ) // "radius_increment"
    {
      e->set_radius_increment(delta_r);
    }
  }


  // At this point al the edges are done. The only things left are the info
  // of the nodes, including the descriptor list

  // 3. Add edges to the nodes and fill in info for shock node descriptors
  for(bxml_element::const_data_iterator itr = node_list_elm->data_begin();
    itr != node_list_elm->data_end();  ++itr)
  {
    if ( (*itr)->type() != bxml_data::ELEMENT )
      continue;

    bxml_element* node_elm = static_cast<bxml_element* >((*itr).ptr());
    if (node_elm->name() != dbsksp_xio_tag[SHOCK_NODE])
      continue;

    // i. find id of the node and retrieve the pointer to the node
    int id = 0;
    node_elm->get_attribute(dbsksp_xio_tag[ID], id);
    dbsksp_shock_node_sptr node_sptr = node_list.find(id)->second;

    // ii. Point coordinate
    if ( bxml_element* point_elm = 
      dbsksp_xio_shock_graph_xml_parser::find_child_element(node_elm, 
      bxml_element(dbsksp_xio_tag[POINT])) )
    {
      double x = 0;
      double y = 0;
      dbsksp_xio_shock_graph_xml_parser::get_data_from_simple_child_element(
        point_elm, dbsksp_xio_tag[X], x);
      dbsksp_xio_shock_graph_xml_parser::get_data_from_simple_child_element(
        point_elm, dbsksp_xio_tag[Y], y);
      node_sptr->set_pt(vgl_point_2d<double >(x, y));    
    }

    
    // iii. radius
    double radius = -1;
    if ( dbsksp_xio_shock_graph_xml_parser::get_data_from_simple_child_element(
      node_elm, dbsksp_xio_tag[RADIUS], radius) )
    {
      node_sptr->set_radius(radius);
    }

    // iv. Descriptor list
    // required: set the descriptors
    bxml_element* descriptor_list_elm = static_cast<bxml_element* >(
      bxml_find(node_elm, 
      bxml_element(dbsksp_xio_tag[SHOCK_NODE_DESCRIPTOR_LIST])).ptr());

    if (!descriptor_list_elm)
    {
      vcl_cout << "ERROR: shock_node_descriptor_list not found.\n";
      return false;
    }

    // parse the descriptor list
    for(bxml_element::const_data_iterator itr = descriptor_list_elm->data_begin();
      itr != descriptor_list_elm->data_end();  ++itr)
    {
      // type check
      if ( (*itr)->type() != bxml_data::ELEMENT )
        continue;

      bxml_element* descriptor_elm = static_cast<bxml_element* >((*itr).ptr());

      if ( descriptor_elm->name() != dbsksp_xio_tag[SHOCK_NODE_DESCRIPTOR] )
        continue;


      // a. Edge id (required)
      int edge_id = -1;
      if ( !dbsksp_xio_shock_graph_xml_parser::get_data_from_simple_child_element(
        descriptor_elm, dbsksp_xio_tag[EDGE_ID], edge_id) )
      {
        vcl_cerr << "ERROR: Missing edge id for node (id = " << node_sptr->id() << " ).\n";
        return false;
      }
      // retrieve edge from edge id
      dbsksp_shock_edge_sptr edge_sptr = edge_list.find(edge_id)->second;
      node_sptr->add_edge(edge_sptr);
      dbsksp_shock_node_descriptor_sptr desc_sptr = node_sptr->descriptor(edge_sptr);
      desc_sptr->edge = edge_sptr;

      // b. Angle alpha (optional)
      double alpha = 0;
      if ( dbsksp_xio_shock_graph_xml_parser::get_data_from_simple_child_element(
        descriptor_elm, dbsksp_xio_tag[ALPHA], alpha) )
      {
        desc_sptr->alpha = alpha;
      }

      // c. Angle phi (required)
      double phi = 0;
      if ( ! dbsksp_xio_shock_graph_xml_parser::get_data_from_simple_child_element(
        descriptor_elm, dbsksp_xio_tag[PHI], phi) )
      {
        vcl_cerr << "ERROR: angle phi missing for node descriptor ( node_id= " 
          << node_sptr->id() << " , edge_id = " << edge_sptr->id() << " ).\n";
        return false;
      }
      desc_sptr->phi = phi;

      // d. Rotation angle to successor edge (optional)
      double rot_angle_to_succ_edge = 0;
      if ( dbsksp_xio_shock_graph_xml_parser::get_data_from_simple_child_element(
        descriptor_elm, dbsksp_xio_tag[ROT_ANGLE_TO_SUCC_EDGE], rot_angle_to_succ_edge) )
      {
        desc_sptr->rot_angle_to_succ_edge = rot_angle_to_succ_edge;
      }
      
      // e. Shock direction
      int shock_flow_dir = 0;
      if ( dbsksp_xio_shock_graph_xml_parser::get_data_from_simple_child_element(
        descriptor_elm, dbsksp_xio_tag[SHOCK_FLOW_DIR], shock_flow_dir) )
      {
        desc_sptr->shock_flow_dir = shock_flow_dir;
      }
    }
  }

  // insert all the nodes and edges to the shock graph
  for (vcl_map<int, dbsksp_shock_node_sptr >::iterator itr = node_list.begin();
    itr != node_list.end(); ++itr)
  {
    graph->add_vertex(itr->second);
  }

  for (vcl_map<int, dbsksp_shock_edge_sptr >::iterator itr = edge_list.begin();
    itr != edge_list.end(); ++itr)
  {
    graph->add_edge(itr->second);
  }



  // 4. Parse the reference node and reference edge info

  // i. Reference node
  bxml_element* ref_node_elm = dbsksp_xio_shock_graph_xml_parser::find_child_element(
    root, bxml_element(dbsksp_xio_tag[REFERENCE_NODE]) );

  if ( !ref_node_elm )
  {
    vcl_cerr << "ERROR: Could not find reference node.\n";
    return false;
  }

  // a. id of ref_node
  int ref_node_id = -1;
  if ( ! dbsksp_xio_shock_graph_xml_parser::get_data_from_simple_child_element(
    ref_node_elm, "id", ref_node_id) )
  {
    vcl_cerr << "ERROR: Id of reference node missing.\n";
    return false;
  }
  graph->set_ref_node(node_list.find(ref_node_id)->second);

  // b. coordinate of reference node
  bxml_element* ref_node_point_elm = dbsksp_xio_shock_graph_xml_parser::find_child_element(
    ref_node_elm, bxml_element(dbsksp_xio_tag[POINT]) );
  if ( !ref_node_point_elm )
  {
    vcl_cerr << "ERROR: Coordinate of reference node missing.\n";
    return false;
  }

  double ref_node_x = 0;
  double ref_node_y = 0;
  dbsksp_xio_shock_graph_xml_parser::get_data_from_simple_child_element(
    ref_node_point_elm, dbsksp_xio_tag[X], ref_node_x);
  dbsksp_xio_shock_graph_xml_parser::get_data_from_simple_child_element(
    ref_node_point_elm, dbsksp_xio_tag[Y], ref_node_y);

  graph->set_ref_origin(vgl_point_2d<double >(ref_node_x, ref_node_y) );


  // c. radius at reference node
  double ref_node_radius = 0;
  if ( !dbsksp_xio_shock_graph_xml_parser::get_data_from_simple_child_element(
    ref_node_elm, dbsksp_xio_tag[RADIUS], ref_node_radius) )
  {
    vcl_cerr << "ERROR: radius at reference node missing. ref_node_id = "
      << graph->ref_node()->id() << " \n";
    return false;
  }
  graph->set_ref_node_radius(ref_node_radius);

  // ii. Reference edge

  bxml_element* ref_edge_elm = dbsksp_xio_shock_graph_xml_parser::find_child_element(
    root, bxml_element(dbsksp_xio_tag[REFERENCE_EDGE]) );

  if ( !ref_edge_elm )
  {
    vcl_cerr << "Reference edge element missing.\n";
    return false;
  }

  // a. id of the reference edge
  int ref_edge_id = -1;
  if ( !dbsksp_xio_shock_graph_xml_parser::get_data_from_simple_child_element(
    ref_edge_elm, dbsksp_xio_tag[ID], ref_edge_id ) )
  {
    vcl_cerr << "Reference edge id missing.\n";
    return false;
  }
  graph->set_ref_edge(edge_list.find(ref_edge_id)->second);


  // b. reference edge direction
  bxml_element* ref_edge_dir_elm = dbsksp_xio_shock_graph_xml_parser::find_child_element(
  ref_edge_elm, bxml_element(dbsksp_xio_tag[CHORD_DIR]) );

  if (!ref_edge_dir_elm)
  {
    vcl_cerr << "Direction of reference edge missing.\n";
    return false;
  }

  // x
  double ref_edge_dir_x = 0;
  dbsksp_xio_shock_graph_xml_parser::get_data_from_simple_child_element(ref_edge_dir_elm, 
    dbsksp_xio_tag[X], ref_edge_dir_x );
  
  // y
  double ref_edge_dir_y = 0;
  dbsksp_xio_shock_graph_xml_parser::get_data_from_simple_child_element(ref_edge_dir_elm, 
    dbsksp_xio_tag[Y], ref_edge_dir_y );

  graph->set_ref_direction(vgl_vector_2d<double >(ref_edge_dir_x, ref_edge_dir_y));
  

  // set the next available ID to be the max ID in the graph
  unsigned max_id = 0;
  for (dbsksp_shock_graph::vertex_iterator vit = graph->vertices_begin();
    vit != graph->vertices_end(); ++vit)
  {
    max_id = (max_id < (*vit)->id() ) ? (*vit)->id() : max_id;
  }

  for (dbsksp_shock_graph::edge_iterator eit = graph->edges_begin();
    eit != graph->edges_end(); ++eit)
  {
    max_id = (max_id < (*eit)->id() ) ? (*eit)->id() : max_id;
  }

  graph->set_next_available_id(max_id);
  
  return true;
}


