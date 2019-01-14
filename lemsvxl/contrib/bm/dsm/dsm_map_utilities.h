#ifndef DSM_MAP_UTILITIES_H_
#define DSM_MAP_UTILITIES_H_

#include<map>
#include<string>
#include<utility>

#include<vsl/vsl_binary_io.h>

// \brief binary IO functions for std::map<T1, std::map<T2, T3, Compare2>, Compare3>
// \author B.A.Mayer
//====================================================================================
//: Write a map of maps to stream
template<class T1, class T2, class T3, class Compare1, class Compare2>
void vsl_b_write(vsl_b_ostream& os, std::map<T1, std::map<T2, T3, Compare2>, Compare1> const& v);
//====================================================================================
//: Read a map of maps from stream
template<class T1, class T2, class T3, class Compare1, class Compare2>
void vsl_b_read(vsl_b_istream& is, std::map<T1, std::map<T2, T3, Compare2>, Compare1>& v);
//====================================================================================
//: Print Summary
template<class T1, class T2, class T3, class Compare1, class Compare2>
void vsl_print_summary( std::ostream& os, const std::map<T1, std::map<T2, T3, Compare2>, Compare1>& v);


// \brief binary IO functions for std::map<T1, std::map< T2, std::map<T3, T4, Compare3>, Compare2>, Compare1>
//====================================================================================
//: Read a map of maps of maps from
template<class T1, class T2, class T3, class T4, class Compare1, class Compare2, class Compare3>
void vsl_b_read(vsl_b_istream& is, std::map<T1, std::map<T2, std::map<T3, T4, Compare3>, Compare2>, Compare1>& v);
//: Write a map of maps of maps to stream
template<class T1, class T2, class T3, class T4, class Compare1, class Compare2, class Compare3>
void vsl_b_write(vsl_b_ostream& os,std::map<T1, std::map<T2, std::map<T3, T4, Compare3>, Compare2>, Compare1> const& v);
//: Print summmary
template<class T1, class T2, class T3, class T4, class Compare1, class Compare2, class Compare3>
void vsl_print_summary( std::ostream& os, std::map<T1, std::map<T2, std::map<T3, T4, Compare3>, Compare2>, Compare1> const& v);

#endif //DSM_MAP_UTILITIES_H_