//this is /contrib/bm/dsm/io/dsm_io_map_map_map.h
#ifndef DSM_IO_MAP_MAP_MAP_H_
#define DSM_IO_MAP_MAP_MAP_H_
//:
// \file
// \brief binary IO functions for 
//   std::map<T1, std::map<T2, std::map<T3, T4, Compare>, Compare>, Compare>
// \author Brandon A. Mayer
// \3/24/2008
#include<map>
#include<vsl/vsl_binary_io.h>

//=============================================================================
//: Write a map of maps of maps to stream
template<class T1, class T2, class T3, class T4,
         class Compare1, class Compare2, class Compare3>
void vsl_b_write( vsl_b_ostream& os,
                  std::map<T1, std::map<T2, 
                  std::map<T3, T4, Compare3>, Compare2>, Compare1> const& m );

//=============================================================================
//: Read a map of maps from stream
template<class T1, class T2, class T3, class T4,
         class Compare1, class Compare2, class Compare3>
void vsl_b_read(vsl_b_istream& is,
                  std::map<T1, std::map<T2, 
                  std::map<T3, T4, Compare3>, Compare2>, Compare1>& m );

template<class T1, class T2, class T3, class T4,
         class Compare1, class Compare2, class Compare3>
void vsl_print_summary( std::ostream& os, 
                        std::map<T1, std::map<T2, 
                        std::map<T3, T4, Compare3>, Compare2>, Compare1>& m);

#endif //DSM_IO_MAP_MAP_MAP_H_
