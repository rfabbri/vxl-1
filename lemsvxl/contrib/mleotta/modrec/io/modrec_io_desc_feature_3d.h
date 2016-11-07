// This is mleotta/modrec/modrec_io_desc_feature_3d.h
#ifndef modrec_io_desc_feature_3d_h
#define modrec_io_desc_feature_3d_h
//:
// \file
// \author Matt Leotta
// \date 2/5/2008

#include <modrec/modrec_feature_3d.h>
#include <vsl/vsl_binary_io.h>

//: Binary save modrec_desc_feature_3d<n> to stream.
template <unsigned n>
void vsl_b_write(vsl_b_ostream &os, const modrec_desc_feature_3d<n> & f);

//: Binary load modrec_desc_feature_3d<n> from stream.
template <unsigned n>
void vsl_b_read(vsl_b_istream &is, modrec_desc_feature_3d<n> & f);

//: Print human readable summary of object to a stream
template <unsigned n>
void vsl_print_summary(vcl_ostream& os, const modrec_desc_feature_3d<n> & f);

#endif // modrec_io_desc_feature_3d_h
