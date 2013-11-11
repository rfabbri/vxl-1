//this is /contrib/bm/dsm/dsm_manager_base.cxx
#include"dsm_manager_base.h"

bool dsm_manager_base::change(vgl_point_2d<unsigned> const& target, unsigned const& time)
{
	dsm_state_machine_base_sptr sm_base_sptr = this->target_stateMachine_map_[target];
	return sm_base_sptr->change(time);
}