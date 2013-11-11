// This is dbsksp/xio/dbsks_xio_xgraph_geom_model.cxx


#include "dbsks_xio_xgraph_geom_model.h"

#include <dbsks/dbsks_xfrag_geom_model.h>
#include <dbsks/dbsks_xgraph_geom_model.h>
#include <dbsks/dbsks_xnode_geom_model.h>

#include <dbxml/dbxml_xio.h>
#include <dbxml/dbxml_vector_xio.h>
#include <dbxml/dbxml_algos.h>

#include <bxml/bxml_read.h>
#include <bxml/bxml_write.h>
#include <bxml/bxml_find.h>

#include <vcl_utility.h>



//=============================================================================
// SUPPORTING FUNCTIONS - DECLARATIONS
//=============================================================================

//: place holders for XML I/O functions. No instance of this class can be created
class dbsks_xio_xgraph_geom_model
{
public:
  // NEW XML ELEMENT------------------------------------------------------------
  
  //: Create a new xml element for a xgraph_geom_model - version 1
  static bxml_element* new_v1(const dbsks_xgraph_geom_model_sptr& xgraph_geom);

  //: Create a new xml element for a xgraph_geom_model - version 2
  static bxml_element* new_v2(const dbsks_xgraph_geom_model_sptr& xgraph_geom);

  // PARSE XML ELEMENT----------------------------------------------------------
  
  //: Parse bxml_element to xgraph geom model - version 1
  static bool parse_v1(bxml_element* root, dbsks_xgraph_geom_model_sptr& xgraph_geom);

  //: Parse bxml_element to xgraph geom model - version 2
  static bool parse_v2(bxml_element* root, dbsks_xgraph_geom_model_sptr& xgraph_geom);

  //: Parse bxml_element to xgraph geom model - version 2
  static bool parse_v2_1(bxml_element* root, bxml_element* param_root, dbsks_xgraph_geom_model_sptr& xgraph_geom);

private:
  dbsks_xio_xgraph_geom_model(){};
};









//=============================================================================
// SUPPORTING FUNCTIONS - DEFINITIONS
//=============================================================================

//------------------------------------------------------------------------------
//: Create a new xml element for a xgraph_geom_model - version 1
bxml_element* dbsks_xio_xgraph_geom_model::
new_v1(const dbsks_xgraph_geom_model_sptr& xgraph_geom)
{
  bxml_element *root = new bxml_element("dbsks_xgraph_geom_model");

  root->set_attribute("version", "1");
  root->set_attribute("root_vid", xgraph_geom->root_vid());
  root->set_attribute("major_child_eid", xgraph_geom->major_child_eid());

  // Prepare space to store the xgraph geometry model
  vcl_map<unsigned int, dbsks_xfrag_geom_model_sptr >& map_edge2geom = xgraph_geom->map_edge2geom();
  vcl_map<unsigned int, dbsks_xnode_geom_model_sptr >& map_node2geom = xgraph_geom->map_node2geom();


  for (vcl_map<unsigned, dbsks_xfrag_geom_model_sptr >::iterator mit = map_edge2geom.begin(); 
    mit != map_edge2geom.end(); ++mit)
  {
    // edge id
    unsigned eid = mit->first;
    dbsks_xfrag_geom_model_sptr xfrag_geom_model = mit->second;

    // sample other parameters
    double min_psi_start, max_psi_start;
    double min_r_start, max_r_start;
    double min_phi_start, max_phi_start;
    double min_alpha_start, max_alpha_start;
    double min_chord, max_chord;
    double min_dpsi, max_dpsi;
    double min_r_end, max_r_end;
    double min_phi_end, max_phi_end;
    double graph_size;

    xfrag_geom_model->get_param_range(min_psi_start, max_psi_start,
                min_r_start, max_r_start,
                min_phi_start, max_phi_start,
                min_alpha_start, max_alpha_start,
                min_chord, max_chord,
                min_dpsi, max_dpsi,
                min_r_end, max_r_end,
                min_phi_end, max_phi_end,
                graph_size);

    // write stats of each fragment to XML file
    bxml_element* xfrag_elm = new bxml_element("dbsks_xfrag_geom_model");
    root->append_data(xfrag_elm);
    xfrag_elm->set_attribute("edge_id", eid); // "id"

    // start
    xfrag_elm->set_attribute("min_psi_start", min_psi_start);
    xfrag_elm->set_attribute("max_psi_start", max_psi_start);
    
    xfrag_elm->set_attribute("min_r_start", min_r_start);
    xfrag_elm->set_attribute("max_r_start", max_r_start);
    
    xfrag_elm->set_attribute("min_phi_start", min_phi_start);
    xfrag_elm->set_attribute("max_phi_start", max_phi_start);

    // end relative to start
    xfrag_elm->set_attribute("min_alpha_start", min_alpha_start);
    xfrag_elm->set_attribute("max_alpha_start", max_alpha_start);

    xfrag_elm->set_attribute("min_chord", min_chord);
    xfrag_elm->set_attribute("max_chord", max_chord);

    xfrag_elm->set_attribute("min_dpsi", min_dpsi);
    xfrag_elm->set_attribute("max_dpsi", max_dpsi);

    // end
    xfrag_elm->set_attribute("min_r_end", min_r_end);
    xfrag_elm->set_attribute("max_r_end", max_r_end);

    xfrag_elm->set_attribute("min_phi_end", min_phi_end);
    xfrag_elm->set_attribute("max_phi_end", max_phi_end);

    xfrag_elm->set_attribute("graph_size", graph_size);
  }

  // node info
  for (vcl_map<unsigned, dbsks_xnode_geom_model_sptr >::iterator mit = 
    map_node2geom.begin(); mit != map_node2geom.end(); ++mit)
  {
    unsigned vid = mit->first;
    dbsks_xnode_geom_model_sptr xnode_geom_model = mit->second;

    // retrieve parameter range
    double min_psi, max_psi;
    double min_radius, max_radius;
    double min_phi, max_phi;
    double min_phi_diff, max_phi_diff;
    double graph_size;
    xnode_geom_model->get_param_range(min_psi, max_psi, min_radius, max_radius, 
      min_phi, max_phi, min_phi_diff, max_phi_diff, graph_size);

    // write stats of each fragment to XML file
    bxml_element* xnode_elm = new bxml_element("dbsks_xnode_geom_model");
    root->append_data(xnode_elm);
    xnode_elm->set_attribute("vertex_id", vid); // "id"

    xnode_elm->set_attribute("min_psi", min_psi);
    xnode_elm->set_attribute("max_psi", max_psi);

    xnode_elm->set_attribute("min_phi", min_phi);
    xnode_elm->set_attribute("max_phi", max_phi);

    xnode_elm->set_attribute("min_radius", min_radius);
    xnode_elm->set_attribute("max_radius", max_radius);

    xnode_elm->set_attribute("min_phi_diff", min_phi_diff);
    xnode_elm->set_attribute("max_phi_diff", max_phi_diff);
    
    xnode_elm->set_attribute("graph_size", graph_size);
  }
  return root;
};




