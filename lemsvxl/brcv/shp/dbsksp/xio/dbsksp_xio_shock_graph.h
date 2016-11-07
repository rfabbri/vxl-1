// This is dbsksp/xio/dbsksp_xio_shock_graph.h 

#ifndef dbsksp_xio_shock_graph_h_
#define dbsksp_xio_shock_graph_h_

//: 
// \file     
// \brief    xml I/O functions for dbsksp_shock_graph class
// \author   Nhon Trinh (ntrinh@lems.brown.edu)
// \date     Feb 8, 2007

#include <vcl_string.h>
#include <vcl_iostream.h>
#include <dbsksp/dbsksp_shock_graph_sptr.h>


//: write a shock graph to an xml file
bool x_write(const vcl_string& filepath, const dbsksp_shock_graph_sptr& graph);

//: write a shock graph to a stream
bool x_write(vcl_ostream& os, const dbsksp_shock_graph_sptr& graph);

//: load a shock graph from an xml file
bool x_read(const vcl_string& filepath, dbsksp_shock_graph_sptr& graph);

#endif
