//      dbcfg_io_amir_conversion.cxx

// Implementation for conversion between dbcfg_contour_fragment_graph and Amir's cfg
// Benjamin Gould
// 7/22/09

#include <vcl_list.h>
#include <vcl_deque.h>
#include "dbcfg_io_amir_conversion.h"



void dbcfg_amir_to_cfg(dbcfg_contour_fragment_graph<double> &cfg, dbdet_edgemap_sptr edgemap, dbdet_curve_fragment_graph &curve_graph) {
  cfg.reset((*edgemap).width(), (*edgemap).height());
  vcl_list<dbdet_edgel_chain* > chains = curve_graph.frags;
  for (vcl_list<dbdet_edgel_chain* >::iterator iter = chains.begin(); iter != chains.end(); iter++) {
    dbcfg_curve<double>* curve = new dbcfg_curve<double>(*iter, 1);
    cfg.add_curve(curve);
  }
}

dbdet_edgemap_sptr dbcfg_cfg_to_amir(dbcfg_contour_fragment_graph<double> &cfg, dbdet_curve_fragment_graph &curve_graph, int depth) {
  curve_graph.clear();
  dbdet_edgemap_sptr edgemap = new dbdet_edgemap((int) vcl_floor(cfg.width() + 0.5), (int) vcl_floor(cfg.height() + 0.5));

  vcl_vector<dbcfg_curve<double> * > all_curves = cfg.get_curves();
  for (vcl_vector<dbcfg_curve<double> * >::iterator iter = all_curves.begin(); iter < all_curves.end(); iter++) {
    if ((*iter)->exists_at(depth)) {
      dbdet_edgel_chain* echain = (*iter)->get_edgel_chain();
      curve_graph.insert_fragment(echain);
      for (vcl_deque<dbdet_edgel* >::iterator iter = echain->edgels.begin(); iter < echain->edgels.end(); iter++) {
        edgemap->insert(*iter);
      }
    }
  }
  return edgemap;
}