//------------------------------------------------------------------------------
//: Create a new xml element for a xgraph_geom_model - version 2
// This version has all data in it
bxml_element* dbsks_xio_xgraph_geom_model::
new_v2(const dbsks_xgraph_geom_model_sptr& xgraph_geom)
{
  bxml_element *root = new bxml_element("dbsks_xgraph_geom_model");

  root->set_attribute("version", "2");

  root->set_attribute("root_vid", xgraph_geom->root_vid());
  root->set_attribute("major_child_eid", xgraph_geom->major_child_eid());
  root->set_attribute("graph_size", xgraph_geom->graph_size());

  // Prepare space to store the xgraph geometry model
  const vcl_map<unsigned int, dbsks_xfrag_geom_model_sptr >& map_edge2geom = xgraph_geom->map_edge2geom();
  const vcl_map<unsigned int, dbsks_xnode_geom_model_sptr >& map_node2geom = xgraph_geom->map_node2geom();

  for (vcl_map<unsigned, dbsks_xfrag_geom_model_sptr >::const_iterator mit = map_edge2geom.begin(); 
    mit != map_edge2geom.end(); ++mit)
  {
    // edge id
    unsigned eid = mit->first;
    dbsks_xfrag_geom_model_sptr xfrag_geom_model = mit->second;

    // sample other parameters
    double min_psi_start, max_psi_start;
    double min_r_start, max_r_start;
    double min_phi_start, max_phi_start;
    double min_alpha_start, max_alpha_start;
    double min_chord, max_chord;
    double min_dpsi, max_dpsi;
    double min_r_end, max_r_end;
    double min_phi_end, max_phi_end;
    double graph_size;

    xfrag_geom_model->get_param_range(min_psi_start, max_psi_start,
                min_r_start, max_r_start,
                min_phi_start, max_phi_start,
                min_alpha_start, max_alpha_start,
                min_chord, max_chord,
                min_dpsi, max_dpsi,
                min_r_end, max_r_end,
                min_phi_end, max_phi_end,
                graph_size);

    // write stats of each fragment to XML file
    bxml_element* xfrag_elm = new bxml_element("dbsks_xfrag_geom_model");
    root->append_data(xfrag_elm);
    xfrag_elm->set_attribute("edge_id", eid); // "id"

    // start
    xfrag_elm->set_attribute("min_psi_start", min_psi_start);
    xfrag_elm->set_attribute("max_psi_start", max_psi_start);
    
    xfrag_elm->set_attribute("min_r_start", min_r_start);
    xfrag_elm->set_attribute("max_r_start", max_r_start);
    
    xfrag_elm->set_attribute("min_phi_start", min_phi_start);
    xfrag_elm->set_attribute("max_phi_start", max_phi_start);

    // end relative to start
    xfrag_elm->set_attribute("min_alpha_start", min_alpha_start);
    xfrag_elm->set_attribute("max_alpha_start", max_alpha_start);

    xfrag_elm->set_attribute("min_chord", min_chord);
    xfrag_elm->set_attribute("max_chord", max_chord);

    xfrag_elm->set_attribute("min_dpsi", min_dpsi);
    xfrag_elm->set_attribute("max_dpsi", max_dpsi);

    // end
    xfrag_elm->set_attribute("min_r_end", min_r_end);
    xfrag_elm->set_attribute("max_r_end", max_r_end);

    xfrag_elm->set_attribute("min_phi_end", min_phi_end);
    xfrag_elm->set_attribute("max_phi_end", max_phi_end);

    xfrag_elm->set_attribute("graph_size", graph_size);

    // Output training data too
    bxml_element* elm_attr_data = dbxml_algos::append_element(xfrag_elm, new bxml_element("attr_data"));
    elm_attr_data->set_attribute("graph_size", xfrag_geom_model->graph_size_for_attr_data());
  
    const vcl_map<vcl_string, vcl_vector<double > >& attr_data = xfrag_geom_model->attr_data();
    for (vcl_map<vcl_string, vcl_vector<double > >::const_iterator iter = attr_data.begin();
      iter != attr_data.end(); ++iter)
    {
      elm_attr_data->append_data(xml_new(iter->first, iter->second));
    }
  }

  // node info
  for (vcl_map<unsigned, dbsks_xnode_geom_model_sptr >::const_iterator mit = 
    map_node2geom.begin(); mit != map_node2geom.end(); ++mit)
  {
    unsigned vid = mit->first;
    dbsks_xnode_geom_model_sptr xnode_geom_model = mit->second;

    // retrieve parameter range
    double min_psi, max_psi;
    double min_radius, max_radius;
    double min_phi, max_phi;
    double min_phi_diff, max_phi_diff;
    double graph_size;
    xnode_geom_model->get_param_range(min_psi, max_psi, min_radius, max_radius, 
      min_phi, max_phi, min_phi_diff, max_phi_diff, graph_size);

    // write stats of each fragment to XML file
    bxml_element* xnode_elm = new bxml_element("dbsks_xnode_geom_model");
    root->append_data(xnode_elm);
    xnode_elm->set_attribute("vertex_id", vid); // "id"

    xnode_elm->set_attribute("min_psi", min_psi);
    xnode_elm->set_attribute("max_psi", max_psi);

    xnode_elm->set_attribute("min_phi", min_phi);
    xnode_elm->set_attribute("max_phi", max_phi);

    xnode_elm->set_attribute("min_radius", min_radius);
    xnode_elm->set_attribute("max_radius", max_radius);

    xnode_elm->set_attribute("min_phi_diff", min_phi_diff);
    xnode_elm->set_attribute("max_phi_diff", max_phi_diff);
    
    xnode_elm->set_attribute("graph_size", graph_size);

    // Output training data too
    bxml_element* elm_attr_data = dbxml_algos::append_element(xnode_elm, new bxml_element("attr_data"));
    elm_attr_data->set_attribute("graph_size", xnode_geom_model->graph_size_for_attr_data());
  
    const vcl_map<vcl_string, vcl_vector<double > >& attr_data = xnode_geom_model->attr_data();
    for (vcl_map<vcl_string, vcl_vector<double > >::const_iterator iter = attr_data.begin();
      iter != attr_data.end(); ++iter)
    {
      elm_attr_data->append_data(xml_new(iter->first, iter->second));
    }
  }
  return root;
}







