//this is /contrib/bm/dsm2/io/dsm2_io_manager_base.h
#ifndef DSM2_IO_MANAGER_BASE_H_
#define DSM2_IO_MANAGER_BASE_H_
//:
// \file
// \date August 24, 2011
// \author Brandon A. Mayer
//
// I/O for dsm2_manager_base.
//
// \verbatim
//  Modifications
// \endverbatim
#include<dsm2/dsm2_manager_base.h>

#include<vsl/vsl_binary_io.h>

//: Binary save parameters to stream.
void vsl_b_read(vsl_b_istream &is, dsm2_manager_base &manager_base);

void vsl_b_write(vsl_b_ostream &os, dsm2_manager_base const& manager_base);

void vsl_b_read(vsl_b_istream &is, dsm2_manager_base* &p);

void vsl_b_write(vsl_b_ostream &os, const dsm2_manager_base* p);

void vsl_print_summary(vcl_ostream& os, const dsm2_manager_base *p);


#endif //DSM2_IO_MANAGER_BASE_H_