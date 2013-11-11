//this /contrib/bm/dsm2/dsm2_io_transition_map.txx
#ifndef DSM2_IO_TRANSITION_MAP_TXX_
#define DSM2_IO_TRANSITION_MAP_TXX_
#include<dsm2/io/dsm2_io_transition_map.h>

template<class T1, class T2, class T3>
void vsl_b_read(vsl_b_istream &is, dsm2_transition_map<T1,T2,T3> &transition_map)
{
	transition_map.b_read(is);
}

template<class T1, class T2, class T3>
void vsl_b_write(vsl_b_ostream &os, dsm2_transition_map<T1,T2,T3> const& transition_map)
{
	transition_map.b_write(os);
}

template<class T1, class T2, class T3>
void vsl_b_read(vsl_b_istream &is, dsm2_transition_map<T1,T2,T3>* &p)
{
	delete p;
	bool not_null_ptr;
	vsl_b_read(is, not_null_ptr);
	if(not_null_ptr)
	{
		p = new dsm2_transition_map<T1,T2,T3>();
		vsl_b_read(is,*p);
	}
	else
		p = 0;
}

template<class T1, class T2, class T3>
void vsl_b_write(vsl_b_ostream &os, const dsm2_transition_map<T1,T2,T3>* p)
{
	if( p == 0 )
		vsl_b_write(os,false); //indicate null pointer stored
	else
	{
		vsl_b_write(os, true);//indicate non-null pointer stored
		vsl_b_write(os, *p);
	}
}

template<class T1, class T2, class T3>
void vsl_print_summary(vcl_ostream& os, const dsm2_transition_map<T1,T2,T3> *p)
{
	os << "vsl_print_summary not yet implemented. " << vcl_flush;
}

#define DSM2_IO_TRANSITION_MAP_INSTANTIATE(T1,T2,T3)\
template void vsl_b_read(vsl_b_istream &is, dsm2_transition_map<T1,T2,T3> &transition_map);\
template void vsl_b_write(vsl_b_ostream &os, dsm2_transition_map<T1,T2,T3> const& transition_map);\
template void vsl_b_read(vsl_b_istream &is, dsm2_transition_map<T1,T2,T3>* &p);\
template void vsl_b_write(vsl_b_ostream &os, const dsm2_transition_map<T1,T2,T3>* p);\
template void vsl_print_summary(vcl_ostream& os, const dsm2_transition_map<T1,T2,T3> *p)

#endif //DSM2_IO_TRANSITION_MAP_TXX_