//------------------------------------------------------------------------------
//: Parse bxml_element to xgraph geom model - version 1
bool dbsks_xio_xgraph_geom_model::
parse_v1(bxml_element* root, dbsks_xgraph_geom_model_sptr& xgraph_geom)
{
  // Prepare space to store the xgraph geometry model
  xgraph_geom = new dbsks_xgraph_geom_model();
  vcl_map<unsigned int, dbsks_xfrag_geom_model_sptr >& map_edge2geom = xgraph_geom->map_edge2geom();
  vcl_map<unsigned int, dbsks_xnode_geom_model_sptr >& map_node2geom = xgraph_geom->map_node2geom();

  // clean up existing data
  map_edge2geom.clear();
  map_node2geom.clear();

  // Read graph tree info: root node and major child edge id
  unsigned root_vid = 0;
  root->get_attribute("root_vid", root_vid);

  unsigned major_child_eid = 0;
  root->get_attribute("major_child_eid", major_child_eid);

  xgraph_geom->set_root_vid(root_vid);
  xgraph_geom->set_major_child_eid(major_child_eid);

  // statistics of the fragments
  for (bxml_element::const_data_iterator it = root->data_begin();
    it != root->data_end(); ++it)
  {
    bxml_data_sptr data = *it;
    if (data->type() != bxml_data::ELEMENT)
    {
      continue;
    }

    bxml_element* elm = static_cast<bxml_element* >(data.ptr());
    if (elm->name() == "dbsks_xfrag_geom_model")
    {
      bxml_element* frag_elm = elm;

      unsigned int edge_id;
      frag_elm->get_attribute("edge_id", edge_id);

      double min_psi_start, max_psi_start;
      double min_r_start, max_r_start;
      double min_phi_start, max_phi_start;
      double min_alpha_start, max_alpha_start; 
      double min_chord, max_chord; 
      double min_dpsi, max_dpsi; 
      double min_r_end, max_r_end;
      double min_phi_end, max_phi_end; 
      double graph_size;

      if (!frag_elm->get_attribute("min_psi_start", min_psi_start) ||
        !frag_elm->get_attribute("max_psi_start", max_psi_start) ||

        !frag_elm->get_attribute("min_r_start", min_r_start) ||
        !frag_elm->get_attribute("max_r_start", max_r_start) ||

        !frag_elm->get_attribute("min_phi_start", min_phi_start) ||
        !frag_elm->get_attribute("max_phi_start", max_phi_start) ||

        !frag_elm->get_attribute("min_alpha_start", min_alpha_start) ||
        !frag_elm->get_attribute("max_alpha_start", max_alpha_start) ||

        !frag_elm->get_attribute("min_chord", min_chord) ||
        !frag_elm->get_attribute("max_chord", max_chord) ||

        !frag_elm->get_attribute("min_dpsi", min_dpsi) ||
        !frag_elm->get_attribute("max_dpsi", max_dpsi) ||

        !frag_elm->get_attribute("min_r_end", min_r_end) ||
        !frag_elm->get_attribute("max_r_end", max_r_end) ||

        !frag_elm->get_attribute("min_phi_end", min_phi_end) ||
        !frag_elm->get_attribute("max_phi_end", max_phi_end) ||

        !frag_elm->get_attribute("graph_size", graph_size)
      )
      {
        vcl_cout << "\nERROR: couldn't retrieving xfrag geom model params in " __FILE__ << vcl_endl;
        return false;
      }

      // construct a xfrag geomtric model
      dbsks_xfrag_geom_model_sptr xfrag_gm = new dbsks_xfrag_geom_model();
      xfrag_gm->set_param_range(min_psi_start, max_psi_start, 
        min_r_start, max_r_start, 
        min_phi_start, max_phi_start, 
        min_alpha_start, max_alpha_start, 
        min_chord, max_chord, 
        min_dpsi, max_dpsi, 
        min_r_end, max_r_end, 
        min_phi_end, max_phi_end,
        graph_size);
      map_edge2geom.insert(vcl_make_pair(edge_id, xfrag_gm));
    }
    else if (elm->name() == "dbsks_xnode_geom_model")
    {
      bxml_element* xnode_elm = elm;

      unsigned int vertex_id;
      xnode_elm->get_attribute("vertex_id", vertex_id);

      double min_psi, max_psi;
      double min_radius, max_radius;
      double min_phi, max_phi;
      double min_phi_diff, max_phi_diff;
      double graph_size;

      if (!xnode_elm->get_attribute("min_psi", min_psi) ||
        !xnode_elm->get_attribute("max_psi", max_psi) ||

        !xnode_elm->get_attribute("min_radius", min_radius) ||
        !xnode_elm->get_attribute("max_radius", max_radius) ||

        !xnode_elm->get_attribute("min_phi", min_phi) ||
        !xnode_elm->get_attribute("max_phi", max_phi) ||

        !xnode_elm->get_attribute("min_phi_diff", min_phi_diff) ||
        !xnode_elm->get_attribute("max_phi_diff", max_phi_diff) ||

        !xnode_elm->get_attribute("graph_size", graph_size))
      {
        vcl_cout << "\nERROR: couldn't retrieve xnode geom model param in " __FILE__ << vcl_endl;
        return false;
      }

      // construct a xfrag geomtric model
      dbsks_xnode_geom_model_sptr xnode_geom = new dbsks_xnode_geom_model();
      xnode_geom->set_param_range(min_psi, max_psi, min_radius, max_radius, 
        min_phi, max_phi, min_phi_diff, max_phi_diff, graph_size);
      map_node2geom.insert(vcl_make_pair(vertex_id, xnode_geom));
    }
  }

  return true;
}

