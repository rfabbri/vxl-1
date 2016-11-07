//this is /contrib/bm/dsm/io/dsm_io_features.h
#ifndef DSM_IO_FEATURES_H_
#define DSM_IO_FEATURES_H_
//:
// \file
// \brief Binary IO for dsm_features class
//
// \author Brandon A. Mayer b.mayer1@gmail.com
//
// \date  February 4, 2011
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
#include<vsl/vsl_binary_io.h>
#include<vcl_iostream.h>
#include"../dsm_features.h"

//: Binary save parameters to stream.
void vsl_b_read(vsl_b_istream &is, dsm_features &features);
void vsl_b_write(vsl_b_ostream &os, dsm_features const& features);
void vsl_b_read(vsl_b_istream &is, dsm_features* &p);
void vsl_b_write(vsl_b_ostream &os, const dsm_features* p);
void vsl_print_summary(vcl_ostream& os, const dsm_features *p);

#endif //DSM_IO_FEATURES_H_