//this is /contrib/bm/dts/io/dts_io_time_series_base.h
#ifndef DTS_IO_TIME_SERIES_BASE_H_
#define DTS_IO_TIME_SERIES_BASE_H_
//:
// \file
// \brief Binary IO for dsm_manager_base class
//
// \author Brandon A. Mayer b.mayer1@gmail.com
//
// \date  October 9, 2011
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include<dts/dts_time_series_base.h>
#include<vsl/vsl_binary_io.h>

//: Binary save parameters to stream.
void vsl_b_read(vsl_b_istream &is, dts_time_series_base &time_series_base);
void vsl_b_write(vsl_b_ostream &os, dts_time_series_base const& time_series_base);
void vsl_b_read(vsl_b_istream &is, dts_time_series_base* &p);
void vsl_b_write(vsl_b_ostream &os, const dts_time_series_base* p);
void vsl_print_summary(vcl_ostream& os, const dts_time_series_base *p);

#endif //DTS_IO_TIME_SERIES_BASE_H_