//this is /contrib/bm/dsm/io/dsm_io_features.cxx
#include"dsm_io_features.h"

void vsl_b_read(vsl_b_istream &is, dsm_features &features)
{
	features.b_read(is);
}

void vsl_b_write(vsl_b_ostream &os, dsm_features const& features)
{
	features.b_write(os);
}

void vsl_b_read(vsl_b_istream &is, dsm_features* &p)
{
	delete p;
	bool not_null_ptr;
	vsl_b_read(is, not_null_ptr);
	if(not_null_ptr)
	{
		p = new dsm_features();
		vsl_b_read(is,*p);
	}
	else
		p = 0;
}

void vsl_b_write(vsl_b_ostream &os, const dsm_features* p)
{
	if( p == 0 )
		vsl_b_write(os,false); //indicate null pointer stored
	else
	{
		vsl_b_write(os, true);//indicate non-null pointer stored
		vsl_b_write(os, *p);
	}
}

void vsl_print_summary(vcl_ostream& os, const dsm_features *p)
{
	os << "vsl_print_summary not yet implemented." << vcl_endl;
}