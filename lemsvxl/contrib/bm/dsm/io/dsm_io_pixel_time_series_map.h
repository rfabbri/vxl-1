//this is /contrib/bm/dsm/io/dsm_io_pixel_time_series_map.h
#ifndef DSM_IO_PIXEL_TIME_SERIES_MAP_H_
#define DSM_IO_PIXEL_TIME_SERIES_MAP_H_
//:
// \file
// \brief Binary IO for dsm_pixel_time_series_map class
//
// \author Brandon A. Mayer b.mayer1@gmail.com
//
// \date  mAY 18, 2011
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
#include<vsl/vsl_binary_io.h>
#include<vcl_iostream.h>
#include"../dsm_pixel_time_series_map.h"

//: Binary save parameters to stream.
void vsl_b_read(vsl_b_istream &is, dsm_pixel_time_series_map &pixel_time_series_map);
void vsl_b_write(vsl_b_ostream &os, dsm_pixel_time_series_map const& pixel_time_series_map);
void vsl_b_read(vsl_b_istream &is, dsm_pixel_time_series_map* &p);
void vsl_b_write(vsl_b_ostream &os, const dsm_pixel_time_series_map* p);
void vsl_print_summary(vcl_ostream& os, const dsm_pixel_time_series_map *p);

#endif//DSM_IO_PIXEL_TIME_SERIES_MAP_H_