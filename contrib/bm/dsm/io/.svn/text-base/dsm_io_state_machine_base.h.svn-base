//this is /contrib/bm/dsm/io/dsm_io_state_machine_base.h
#ifndef DSM_IO_STATE_MACHINE_BASE_H_
#define DSM_IO_STATE_MACHINE_BASE_H_
//:
// \file
// \brief Binary IO for dsm_state_machine_base class
//
// \author Brandon A. Mayer b.mayer1@gmail.com
//
// \date  March 28, 2011
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
#include<vsl/vsl_binary_io.h>
#include<vcl_iostream.h>
#include"../dsm_manager_base.h"
#include"../dsm_manager.h"

//: Binary save parameters to stream.
void vsl_b_read(vsl_b_istream &is, dsm_state_machine_base &state_machine_base);
void vsl_b_write(vsl_b_ostream &os, dsm_state_machine_base const& state_machine_base);
void vsl_b_read(vsl_b_istream &is, dsm_state_machine_base* &p);
void vsl_b_write(vsl_b_ostream &os, const dsm_state_machine_base* p);
void vsl_print_summary(vcl_ostream& os, const dsm_state_machine_base *p);

#endif//DSM_IO_STATE_MACHINE_BASE_H_