#include "mw_sift_curve_algo.h"

void mw_sift_curve_algo::
compute( const dbdet_edgel_chain &crv, mw_sift_curve *s_crv)
{
  unsigned num_scales = sift_.get_total_num_scales(); /* compute total number of scales in scale-space */
  s_crv->set_size(num_scales, crv.edgels.size());

  // process one octave at a time
  unsigned is = 0;
  for(int o = sift_.get_omin(); o < sift_.get_omin() + sift_.get_O(); ++o) {
    // for each scale within the octave
    for (int s = sift_.get_smin(); s <= sift_.get_smax(); ++s) {
      assert (is < num_scales);
      VL::float_t sigma = sift_.getScaleFromIndex(o, s);
      for (unsigned i=0; i < crv.edgels.size(); ++i) {
        // fill up the keypoint datastructure
        VL::Sift::Keypoint k = sift_.getKeypoint(crv.edgels[i]->pt.x(), crv.edgels[i]->pt.y(), sigma);
        // compute the descriptor at the edgel orientation
        bool is_valid = sift_.computeKeypointDescriptor(s_crv->descriptor_data_block(is,i), k, crv.edgels[i]->tangent);
        s_crv->set_validity(is, i, is_valid);
      }
      ++is;
    }
  }
}

void mw_sift_curve_algo::
compute_many(const vcl_vector<dbdet_edgel_chain> &ec_v, vcl_vector<mw_sift_curve> *ptr_sc_v)
{
  vcl_vector<mw_sift_curve> &sc_v = *ptr_sc_v;
  unsigned num_scales = sift_.get_total_num_scales(); /* compute total number of scales in scale-space */

  assert(ec_v.size() == sc_v.size());

  for (unsigned i=0; i < ec_v.size(); ++i)
    sc_v[i].set_size(num_scales, ec_v[i].edgels.size());

  // process one octave at a time
  unsigned is = 0;
  for(int o = sift_.get_omin(); o < sift_.get_omin() + sift_.get_O(); ++o) {
    // for each scale within the octave
    for (int s = sift_.get_smin(); s <= sift_.get_smax(); ++s) {
      assert (is < num_scales);
      VL::float_t sigma = sift_.getScaleFromIndex(o, s);

      // for each curve
      for (unsigned c=0; c < ec_v.size(); ++c) {
        vcl_cout << "Computing descriptors for curve[" << c << "], sigma = " << sigma << vcl_endl;
        for (unsigned i=0; i < ec_v[c].edgels.size(); ++i) {
          // fill up the keypoint datastructure
          VL::Sift::Keypoint k = sift_.getKeypoint(ec_v[c].edgels[i]->pt.x(), ec_v[c].edgels[i]->pt.y(), sigma);
          // compute the descriptor at the edgel orientation
          bool is_valid = sift_.computeKeypointDescriptor(sc_v[c].descriptor_data_block(is,i), k, ec_v[c].edgels[i]->tangent);
          sc_v[c].set_validity(is, i, is_valid);
        }
        vcl_cout << "Done computing descriptors for curve[" << c << "]" << vcl_endl;
      }

      ++is;
    }
  }
}

void mw_sift_curve_algo::
get_sigmas(vcl_vector<double> *sigmas) const
{
  sigmas->reserve(sift_.get_total_num_scales());
  for(int o = sift_.get_omin(); o < sift_.get_omin() + sift_.get_O(); ++o) {
    // for each scale within the octave
    for (int s = sift_.get_smin(); s <= sift_.get_smax(); ++s) {
      sigmas->push_back(sift_.getScaleFromIndex(o, s));
    }
  }
  assert(sigmas->size() == sift_.get_total_num_scales());
}
