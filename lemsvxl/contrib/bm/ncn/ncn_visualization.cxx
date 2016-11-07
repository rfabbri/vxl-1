//this is /contrib/bm/ncn/ncn_visualization

#include "ncn_visualization.h"

bool ncn_visualization::neighborhood2dat(target_pixel_type const& target_pixel_list, pivot_pixel_candidate_type const& pivot_pixel_candidates,
					neighborhood_type const& neighborhood, vcl_ostream& os)
{
	neighborhood_type::const_iterator neighborhood_itr;
	neighborhood_type::const_iterator neighborhood_end = neighborhood.end();

	unsigned count = 0;
	for(neighborhood_itr = neighborhood.begin(); neighborhood_itr != neighborhood_end; ++neighborhood_itr)
	{
		os << "target_pixel" << count << " = [" << neighborhood_itr->first->x() <<  " " << neighborhood_itr->first->y() << "];" << vcl_endl;

		vcl_vector<pivot_pixel_candidate_type::const_iterator>::const_iterator pivot_pixel_itr;
		vcl_vector<pivot_pixel_candidate_type::const_iterator>::const_iterator pivot_pixel_end = neighborhood_itr->second.end();
		os << "neighborhood" << count << " = [";
		unsigned n_neighbors = neighborhood_itr->second.size();
		for(unsigned i = 0; i < n_neighbors; ++i)
		{
			os << neighborhood_itr->second[i]->x() << ' ' << neighborhood_itr->second[i]->y() << ";\n";
		}
		os << "];" << vcl_endl << vcl_endl;

		++count;
	}

	return true;

}//end ncn_visualiztion::neighborhood2dat