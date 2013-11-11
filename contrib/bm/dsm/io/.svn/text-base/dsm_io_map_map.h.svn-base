//this is /contrib/bm/dsm/io/dsm_io_map_map.h
#ifndef DSM_IO_MAP_MAP_H_
#define DSM_IO_MAP_MAP_H_
//:
// \file
// \brief binary IO functions for vcl_map<T, vcl_map<T, T, Compare>, Compare>
// \author B.A.Mayer
// \3/15/2011
#include<vcl_map.h>
#include<vsl/vsl_binary_io.h>


//=============================================================================
//: Write a map of maps to stream
template<class T1, class T2, class T3, class Compare1, class Compare2>
void vsl_b_write(vsl_b_ostream& os, 
                 vcl_map<T1, vcl_map<T2, T3, Compare2>, Compare1> const& v);

//=============================================================================
//: Read a map of maps from stream
template<class T1, class T2, class T3, class Compare1, class Compare2>
void vsl_b_read(vsl_b_istream& is, 
                vcl_map<T1, vcl_map<T2, T3, Compare2>, Compare1>& v);

template<class T1, class T2, class T3, class Compare1, class Compare2>
void vsl_print_summary( vcl_ostream& os, 
                        const vcl_map<T1, vcl_map<T2, T3, Compare2>, 
                        Compare1>& v);

#endif //DSM_IO_MAP_MAP_H_
