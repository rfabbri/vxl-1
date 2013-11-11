//this is /contrib/bm/dsm2/io/dsm2_io_manager_pixels.txx
#ifndef DSM2_IO_MANAGER_PIXELS_TXX_
#define DSM2_IO_MANAGER_PIXELS_TXX_

#include<dsm2/io/dsm2_io_manager_pixels.h>

template<class pixelT, class smT>
void vsl_b_read(vsl_b_istream &is, dsm2_manager_pixels<pixelT, smT> &manager)
{
	manager.b_read(is);
}

template<class pixelT, class smT>
void vsl_b_write(vsl_b_ostream &os, dsm2_manager_pixels<pixelT, smT> const& manager)
{
	manager.b_write(os);
}

template<class pixelT, class smT>
void vsl_b_read(vsl_b_istream &is, dsm2_manager_pixels<pixelT, smT>* &p)
{
	delete p;
	bool not_null_ptr;
	vsl_b_read(is, not_null_ptr);
	if(not_null_ptr)
	{
		p = new dsm2_manager_pixels<pixelT, smT>();
		vsl_b_read(is,*p);
	}
	else
		p = 0;
}

template<class pixelT, class smT>
void vsl_b_write(vsl_b_ostream &os, const dsm2_manager_pixels<pixelT, smT>* p)
{
	if( p == 0 )
		vsl_b_write(os,false); //indicate null pointer stored
	else
	{
		vsl_b_write(os, true);//indicate non-null pointer stored
		vsl_b_write(os, *p);
	}
}

template<class pixelT, class smT>
void vsl_print_summary(vcl_ostream& os, const dsm2_manager_pixels<pixelT, smT> *p)
{
	os << "vsl_print_summary not yet implemented. " << vcl_flush;
}

#define DSM2_IO_MANAGER_PIXELS_INSTANTIATE(...)\
template void vsl_b_read(vsl_b_istream &is, dsm2_manager_pixels<__VA_ARGS__> &manager);\
template void vsl_b_write(vsl_b_ostream &os, dsm2_manager_pixels<__VA_ARGS__> const& manager);\
template void vsl_b_read(vsl_b_istream &is, dsm2_manager_pixels<__VA_ARGS__>* &p);\
template void vsl_b_write(vsl_b_ostream &os, const dsm2_manager_pixels<__VA_ARGS__>* p);\
template void vsl_print_summary(vcl_ostream& os, const dsm2_manager_pixels<__VA_ARGS__> *p)

#endif //DSM2_IO_MANAGER_PIXELS_TXX_