//      dbcfg_io_cem_conversion.h
#ifndef dbcfg_io_cem_conversion_h_
#define dbcfg_io_cem_conversion_h_

//:
// \file
// \conversion of a dbcfg_contour_fragment_graph to and from a .cem file
// \author Benjamin Gould
// \date 7/27/09
//
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_fstream.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_cstring.h>
#include <structure/dbcfg_contour_fragment_graph.h>
#include <dbdet/sel/dbdet_edgel.h>



// -------------------------
// functions
// -------------------------
void dbcfg_cem_to_cfg(dbcfg_contour_fragment_graph<double> &cfg, vcl_string filename);

void dbcfg_cfg_to_cem(dbcfg_contour_fragment_graph<double> &cfg, vcl_string filename, int depth = 1);



#endif dbcfg_io_cem_conversion_h_
// end dbcfg_io_cem_conversion.h

