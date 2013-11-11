#include "mw_sift_curve_algo.h"
#include <vul/vul_timer.h>

void mw_sift_curve_algo::
compute( const dbdet_edgel_chain &crv, mw_sift_curve *s_crv)
{
    unsigned num_scales = sift_.get_total_num_scales(); /* compute total number of scales in scale-space */
    s_crv->set_size(num_scales, crv.edgels.size());

    // process one octave at a time
    unsigned is = 0;
    
//    vcl_string filename = "/home/anilusumezbas/Documents/Research/histogram-files/keypoints.txt";
//    vcl_ofstream keypoints(filename.c_str());
    
    for(int o = sift_.get_omin(); o < sift_.get_omin() + sift_.get_O(); ++o) {
        // for each scale within the octave
        for (int s = sift_.get_smin(); s <= sift_.get_smax(); ++s) {
            assert (is < num_scales);
            VL::float_t sigma = sift_.getScaleFromIndex(o, s);
            for (unsigned i=0; i < crv.edgels.size(); ++i) {
                // fill up the keypoint datastructure
                VL::Sift::Keypoint k = sift_.getKeypoint(crv.edgels[i]->pt.x(), crv.edgels[i]->pt.y(), sigma);
                
                //keypoints << crv.edgels[i]->pt.x();
                //keypoints << " ";
                //keypoints << crv.edgels[i]->pt.y();
                //keypoints << " ";
                //keypoints << crv.edgels[i]->tangent;
                //keypoints << vcl_endl;

                // compute the descriptor at the edgel orientation
                bool is_valid = sift_.computeKeypointDescriptor(s_crv->descriptor_data_block(is,i), k, crv.edgels[i]->tangent);
                s_crv->set_validity(is, i, is_valid);
            }
            ++is;
        }
    }

    //keypoints.close();
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
        vcl_cout << "octave index: " << o << vcl_endl;
        for (int s = sift_.get_smin(); s <= sift_.get_smax(); ++s) {
            assert (is < num_scales);
            vcl_cout << "scale_index: " << s << vcl_endl;
            VL::float_t sigma = sift_.getScaleFromIndex(o, s);
            vcl_cout << "Computing descriptors for sigma = " << sigma << vcl_endl;
            double curve_time = 0;

            // for each curve
            for (unsigned c=0; c < ec_v.size(); ++c) {
                vul_timer t;

                for (unsigned i=0; i < ec_v[c].edgels.size(); ++i) {
                    // fill up the keypoint datastructure
                    VL::Sift::Keypoint k = sift_.getKeypoint(ec_v[c].edgels[i]->pt.x(), ec_v[c].edgels[i]->pt.y(), sigma);
                    // compute the descriptor at the edgel orientation
                    bool is_valid = sift_.computeKeypointDescriptor(sc_v[c].descriptor_data_block(is,i), k, ec_v[c].edgels[i]->tangent);
                    sc_v[c].set_validity(is, i, is_valid);
                }
                curve_time = curve_time + t.real();
                t.mark();
            }

            double average_time = curve_time / ec_v.size();
            vcl_cout << "Average computation time for a curve in sigma = " << sigma << " is: " << average_time << vcl_endl;

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
