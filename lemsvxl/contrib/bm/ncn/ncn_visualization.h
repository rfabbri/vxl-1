//this is /contrib/bm/ncn/ncn_visualization.h
#ifndef ncn_visualization_h_
#define ncn_visualization_h_

#include"ncn_factory.h"

class ncn_visualization
{
public:
	static bool neighborhood2dat(target_pixel_type const& target_pixel_list, pivot_pixel_candidate_type const& pivot_pixel_candidates,
					neighborhood_type const& neighborhood,vcl_ostream& os);

};

#endif //ncn_visualization_h_