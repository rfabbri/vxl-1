//this is contrib/bm/dsm2/dsm2_io_transition_map.h
#ifndef DSM2_IO_TRANSITION_MAP_H_
#define DSM2_IO_TRANSITION_MAP_H_
#include<dsm2/dsm2_transition_map.h>

#include<vsl/vsl_binary_io.h>

//: Binary save parameters to stream.
template<class T1, class T2, class T3>
void vsl_b_read(vsl_b_istream &is, dsm2_transition_map<T1, T2, T3> &transition_map);

template<class T1, class T2, class T3>
void vsl_b_write(vsl_b_ostream &os, dsm2_transition_map<T1, T2, T3> const& transition_map);

template<class T1, class T2, class T3>
void vsl_b_read(vsl_b_istream &is, dsm2_transition_map<T1, T2, T3>* &p);

template<class T1, class T2, class T3>
void vsl_b_write(vsl_b_ostream &os, const dsm2_transition_map<T1,T2,T3>* p);

template<class T1, class T2, class T3>
void vsl_print_summary(vcl_ostream& os, const dsm2_transition_map<T1,T2,T3> *p);

#endif //DSM2_IO_TRANSITION_MAP_H_