//------------------------------------------------------------------------------
//: Parse bxml_element to xgraph geom model - version 2
bool dbsks_xio_xgraph_geom_model::
parse_v2(bxml_element* root, dbsks_xgraph_geom_model_sptr& xgraph_geom)
{
  // Prepare space to store the xgraph geometry model
  xgraph_geom = new dbsks_xgraph_geom_model();
  vcl_map<unsigned int, dbsks_xfrag_geom_model_sptr >& map_edge2geom = xgraph_geom->map_edge2geom();
  vcl_map<unsigned int, dbsks_xnode_geom_model_sptr >& map_node2geom = xgraph_geom->map_node2geom();

  // clean up existing data
  map_edge2geom.clear();
  map_node2geom.clear();

  // Read graph tree info: root node and major child edge id
  unsigned root_vid = 0;
  root->get_attribute("root_vid", root_vid);

  unsigned major_child_eid = 0;
  root->get_attribute("major_child_eid", major_child_eid);

  double graph_size;
  root->get_attribute("graph_size", graph_size);

  xgraph_geom->set_root_vid(root_vid);
  xgraph_geom->set_major_child_eid(major_child_eid);
  xgraph_geom->set_graph_size(graph_size);

  // statistics of the fragments
  for (bxml_element::const_data_iterator it = root->data_begin();
    it != root->data_end(); ++it)
  {
    bxml_element* elm = 0;
    if (elm = dbxml_algos::cast_to_element(*it, "dbsks_xfrag_geom_model"))
    {   
      bxml_element* frag_elm = elm;
      unsigned int edge_id;
      frag_elm->get_attribute("edge_id", edge_id);

      double min_psi_start, max_psi_start;
      double min_r_start, max_r_start;
      double min_phi_start, max_phi_start;
      double min_alpha_start, max_alpha_start; 
      double min_chord, max_chord; 
      double min_dpsi, max_dpsi; 
      double min_r_end, max_r_end;
      double min_phi_end, max_phi_end; 
      double graph_size;

      if (!frag_elm->get_attribute("min_psi_start", min_psi_start) ||
        !frag_elm->get_attribute("max_psi_start", max_psi_start) ||

        !frag_elm->get_attribute("min_r_start", min_r_start) ||
        !frag_elm->get_attribute("max_r_start", max_r_start) ||

        !frag_elm->get_attribute("min_phi_start", min_phi_start) ||
        !frag_elm->get_attribute("max_phi_start", max_phi_start) ||

        !frag_elm->get_attribute("min_alpha_start", min_alpha_start) ||
        !frag_elm->get_attribute("max_alpha_start", max_alpha_start) ||

        !frag_elm->get_attribute("min_chord", min_chord) ||
        !frag_elm->get_attribute("max_chord", max_chord) ||

        !frag_elm->get_attribute("min_dpsi", min_dpsi) ||
        !frag_elm->get_attribute("max_dpsi", max_dpsi) ||

        !frag_elm->get_attribute("min_r_end", min_r_end) ||
        !frag_elm->get_attribute("max_r_end", max_r_end) ||

        !frag_elm->get_attribute("min_phi_end", min_phi_end) ||
        !frag_elm->get_attribute("max_phi_end", max_phi_end) ||

        !frag_elm->get_attribute("graph_size", graph_size)
      )
      {
        vcl_cout << "\nERROR: couldn't retrieving xfrag geom model params in " __FILE__ << vcl_endl;
        return false;
      }

      // construct a xfrag geomtric model
      dbsks_xfrag_geom_model_sptr xfrag_gm = new dbsks_xfrag_geom_model();
      xfrag_gm->set_param_range(min_psi_start, max_psi_start, 
        min_r_start, max_r_start, 
        min_phi_start, max_phi_start, 
        min_alpha_start, max_alpha_start, 
        min_chord, max_chord, 
        min_dpsi, max_dpsi, 
        min_r_end, max_r_end, 
        min_phi_end, max_phi_end,
        graph_size);
      map_edge2geom.insert(vcl_make_pair(edge_id, xfrag_gm));

      // parse attr_data
      bxml_element* elm_attr_data = dbxml_algos::find_by_name(frag_elm, "attr_data");
      // if found, parse it!
      if (elm_attr_data)
      {
        // determine graph size used during training
        double graph_size_for_attr_data = 0;
        if (!elm_attr_data->get_attribute("graph_size", graph_size_for_attr_data))
          continue;
        xfrag_gm->set_graph_size_for_attr_data(graph_size_for_attr_data);

        // parse the recorded geometric attributes
        for (bxml_element::const_data_iterator idata = elm_attr_data->data_begin();
          idata != elm_attr_data->data_end(); ++idata)
        {
          if (!dbxml_algos::cast_to_element(*idata))
            continue;

          bxml_element* elm = dbxml_algos::cast_to_element(*idata);
          vcl_string type = "";
          if (!elm->get_attribute("type", type) || type != "vector+double")
            continue;

          vcl_string attr_name = elm->name();
          vcl_vector<double > attr_values;
          xml_parse(elm, attr_values);

          xfrag_gm->set_attr_data(attr_name, attr_values);
        }
      }

    }
    else if (elm = dbxml_algos::cast_to_element(*it, "dbsks_xnode_geom_model"))
    {
      bxml_element* xnode_elm = elm;

      unsigned int vertex_id;
      xnode_elm->get_attribute("vertex_id", vertex_id);

      double min_psi, max_psi;
      double min_radius, max_radius;
      double min_phi, max_phi;
      double min_phi_diff, max_phi_diff;
      double graph_size;

      if (!xnode_elm->get_attribute("min_psi", min_psi) ||
        !xnode_elm->get_attribute("max_psi", max_psi) ||

        !xnode_elm->get_attribute("min_radius", min_radius) ||
        !xnode_elm->get_attribute("max_radius", max_radius) ||

        !xnode_elm->get_attribute("min_phi", min_phi) ||
        !xnode_elm->get_attribute("max_phi", max_phi) ||

        !xnode_elm->get_attribute("min_phi_diff", min_phi_diff) ||
        !xnode_elm->get_attribute("max_phi_diff", max_phi_diff) ||

        !xnode_elm->get_attribute("graph_size", graph_size))
      {
        vcl_cout << "\nERROR: couldn't retrieve xnode geom model param in " __FILE__ << vcl_endl;
        return false;
      }

      // construct a xfrag geomtric model
      dbsks_xnode_geom_model_sptr xnode_geom = new dbsks_xnode_geom_model();
      xnode_geom->set_param_range(min_psi, max_psi, min_radius, max_radius, 
        min_phi, max_phi, min_phi_diff, max_phi_diff, graph_size);
      map_node2geom.insert(vcl_make_pair(vertex_id, xnode_geom));
    }
  }

  return true;
}



