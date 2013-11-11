#if !defined(SPLR_IO_MAP_TXX_)
#define SPLR_IO_MAP_TXX_

#include "splr_io_map.h"
#include <vsl/vsl_pair_io.txx>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_map_io.h>

template <class K, class V>
 void vsl_b_write(vsl_b_ostream & os, const splr_map<K, V> & map){
    vsl_b_write(os, map.map_);
 }      

 //: Binary-loads data into given map (on top of what is already there, which should be nothing)
template <class K, class V>
 void vsl_b_read(vsl_b_istream & is, splr_map<K, V> & map){
  vsl_b_read(is, map.map_);
 }

#define SPLR_IO_MAP_INSTANTIATE(K, V) \
template void vsl_b_write(vsl_b_ostream & os, const splr_map<K, V> & map); \
template void vsl_b_read(vsl_b_istream & is, splr_map<K, V> & map); \
VSL_PAIR_IO_INSTANTIATE(K, V);

//previously had VSL_MAP_IO_INSTANTIATE(K, V, vcl_less<K>);--not sure why needed

#endif
