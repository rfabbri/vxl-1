// This is dbsksp/xio/dbsks_xio_xgraph_ccm_model.h 

#ifndef dbsks_xio_xgraph_ccm_model_h_
#define dbsks_xio_xgraph_ccm_model_h_

//: 
// \file     
// \brief    XML I/O functions for xshock contour-chamfer-matching models
// \author   Nhon Trinh (ntrinh@lems.brown.edu)
// \date     July 12, 2009

#include <dbsks/dbsks_xgraph_ccm_model_sptr.h>
#include <vcl_string.h>

//==============================================================================
// WRITE
//==============================================================================

////:  Write xgraph contour ocm model to a stream
//bool x_write(vcl_ostream& os, const dbsks_xgraph_ccm_model_sptr& xgraph_ccm);


//==============================================================================
// READ
//==============================================================================

//: Read xgraph contour ocm model from a file
bool x_read(const vcl_string& filepath, dbsks_xgraph_ccm_model_sptr& xgraph_ccm);

//: Read xgraph contour ocm model from a file and also read ccm parameters from a separate file
bool x_read(const vcl_string& filepath, const vcl_string& param_filepath, dbsks_xgraph_ccm_model_sptr& xgraph_ccm);


#endif