//------------------------------------------------------------------------------
//: Parse bxml_element to xgraph geom model - version 2
bool dbsks_xio_xgraph_geom_model::
parse_v2_1(bxml_element* root, bxml_element* param_root, dbsks_xgraph_geom_model_sptr& xgraph_geom)
{

    vcl_cout << vcl_endl << "Geom Model Parameters:" << vcl_endl;
      vcl_cout << "-------------------------" << vcl_endl;
  double nkdiff_threshold_param;
  bxml_element* elm_nkdiff_threshold = dbxml_algos::find_by_name(param_root, "nkdiff_threshold");
  if (!elm_nkdiff_threshold)
    {
      vcl_cout << "\nERROR: Missing '" << "nkdiff_threshold" << "' element in XML file.\n";
      return false;
    }
    else
    {
        xml_parse(elm_nkdiff_threshold, nkdiff_threshold_param);
        vcl_cout << "Geom Nkdiff Threshold: " << nkdiff_threshold_param << vcl_endl;
    }


  // Prepare space to store the xgraph geometry model
  xgraph_geom = new dbsks_xgraph_geom_model();
  vcl_map<unsigned int, dbsks_xfrag_geom_model_sptr >& map_edge2geom = xgraph_geom->map_edge2geom();
  vcl_map<unsigned int, dbsks_xnode_geom_model_sptr >& map_node2geom = xgraph_geom->map_node2geom();

  // clean up existing data
  map_edge2geom.clear();
  map_node2geom.clear();

  // Read graph tree info: root node and major child edge id
  unsigned root_vid = 0;
  root->get_attribute("root_vid", root_vid);

  unsigned major_child_eid = 0;
  root->get_attribute("major_child_eid", major_child_eid);

  double graph_size;
  root->get_attribute("graph_size", graph_size);

  xgraph_geom->set_root_vid(root_vid);
  xgraph_geom->set_major_child_eid(major_child_eid);
  xgraph_geom->set_graph_size(graph_size);

  // statistics of the fragments
  for (bxml_element::const_data_iterator it = root->data_begin();
    it != root->data_end(); ++it)
  {
    bxml_element* elm = 0;
    if (elm = dbxml_algos::cast_to_element(*it, "dbsks_xfrag_geom_model"))
    {
      bxml_element* frag_elm = elm;
      unsigned int edge_id;
      frag_elm->get_attribute("edge_id", edge_id);

      double min_psi_start, max_psi_start;
      double min_r_start, max_r_start;
      double min_phi_start, max_phi_start;
      double min_alpha_start, max_alpha_start;
      double min_chord, max_chord;
      double min_dpsi, max_dpsi;
      double min_r_end, max_r_end;
      double min_phi_end, max_phi_end;
      double graph_size;

      if (!frag_elm->get_attribute("min_psi_start", min_psi_start) ||
        !frag_elm->get_attribute("max_psi_start", max_psi_start) ||

        !frag_elm->get_attribute("min_r_start", min_r_start) ||
        !frag_elm->get_attribute("max_r_start", max_r_start) ||

        !frag_elm->get_attribute("min_phi_start", min_phi_start) ||
        !frag_elm->get_attribute("max_phi_start", max_phi_start) ||

        !frag_elm->get_attribute("min_alpha_start", min_alpha_start) ||
        !frag_elm->get_attribute("max_alpha_start", max_alpha_start) ||

        !frag_elm->get_attribute("min_chord", min_chord) ||
        !frag_elm->get_attribute("max_chord", max_chord) ||

        !frag_elm->get_attribute("min_dpsi", min_dpsi) ||
        !frag_elm->get_attribute("max_dpsi", max_dpsi) ||

        !frag_elm->get_attribute("min_r_end", min_r_end) ||
        !frag_elm->get_attribute("max_r_end", max_r_end) ||

        !frag_elm->get_attribute("min_phi_end", min_phi_end) ||
        !frag_elm->get_attribute("max_phi_end", max_phi_end) ||

        !frag_elm->get_attribute("graph_size", graph_size)
      )
      {
        vcl_cout << "\nERROR: couldn't retrieving xfrag geom model params in " __FILE__ << vcl_endl;
        return false;
      }

      // construct a xfrag geomtric model
      dbsks_xfrag_geom_model_sptr xfrag_gm = new dbsks_xfrag_geom_model();
      xfrag_gm->set_param_range(min_psi_start, max_psi_start,
        min_r_start, max_r_start,
        min_phi_start, max_phi_start,
        min_alpha_start, max_alpha_start,
        min_chord, max_chord,
        min_dpsi, max_dpsi,
        min_r_end, max_r_end,
        min_phi_end, max_phi_end,
        graph_size);
      xfrag_gm->set_nkdiff_threshold(nkdiff_threshold_param);
      map_edge2geom.insert(vcl_make_pair(edge_id, xfrag_gm));

      // parse attr_data
      bxml_element* elm_attr_data = dbxml_algos::find_by_name(frag_elm, "attr_data");
      // if found, parse it!
      if (elm_attr_data)
      {
        // determine graph size used during training
        double graph_size_for_attr_data = 0;
        if (!elm_attr_data->get_attribute("graph_size", graph_size_for_attr_data))
          continue;
        xfrag_gm->set_graph_size_for_attr_data(graph_size_for_attr_data);

        // parse the recorded geometric attributes
        for (bxml_element::const_data_iterator idata = elm_attr_data->data_begin();
          idata != elm_attr_data->data_end(); ++idata)
        {
          if (!dbxml_algos::cast_to_element(*idata))
            continue;

          bxml_element* elm = dbxml_algos::cast_to_element(*idata);
          vcl_string type = "";
          if (!elm->get_attribute("type", type) || type != "vector+double")
            continue;

          vcl_string attr_name = elm->name();
          vcl_vector<double > attr_values;
          xml_parse(elm, attr_values);

          xfrag_gm->set_attr_data(attr_name, attr_values);
        }
      }

    }
    else if (elm = dbxml_algos::cast_to_element(*it, "dbsks_xnode_geom_model"))
    {
      bxml_element* xnode_elm = elm;

      unsigned int vertex_id;
      xnode_elm->get_attribute("vertex_id", vertex_id);

      double min_psi, max_psi;
      double min_radius, max_radius;
      double min_phi, max_phi;
      double min_phi_diff, max_phi_diff;
      double graph_size;

      if (!xnode_elm->get_attribute("min_psi", min_psi) ||
        !xnode_elm->get_attribute("max_psi", max_psi) ||

        !xnode_elm->get_attribute("min_radius", min_radius) ||
        !xnode_elm->get_attribute("max_radius", max_radius) ||

        !xnode_elm->get_attribute("min_phi", min_phi) ||
        !xnode_elm->get_attribute("max_phi", max_phi) ||

        !xnode_elm->get_attribute("min_phi_diff", min_phi_diff) ||
        !xnode_elm->get_attribute("max_phi_diff", max_phi_diff) ||

        !xnode_elm->get_attribute("graph_size", graph_size))
      {
        vcl_cout << "\nERROR: couldn't retrieve xnode geom model param in " __FILE__ << vcl_endl;
        return false;
      }

      // construct a xfrag geomtric model
      dbsks_xnode_geom_model_sptr xnode_geom = new dbsks_xnode_geom_model();
      xnode_geom->set_param_range(min_psi, max_psi, min_radius, max_radius,
        min_phi, max_phi, min_phi_diff, max_phi_diff, graph_size);
      map_node2geom.insert(vcl_make_pair(vertex_id, xnode_geom));
    }
  }

  return true;
}





