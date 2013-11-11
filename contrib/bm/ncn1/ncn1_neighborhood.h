//this is contrib/bm/ncn1/ncn1_neighborhood.h
#ifndef ncn1_neighborhood_h_
#define ncn1_neighborhood_h_

#include "ncn1_vgl_point_2d_less_than.h"

//#include "ncn1_feature.h"

#include<vbl/vbl_ref_count.h>

#include<vcl_map.h>
#include<vcl_vector.h>

#include<vgl/vgl_point_2d.h>

#include<vil/vil_image_view.h>

namespace neighborhood_typedefs
{
	typedef vcl_vector<vgl_point_2d<unsigned> > target_pixel_list_type;
	typedef vcl_vector<vgl_point_2d<unsigned> > pivot_pixel_list_type;
    typedef vcl_map< vgl_point_2d<unsigned>,pivot_pixel_list_type,ncn1_vgl_point_2d_less_than > neighborhood_type;
	typedef vcl_map< vgl_point_2d<unsigned>,vcl_vector<vgl_point_2d<unsigned> >,ncn1_vgl_point_2d_less_than >   neighborhood_type;
    
}

using namespace neighborhood_typedefs;

class ncn1_neighborhood: public vbl_ref_count
{
public:
	ncn1_neighborhood():n_neighbors_(unsigned(10)){}

	ncn1_neighborhood(target_pixel_list_type& target_pixel_list, pivot_pixel_list_type& pivot_pixel_candidates, unsigned& n_neighbors)
	{target_pixel_list_ = target_pixel_list; pivot_pixel_candidates_ = pivot_pixel_candidates; n_neighbors_ = n_neighbors;}
    
    ~ncn1_neighborhood(){}

    void set_target_pixel_list( target_pixel_list_type tl){target_pixel_list_ = tl;}

    target_pixel_list_type target_pixel_list() {return target_pixel_list_;}

    unsigned ntargets(){ return this->target_pixel_list_.size();}
     
    void set_pivot_pixel_candidates( pivot_pixel_list_type& ppc ){ pivot_pixel_candidates_ = ppc; }
    
    vcl_vector<vgl_point_2d<unsigned> > pivot_pixel_candidates() {return pivot_pixel_candidates_;}

    void set_neighborhood( neighborhood_type& nbrhd ) {neighborhood_ = nbrhd;}

    neighborhood_type neighborhood(){ return neighborhood_; }

    void set_num_neighbors(unsigned n_neighbors){ n_neighbors_ = n_neighbors; }

    unsigned n_neighbors(){ return n_neighbors_; }
    
    void set_neighborhood_valid(bool valid){ this->neihborhood_valid_ = valid; }

    bool neighborhood_valid(){ return this->neihborhood_valid_; }

    //feature_map_type feature_vector() { return feature_map_; } //ambiguous feature_map_type symbol

    //void set_feature_map( feature_vector_type& fm ) { feature_map_ = fm; }



private:
	unsigned n_neighbors_;
    bool neihborhood_valid_;
	target_pixel_list_type target_pixel_list_;
	pivot_pixel_list_type pivot_pixel_candidates_;
	neighborhood_type neighborhood_;
    //feature_map_type feature_map_;
};

#endif //ncn1_neighborhood_h_