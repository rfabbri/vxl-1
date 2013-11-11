//this is /contrib/bm/dsm2/io/dsm2_io_manager_pixels.h
#ifndef DSM2_IO_MANAGER_PIXELS_H_
#define DSM2_IO_MANAGER_PIXELS_H_
//:
// \file
// \date August 24, 2011
// \author Brandon A. Mayer
//
// I/O for dsm2_manager_pixels templated class.
//
// \verbatim
//  Modifications
// \endverbatim
#include<dsm2/dsm2_manager_pixels.h>

#include<vsl/vsl_binary_io.h>

//: Binary save parameters to stream.
template<class pixelT, class smT>
void vsl_b_read(vsl_b_istream &is, dsm2_manager_pixels<pixelT, smT> &manager_pixels);

template<class pixelT, class smT>
void vsl_b_write(vsl_b_ostream &os, dsm2_manager_pixels<pixelT, smT> const& manager_pixels);

template<class pixelT, class smT>
void vsl_b_read(vsl_b_istream &is, dsm2_manager_pixels<pixelT, smT>* &p);

template<class pixelT, class smT>
void vsl_b_write(vsl_b_ostream &os, const dsm2_manager_pixels<pixelT, smT>* p);

template<class pixelT, class smT>
void vsl_print_summary(vcl_ostream& os, const dsm2_manager_pixels<pixelT, smT> *p);

#endif //DSM2_IO_MANAGER_PIXELS_H_