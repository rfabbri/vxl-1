//this is /contrib/bm/ncn/ncn_neighborhood
#ifndef ncn_neighborhood_h_
#define ncn_neighborhood_h_
//:
// \file
// \brief A factory class to produce neighborhood objects
// \Author Brandon A. Mayer
// \date June 11, 2010
//
// \verbatim
//  No modifications yet.
// \endverbatim
#include"ncn_vgl_point_2d_less_than.h"

#include<vbl/vbl_ref_count.h>

#include<vcl_map.h>
#include<vcl_vector.h>
#include<vcl_set.h>

#include<vnl/vnl_matrix.h>

namespace neighborhood_typedefs
{
	typedef vcl_vector<vgl_point_2d<unsigned> > target_pixel_type;
    typedef vcl_set<vgl_point_2d<unsigned>,ncn_vgl_point_2d_less_than> pivot_pixel_candidate_type;
	typedef vcl_map<target_pixel_type::const_iterator,vcl_vector<pivot_pixel_candidate_type::const_iterator> > neighborhood_type;
}

using namespace neighborhood_typedefs;

class ncn_neighborhood: public vbl_ref_count
{
public:
	ncn_neighborhood():n_neighbors_(10){}
	ncn_neighborhood( ncn_neighborhood const& nbrhd ):vbl_ref_count(){}
	ncn_neighborhood(target_pixel_type const& target_pixel_list, pivot_pixel_candidate_type const& pivot_pixel_candidates,
						neighborhood_type const& neighborhood,unsigned const& n_neighbors):
							target_pixel_list_(target_pixel_list), pivot_pixel_candidates_(pivot_pixel_candidates),
								neighborhood_(neighborhood),n_neighbors_(n_neighbors){}
	~ncn_neighborhood(){}

	target_pixel_type target_pixels(){return target_pixel_list_;}
	pivot_pixel_candidate_type pivot_pixel_candidates(){return pivot_pixel_candidates_;}
	neighborhood_type neighborhood(){return neighborhood_;}
	unsigned n_neighbors(){return n_neighbors_;}

	void set_targets(target_pixel_type const& targets){target_pixel_list_ = targets;}
	void set_pivot_pixel_candidates(pivot_pixel_candidate_type const& pivot_pixel_candidates){pivot_pixel_candidates_ = pivot_pixel_candidates;}
	void set_neighborhood(neighborhood_type const& neighborhood){neighborhood_ = neighborhood;}
	void set_n_neighbors(unsigned const& n_neighbors){n_neighbors_ = n_neighbors;}

private:
	target_pixel_type target_pixel_list_;
	pivot_pixel_candidate_type pivot_pixel_candidates_;
	neighborhood_type neighborhood_;
	unsigned n_neighbors_;

};


#endif //ncn_neighborhood_h_