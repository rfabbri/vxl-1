// This is dbsks/xio/dbsks_xio_xshock_det.h 

#ifndef dbsks_xio_xshock_det_h_
#define dbsks_xio_xshock_det_h_

//: 
// \file     
// \brief    XML I/O functions for xshock geometry and statistical models
// \author   Nhon Trinh (ntrinh@lems.brown.edu)
// \date     Nov 11, 2008


#include <dbsks/dbsks_xshock_det_record.h>
#include <string>
#include <iostream>
#include <vector>


//==============================================================================
// CREATE
//==============================================================================

//: Create a template for a xshock detection record with all necessary fields
dbsks_xshock_det_record_sptr dbsks_xshock_det_record_new(const std::string& version = "3");




//==============================================================================
// WRITE
//==============================================================================

//: Write a list of xshock detection to a stream
bool x_write(std::string outfile, const std::vector<dbsks_xshock_det_record_sptr >& xshock_det_list);

//: Write a list of xshock detection to a stream
bool x_write(std::ostream& os, const std::vector<dbsks_xshock_det_record_sptr >& xshock_det_list);


//==============================================================================
// READ
//==============================================================================

//: load a list of xshock detection
bool x_read(const std::string& filepath, std::vector<dbsks_xshock_det_record_sptr >& xshock_det_list);


#endif
