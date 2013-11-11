#if !defined(XMVG_IO_COMPOSITE_FILTER_2D_TXX_)
#define XMVG_IO_COMPOSITE_FILTER_2D_TXX_

#include "xmvg_io_composite_filter_2d.h"
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_vector_io.txx>

template<class T>
   void vsl_b_write(vsl_b_ostream & os, const xmvg_composite_filter_2d<T> & filter){
    const short io_version_no = 1;
    vsl_b_write(os, io_version_no);
    vsl_b_write(os, filter.filters_);
  }

template<class T>
  void vsl_b_read(vsl_b_istream & is, xmvg_composite_filter_2d<T> & filter){
  if(!is) return;

    short ver;
  
    vsl_b_read(is, ver);

    switch (ver)
    {
      case 1:
        vsl_b_read(is, filter.filters_);
        break;

      default:
        vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, xmvg_composite_filter_2d<T>&)\n"
               << "           Unknown version number "<< ver << '\n';
        is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
        return;
    }
  }

template<class T>
   void vsl_print_summary(vcl_ostream & os,const xmvg_composite_filter_2d<T> & p)
     {
       os << "Size: " << p.size() << vcl_endl;

       unsigned size = p.size();
  
       for(unsigned i = 0; i < size; i++){
         //xmvg_atomic_filter_2d<T> af = p.atomic_filter(i);
         //vsl_print_summary(os, af);
       }
   }
  

#define XMVG_IO_COMPOSITE_FILTER_2D_INSTANTIATE(T) \
template void vsl_b_write(vsl_b_ostream & , const xmvg_composite_filter_2d<T> &); \
template void vsl_b_read(vsl_b_istream & , xmvg_composite_filter_2d<T> &); \
template void vsl_print_summary(vcl_ostream & , const xmvg_composite_filter_2d<T> & ); \
VSL_VECTOR_IO_INSTANTIATE(xmvg_atomic_filter_2d<T > );

#endif
