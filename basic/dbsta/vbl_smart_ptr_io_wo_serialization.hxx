#include <dbsta/vbl_smart_ptr_io_wo_serialization.h>
#include <vsl/vsl_binary_loader.h>
//: Binary save vbl_smart_ptr<dbsta_distribution<T> > to stream.
template <class T>
void vsl_b_write(vsl_b_ostream &os, const vbl_smart_ptr<T > &p)
    {
        vsl_b_write(os,p.ptr());
    }

//: Binary load vbl_smart_ptr<dbsta_distribution<T> > from stream.
template <class T>
void vsl_b_read(vsl_b_istream &is, vbl_smart_ptr<T > & p)
    {
    T *ptr=0;    
    vsl_b_read(is,ptr);
    p =ptr;
    }

#define VBL_SMART_PTR_WO_SERIALIZATION_INSTANTIATE(T) \
template void vsl_b_read(vsl_b_istream &, vbl_smart_ptr<T > &); \
template void vsl_b_write(vsl_b_ostream &, const vbl_smart_ptr<T > &) 

