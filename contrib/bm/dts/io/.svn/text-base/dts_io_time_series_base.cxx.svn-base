//this is /contrib/bm/dts/io/dts_io_time_series_base.cxx

#include<dts/io/dts_io_time_series_base.h>

void vsl_b_read(vsl_b_istream &is, dts_time_series_base &time_series_base)
{
    time_series_base.b_read(is);
}

void vsl_b_write(vsl_b_ostream &os, dts_time_series_base const& time_series_base)
{
    time_series_base.b_write(os);
}
void vsl_b_read(vsl_b_istream &is, dts_time_series_base* &p)
{
    //delete p;
	bool not_null_ptr;
	vsl_b_read(is, not_null_ptr);
	if(not_null_ptr)
	{
		//p = new dts_time_series_base();
		vsl_b_read(is,*p);
	}
	else
		p = 0;
}

void vsl_b_write(vsl_b_ostream &os, const dts_time_series_base* p)
{
    if( p == 0 )
		vsl_b_write(os,false); //indicate null pointer stored
	else
	{
		vsl_b_write(os, true);//indicate non-null pointer stored
		vsl_b_write(os, *p);
    }
}

void vsl_print_summary(vcl_ostream& os, const dts_time_series_base *p)
{
    os << "dsm_io_pixel_time_series_base -- vsl_print_summary not implemented.\n";
}