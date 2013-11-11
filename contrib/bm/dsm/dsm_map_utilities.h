#ifndef DSM_MAP_UTILITIES_H_
#define DSM_MAP_UTILITIES_H_

#include<vcl_map.h>
#include<vcl_string.h>
#include<vcl_utility.h>

#include<vsl/vsl_binary_io.h>

// \brief binary IO functions for vcl_map<T1, vcl_map<T2, T3, Compare2>, Compare3>
// \author B.A.Mayer
//====================================================================================
//: Write a map of maps to stream
template<class T1, class T2, class T3, class Compare1, class Compare2>
void vsl_b_write(vsl_b_ostream& os, vcl_map<T1, vcl_map<T2, T3, Compare2>, Compare1> const& v);
//====================================================================================
//: Read a map of maps from stream
template<class T1, class T2, class T3, class Compare1, class Compare2>
void vsl_b_read(vsl_b_istream& is, vcl_map<T1, vcl_map<T2, T3, Compare2>, Compare1>& v);
//====================================================================================
//: Print Summary
template<class T1, class T2, class T3, class Compare1, class Compare2>
void vsl_print_summary( vcl_ostream& os, const vcl_map<T1, vcl_map<T2, T3, Compare2>, Compare1>& v);


// \brief binary IO functions for vcl_map<T1, vcl_map< T2, vcl_map<T3, T4, Compare3>, Compare2>, Compare1>
//====================================================================================
//: Read a map of maps of maps from
template<class T1, class T2, class T3, class T4, class Compare1, class Compare2, class Compare3>
void vsl_b_read(vsl_b_istream& is, vcl_map<T1, vcl_map<T2, vcl_map<T3, T4, Compare3>, Compare2>, Compare1>& v);
//: Write a map of maps of maps to stream
template<class T1, class T2, class T3, class T4, class Compare1, class Compare2, class Compare3>
void vsl_b_write(vsl_b_ostream& os,vcl_map<T1, vcl_map<T2, vcl_map<T3, T4, Compare3>, Compare2>, Compare1> const& v);
//: Print summmary
template<class T1, class T2, class T3, class T4, class Compare1, class Compare2, class Compare3>
void vsl_print_summary( vcl_ostream& os, vcl_map<T1, vcl_map<T2, vcl_map<T3, T4, Compare3>, Compare2>, Compare1> const& v);

#endif //DSM_MAP_UTILITIES_H_