//this is contrib/bm/dncn/dncn_target_list_2d_h_
#ifndef dncn_target_list_2d_h_
#define dncn_target_list_2d_h_

//:
// \file
// \date August 11, 2010
// \author Brandon A. Mayer
//
// Class to store a list of dncn_targets whith corresponding neighborhoods and features.
//
// \verbatim
//  Modifications
// \endverbatim

#include"dncn_less_than.h"
#include"dncn_target_2d_sptr.h"
#include"dncn_feature_sptr.h"

#include<vbl/vbl_ref_count.h>

#include<vcl_map.h>
#include<vcl_string.h>
#include<vcl_utility.h>

#include<vgl/vgl_distance.h>
#include<vgl/vgl_point_2d.h>

#include<vnl/algo/vnl_svd.h>
#include<vnl/vnl_matrix.h>


class dncn_target_list_2d: public vbl_ref_count
{
public:
    //typedef vcl_map<vgl_point_2d<unsigned>, dncn_target_2d_sptr, dncn_less_than> target_list_type;

	typedef vcl_vector<dncn_target_2d_sptr> target_list_type;

    dncn_target_list_2d(){}

    dncn_target_list_2d( target_list_type& tl ): target_list_(tl){}

    ~dncn_target_list_2d(){}

    //PROCESSES
    // reduce the dimensionality of each feature to dims_to_keep using svd. But consider all
    // points so as to form a consistent coordinate system.
    void reduce_dimensionality( unsigned const& dimensions_to_retain = 2 );

    //setters

    //will replace a target if the index already exists in the map
    void add_target(dncn_target_2d_sptr target_sptr);

    void set_target_list( target_list_type& target_list ){ target_list_ = target_list; }

    void set_rows( unsigned const& rows ){ this->rows_ = rows; }

    void set_cols( unsigned const& cols ){ this->cols_ = cols; }

    void set_nframes( unsigned const& nframes) {this->frames_ = nframes;}

    //getters
    unsigned num_neighbors(){ return this->target_list_[0]->num_neighbors(); }

    target_list_type target_list(){ return target_list_;}

    target_list_type::iterator target_list_begin(){ return this->target_list_.begin(); }

    target_list_type::iterator target_list_end(){ return this->target_list_.end(); }

    unsigned num_targets(){ return this->target_list_.size(); }

    void write_neighborhood_mfile( vcl_string const& filename );

    void write_feature_mfile( vcl_string const& filename, unsigned const& num_frames );

    void write_reduced_feature_mfile( vcl_string const& filename, unsigned const& num_frames );

    unsigned rows(){ return rows_; }

    unsigned cols(){ return cols_; }

    unsigned frames(){ return frames_; }

protected:
    
    unsigned rows_;
    unsigned cols_;
    unsigned frames_;
	target_list_type target_list_;
	vnl_matrix<double> reduced_U_;
	vnl_vector<double> dimension_means_;
};

#endif //dncn_target_list_2d_h_