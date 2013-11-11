#if !defined(SPLR_IO_PER_ORBIT_INDEX_SPLAT_COLLECTION_TXX_)
#define SPLR_IO_PER_ORBIT_INDEX_SPLAT_COLLECTION_TXX_

#include "splr_io_per_orbit_index_splat_collection.h"
#include <vsl/vsl_binary_io.h>
#include "splr_io_map.h"
#include <xmvg/xmvg_io_composite_filter_2d.h>

template<class T, class F>
 void vsl_b_write(vsl_b_ostream & os, 
          const splr_per_orbit_index_splat_collection<T, F> & splat_collection){
         vsl_b_write(os, splat_collection.map_);
  }

template<class T, class F>
 void vsl_b_read(vsl_b_istream & os,
           splr_per_orbit_index_splat_collection<T, F> & splat_collection){
        vsl_b_read(os, splat_collection.map_);
  }

#define SPLR_IO_PER_ORBIT_INDEX_SPLAT_COLLECTION(T, F) \
template void vsl_b_write(vsl_b_ostream & os, const splr_per_orbit_index_splat_collection<T, F> & splat_collection); \
void vsl_b_read(vsl_b_istream & os, splr_per_orbit_index_splat_collection<T, F> & splat_collection); \

#endif
