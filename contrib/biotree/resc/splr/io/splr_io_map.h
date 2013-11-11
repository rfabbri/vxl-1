#if !defined(SPLR_IO_MAP_H_)
#define SPLR_IO_MAP_H_

#include <splr/splr_map.h>
#include <vsl/vsl_binary_io.h>

template< class K, class V>
  void vsl_b_write(vsl_b_ostream & os, const splr_map<K, V> & map);

 //: Binary-loads data into given map (on top of what is already there, which should be nothing)
template <class K, class V>
  void vsl_b_read(vsl_b_istream & is, splr_map<K, V> & map);

//should provide print_summary

#endif
