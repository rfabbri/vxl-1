//this is /contrib/bm/dts/io/dts_io_pixel_time_series.h
#ifndef DTS_IO_PIXEL_TIME_SERIES_H_
#define DTS_IO_PIXEL_TIME_SERIES_H_
#include<vsl/vsl_binary_io.h>
#include<dts/dts_pixel_time_series.h>

//: Binary save parameters to stream.

template<class pixelType, class timeType, class elementType, unsigned n >
void vsl_b_read(vsl_b_istream &is, 
    dts_pixel_time_series<pixelType, timeType, elementType, n> &pixel_time_series);

template<class pixelType, class timeType, class elementType, unsigned n >
void vsl_b_write(vsl_b_ostream &os, 
    dts_pixel_time_series<pixelType, timeType, elementType, n> const& pixel_time_series);

template<class pixelType, class timeType, class elementType, unsigned n >
void vsl_b_read(vsl_b_istream &is, 
    dts_pixel_time_series<pixelType, timeType, elementType, n> * &p);

template<class pixelType, class timeType, class elementType, unsigned n >
void vsl_b_write(vsl_b_ostream &os, 
    const dts_pixel_time_series<pixelType, timeType, elementType, n> * p);

template<class pixelType, class timeType, class elementType, unsigned n >
void vsl_print_summary(vcl_ostream& os, 
    const dts_pixel_time_series<pixelType, timeType, elementType, n> *p);

#endif //DTS_IO_PIXEL_TIME_SERIES_H_