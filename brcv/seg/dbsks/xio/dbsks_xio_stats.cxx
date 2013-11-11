// This is dbsksp/xio/dbsks_xio_stats.cxx


#include "dbsks_xio_stats.h"
#include <bxml/bxml_read.h>
#include <bxml/bxml_write.h>
#include <bxml/bxml_find.h>

#include <dbsksp/dbsksp_shock_graph.h>
#include <vcl_fstream.h>

// ============================================================================
// IMPLEMENTATION OF X_WRITE and X_READ
// ============================================================================


// ----------------------------------------------------------------------------
//: write a shock graph to an xml file
bool x_write(const vcl_string& filepath, const dbsks_shock_graph_stats& stats)
{
  vcl_ofstream file(filepath.c_str());
  if (x_write(file, stats))
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
bool x_write(vcl_ostream& os, const dbsks_shock_graph_stats& stats){
  bxml_document doc;
  bxml_element *root = new bxml_element("dbsks_shock_graph_stats");
  root->set_attribute("version", "1");
  root->set_attribute("graph_size", stats.graph_size());
  doc.set_root_element(root);

  // Edge list
  for (dbsksp_shock_graph::edge_iterator eit = stats.shock_graph()->edges_begin();
    eit != stats.shock_graph()->edges_end(); ++eit)
  {
    dbsksp_shock_edge_sptr e = *eit;

    // ignore terminal edges
    if (e->is_terminal_edge())
      continue;

    // statistics for each fragment
    bxml_element* frag_elm = new bxml_element("dbsks_shapelet_stats");
    root->append_data(frag_elm);

    frag_elm->set_attribute("edge_id", e->id()); // "id"
    
    // retrieve the statistics of the fragment
    dbsks_shapelet_stats frag_stats = stats.shapelet_stats(e->id());

    double mean_log2_rA, var_log2_rA, min_log2_rA, max_log2_rA;
    frag_stats.stats_log2_rA(mean_log2_rA, var_log2_rA, min_log2_rA, max_log2_rA);
    frag_elm->set_attribute("mean_log2_rA", mean_log2_rA);
    frag_elm->set_attribute("var_log2_rA", var_log2_rA);
    frag_elm->set_attribute("min_log2_rA", min_log2_rA);
    frag_elm->set_attribute("max_log2_rA", max_log2_rA);



    double mean_phiA, var_phiA, min_phiA, max_phiA;
    frag_stats.stats_phiA(mean_phiA, var_phiA, min_phiA, max_phiA);
    frag_elm->set_attribute("mean_phiA", mean_phiA);
    frag_elm->set_attribute("var_phiA", var_phiA);
    frag_elm->set_attribute("min_phiA", min_phiA);
    frag_elm->set_attribute("max_phiA", max_phiA);

    double mean_phiB, var_phiB, min_phiB, max_phiB;
    frag_stats.stats_phiB(mean_phiB, var_phiB, min_phiB, max_phiB);
    frag_elm->set_attribute("mean_phiB", mean_phiB);
    frag_elm->set_attribute("var_phiB", var_phiB);
    frag_elm->set_attribute("min_phiB", min_phiB);
    frag_elm->set_attribute("max_phiB", max_phiB);


    double mean_m, var_m, min_m, max_m;
    frag_stats.stats_m(mean_m, var_m, min_m, max_m);
    frag_elm->set_attribute("mean_m", mean_m);
    frag_elm->set_attribute("var_m", var_m);
    frag_elm->set_attribute("min_m", min_m);
    frag_elm->set_attribute("max_m", max_m);

    double mean_log2_len, var_log2_len, min_log2_len, max_log2_len;
    frag_stats.stats_log2_len(mean_log2_len, var_log2_len, min_log2_len, max_log2_len);
    frag_elm->set_attribute("mean_log2_len", mean_log2_len);
    frag_elm->set_attribute("var_log2_len", var_log2_len);
    frag_elm->set_attribute("min_log2_len", min_log2_len);
    frag_elm->set_attribute("max_log2_len", max_log2_len);
  }

  bxml_write(os, doc);
  return true;
}


// ============================================================================
// READ
// ============================================================================

// ----------------------------------------------------------------------------
//: load a shock graph from an xml file
bool x_read(const vcl_string& filepath, const dbsksp_shock_graph_sptr& graph, 
            dbsks_shock_graph_stats& stats)
{
  // clean up existing data
  stats.clear();
  stats.set_shock_graph(graph);

  bxml_document doc = bxml_read(filepath);
  

  bxml_data_sptr result = 
    bxml_find_by_name(doc.root_element(), bxml_element("dbsks_shock_graph_stats"));

  if (!result)
  {
    vcl_cout << "ERROR: could not find node 'dbsks_shock_graph_stats'.\n";
    return false;
  }

  bxml_element* root = static_cast<bxml_element* >(result.ptr());
  unsigned version = 0;
  root->get_attribute("version", version);
  if (version != 1)
  {
    vcl_cout << "ERROR: Unknown dbsks_shock_graph_stats XML version. Quit now.\n";
    return false;
  }

  // read graph size
  double graph_size = 0;
  root->get_attribute("graph_size", graph_size);
  stats.set_graph_size(graph_size);


  // statistics of the fragments
  for (bxml_element::const_data_iterator it = root->data_begin();
    it != root->data_end(); ++it)
  {
    bxml_data_sptr data = *it;
    if (data->type() != bxml_data::ELEMENT)
    {
      continue;
    }

    bxml_element* frag_elm = static_cast<bxml_element* >(data.ptr());
    if (frag_elm->name() != "dbsks_shapelet_stats")
    {
      continue;
    }

    unsigned int edge_id;
    frag_elm->get_attribute("edge_id", edge_id);

    double mean_log2_rA, var_log2_rA, min_log2_rA, max_log2_rA;
    frag_elm->get_attribute("mean_log2_rA", mean_log2_rA);
    frag_elm->get_attribute("var_log2_rA", var_log2_rA);
    frag_elm->get_attribute("min_log2_rA", min_log2_rA);
    frag_elm->get_attribute("max_log2_rA", max_log2_rA);

    double mean_phiA, var_phiA, min_phiA, max_phiA;
    frag_elm->get_attribute("mean_phiA", mean_phiA);
    frag_elm->get_attribute("var_phiA", var_phiA);
    frag_elm->get_attribute("min_phiA", min_phiA);
    frag_elm->get_attribute("max_phiA", max_phiA);

    double mean_phiB, var_phiB, min_phiB, max_phiB;
    frag_elm->get_attribute("mean_phiB", mean_phiB);
    frag_elm->get_attribute("var_phiB", var_phiB);
    frag_elm->get_attribute("min_phiB", min_phiB);
    frag_elm->get_attribute("max_phiB", max_phiB);

    double mean_m, var_m, min_m, max_m;
    frag_elm->get_attribute("mean_m", mean_m);
    frag_elm->get_attribute("var_m", var_m);
    frag_elm->get_attribute("min_m", min_m);
    frag_elm->get_attribute("max_m", max_m);

    double mean_log2_len, var_log2_len, min_log2_len, max_log2_len;
    frag_elm->get_attribute("mean_log2_len", mean_log2_len);
    frag_elm->get_attribute("var_log2_len", var_log2_len);
    frag_elm->get_attribute("min_log2_len", min_log2_len);
    frag_elm->get_attribute("max_log2_len", max_log2_len);

    dbsks_shapelet_stats frag_stats;
    frag_stats.set_stats_log2_rA(mean_log2_rA, var_log2_rA, min_log2_rA, max_log2_rA);
    frag_stats.set_stats_phiA(mean_phiA, var_phiA, min_phiA, max_phiA);
    frag_stats.set_stats_phiB(mean_phiB, var_phiB, min_phiA, max_phiA);
    frag_stats.set_stats_m(mean_m, var_m, min_m, max_m);
    frag_stats.set_stats_log2_len(mean_log2_len, var_log2_len, min_log2_len, max_log2_len);
    
    stats.set_shapelet_stats(edge_id, frag_stats);
  }
  return true;
}
//

