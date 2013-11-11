#if !defined(SPLR_IO_SYMMETRY_SPLAT_COLLECTION_TXX_)
#define SPLR_IO_SYMMETRY_SPLAT_COLLECTION_TXX_

#include "splr_io_symmetry_splat_collection.h"
#include <vsl/vsl_binary_io.h>

template<class T,
    class filter_2d_class = xmvg_composite_filter_2d<T> >
   void vsl_b_write(vsl_b_ostream & os, 
          const splr_symmetry_splat_collection<T, filter_2d_class> & splat_collection){
     splat_collection.sub_splat_collection_->write(os);
  }

template<class T,
    class filter_2d_class = xmvg_composite_filter_2d<T> >
   void vsl_b_read(vsl_b_istream & is,
           splr_symmetry_splat_collection<T,filter_2d_class> & splat_collection){
        splat_collection.sub_splat_collection_->read(is);
  }

#endif

