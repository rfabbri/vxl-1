//this is /contrib/bm/dsm/io/dsm_io_frame_clock.cxx
#include"dsm_io_frame_clock.h"

void vsl_b_read(vsl_b_istream &is, dsm_frame_clock &frame_clock)
{
	frame_clock.b_read(is);

}

void vsl_b_write(vsl_b_ostream &os, dsm_frame_clock const& frame_clock)
{
	frame_clock.b_write(os);
}

void vsl_b_read(vsl_b_istream &is, dsm_frame_clock* &p)
{
	//delete p;
	bool not_null_ptr;
	vsl_b_read(is, not_null_ptr);
	if(not_null_ptr)
	{
		p = dsm_frame_clock::instance();
		vsl_b_read(is,*p);
	}
	else
		p = 0;
}

void vsl_b_write(vsl_b_ostream &os, const dsm_frame_clock* p)
{
	if( p == 0 )
		vsl_b_write(os,false); //indicate null pointer stored
	else
	{
		vsl_b_write(os, true);//indicate non-null pointer stored
		vsl_b_write(os, *p);
	}
}

void vsl_print_summary(vcl_ostream& os, const dsm_frame_clock *p)
{
}