//this is /contrib/bm/dsm2/dsm2_io_node_gaussian_full.txx
#ifndef DSM2_IO_GAUSSIAN_TXX_
#define DSM2_IO_GAUSSIAN_TXX_
#include<dsm2/io/dsm2_io_node_gaussian.h>

//: Binary save parameters to stream.
template<class gaussT>
void vsl_b_read(vsl_b_istream &is, dsm2_node_gaussian<gaussT> &node)
{
	node.b_read(is);
}

template<class gaussT>
void vsl_b_write(vsl_b_ostream &os, dsm2_node_gaussian<gaussT> const& node)
{
	node.b_write(os);
}

template<class gaussT>
void vsl_b_read(vsl_b_istream &is, dsm2_node_gaussian<gaussT>* &p)
{
	delete p;
	bool not_null_ptr;
	vsl_b_read(is, not_null_ptr);
	if(not_null_ptr)
	{
		p = new dsm2_node_gaussian<gaussT>;
		vsl_b_read(is,*p);
	}
	else
		p = 0;
}

template<class gaussT>
void vsl_b_write(vsl_b_ostream &os, const dsm2_node_gaussian<gaussT>* p)
{
	if( p == 0 )
		vsl_b_write(os,false); //indicate null pointer stored
	else
	{
		vsl_b_write(os, true);//indicate non-null pointer stored
		vsl_b_write(os, *p);
	}
}

template<class gaussT>
void vsl_print_summary(vcl_ostream& os, const dsm2_node_gaussian<gaussT> *p)
{
	os << "vsl_print_summary not yet implemented. " << vcl_flush;
}

#define DSM2_IO_NODE_GAUSSIAN_INSTANTIATE(...)\
template void vsl_b_read(vsl_b_istream &is, dsm2_node_gaussian<__VA_ARGS__> &node);\
template void vsl_b_write(vsl_b_ostream &os, dsm2_node_gaussian<__VA_ARGS__> const& node);\
template void vsl_b_read(vsl_b_istream &is, dsm2_node_gaussian<__VA_ARGS__>* &p);\
template void vsl_b_write(vsl_b_ostream &os, const dsm2_node_gaussian<__VA_ARGS__>* p);\
template void vsl_print_summary(vcl_ostream& os, const dsm2_node_gaussian<__VA_ARGS__> *p)

#endif //DSM2_IO_NODE_GAUSSIAN_TXX_