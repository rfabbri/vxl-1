#if !defined(SPLR_IO_SYMMETRY_SPLAT_COLLECTION_H_)
#define SPLR_IO_SYMMETRY_SPLAT_COLLECTION_H_

#include <splr/splr_symmetry_splat_collection.h>
#include <vsl/vsl_fwd.h>
//#include <vsl/vsl_binary_io.h>

template<class T,
    class filter_2d_class = xmvg_composite_filter_2d<T> >
   void vsl_b_write(vsl_b_ostream & os, 
        const splr_symmetry_splat_collection<T, filter_2d_class> & splat_collection);

template<class T,
    class filter_2d_class = xmvg_composite_filter_2d<T> >
  void vsl_b_read(vsl_b_istream & is,
         splr_symmetry_splat_collection<T,filter_2d_class> & splat_collection)

#endif
