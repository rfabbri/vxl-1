//this is /contrib/bm/dsm/io/dsm_io_manager.h
#ifndef DSM_IO_MANAGER_H_
#define DSM_IO_MANAGER_H_
//:
// \file
// \brief Binary IO for dsm_manager class
//
// \author Brandon A. Mayer b.mayer1@gmail.com
//
// \date  March 11, 2011
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
#include<vsl/vsl_binary_io.h>
#include<vcl_iostream.h>
#include"../dsm_manager_base.h"
#include"../dsm_manager.h"

////: Binary save parameters to stream.
//void vsl_b_read(vsl_b_istream &is, dsm_manager_base &manager);
//void vsl_b_write(vsl_b_ostream &os, dsm_manager_base const& manager);
//void vsl_b_read(vsl_b_istream &is, dsm_manager_base* &p);
//void vsl_b_write(vsl_b_ostream &os, const dsm_manager_base* p);
//void vsl_print_summary(vcl_ostream& os, const dsm_manager_base *p);

template<unsigned T>
void vsl_b_read(vsl_b_istream &is, dsm_manager<T>& manager);
template<unsigned T>
void vsl_b_write(vsl_b_ostream &os, dsm_manager<T> const& manager);
template<unsigned T>
void vsl_b_read(vsl_b_istream &is, dsm_manager<T>* &p);
template<unsigned T>
void vsl_b_write(vsl_b_ostream &os, const dsm_manager<T>* p);
template<unsigned T>
void vsl_prunsigned_summary(vcl_ostream& os, const dsm_manager<T>* p);


#endif //DSM_IO_MANAGER_H_
