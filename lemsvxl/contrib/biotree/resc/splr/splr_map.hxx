#if !defined(SPLR_MAP_TXX_)
#define SPLR_MAP_TXX_

#include <splr/splr_map.h>
#include <vsl/vsl_map_io.hxx>
#include <functional>

//Code for instantiation
#undef SPLR_MAP_INSTANTIATE
#define SPLR_MAP_INSTANTIATE(K, V) \
template class splr_map<K, V >;

#endif
