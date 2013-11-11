// This is mleotta/modrec/io/modrec_io_desc_feature_3d.txx
#ifndef modrec_io_desc_feature_3d_txx_
#define modrec_io_desc_feature_3d_txx_
//:
// \file

#include "modrec_io_desc_feature_3d.h"
#include <vgl/io/vgl_io_point_3d.h>
#include <vnl/io/vnl_io_vector_fixed.h>
#include <vsl/vsl_binary_io.h>

//============================================================================
//: Binary save self to stream.
template<unsigned n>
void vsl_b_write(vsl_b_ostream &os, const modrec_desc_feature_3d<n> & f)
{
  const short io_version_no = 1;
  vsl_b_write(os, io_version_no);
  vsl_b_write(os, f.position());
  vsl_b_write(os, f.orientation().as_quaternion());
  vsl_b_write(os, f.scale());
  vsl_b_write(os, f.descriptor());
}

//============================================================================
//: Binary load self from stream.
template<unsigned n>
void vsl_b_read(vsl_b_istream &is, modrec_desc_feature_3d<n> & f)
{
  if (!is) return;

  short v;
  vsl_b_read(is, v);
  switch (v)
  {
  case 1:
  {
    vgl_point_3d<double> p;
    vsl_b_read(is, p);
    f.set_position(p);
     
    vnl_vector_fixed<double,4> o;
    vsl_b_read(is, o);
    f.set_orientation(
        vgl_rotation_3d<double>(
            static_cast<vnl_quaternion<double> >(o) ) );

    double s;
    vsl_b_read(is, s);
    f.set_scale(s);

    vnl_vector_fixed<double,n> d;
    vsl_b_read(is, d);
    f.set_descriptor(d);
    break;
  }

  default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, modrec_desc_feature_3d<n>&)\n"
             << "           Unknown version number "<< v << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}


//============================================================================
//: Output a human readable summary to the stream
template<unsigned n>
void vsl_print_summary(vcl_ostream& os,const modrec_desc_feature_3d<n> & f)
{
  os << "{p=" << f.position() 
     << " o=" << f.orientation()
     << " s=" << f.scale()<<"}";
}

#define MODREC_IO_DESC_FEATURE_3D_INSTANTIATE(n) \
template void vsl_print_summary(vcl_ostream &, const modrec_desc_feature_3d<n> &); \
template void vsl_b_read(vsl_b_istream &, modrec_desc_feature_3d<n> &); \
template void vsl_b_write(vsl_b_ostream &, const modrec_desc_feature_3d<n> &)

#endif // modrec_io_desc_feature_3d_txx_
