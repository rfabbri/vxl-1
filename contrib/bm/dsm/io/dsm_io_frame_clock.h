//this is /contrib/bm/dsm/io/dsm_io_frame_clock.h
#ifndef	DSM_IO_FRAME_CLOCK_H_
#define DSM_IO_FRAME_CLOCK_H_
//:
// \file
// \brief Binary IO for dsm_ncn class
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
#include"../dsm_frame_clock.h"

//: Binary save parameters to stream.
void vsl_b_read(vsl_b_istream &is, dsm_frame_clock &frame_clock);
void vsl_b_write(vsl_b_ostream &os, dsm_frame_clock const& frame_clock);
void vsl_b_read(vsl_b_istream &is, dsm_frame_clock* &p);
void vsl_b_write(vsl_b_ostream &os, const dsm_frame_clock* p);
void vsl_print_summary(vcl_ostream& os, const dsm_frame_clock *p);

#endif //DSM_IO_FRAME_CLOCK_H_