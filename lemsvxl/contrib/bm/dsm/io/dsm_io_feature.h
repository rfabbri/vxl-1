//this is /contrib/bm/dsm/io/dsm_io_feature.h
#ifndef DSM_IO_FEATURE_H_
#define DSM_IO_FEATURE_H_
//:
// \file
// \brief Binary IO for dsm_feature class
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
#include"../dsm_feature.h"

//: Binary save parameters to stream.
void vsl_b_read(vsl_b_istream &is, dsm_feature &feature);
void vsl_b_write(vsl_b_ostream &os, dsm_feature const& feature);
void vsl_b_read(vsl_b_istream &is, dsm_feature* &p);
void vsl_b_write(vsl_b_ostream &os, const dsm_feature* p);
void vsl_print_summary(vcl_ostream& os, const dsm_feature *p);


#endif //DSM_IO_FEATURE_H_