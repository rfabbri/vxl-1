// This is bmcsd_curve_3d_sketch_e.h
#ifndef bmcsd_curve_3d_sketch_e_h
#define bmcsd_curve_3d_sketch_e_h
//:
//\file
//\brief File to hold 3D curves and attributes, enhanced version
//\author Ricardo Fabbri (rfabbri), Brown University  (@gmail.com)
//\date 11/11/09 18:24:24 EST
//

#include <bdifd/bdifd_frenet.h>
#include <bmcsd/bmcsd_view_set.h>
#include <bmcsd/bmcsd_curve_3d_attributes_e.h>
#include <bmcsd/bmcsd_util.h>

// This class holds 3D curves together with attributes such as pointers to which
// views were used to reconstruct them, etc. It is used as a basic
// representation for communicating between multiview curve stereo and multiview
// curve photogrammetry
class bmcsd_curve_3d_sketch_e {
public:

  bmcsd_curve_3d_sketch_e() {}

  //: You should be using this always with attributes, otherwise just use the
  // curve itself.
  bmcsd_curve_3d_sketch_e(
      const std::vector< bdifd_1st_order_curve_3d > &crv3d,
      const std::vector< bmcsd_curve_3d_attributes_e > &attr)
  : 
    crv3d_(crv3d),
    attr_(attr)
  { 
    assert(attr_.size() == crv3d.size());
  }


  void set(const std::vector< bdifd_1st_order_curve_3d > &crv3d,
      const std::vector< bmcsd_curve_3d_attributes_e > &attr)
  {
    crv3d_ = crv3d;
    attr_ = attr;
    assert(attr_.size() == crv3d.size());
  }

  //: Quick n' dirty: write in a directory structure format. The directory will
  // contain a file for each 3D curve, and for each 3D curve there will be a
  // corresponding entry on an attributes.vsl file with the remaining attributes
  // in text format.
  bool write_dir_format(std::string dirname) const;
  bool read_dir_format(std::string dirname);
  short dir_format_version() const { return 3;} // 3 is enhanced curve sketch, 2 for original curve sketch
  unsigned num_curves() const { return crv3d_.size(); }
  unsigned total_num_points() const {
    unsigned npts=0;
    for (unsigned c=0; c < num_curves(); ++c)
      npts += curves_3d()[c].size();
    return npts;
  }

  //: Getters and setters. Setters should assert for consistency.
  const std::vector< bdifd_1st_order_curve_3d > & curves_3d() const
  { return crv3d_; }
  const std::vector< bmcsd_curve_3d_attributes_e > &attributes() const
  { return attr_; }

  typedef std::vector<vgl_point_3d<double> > curve_points;

  void
  get_curves_as_vgl_points(std::vector< curve_points > *pts3d_ptr) const
  {
    std::vector< curve_points > &pts3d = *pts3d_ptr;
    pts3d.resize(crv3d_.size());

    for (unsigned c=0; c < num_curves(); ++c) {
      pts3d[c].resize(crv3d_[c].size());
      for (unsigned p=0; p < crv3d_[c].size(); ++p) {
        const bmcsd_vector_3d &pt = crv3d_[c][p].Gama;
        pts3d[c][p] = vgl_point_3d<double>(pt[0], pt[1], pt[2]);
      }
    }
  }

  bool operator==(const bmcsd_curve_3d_sketch_e &o) const { 
    return crv3d_ == o.crv3d_ && attr_ == o.attr_;
  }

  //: prune
  void prune_by_total_support(double tau_support)
  {
    std::vector< bdifd_1st_order_curve_3d > crv3d_new;
    std::vector< bmcsd_curve_3d_attributes_e > attr_new;

    crv3d_new.reserve(num_curves());
    attr_new.reserve(num_curves());

    for (unsigned i=0; i < num_curves(); ++i) {
      if (attributes()[i].total_support_ >= tau_support) {
        crv3d_new.push_back(curves_3d()[i]);
        attr_new.push_back(attributes()[i]);
      }
    }

    crv3d_ = crv3d_new;
    attr_ = attr_new;
  }

  unsigned totalMatchCount_;
  unsigned totalReconCount_;

//private:
  std::vector< bdifd_1st_order_curve_3d > crv3d_;
  std::vector< bmcsd_curve_3d_attributes_e > attr_;
};

#endif // bmcsd_curve_3d_sketch_e_h
