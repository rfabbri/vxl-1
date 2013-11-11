// This is dbsksp/xio/dbsksp_xio_xshock_graph.h 

#ifndef dbsksp_xio_xshock_graph_h_
#define dbsksp_xio_xshock_graph_h_

//: 
// \file     
// \brief    xml I/O functions for dbsksp_xshock_graph class
// \author   Nhon Trinh (ntrinh@lems.brown.edu)
// \date     Sep 7, 2008

#include <vcl_string.h>
#include <vcl_iostream.h>
#include <dbsksp/dbsksp_xshock_graph_sptr.h>


//: write a shock graph to an xml file
bool x_write(const vcl_string& filepath, const dbsksp_xshock_graph_sptr& xg);

//: write a shock graph to a stream
bool x_write(vcl_ostream& os, const dbsksp_xshock_graph_sptr& xg);

//: load a shock graph from an xml file
bool x_read(const vcl_string& filepath, dbsksp_xshock_graph_sptr& xg);

#endif
