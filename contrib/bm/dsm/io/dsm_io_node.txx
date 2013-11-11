//this is /contrib/bm/dsm/io/dsm_io_node.txx
#ifndef DSM_IO_NODE_TXX_
#define DSM_IO_NODE_TXX_

#include"dsm_io_node.h"

//TEMPLATED FUNCTIONS FOR DERIVED dsm_node<T>
template<int T>
void vsl_b_read(vsl_b_istream &is, dsm_node<T>& node)
{
	node.b_read(is);
}//end vsl_b_read

template<int T>
void vsl_b_write(vsl_b_ostream& os, dsm_node<T> const& node)
{
	node.b_write(os);
}//end vsl_b_write

template<int T>
void vsl_b_read(vsl_b_istream& is, dsm_node<T>* &p)
{
    delete p;
	bool not_null_ptr;
	vsl_b_read(is, not_null_ptr);
	if(not_null_ptr)
	{
		p = new dsm_node<T>();
		vsl_b_read(is,*p);
	}
	else
		p = 0;
}//end vsl_b_read

template<int T>
void vsl_b_write(vsl_b_ostream &os, const dsm_node<T>* p)
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
void vsl_print_summary(vcl_ostream& os, const dsm_node<T>* p)
{
    os << "vsl_print_summary(vcl_ostream& os, const dsm_node<T>* p -- not yet implemented.\n";
}//end vsl_print_summary

#define DSM_IO_NODE_INSTANTIATE(T)\
template void vsl_b_read(vsl_b_istream &is, dsm_node<T>& node);\
template void vsl_b_write(vsl_b_ostream &os, dsm_node<T> const& node);\
template void vsl_b_read(vsl_b_istream &is, dsm_node<T>* &p);\
template void vsl_b_write(vsl_b_ostream &os, const dsm_node<T>* p);\
template void vsl_print_summary(vcl_ostream& os, const dsm_node<T>* p)

#endif //DSM_IO_NODE_TXX_