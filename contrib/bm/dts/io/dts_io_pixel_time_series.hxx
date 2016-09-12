#ifndef DTS_IO_PIXEL_TIME_SERIES_TXX_
#define DTS_IO_PIXEL_TIME_SERIES_TXX_

#include<dts/io/dts_io_pixel_time_series.h>

template<class pixelType, class timeType, class elementType, unsigned n>
void vsl_b_read(vsl_b_istream &is, 
    dts_pixel_time_series<pixelType, timeType, elementType, n> &pixel_time_series)
{
    pixel_time_series.b_read(is);
}

template<class pixelType, class timeType, class elementType, unsigned n >
void vsl_b_write(vsl_b_ostream &os, 
    dts_pixel_time_series<pixelType, timeType, elementType, n> const& pixel_time_series)
{
    pixel_time_series.b_write(os);
}

template<class pixelType, class timeType, class elementType, unsigned n >
void vsl_b_read(vsl_b_istream &is, 
    dts_pixel_time_series<pixelType, timeType, elementType, n> * &p)
{
    delete p;
	bool not_null_ptr;
	vsl_b_read(is, not_null_ptr);
	if(not_null_ptr)
	{
		p = new dts_pixel_time_series<pixelType, timeType, elementType, n>();
		vsl_b_read(is,*p);
	}
	else
		p = 0;
}

template<class pixelType, class timeType, class elementType, unsigned n >
void vsl_b_write(vsl_b_ostream &os, 
    const dts_pixel_time_series<pixelType, timeType, elementType, n> * p)
{
    if( p == 0 )
		vsl_b_write(os,false); //indicate null pointer stored
	else
	{
		vsl_b_write(os, true);//indicate non-null pointer stored
		vsl_b_write(os, *p);
	}
}

template<class pixelType, class timeType, class elementType, unsigned n >
void vsl_print_summary(vcl_ostream& os, 
    const dts_pixel_time_series<pixelType, timeType, elementType, n> *p)
{
    os << "vsl_print_summary not yet implemented. " << vcl_flush;
}

#define DTS_IO_PIXEL_TIME_SERIES_INSTANTIATE(pixelType,timeType,elementType,n)\
template void vsl_b_read(vsl_b_istream &is, dts_pixel_time_series<pixelType, timeType, elementType, n> &pixel_time_series);\
template void vsl_b_write(vsl_b_ostream &os, dts_pixel_time_series<pixelType, timeType, elementType, n> const& pixel_time_series);\
template void vsl_b_read(vsl_b_istream &is, dts_pixel_time_series<pixelType, timeType, elementType, n> * &p);\
template void vsl_b_write(vsl_b_ostream &os, const dts_pixel_time_series<pixelType, timeType, elementType, n> * p);\
template void vsl_print_summary(vcl_ostream& os, const dts_pixel_time_series<pixelType, timeType, elementType, n> *p)

#endif DTS_IO_PIXEL_TIME_SERIES_TXX_