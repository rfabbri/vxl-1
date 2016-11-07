//this is /contrib/bm/dsm/dsm_io_time_series.cxx

#include"dsm_io_time_series.h"

void vsl_b_read(vsl_b_istream &is, dsm_time_series &time_series)
{
	time_series.b_read(is);
}

void vsl_b_write(vsl_b_ostream &os, dsm_time_series const& time_series)
{
	time_series.b_write(os);
}

void vsl_b_read(vsl_b_istream &is, dsm_time_series* &p)
{
	delete p;
	bool not_null_ptr;
	vsl_b_read(is, not_null_ptr);
	if(not_null_ptr)
	{
		p = new dsm_time_series();
		vsl_b_read(is,*p);
	}
	else
		p = 0;
}

void vsl_b_write(vsl_b_ostream &os, const dsm_time_series* p)
{
	if( p == 0 )
		vsl_b_write(os,false); //indicate null pointer stored
	else
	{
		vsl_b_write(os, true);//indicate non-null pointer stored
		vsl_b_write(os, *p);
	}
}

void vsl_print_summary(vcl_ostream& os, const dsm_time_series *p)
{
	os << "vsl_print_summary for dsm_time_series not yet implemented." << vcl_endl;
}