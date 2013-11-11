//this is /contirb/bm/dsm/io/dsm_io_ncn.h
#ifndef dsm_io_ncn_h_
#define dsm_io_ncn_h_
//:
// \file
// \brief Binary IO for dsm_ncn class
//
// \author Brandon A. Mayer b.mayer1@gmail.com
//
// \date  Jan 25, 2011
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include<vsl/vsl_binary_io.h>
#include<vcl_iostream.h>
#include"../dsm_ncn.h"
#include"../dsm_ncn_sptr.h"
#include<bxml/bxml_document.h>

//: Binary save parameters to stream.
void vsl_b_read(vsl_b_istream &is, dsm_ncn &ncn);
void vsl_b_write(vsl_b_ostream &os, dsm_ncn const& ncn);
void vsl_b_read(vsl_b_istream &is, dsm_ncn* &p);
void vsl_b_write(vsl_b_ostream &os, const dsm_ncn* p);
void vsl_print_summary(vcl_ostream& os, const dsm_ncn *p);

// write xml summary of ncn
void write_xml( vcl_ostream& os, const dsm_ncn_sptr ncn_sptr );

#endif //dsm_io_ncn_h_