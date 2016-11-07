// This is dbsks/xio/dbsks_xio_stats.h 

#ifndef dbsks_xio_stats_h_
#define dbsks_xio_stats_h_

//: 
// \file     
// \brief    xml I/O functions for statistics about shock graphs
// \author   Nhon Trinh (ntrinh@lems.brown.edu)
// \date     May 19, 2008

#include <vcl_string.h>
#include <vcl_iostream.h>
#include <dbsks/dbsks_shock_graph_stats.h>


//: write a shock graph to an xml file
bool x_write(const vcl_string& filepath, const dbsks_shock_graph_stats& stats);

//: write a shock graph to a stream
bool x_write(vcl_ostream& os, const dbsks_shock_graph_stats& stats);

//: load a shock graph from an xml file
bool x_read(const vcl_string& filepath, const dbsksp_shock_graph_sptr& graph, 
            dbsks_shock_graph_stats& stats);

#endif
