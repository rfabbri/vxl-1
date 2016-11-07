//this is /contrib/bm/dsm2/dsm2_io_node_base.h
#ifndef DSM2_IO_NODE_BASE_H_
#define DSM2_IO_NODE_BASE_H_
#include<dsm2/dsm2_node_base.h>

#include<vsl/vsl_binary_io.h>

//: Binary save parameters to stream.
void vsl_b_read(vsl_b_istream &is, dsm2_node_base& node_base);

void vsl_b_write(vsl_b_ostream &os, dsm2_node_base const& node_base);

void vsl_b_read(vsl_b_istream &is, dsm2_node_base* &p);

void vsl_b_write(vsl_b_ostream &os, const dsm2_node_base* p);

void vsl_print_summary(vcl_ostream& os, const dsm2_node_base *p);

#endif //DSM2_IO_NODE_BASE_H_