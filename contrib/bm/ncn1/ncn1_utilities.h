//this is contrib/bm/ncn1/ncn1_utilities.h

#ifndef ncn1_utilities_h_
#define ncn1_utilities_h_

#include"ncn1_factory.h"

class ncn1_utilities
{
public:
    static void save_pivot_pixel_candidate_matlab( vcl_string const& filename, pivot_pixel_list_type& pivot_pixel_candidates );

    static void save_neighborhood_dat( vcl_string const& filename, ncn1_neighborhood_sptr neighborhood_sptr );

    //static void save_feature_dat( vcl_string const& filename, ncn1_neighborhood_sptr neighborhood_sptr );


};

#endif //ncn1_utilities_h_