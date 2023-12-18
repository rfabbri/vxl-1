// This is bmcsd_curve_3d_attributes_e.h
#ifndef bmcsd_curve_3d_attributes_e_h
#define bmcsd_curve_3d_attributes_e_h
//:
//\file
//\brief Attributes to a 3D curve
//\author Ricardo Fabbri (rfabbri), Brown University  (@gmail.com)
//\date 08/31/2009 04:05:46 PM PDT
//

#include <bmcsd/bmcsd_curve_3d_attributes.h>

class bmcsd_curve_3d_attributes_e : bmcsd_curve_3d_attributes {
public:
  //: Anil: the supporting edgel IDs in each confirmation view
  vcl_vector<vcl_set<int> > supportingEdgelsPerConfView_;

  //: Anil: the support count for each hypothesis curve sample
  vcl_vector<unsigned> edgeSupportCount_;

  //: Anil: the supporting edges for each curve sample reconstructed
  vcl_vector<vcl_vector<int> > edge_index_chain_;

  //: Anil: offset between the image curve in v0 and the reconstructed curve
  unsigned imageCurveOffset_;

  //: Anil: offset between the image curve in v1 and the reconstructed curve
  unsigned imageCurveOffset_v1_;

  //: Anil: size of the original image curve in v0 and v1
  unsigned origCurveSize_;
  unsigned origCurveSize_v1_;

  //: Anil: the ID of the original curve in v0() (first anchor view)
  //        before epipolar tangency breaking
  unsigned orig_id_v0_;
  //: Anil: the ID of the original curve in v1() (second anchor view)
  //        before epipolar tangency breaking
  unsigned orig_id_v1_;

  //: Anil: the ID of the original curve in v0() (first anchor view)
  //        after epipolar tangency breaking, before pruning
  unsigned int_id_v0_;
  //: Anil: the ID of the original curve in v1() (second anchor view)
  //        after epipolar tangency breaking, before pruning
  unsigned int_id_v1_;

  //: Anil: flags marking reliably-reconstructed 3D samples
  vcl_vector<bool> certaintyFlags_;

  //: Anil: mate curves of the 3d curve on all confirmation views
  vcl_vector<vcl_set<int> > mate_curves_;

  //: Anil; weights of each mate curve on all confirmation views
  //: indicating level of support provided
  vcl_vector<vcl_map<unsigned,unsigned> > mate_curve_weights_;

  //: Anil: Vector of used sample IDs in v1()
  vcl_vector<unsigned> used_samples_v1_;


  bool operator==(const bmcsd_curve_3d_attributes &o) const {
    // XXX
  }

  // I/O ---------------------------------
  short version() const { return 3; }
  void b_read(vsl_b_istream &is);
  void b_write(vsl_b_ostream &os) const;

  //: Print an ascii summary to the stream
  void print_summary(std::ostream &os) const;

  std::string is_a() const { return "bmcsd_curve_3d_attributes_e"; }
};

#endif // bmcsd_curve_3d_attributes_e_h
