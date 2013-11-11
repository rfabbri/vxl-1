//this is /contrib/bm/dsm2/dsm2_io_state_machine.txx
#ifndef DSM2_IO_STATE_MACHINE_TXX_
#define DSM2_IO_STATE_MACHINE_TXX_

#include<dsm2/io/dsm2_io_state_machine.h>

template<class nodeT, class transitionT>
void vsl_b_read(vsl_b_istream &is, dsm2_state_machine<nodeT, transitionT> &state_machine)
{
	state_machine.b_read(is);
}

template<class nodeT, class transitionT>
void vsl_b_write(vsl_b_ostream &os, dsm2_state_machine<nodeT, transitionT> const& state_machine)
{
	state_machine.b_write(os);
}

template<class nodeT, class transitionT>
void vsl_b_read(vsl_b_istream &is, dsm2_state_machine<nodeT, transitionT>* &p)
{
	delete p;
	bool not_null_ptr;
	vsl_b_read(is, not_null_ptr);
	if(not_null_ptr)
	{
		p = new dsm2_state_machine<nodeT, transitionT>();
		vsl_b_read(is,*p);
	}
	else
		p = 0;
}

template<class nodeT, class transitionT>
void vsl_b_write(vsl_b_ostream &os, const dsm2_state_machine<nodeT, transitionT>* p)
{
	if( p == 0 )
		vsl_b_write(os,false); //indicate null pointer stored
	else
	{
		vsl_b_write(os, true);//indicate non-null pointer stored
		vsl_b_write(os, *p);
	}
}

template<class nodeT, class transitionT>
void vsl_print_summary(vcl_ostream& os, const dsm2_state_machine<nodeT, transitionT> *p)
{
	os << "vsl_print_summary not yet implemented. " << vcl_flush;
}

#define DSM2_IO_STATE_MACHINE_INSTANTIATE(...)\
template void vsl_b_read(vsl_b_istream &is, dsm2_state_machine<__VA_ARGS__> &state_machine);\
template void vsl_b_write(vsl_b_ostream &os, dsm2_state_machine<__VA_ARGS__> const& state_machine);\
template void vsl_b_read(vsl_b_istream &is, dsm2_state_machine<__VA_ARGS__>* &p);\
template void vsl_b_write(vsl_b_ostream &os, const dsm2_state_machine<__VA_ARGS__>* p);\
template void vsl_print_summary(vcl_ostream& os, const dsm2_state_machine<__VA_ARGS__> *p)

#endif //DSM2_IO_STATE_MACHINE_TXX_