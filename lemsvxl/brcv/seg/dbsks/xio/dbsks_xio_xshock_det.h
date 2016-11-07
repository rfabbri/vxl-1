// This is dbsks/xio/dbsks_xio_xshock_det.h 

#ifndef dbsks_xio_xshock_det_h_
#define dbsks_xio_xshock_det_h_

//: 
// \file     
// \brief    XML I/O functions for xshock geometry and statistical models
// \author   Nhon Trinh (ntrinh@lems.brown.edu)
// \date     Nov 11, 2008


#include <dbsks/dbsks_xshock_det_record.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>


//==============================================================================
// CREATE
//==============================================================================

//: Create a template for a xshock detection record with all necessary fields
dbsks_xshock_det_record_sptr dbsks_xshock_det_record_new(const vcl_string& version = "3");




//==============================================================================
// WRITE
//==============================================================================

//: Write a list of xshock detection to a stream
bool x_write(vcl_string outfile, const vcl_vector<dbsks_xshock_det_record_sptr >& xshock_det_list);

//: Write a list of xshock detection to a stream
bool x_write(vcl_ostream& os, const vcl_vector<dbsks_xshock_det_record_sptr >& xshock_det_list);


//==============================================================================
// READ
//==============================================================================

//: load a list of xshock detection
bool x_read(const vcl_string& filepath, vcl_vector<dbsks_xshock_det_record_sptr >& xshock_det_list);


#endif
