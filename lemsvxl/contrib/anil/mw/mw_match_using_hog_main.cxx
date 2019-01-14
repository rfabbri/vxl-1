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
    std::vector<vil_image_view<float> > img_;
    std::vector<vsol_polyline_2d_sptr> crv_candidates_ptrs_;

    std::cout << "Loading the images" << std::endl;



    std::cout << "Started computing HOGs" << std::endl;

    mw_sift_curve sc0;
    std::cout << "Initializign SIFT / Gaussian Scale Space for image[0]." << std::endl;
    mw_sift_curve_algo computor(img_[0]);
    computor.print_sigmas();
    std::cout << "Done initializign SIFT / Gaussian Scale Space for image[0]." << std::endl;

    std::cout << "Computing descriptors for the curve in image[0]. Also computing the image gradient." << std::endl;
    {
        dbdet_edgel_chain curvelet_ec;
        mw_algo_util::extract_edgel_chain(*subcurve_, &curvelet_ec);
        computor.compute(curvelet_ec, &sc0);

        for (unsigned i=0; i < curvelet_ec.edgels.size(); ++i)
            delete curvelet_ec.edgels[i];
    }
    std::cout << "Done computing descriptors for the curve in image[0]." << std::endl;

    // Now for all other curves

    std::cout << "Initializign SIFT / Gaussian Scale Space for image[1]." << std::endl;
    mw_sift_curve_algo computor1(img_[1]);
    computor.print_sigmas();
    std::cout << "Done initializign SIFT / Gaussian Scale Space for image[1]." << std::endl;

    std::vector<dbdet_edgel_chain> ec_v(crv_candidates_ptrs_.size());

    std::cout << "\tExtracting edgel chain/tangents in image[1]" << std::endl;
    for (unsigned ii=0; ii < crv_candidates_ptrs_.size(); ++ii)
        mw_algo_util::extract_edgel_chain(*(crv_candidates_ptrs_[ii]), &(ec_v[ii]));
    std::cout << "\tDone extracting edgel chain/tangents in image[1]" << std::endl;

    std::vector<mw_sift_curve> sc_img1(crv_candidates_ptrs_.size());

    computor1.compute_many(ec_v, &sc_img1);

    for (unsigned ii=0; ii < crv_candidates_ptrs_.size(); ++ii)
        for (unsigned k=0; k < ec_v[ii].edgels.size(); ++k)
            delete ec_v[ii].edgels[k];

    std::cout << "Finished computing HOGs" << std::endl;

    std::cout << "Started feature matching as bag of features" << std::endl;

    std::vector<unsigned> votes(sc_img1.size(), 0);
    std::vector<double> sigmas;
    computor1.get_sigmas(&sigmas);
    assert(sigmas.size() == sc0.num_scales());

    for (unsigned s=0; s < sc0.num_scales(); ++s) {
        std::cout << "scale s = " << s << std::endl;
        for (unsigned i=0; i < sc0.num_samples(); ++i) {
            mw_sift_curve_algo::t_descriptor_float dmin = std::numeric_limits<mw_sift_curve_algo::t_descriptor_float>::infinity();
            mw_sift_curve_algo::t_descriptor_float dmin_second = std::numeric_limits<mw_sift_curve_algo::t_descriptor_float>::infinity();
            unsigned ic_min = 0;

//      std::cout << "scale s = " << s << "; sample i = " << i << std::endl;

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
//      std::cout << "dmin = " << dmin << "; dmin_second = " << dmin_second << std::endl;
//      votes[ic_min]++;
            mw_sift_curve_algo::t_descriptor_float thresh=1.5;
            if(dmin != std::numeric_limits<mw_sift_curve_algo::t_descriptor_float>::infinity() && thresh * dmin < dmin_second) {
                std::cout << "Placing a vote for curve[" << ic_min << "]" << " sigma = " << sigmas[s] << std::endl;
                votes[ic_min]++;
            }
        }
    }

    unsigned i_best;
    mw_util::max(votes, i_best);

    std::cout << "Best curve has index " << i_best << " among candidates, with #votes = " << votes[i_best] << std::endl;

    std::cout << "Finished feature matching as bag of features" << std::endl;
  

    return 0;
}
