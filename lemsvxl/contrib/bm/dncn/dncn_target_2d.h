//this is contrib/bm/dncn/dncn_target_2d.h
#ifndef dncn_target_2d_h_
#define dncn_target_2d_h_

#include"dncn_neighborhood_2d_sptr.h"
#include"dncn_feature_sptr.h"

#include<vbl/vbl_ref_count.h>

#include<vcl_iostream.h>
#include<vcl_map.h>
#include<vcl_utility.h>

#include<vgl/vgl_point_2d.h>

class dncn_target_2d: public vbl_ref_count
{
public:
    typedef vcl_map<unsigned, dncn_feature_sptr> feature_map_type;

    dncn_target_2d(){}

	dncn_target_2d(unsigned const& x, unsigned const& y, dncn_neighborhood_2d_sptr neighborhood_sptr = NULL):target_(vgl_point_2d<unsigned>(x,y)){/*target_ = vgl_point_2d<unsigned>(x,y);*/}

    dncn_target_2d(vgl_point_2d<unsigned>& target):target_(target),neighborhood_sptr_(NULL){}

    dncn_target_2d(vgl_point_2d<unsigned>& target, feature_map_type& feature_map, dncn_neighborhood_2d_sptr nbrhd_sptr = NULL):
        target_(target), feature_map_(feature_map), neighborhood_sptr_(nbrhd_sptr){}

    dncn_target_2d(vgl_point_2d<unsigned>& target, unsigned const& num_frames);

    ~dncn_target_2d(){}

    //setters
    void set_target(vgl_point_2d<unsigned>& target){ target_ = target; }

    void set_neighborhood_sptr( dncn_neighborhood_2d_sptr nbrhd_sptr ){ neighborhood_sptr_ = nbrhd_sptr; }

    void set_feature_map( feature_map_type fm ){ feature_map_ = fm; }

	void set_target_map( feature_map_type tm){ target_map_ = tm; }

    //will replace entry if it exists
    void add_feature_sptr( dncn_feature_sptr feature ); 

    //getters
    vgl_point_2d<unsigned> target(){ return this->target_; }

    dncn_neighborhood_2d_sptr neighborhood_sptr(){ return this->neighborhood_sptr_; }

    feature_map_type feature_map(){ return this->feature_map_; }

	feature_map_type target_map(){ return this->target_map_; }

    feature_map_type::iterator feature_map_begin(){ return this->feature_map_.begin(); }

    feature_map_type::iterator feature_map_end(){ return this->feature_map_.end(); }

	feature_map_type::iterator target_map_begin(){ return this->target_map_.begin(); }

	feature_map_type::iterator target_map_end(){ return this->target_map_.end(); }

    //returns null if no frame index exists in the map
    dncn_feature_sptr feature_sptr( unsigned const& frame );

	dncn_feature_sptr target_sptr( unsigned const& frame );

    unsigned num_neighbors(){ return this->neighborhood_sptr_->num_neighbors(); }

    unsigned num_frames(){ return this->feature_map_.size(); }
    
	//void set_frame_keypoint(unsigned const& frame, bapl_lowe_keypoint_sptr kp_sptr)
	//{ frame_keypoint_map_[frame] = kp_sptr; }

	//bapl_lowe_keypoint_sptr keypoint(unsigned const& frame){return frame_keypoint_map_[frame];}

	//vcl_map<unsigned,bapl_lowe_keypoint_sptr> frame_keypoint_map(){return frame_keypoint_map_;}

	//void set_target_frame_keypoint(unsigned const& frame, bapl_lowe_keypoint_sptr kp_sptr){this->frame_keypoint_map_[frame] = kp_sptr;}

	//void set_keypoint_map( vcl_map<unsigned,bapl_lowe_keypoint_sptr> const& f_kp_map){frame_keypoint_map_ = f_kp_map;}

	//void set_frame_neighbor_keypoint(unsigned const& frame, vgl_point_2d<unsigned>& neighbor, bapl_lowe_keypoint_sptr kp)
	//{ frame_neighborhood_keypoint_map_[frame][neighbor] = kp; }

	//bapl_lowe_keypoint_sptr neighbor_keypoint(unsigned const& frame, vgl_point_2d<unsigned>& neighbor)
	//{//return frame_neighborhood_keypoint_map_[frame][neighbor];}

	//vcl_map<unsigned,vcl_map<vgl_point_2d<unsigned>,bapl_lowe_keypoint_sptr> > frame_neighborhood_keypoint_map()
	//{return this->frame_neighborhood_keypoint_map_;}

	//void set_frame_neighborhood_keypoint_map( vcl_map<unsigned,vcl_map<vgl_point_2d<unsigned>,bapl_lowe_keypoint_sptr> > & fnkp )
	//{this->frame_neighborhood_keypoint_map_ = fnkp; }

protected:
    //protected member variables
    vgl_point_2d<unsigned> target_;    
    dncn_neighborhood_2d_sptr neighborhood_sptr_;
    feature_map_type feature_map_;
	feature_map_type target_map_;

	//the keypoint of the target at each frame
	//vcl_map<unsigned,bapl_lowe_keypoint_sptr> frame_keypoint_map_;
	//vcl_map<unsigned,vcl_map<vgl_point_2d<unsigned>,bapl_lowe_keypoint_sptr> > frame_neighborhood_keypoint_map_;
};

#endif //dncn_target_2d_h_