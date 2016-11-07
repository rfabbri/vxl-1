//this is /contrib/bm/dsm2/dsm2_io_state_machine.h
#ifndef DSM2_IO_STATE_MACHINE_H_
#define DSM2_IO_STATE_MACHINE_H_
//:
// \file
// \date August 8, 2011
// \author Brandon A. Mayer
//
// I/O for dsm2_state_machine templated class.
//
// \verbatim
//  Modifications
// \endverbatim
#include<dsm2/dsm2_state_machine.h>

#include<vsl/vsl_binary_io.h>

//: Binary save parameters to stream.
template<class nodeT, class transitionT>
void vsl_b_read(vsl_b_istream &is, dsm2_state_machine<nodeT, transitionT> &state_machine);

template<class nodeT, class transitionT>
void vsl_b_write(vsl_b_ostream &os, dsm2_state_machine<nodeT, transitionT> const& state_machine);

template<class nodeT, class transitionT>
void vsl_b_read(vsl_b_istream &is, dsm2_state_machine<nodeT, transitionT>* &p);

template<class nodeT, class transitionT>
void vsl_b_write(vsl_b_ostream &os, const dsm2_state_machine<nodeT, transitionT>* p);

template<class nodeT, class transitionT>
void vsl_print_summary(vcl_ostream& os, const dsm2_state_machine<nodeT, transitionT> *p);

#endif //DSM2_IO_FEATURE_H_