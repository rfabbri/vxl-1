// This is bpro1/xio/bpro1_xio_parameters.h 

#ifndef bpro1_xio_parameters_h_
#define bpro1_xio_parameters_h_

//: 
// \file     
// \brief    XML I/O functions for parameters of a bpro1_process
// \author   Nhon Trinh (ntrinh@lems.brown.edu)
// \date     Jan 4, 2008

#include <vcl_string.h>
#include <vcl_iostream.h>
#include <dbsksp/dbsksp_shock_graph_sptr.h>
#include <bpro1/bpro1_parameters_sptr.h>

#include <bxml/bxml_document.h>

//: write a bpro1_parameters to an xml file
bool x_write(const vcl_string& filepath, 
             const bpro1_parameters_sptr& parameters,
             const vcl_string& name = "bpro1_parameters");

//: write parameters of a bpro1_process to a stream
bool x_write(vcl_ostream& os, const bpro1_parameters_sptr& parameters, 
             const vcl_string& name = "bpro1_parameters");

// append parameters as attributes to an element
bool x_write(bxml_element* elm, const bpro1_parameters_sptr& parameters);

// Load parameters from a file
// Only parameters already existed in ``parameters'' will be loaded. Others are ignored.
bool x_read(const vcl_string& filepath, bpro1_parameters_sptr& parameters,
            const vcl_string& name = "bpro1_parameters");

// Load parameters from an XML element
// Only parameters already existed in ``parameters'' will be loaded. Others are ignored.
bool x_read(const bxml_element* elm, bpro1_parameters_sptr& parameters);


#endif
