// This is mleotta/modrec/modrec_io_codeword.h
#ifndef modrec_io_codeword_h
#define modrec_io_codeword_h
//:
// \file
// \author Matt Leotta
// \date 2/25/2008

#include <modrec/modrec_codeword.h>
#include <vsl/vsl_binary_io.h>

//: Binary save modrec_codeword<n> to stream.
template <unsigned n>
void vsl_b_write(vsl_b_ostream &os, const modrec_codeword<n> & f);

//: Binary load modrec_codeword<n> from stream.
template <unsigned n>
void vsl_b_read(vsl_b_istream &is, modrec_codeword<n> & f);

//: Print human readable summary of object to a stream
template <unsigned n>
void vsl_print_summary(vcl_ostream& os, const modrec_codeword<n> & f);

#endif // modrec_io_codeword_h
