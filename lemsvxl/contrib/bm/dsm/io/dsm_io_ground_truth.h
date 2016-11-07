//this is /contrib/bm/dsm/io/dsm_io_ground_truth.h
#ifndef DSM_IO_GROUND_TRUTH_H_
#define DSM_IO_GROUND_TRUTH_H_
//:
// \file
// \brief Binary IO for dsm_ground_truth class
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
#include"../dsm_ground_truth.h"

//: Binary save parameters to stream.
void vsl_b_read(vsl_b_istream &is, dsm_ground_truth &ground_truth);
void vsl_b_write(vsl_b_ostream &os, dsm_ground_truth const& ground_truth);
void vsl_b_read(vsl_b_istream &is, dsm_ground_truth* &p);
void vsl_b_write(vsl_b_ostream &os, const dsm_ground_truth* p);
void vsl_print_summary(vcl_ostream& os, const dsm_ground_truth *p);


#endif //DSM_IO_GROUND_TRUTH_H_