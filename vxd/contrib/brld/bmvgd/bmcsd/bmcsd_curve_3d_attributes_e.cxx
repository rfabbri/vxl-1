#include "bmcsd_curve_3d_attributes_e.h"
#include <vsl/vsl_vector_io.h>
#include <bmcsd/bmcsd_view_set.h>

void bmcsd_curve_3d_attributes_e::
b_write(vsl_b_ostream &os) const
{
  //XXX call base b_write
  bmcsd_curve_3d_attributes::b_write(os);
  //Anil: Adding modifications to write original curve IDs as well
  vsl_b_write(os, orig_id_v0_);
  vsl_b_write(os, orig_id_v1_);
  vsl_b_write(os, int_id_v0_);
  vsl_b_write(os, int_id_v1_);
  vsl_b_write(os, certaintyFlags_);
}


void bmcsd_curve_3d_attributes_e::
b_read(vsl_b_istream &is)
{
  if (!is) return;

  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
    case 3:
      v_ = new dbmcs_stereo_views();
      vsl_b_read(is, *v_); // urghhhh, nasty
      vsl_b_read(is, inlier_views_);
      vsl_b_read(is, total_support_);
      vsl_b_read(is, i0_);
      vsl_b_read(is, i1_);
      vsl_b_read(is, orig_id_v0_);
      vsl_b_read(is, orig_id_v1_);
      vsl_b_read(is, int_id_v0_);
      vsl_b_read(is, int_id_v1_);
      vsl_b_read(is, certaintyFlags_);
    break;

    default:
      XXX call base b_read
        std::cerr << "I/O ERROR: bmcsd_curve_2d_attributes_e::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

//: Print an ascii summary to the stream
void bmcsd_curve_3d_attributes_e::
print_summary(std::ostream &os) const
{
  os << "[" << is_a() << ": " << *v_
    << " inlier_views: ";

  for (unsigned i=0; i < inlier_views_.size(); ++i)
    os << inlier_views_[i] << " ";
  os << " i0: " << i0() << " i1: " << i1() << "total_support: " << total_support_;
  os << "]";
}
