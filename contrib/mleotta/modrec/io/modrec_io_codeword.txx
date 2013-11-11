// This is mleotta/modrec/io/modrec_io_codeword.txx
#ifndef modrec_io_codeword_txx_
#define modrec_io_codeword_txx_
//:
// \file

#include "modrec_io_codeword.h"
#include <vgl/io/vgl_io_point_3d.h>
#include <vnl/io/vnl_io_vector_fixed.h>
#include <vsl/vsl_binary_io.h>

//============================================================================
//: Binary save self to stream.
template<unsigned n>
void vsl_b_write(vsl_b_ostream &os, const modrec_codeword<n> & c)
{
  const short io_version_no = 1;
  vsl_b_write(os, io_version_no);
  vsl_b_write(os, c.position());
  vsl_b_write(os, c.var_position());
  vsl_b_write(os, c.orientation().as_quaternion());
  vsl_b_write(os, c.var_orientation());
  vsl_b_write(os, c.scale());
  vsl_b_write(os, c.var_scale());
  vsl_b_write(os, c.descriptor());
  vsl_b_write(os, c.var_descriptor());
}

//============================================================================
//: Binary load self from stream.
template<unsigned n>
void vsl_b_read(vsl_b_istream &is, modrec_codeword<n> & c)
{
  if (!is) return;

  short v;
  vsl_b_read(is, v);
  switch (v)
  {
  case 1:
  {
    vgl_point_3d<double> p;
    double var;
    vsl_b_read(is, p);
    vsl_b_read(is, var);
    c.set_position(p,var);
     
    vnl_vector_fixed<double,4> o;
    vsl_b_read(is, o);
    vsl_b_read(is, var);
    c.set_orientation(
        vgl_rotation_3d<double>(
            static_cast<vnl_quaternion<double> >(o) ), var );

    double s;
    vsl_b_read(is, s);
    vsl_b_read(is, var);
    c.set_scale(s,var);

    vnl_vector_fixed<double,n> d;
    vsl_b_read(is, d);
    vsl_b_read(is, var);
    c.set_descriptor(d,var);
    break;
  }

  default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, modrec_codeword<n>&)\n"
             << "           Unknown version number "<< v << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}


//============================================================================
//: Output a human readable summary to the stream
template<unsigned n>
void vsl_print_summary(vcl_ostream& os,const modrec_codeword<n> & c)
{
  os << "{p=" << c.position()
     << " o=" << c.orientation()
     << " s=" << c.scale()<<"}";
}

#define MODREC_IO_CODEWORD_INSTANTIATE(n) \
template void vsl_print_summary(vcl_ostream &, const modrec_codeword<n> &); \
template void vsl_b_read(vsl_b_istream &, modrec_codeword<n> &); \
template void vsl_b_write(vsl_b_ostream &, const modrec_codeword<n> &)

#endif // modrec_io_codeword_txx_
