//this is /contrib/bm/dsm/io/dsm_io_ncn.cxx

#include"dsm_io_ncn.h"

void vsl_b_read(vsl_b_istream &is, dsm_ncn &ncn)
{
	ncn.b_read(is);
}

void vsl_b_write(vsl_b_ostream &os, dsm_ncn const& ncn)
{
	ncn.b_write(os);
}

void vsl_b_read(vsl_b_istream &is, dsm_ncn* &p)
{
	delete p;
	bool not_null_ptr;
	vsl_b_read(is, not_null_ptr);
	if(not_null_ptr)
	{
		p = new dsm_ncn();
		vsl_b_read(is,*p);
	}
	else
		p = 0;
}

void vsl_b_write(vsl_b_ostream &os, const dsm_ncn* p)
{
	if( p == 0 )
		vsl_b_write(os,false); //indicate null pointer stored
	else
	{
		vsl_b_write(os, true);//indicate non-null pointer stored
		vsl_b_write(os, *p);
	}
}

void vsl_print_summary(vcl_ostream& os, const dsm_ncn *p)
{
	os << "Dummy Impl";
}

//void write_xml( vcl_ostream& os, const dsm_ncn_sptr ncn_sptr )
//{
//	bxml_document doc;
//	
//	
//}//end write_xml