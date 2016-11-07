//this is /contrib/bm/dsm/io/dsm_io_time_series.h
#ifndef DSM_IO_TIME_SERIES_H_
#define DSM_IO_TIME_SERIES_H_
//:
// \file
// \brief Binary IO for dsm_time_series class
//
// \author Brandon A. Mayer b.mayer1@gmail.com
//
// \date  May 12, 2011
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
#include<vsl/vsl_binary_io.h>
#include<vcl_iostream.h>
#include"../dsm_time_series.h"

//: Binary save parameters to stream.
void vsl_b_read(vsl_b_istream &is, dsm_time_series &times_series);
void vsl_b_write(vsl_b_ostream &os, dsm_time_series const& time_series);
void vsl_b_read(vsl_b_istream &is, dsm_time_series* &p);
void vsl_b_write(vsl_b_ostream &os, const dsm_time_series* p);
void vsl_print_summary(vcl_ostream& os, const dsm_time_series *p);

#endif//DSM_IO_TIME_SERIES_H_
