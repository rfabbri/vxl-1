//this is /contrib/bm/dsm/io/dsm_io_node.h
#ifndef DSM_IO_NODE_H_
#define DSM_IO_NODE_H_

#include"../dsm_node.h"

//: VSL_B_READ_WRITE FOR DERIVED CLASS
//: Binary save parameters to stream.
template<int T>
void vsl_b_read(vsl_b_istream &is, dsm_node<T>& node);
template<int T>
void vsl_b_write(vsl_b_ostream &os, dsm_node<T> const& node);
template<int T>
void vsl_b_read(vsl_b_istream &is, dsm_node<T>* &p);
template<int T>
void vsl_b_write(vsl_b_ostream &os, const dsm_node<T>* p);
template<int T>
void vsl_print_summary(vcl_ostream& os, const dsm_node<T>* p);

#endif //DSM_IO_NODE_H_