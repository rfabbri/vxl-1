//      dbcfg_io_seg_conversion.cxx

// Implementation for conversion between dbcfg_contour_fragment_graph and Eli Fine's segmentation
// Benjamin Gould
// 7/22/09

#include "dbcfg_io_seg_conversion.h"



void dbcfg_seg_to_cfg(dbcfg_contour_fragment_graph<double> &cfg, dbseg_seg_object<double> &seg) {
  std::vector<dbseg_seg_spatial_object<double> * >& objects = seg.get_object_list();
  for (std::vector<dbseg_seg_spatial_object<double> * >::iterator seg_iter = objects.begin(); seg_iter < objects.end(); seg_iter++) {
    list<vgl_polygon<double> * > outlines = (*seg_iter)->get_outline();
  }
}

void dbcfg_cfg_to_seg(dbcfg_contour_fragment_graph<double> &cfg, dbseg_seg_object<double> &seg) {
}

