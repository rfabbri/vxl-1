//this is /contrib/bm/dsm/dsm_manager.txx
#include<dsm/dsm_manager.h>

template<unsigned T>
void dsm_manager<T>::b_write(vsl_b_ostream& os) const
{
	const short version_no = 1;
	vsl_b_write(os, version_no);

	//1. write the global frame clock
	vsl_b_write(os, this->frame_clock_ptr_);

	//2. write the target/state machine map
	//vsl_b_write(os, this->target_stateMachine_map_);
        unsigned nstate_machines = this->target_stateMachine_map_.size();
        vsl_b_write(os,nstate_machines);

        vcl_map<vgl_point_2d<unsigned>, dsm_state_machine_base_sptr, 
        dsm_vgl_point_2d_coord_compare<unsigned> >::const_iterator 
        tsitr, tsend = this->target_stateMachine_map_.end();

    for(tsitr=this->target_stateMachine_map_.begin(); tsitr!=tsend; ++tsitr)
    {
        vsl_b_write(os,tsitr->first);
        tsitr->second->b_write(os);
    }

	return;
}//end dsm_manager::b_write

template<unsigned T>
void dsm_manager<T>::b_read(vsl_b_istream& is)
{
	if(!is) return;

	short v;
	vsl_b_read(is,v);

	switch(v)
	{
	case 1:
		{
			dsm_frame_clock* fcp;
			vsl_b_read(is, fcp);

			unsigned nstate_machines;
			vsl_b_read(is,nstate_machines);

			for(unsigned i = 0; i < nstate_machines; ++i)
			{
				vgl_point_2d<unsigned> pt;
				vsl_b_read(is,pt);
				dsm_state_machine_base_sptr smp = 
                                    new dsm_state_machine<T>();
				smp->b_read(is);
				this->insert_state_machine(pt,smp);
			}//end state machine iteration
			break;
		}//end case 1
	default:
		{
			vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&,"
                                 << "dsm_manager_sptr2)\n"
				 << "\t\tUnknown version number "<< v << '\n';
                        // Set an unrecoverable IO error on stream
			is.is().clear(vcl_ios::badbit); 
			return;
		}
	}//end switch(v)
	return;
}//end dsm_manager::b_read

#define DSM_MANAGER_INSTANTIATE(T) \
template class dsm_manager<T>
