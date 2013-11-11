//this is /contrib/bm/dsm/io/dsm_io_state_machine_base.cxx
#include"dsm_io_state_machine_base.h"
//FUNCTIONS FOR DSM_STATE_MACHINE_BASE
void vsl_b_read(vsl_b_istream& is, dsm_state_machine_base &state_machine_base)
{
	state_machine_base.b_read(is);
}

void vsl_b_write(vsl_b_ostream &os, dsm_state_machine_base const& state_machine_base)
{
	state_machine_base.b_write(os);
}

void vsl_b_read(vsl_b_istream &is, dsm_state_machine_base* &p)
{
	//delete p;
	bool not_null_ptr;
	vsl_b_read(is, not_null_ptr);
	if(not_null_ptr)
	{
		//p = new dsm_state_machine_base();
		vsl_b_read(is,*p);
	}
	else
		p = 0;
}

void vsl_b_write(vsl_b_ostream &os, const dsm_state_machine_base* p)
{
	if( p == 0 )
		vsl_b_write(os,false); //indicate null pointer stored
	else
	{
		vsl_b_write(os, true);//indicate non-null pointer stored
		vsl_b_write(os, *p);
	}
}

void vsl_print_summary(vcl_ostream& os, const dsm_state_machine_base *p)
{
	os << "dsm_io_manager -- vsl_print_summary not implemented.\n";
}