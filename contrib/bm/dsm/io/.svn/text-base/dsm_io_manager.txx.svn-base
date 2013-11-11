//this is /contrib/bm/dsm/io/dsm_io_manager.txx
#ifndef DSM_IO_MANAGER_TXX_
#define DSM_IO_MANAGER_TXX_

#include<dsm/io/dsm_io_manager.h>

template<unsigned T>
void vsl_b_read(vsl_b_istream &is, dsm_manager<T>& manager)
{
    manager.b_read(is);
}//end vsl_b_read

template<unsigned T>
void vsl_b_write(vsl_b_ostream &os, dsm_manager<T> const& manager)
{
    manager.b_write(os);
}//end vsl_b_write

template<unsigned T>
void vsl_b_read(vsl_b_istream &is, dsm_manager<T>* &p)
{
    delete p;
	bool not_null_ptr;
	vsl_b_read(is, not_null_ptr);
	if(not_null_ptr)
	{
		p = new dsm_manager<T>();
		vsl_b_read(is,*p);
	}
	else
		p = 0;
}//end vsl_b_read

template<unsigned T>
void vsl_b_write(vsl_b_ostream &os, const dsm_manager<T>* p)
{
    if( p == 0 )
		vsl_b_write(os,false); //indicate null pointer stored
	else
	{
		vsl_b_write(os, true);//indicate non-null pointer stored
		vsl_b_write(os, *p);
    }
}//end vsl_b_write

template<unsigned T>
void vsl_print_summary(vcl_ostream &os, const dsm_manager<T>* p)
{
    os << "dsm_io_manager -- vsl_print_summary not implemented.\n";
}//emd vsl_print_summary

#define DSM_IO_MANAGER_INSTANTIATE(T)\
template void vsl_b_read(vsl_b_istream &is, dsm_manager<T>& manager);\
template void vsl_b_write(vsl_b_ostream &os, dsm_manager<T> const& manager);\
template void vsl_b_read(vsl_b_istream &is, dsm_manager<T>* &p);\
template void vsl_b_write(vsl_b_ostream &os, const dsm_manager<T>* p);\
template void vsl_print_summary(vcl_ostream &os, const dsm_manager<T>* p)

#endif //DSM_IO_MANAGER_TXX_
