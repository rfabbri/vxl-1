//this is /contrib/bm/dsm/io/dsm_io_pixel_time_series_map.cxx
#include"dsm_io_pixel_time_series_map.h"

void vsl_b_read(vsl_b_istream &is, dsm_pixel_time_series_map &pixel_time_series_map)
{
	pixel_time_series_map.b_read(is);
}

void vsl_b_write(vsl_b_ostream &os, dsm_pixel_time_series_map const& pixel_time_series_map)
{
	pixel_time_series_map.b_write(os);
}

void vsl_b_read(vsl_b_istream &is, dsm_pixel_time_series_map* &p)
{
	delete p;
	bool not_null_ptr;
	vsl_b_read(is, not_null_ptr);
	if(not_null_ptr)
	{
		p = new dsm_pixel_time_series_map();
		vsl_b_read(is,*p);
	}
	else
		p = 0;
}

void vsl_b_write(vsl_b_ostream &os, const dsm_pixel_time_series_map* p)
{
	if( p == 0 )
		vsl_b_write(os,false); //indicate null pointer stored
	else
	{
		vsl_b_write(os, true);//indicate non-null pointer stored
		vsl_b_write(os, *p);
	}
}

void vsl_print_summary(vcl_ostream& os, const dsm_pixel_time_series_map *p)
{
	os << "vsl_print_summary not yet implemented. " << vcl_flush;
}