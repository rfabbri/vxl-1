// This is dbsksp/xio/dbsks_xio_xgraph_geom.h 

#ifndef dbsks_xio_xgraph_geom_h_
#define dbsks_xio_xgraph_geom_h_

//: 
// \file     
// \brief    XML I/O functions for xshock geometry models
// \author   Nhon Trinh (ntrinh@lems.brown.edu)
// \date     August 16, 2009

#include <vcl_string.h>
#include <vcl_iostream.h>
#include <dbsks/dbsks_xgraph_geom_model_sptr.h>


// forward declaration to avoid including bxml_document.h
class bxml_element;

//------------------------------------------------------------------------------
// WRITE
//------------------------------------------------------------------------------
//: Write xgraph geometry model to a stream
bool x_write(vcl_ostream& os, const dbsks_xgraph_geom_model_sptr& xgraph_geom);


//------------------------------------------------------------------------------
// READ
//------------------------------------------------------------------------------
//: load geometric model of a xgraph
bool x_read(const vcl_string& filepath, dbsks_xgraph_geom_model_sptr& xgraph_geom);

bool x_read(const vcl_string& filepath, const vcl_string& param_filepath, dbsks_xgraph_geom_model_sptr& xgraph_geom);


//------------------------------------------------------------------------------
// XML ELEMENT
//------------------------------------------------------------------------------

//: Create a new xml element for a xgraph_geom_model
bxml_element* xml_new(const dbsks_xgraph_geom_model_sptr& xgraph_geom, 
                      unsigned version = 2);

//: Create a new xml element for a xgraph_geom_model
bool xml_parse(bxml_element* root, dbsks_xgraph_geom_model_sptr& xgraph_geom);

//: Create a new xml element for a xgraph_geom_model
bool xml_parse(bxml_element* root, bxml_element* param_root, dbsks_xgraph_geom_model_sptr& xgraph_geom);



#endif
