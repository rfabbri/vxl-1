#include <vil/vil_image_view.h>
#include <dbdet/sel/dbdet_edgel.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <mw/mw_sift_curve.h>
#include <mw/algo/mw_sift_curve_algo.h>
#include <mw/algo/mw_algo_util.h>
#include <mw/mw_util.h>


int main()
{
    // Selected curve in img_[0] is  selected_crv_[0]; the selected segment is subcurve_
    // The candidate curves in img[1] are crv_candidates_

    // 1 - compute descriptors for subcurve_

    vsol_polyline_2d_sptr subcurve_;
    vcl_vector<vil_image_view<float> > img_;
    vcl_vector<vsol_polyline_2d_sptr> crv_candidates_ptrs_;

    vcl_cout << "Loading the images" << vcl_endl;



    vcl_cout << "Started computing HOGs" << vcl_endl;

    mw_sift_curve sc0;
    vcl_cout << "Initializign SIFT / Gaussian Scale Space for image[0]." << vcl_endl;
    mw_sift_curve_algo computor(img_[0]);
    computor.print_sigmas();
    vcl_cout << "Done initializign SIFT / Gaussian Scale Space for image[0]." << vcl_endl;

    vcl_cout << "Computing descriptors for the curve in image[0]. Also computing the image gradient." << vcl_endl;
    {
        dbdet_edgel_chain curvelet_ec;
        mw_algo_util::extract_edgel_chain(*subcurve_, &curvelet_ec);
        computor.compute(curvelet_ec, &sc0);

        for (unsigned i=0; i < curvelet_ec.edgels.size(); ++i)
            delete curvelet_ec.edgels[i];
    }
    vcl_cout << "Done computing descriptors for the curve in image[0]." << vcl_endl;

    // Now for all other curves

    vcl_cout << "Initializign SIFT / Gaussian Scale Space for image[1]." << vcl_endl;
    mw_sift_curve_algo computor1(img_[1]);
    computor.print_sigmas();
    vcl_cout << "Done initializign SIFT / Gaussian Scale Space for image[1]." << vcl_endl;

    vcl_vector<dbdet_edgel_chain> ec_v(crv_candidates_ptrs_.size());

    vcl_cout << "\tExtracting edgel chain/tangents in image[1]" << vcl_endl;
    for (unsigned ii=0; ii < crv_candidates_ptrs_.size(); ++ii)
        mw_algo_util::extract_edgel_chain(*(crv_candidates_ptrs_[ii]), &(ec_v[ii]));
    vcl_cout << "\tDone extracting edgel chain/tangents in image[1]" << vcl_endl;

    vcl_vector<mw_sift_curve> sc_img1(crv_candidates_ptrs_.size());

    computor1.compute_many(ec_v, &sc_img1);

    for (unsigned ii=0; ii < crv_candidates_ptrs_.size(); ++ii)
        for (unsigned k=0; k < ec_v[ii].edgels.size(); ++k)
            delete ec_v[ii].edgels[k];

    vcl_cout << "Finished computing HOGs" << vcl_endl;

    vcl_cout << "Started feature matching as bag of features" << vcl_endl;

    vcl_vector<unsigned> votes(sc_img1.size(), 0);
    vcl_vector<double> sigmas;
    computor1.get_sigmas(&sigmas);
    assert(sigmas.size() == sc0.num_scales());

    for (unsigned s=0; s < sc0.num_scales(); ++s) {
        vcl_cout << "scale s = " << s << vcl_endl;
        for (unsigned i=0; i < sc0.num_samples(); ++i) {
            mw_sift_curve_algo::t_descriptor_float dmin = vcl_numeric_limits<mw_sift_curve_algo::t_descriptor_float>::infinity();
            mw_sift_curve_algo::t_descriptor_float dmin_second = vcl_numeric_limits<mw_sift_curve_algo::t_descriptor_float>::infinity();
            unsigned ic_min = 0;

//      vcl_cout << "scale s = " << s << "; sample i = " << i << vcl_endl;

            // For each curve
            for (unsigned ic=0; ic < crv_candidates_ptrs_.size(); ++ic) {
                mw_sift_curve_algo::t_descriptor_float d = mw_sift_curve_algo::unambigous_nneighbor(sc_img1[ic], sc0.descriptor(s, i));
                if (d < dmin) {
                    ic_min = ic;
                    dmin = d;
                } else {
                    if (d < dmin_second)
                        dmin_second = d;
                }
            }

            // Lowe's criterion: accept the match only if unique
//      vcl_cout << "dmin = " << dmin << "; dmin_second = " << dmin_second << vcl_endl;
//      votes[ic_min]++;
            mw_sift_curve_algo::t_descriptor_float thresh=1.5;
            if(dmin != vcl_numeric_limits<mw_sift_curve_algo::t_descriptor_float>::infinity() && thresh * dmin < dmin_second) {
                vcl_cout << "Placing a vote for curve[" << ic_min << "]" << " sigma = " << sigmas[s] << vcl_endl;
                votes[ic_min]++;
            }
        }
    }

    unsigned i_best;
    mw_util::max(votes, i_best);

    vcl_cout << "Best curve has index " << i_best << " among candidates, with #votes = " << votes[i_best] << vcl_endl;

    vcl_cout << "Finished feature matching as bag of features" << vcl_endl;
  

    return 0;
}
