// This is dbdet/xio/dbdet_xio_seg3d_info.h 

#ifndef dbdet_xio_seg3d_info_h_
#define dbdet_xio_seg3d_info_h_

//: 
// \file     
// \brief    xml I/O functions for dbdet_seg3d_info class
// \author   Nhon Trinh (ntrinh@lems.brown.edu)
// \date     June 2, 2006

#include <string>
#include <iostream>
#include <dbdet/dbdet_seg3d_info.h>
#include <dbdet/dbdet_seg3d_info_sptr.h>


//: parse an xml file
bool x_read(std::string fname, const dbdet_seg3d_info_sptr& s);

//: write xml file
void x_write(std::ostream& os, const dbdet_seg3d_info_sptr& s);



#endif