// ============================================================================
// WRITE
// ============================================================================

// -----------------------------------------------------------------------------
//: Write xgraph geometry model to a stream
bool x_write(vcl_ostream& os, const dbsks_xgraph_geom_model_sptr& xgraph_geom)
{
  // start a new XML document
  bxml_document doc;

  // write - version 1
  bxml_element *root = dbsks_xio_xgraph_geom_model::new_v2(xgraph_geom);
  doc.set_root_element(root);
  bxml_write(os, doc);
  return true;
}




//=============================================================================
// READ
//=============================================================================


// -----------------------------------------------------------------------------
//: load geometric model of a xgraph
bool x_read(const vcl_string& filepath, dbsks_xgraph_geom_model_sptr& xgraph_geom)
{
  // House-cleaning
  xgraph_geom = 0;

  // load the whole XML file into memory
  bxml_document doc = bxml_read(filepath);

  if (!doc.root_element())
  {
    vcl_cout << "\nERROR: couldn't read XML file: " << filepath << vcl_endl;
    return false;
  }

  bxml_element* root = dbxml_algos::find_by_name(doc.root_element(), 
    "dbsks_xgraph_geom_model");

  if (!root)
  {
    vcl_cout << "ERROR: could not find node 'dbsks_xgraph_geom_model'.\n";
    return false;
  }
  return xml_parse(root, xgraph_geom);  
}

