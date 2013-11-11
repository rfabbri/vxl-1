//this is /contrib/bm/dsm2/dsm2_io_transition_table_base.h
#ifndef DSM2_IO_TRANSITION_TABLE_BASE_H_
#define DSM2_IO_TRANSITION_TABLE_BASE_H_
#include<dsm2/dsm2_transition_table_base.h>

#include<vsl/vsl_binary_io.h>

//: Binary save parameters to stream.
void vsl_b_read(vsl_b_istream &is, dsm2_transition_table_base &transition_table_base);

void vsl_b_write(vsl_b_ostream &os, dsm2_transition_table_base const& transition_table_base);

void vsl_b_read(vsl_b_istream &is, dsm2_transition_table_base* &p);

void vsl_b_write(vsl_b_ostream &os, const dsm2_transition_table_base* p);

void vsl_print_summary(vcl_ostream& os, const dsm2_transition_table_base *p);


#endif //DSM2_IO_TRANSITION_TABLE_BASE_H_