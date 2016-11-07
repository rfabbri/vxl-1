//this is /contrib/bm/dsm/dsm_manager_base.h
#ifndef DSM_MANAGER_BASE_H_
#define DSM_MANAGER_BASE_H_
//:
// \file
// \date March 9, 2011
// \author Brandon A. Mayer
//
// Base class for a state machine manager. This class will associate target locations with
// a state machine base smart pointer. We have a base class because the manager class itself
// must be templated over the dimensionality of the feature vector but we can define smart
// pointers to the base class for insertion into the python database.
//
// \verbatim
//  Modifications
// \endverbatim
#include"dsm_frame_clock.h"
#include"dsm_features_sptr.h"
#include"dsm_node_base_sptr.h"
#include"dsm_state_machine_base_sptr.h"
#include"dsm_utilities.h"

#include"io/dsm_io_frame_clock.h"

#include<vbl/vbl_ref_count.h>

#include<vcl_map.h>
#include<vcl_utility.h>

#include<vgl/vgl_point_2d.h>

#include<vsl/vsl_binary_io.h>
#include<vsl/vsl_map_io.h>


class dsm_manager_base: public vbl_ref_count
{
public:
	dsm_manager_base():frame_clock_ptr_(dsm_frame_clock::instance()){}

	//dsm_manager( dsm_features_sptr feature_sptr );

	virtual ~dsm_manager_base(){}

	//:insert a state machine for a given target
	vcl_pair<vcl_map<vgl_point_2d<unsigned>, dsm_state_machine_base_sptr, dsm_vgl_point_2d_coord_compare<unsigned> >::iterator, bool>
		insert_state_machine( vgl_point_2d<unsigned> const& target, dsm_state_machine_base_sptr state_machine_base_sptr )
	{return this->target_stateMachine_map_.insert(vcl_pair<vgl_point_2d<unsigned>, dsm_state_machine_base_sptr>(target,state_machine_base_sptr));}

	bool remove_state_machine( vgl_point_2d<unsigned> const& target ){return this->target_stateMachine_map_.erase(target);}

	void clear(){this->target_stateMachine_map_.clear();}

	//virtual bool train( vnl_vector<double> const& obs );

	//virtual bool classify ( vnl_vector<double> const& obs );

	//:accessors
	dsm_state_machine_base_sptr state_machine( vgl_point_2d<unsigned> const& target ){ return this->target_stateMachine_map_[target]; }

	vcl_map<vgl_point_2d<unsigned>, dsm_state_machine_base_sptr, dsm_vgl_point_2d_coord_compare<unsigned> > target_stateMachine_map()
	{return this->target_stateMachine_map_;}

	virtual unsigned ndims() const { return 0; }

	bool change(vgl_point_2d<unsigned> const& target, unsigned const& time);

    virtual void b_write(vsl_b_ostream& os) const {}

    virtual void b_read(vsl_b_istream& is) {}

	dsm_frame_clock* frame_clock_ptr_;

	vcl_map<vgl_point_2d<unsigned>, dsm_state_machine_base_sptr, dsm_vgl_point_2d_coord_compare<unsigned> > target_stateMachine_map_;
};

#endif //DSM_MANAGER_BASE_H_