//: load geometric model of a xgraph
bool x_read(const vcl_string& filepath, const vcl_string& param_filepath, dbsks_xgraph_geom_model_sptr& xgraph_geom)
{
  // House-cleaning
  xgraph_geom = 0;

  // load the whole XML file into memory
  bxml_document doc = bxml_read(filepath);

  if (!doc.root_element())
  {
    vcl_cout << "\nERROR: couldn't read XML file: " << filepath << vcl_endl;
    return false;
  }

  bxml_element* root = dbxml_algos::find_by_name(doc.root_element(),
    "dbsks_xgraph_geom_model");

  if (!root)
  {
    vcl_cout << "ERROR: could not find node 'dbsks_xgraph_geom_model'.\n";
    return false;
  }

  // load the whole XML file into memory
    bxml_document param_doc = bxml_read(param_filepath);

    if (!param_doc.root_element())
    {
      vcl_cout << "\nERROR: couldn't read XML file: " << param_filepath << vcl_endl;
      return false;
    }

    bxml_element* param_root = dbxml_algos::find_by_name(param_doc.root_element(),
      "geom_params");

    if (!param_root)
    {
      vcl_cout << "ERROR: could not find node 'geom_params'.\n";
      return false;
    }


  //return xml_parse(root, xgraph_geom);
    return xml_parse(root, param_root, xgraph_geom);
}




