//this is /contrib/bm/dsm/io/dsm_io_state_machine.txx
#ifndef DSM_IO_STATE_MACHINE_TXX_
#define DSM_IO_STATE_MACHINE_TXX_
#include"dsm_io_state_machine.h"

////FUNCTIONS FOR DSM_STATE_MACHINE_BASE
//void vsl_b_read(vsl_b_istream& is, dsm_state_machine_base &state_machine_base)
//{
//	state_machine_base.b_read(is);
//}
//
//void vsl_b_write(vsl_b_ostream &os, dsm_state_machine_base const& state_machine_base)
//{
//	state_machine_base.b_write(os);
//}
//
//void vsl_b_read(vsl_b_istream &is, dsm_state_machine_base* &p)
//{
//	//delete p;
//	bool not_null_ptr;
//	vsl_b_read(is, not_null_ptr);
//	if(not_null_ptr)
//	{
//		//p = new dsm_state_machine_base();
//		vsl_b_read(is,*p);
//	}
//	else
//		p = 0;
//}
//
//void vsl_b_write(vsl_b_ostream &os, const dsm_state_machine_base* p)
//{
//	if( p == 0 )
//		vsl_b_write(os,false); //indicate null pointer stored
//	else
//	{
//		vsl_b_write(os, true);//indicate non-null pointer stored
//		vsl_b_write(os, *p);
//	}
//}
//
//void vsl_print_summary(vcl_ostream& os, const dsm_state_machine_base *p)
//{
//	os << "dsm_io_manager -- vsl_print_summary not implemented.\n";
//}


//TEMPLATED FUNCTIONS FOR DERIVED STATE MACHINE <T>
template<int T>
void vsl_b_read(vsl_b_istream &is, dsm_state_machine<T>& state_machine)
{
    state_machine.b_read(is);
}//end vsl_b_read

template<int T>
void vsl_b_write(vsl_b_ostream &os, dsm_state_machine<T> const& state_machine)
{
    state_machine.b_write(os);
}//end vsl_b_write

template<int T>
void vsl_b_read(vsl_b_istream &is, dsm_state_machine<T>* &p)
{
    delete p;
	bool not_null_ptr;
	vsl_b_read(is, not_null_ptr);
	if(not_null_ptr)
	{
		p = new dsm_state_machine<T>();
		vsl_b_read(is,*p);
	}
	else
		p = 0;
}//end vsl_b_reads

template<int T>
void vsl_b_write(vsl_b_ostream &os, const dsm_state_machine<T>* p)
{
    if( p == 0 )
		vsl_b_write(os,false); //indicate null pointer stored
	else
	{
		vsl_b_write(os, true);//indicate non-null pointer stored
		vsl_b_write(os, *p);
	}
}//end vsl_b_write

template<int T>
void vsl_print_summary(vcl_ostream& os, const dsm_state_machine<T>* p)
{
    os << "vsl_print_summary(vcl_ostream& os, const dsm_state_machine<T>* p -- not yet implemented.\n";
}//end vsl_print_summary

#define DSM_IO_STATE_MACHINE_INSTANTIATE(T)\
template void vsl_b_read(vsl_b_istream &is, dsm_state_machine<T>& state_machine);\
template void vsl_b_write(vsl_b_ostream &os, dsm_state_machine<T> const& state_machine);\
template void vsl_b_read(vsl_b_istream &is, dsm_state_machine<T>* &p);\
template void vsl_b_write(vsl_b_ostream &os, const dsm_state_machine<T>* p);\
template void vsl_print_summary(vcl_ostream& os, const dsm_state_machine<T>* p)

#endif //DSM_IO_STATE_MACHINE_TXX_