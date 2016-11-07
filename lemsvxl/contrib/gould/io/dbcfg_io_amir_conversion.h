//      dbcfg_io_amir_conversion.h
#ifndef dbcfg_io_amir_conversion_h_
#define dbcfg_io_amir_conversion_h_

//:
// \file
// \conversion of a dbcfg_contour_fragment_graph to and from Amir's cfg
// \author Benjamin Gould
// \date 7/22/09
//
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_vector.h>
#include <structure/dbcfg_contour_fragment_graph.h>
#include <dbdet/sel/dbdet_edgel.h>
#include <dbdet/edge/dbdet_edgemap.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <dbdet/sel/dbdet_curve_fragment_graph.h>



// -------------------------
// functions
// -------------------------
void dbcfg_amir_to_cfg(dbcfg_contour_fragment_graph<double> &cfg, dbdet_edgemap_sptr edgemap, dbdet_curve_fragment_graph &curve_graph);

dbdet_edgemap_sptr dbcfg_cfg_to_amir(dbcfg_contour_fragment_graph<double> &cfg, dbdet_curve_fragment_graph &curve_graph, int depth = 1);



#endif dbcfg_io_amir_conversion_h_
// end dbcfg_io_amir_conversion.h