//==============================================================================
// XML ELEMENT
//==============================================================================


//------------------------------------------------------------------------------
//: Create a new xml element for a xgraph_geom_model
bxml_element* xml_new(const dbsks_xgraph_geom_model_sptr& xgraph_geom, 
                      unsigned version)
{
  switch (version)
  {
  case 1:
    return dbsks_xio_xgraph_geom_model::new_v1(xgraph_geom);
  case 2:
    return dbsks_xio_xgraph_geom_model::new_v2(xgraph_geom);
  default:
    return 0;
  }
}


//------------------------------------------------------------------------------
//: Create a new xml element for a xgraph_geom_model
bool xml_parse(bxml_element* root, dbsks_xgraph_geom_model_sptr& xgraph_geom)
{
  unsigned version = 0;
  root->get_attribute("version", version);
  switch (version)
  {
  case 1:
    return dbsks_xio_xgraph_geom_model::parse_v1(root, xgraph_geom);
    break;
  case 2:
    return dbsks_xio_xgraph_geom_model::parse_v2(root, xgraph_geom);
    break;
  default:
    vcl_cout << "\nERROR: Unknown dbsks_xgraph_geom_model XML version. Nothing parsed.\n";
    return false;
  }
}

//: Create a new xml element for a xgraph_geom_model
bool xml_parse(bxml_element* root, bxml_element* param_root, dbsks_xgraph_geom_model_sptr& xgraph_geom)
{
  unsigned version = 0;
  root->get_attribute("version", version);
  switch (version)
  {
  case 1:
    return dbsks_xio_xgraph_geom_model::parse_v1(root, xgraph_geom);
    break;
  case 2:
    return dbsks_xio_xgraph_geom_model::parse_v2_1(root, param_root, xgraph_geom);
    break;
  default:
    vcl_cout << "\nERROR: Unknown dbsks_xgraph_geom_model XML version. Nothing parsed.\n";
    return false;
  }
}


