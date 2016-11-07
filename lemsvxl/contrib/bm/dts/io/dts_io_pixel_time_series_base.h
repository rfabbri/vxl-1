//this is /contrib/bm/dts/io/dts_io_pixel_time_series_base.h
#ifndef DTS_IO_PIXEL_TIME_SERIES_BASE_H_
#define DTS_IO_PIXEL_TIME_SERIES_BASE_H_
#include<vsl/vsl_binary_io.h>
#include<dts/dts_pixel_time_series_base_sptr.h>
#include<dts/dts_pixel_time_series.h>

//: Binary save parameters to stream.
void vsl_b_read(vsl_b_istream &is, dts_pixel_time_series_base &pixel_time_series_base);
void vsl_b_write(vsl_b_ostream &os, dts_pixel_time_series_base const& pixel_time_series_base);
void vsl_b_read(vsl_b_istream &is, dts_pixel_time_series_base* &p);
void vsl_b_write(vsl_b_ostream &os, const dts_pixel_time_series_base* p);
void vsl_print_summary(vcl_ostream& os, const dts_pixel_time_series_base *p);

#endif //DTS_IO_PIXEL_TIME_SERIES_BASE_H_