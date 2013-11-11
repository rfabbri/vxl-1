//this is /contrib/bm/dsm2/dsm2_io_state_machine_basee.h
#ifndef DSM2_IO_STATE_MACHINE_BASE_H_
#define DSM2_IO_STATE_MACHINE_BASE_H_
//:
// \file
// \date August 8, 2011
// \author Brandon A. Mayer
//
// I/O for dsm2_state_machine_base.
//
// \verbatim
//  Modifications
// \endverbatim
#include<dsm2/dsm2_state_machine_base.h>

#include<vsl/vsl_binary_io.h>

//: Binary save parameters to stream.
void vsl_b_read(vsl_b_istream &is, dsm2_state_machine_base &state_machine_base);

void vsl_b_write(vsl_b_ostream &os, dsm2_state_machine_base const& state_machine_base);

void vsl_b_read(vsl_b_istream &is, dsm2_state_machine_base* &p);

void vsl_b_write(vsl_b_ostream &os, const dsm2_state_machine_base* p);

void vsl_print_summary(vcl_ostream& os, const dsm2_state_machine_base *p);

#endif //DSM2_IO_STATE_MACHINE_BASE_H_