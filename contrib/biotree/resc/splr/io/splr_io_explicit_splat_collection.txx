#if !defined(SPLR_IO_EXPLICIT_SPLAT_COLLECTION_TXX_)
#define SPLR_IO_EXPLICIT_SPLAT_COLLECTION_TXX_

#include "splr_io_explicit_splat_collection.h"
#include "splr_io_map.txx"
#include <xmvg/io/xmvg_io_composite_filter_2d.h>
#include <vsl/vsl_binary_io.h>

template<class T, class F> void vsl_b_write(vsl_b_ostream & os, 
          const splr_explicit_splat_collection<T, F> & splat_collection){
         vsl_b_write(os, splat_collection.map_);
  }

template<class T, class F> void vsl_b_read(vsl_b_istream & is,
           splr_explicit_splat_collection<T, F> & splat_collection){
        vsl_b_read(is, splat_collection.map_);
  }

typedef vcl_pair<orbit_index, biob_worldpt_index> splr_io_explicit_splat_collection_h_key;

#define SPLR_IO_EXPLICIT_SPLAT_COLLECTION_INSTANTIATE(T, F) \
template void vsl_b_write(vsl_b_ostream & os, const splr_explicit_splat_collection<T, F> & splat_collection); \
template void vsl_b_read(vsl_b_istream & is, splr_explicit_splat_collection<T, F> & splat_collection);

#endif
