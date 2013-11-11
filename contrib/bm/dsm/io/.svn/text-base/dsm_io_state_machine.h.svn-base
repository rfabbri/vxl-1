//this is /contrib/bm/dsm/io/dsm_io_state_machine.h
#ifndef DSM_IO_STATE_MACHINE_H_
#define DSM_IO_STATE_MACHINE_H_
//:
// \file
// \brief Binary IO for dsm_state_machine<T> class
//
// \author Brandon A. Mayer b.mayer1@gmail.com
//
// \date  March 16, 2011
//
// \verbatim
//  Modifications
//   Brandon A. Mayer 3/28/2011 - ADDED BASE CLASS READ/WRITE FUNCTIONALITY
// \endverbatim
//: Binary save parameters to stream.
//#include"../dsm_state_machine_base.h"
#include"../dsm_state_machine.h"

////: VSL_B_READ_WRITE FOR BASE CLASS
////: Binary save parameters to stream.
//void vsl_b_read(vsl_b_istream &is, dsm_state_machine_base &manager);
//void vsl_b_write(vsl_b_ostream &os, dsm_state_machine_base const& manager);
//void vsl_b_read(vsl_b_istream &is, dsm_state_machine_base* &p);
//void vsl_b_write(vsl_b_ostream &os, const dsm_state_machine_base* p);
//void vsl_print_summary(vcl_ostream& os, const dsm_state_machine_base *p);

//: VSL_B_READ_WRITE FOR DERIVED CLASS
//: Binary save parameters to stream.
template<int T>
void vsl_b_read(vsl_b_istream &is, dsm_state_machine<T>& state_machine);
template<int T>
void vsl_b_write(vsl_b_ostream &os, dsm_state_machine<T> const& state_machine);
template<int T>
void vsl_b_read(vsl_b_istream &is, dsm_state_machine<T>* &p);
template<int T>
void vsl_b_write(vsl_b_ostream &os, const dsm_state_machine<T>* p);
template<int T>
void vsl_print_summary(vcl_ostream& os, const dsm_state_machine<T>* p);

#endif //DSM_IO_STATE_MACHINE_H_