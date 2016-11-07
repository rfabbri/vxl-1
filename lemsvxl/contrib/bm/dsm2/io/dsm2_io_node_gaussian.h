//this is /contrib/bm/dsm2/dsm2_io_node_gaussian_full.h
#ifndef DSM2_IO_NODE_GAUSSIAN_H_
#define DSM2_IO_NODE_GAUSSIAN_H_
#include<dsm2/dsm2_node_gaussian.h>

#include<vsl/vsl_binary_io.h>

//: Binary save parameters to stream.
template<class gaussT>
void vsl_b_read(vsl_b_istream &is, dsm2_node_gaussian<gaussT> &node);

template<class gaussT>
void vsl_b_write(vsl_b_ostream &os, dsm2_node_gaussian<gaussT> const& node);

template<class gaussT>
void vsl_b_read(vsl_b_istream &is, dsm2_node_gaussian<gaussT>* &p);

template<class gaussT>
void vsl_b_write(vsl_b_ostream &os, const dsm2_node_gaussian<gaussT>* p);

template<class gaussT>
void vsl_print_summary(vcl_ostream& os, const dsm2_node_gaussian<gaussT> *p);

#endif//DSM2_IO_NODE_GAUSSIAN_FULL_H_