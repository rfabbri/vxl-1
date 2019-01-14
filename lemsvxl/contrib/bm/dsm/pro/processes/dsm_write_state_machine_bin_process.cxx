//this is /contrib/bm/dsm/pro/processes/dsm_write_state_machine_bin_process.cxx
#include<bprb/bprb_func_process.h>

#include<brdb/brdb_value.h>

#include<dsm/io/dsm_io_state_machine.h>

#include<iostream>
#include<vector>
#include<string>


namespace dsm_write_state_machine_bin_process_globals
{
	const unsigned int n_inputs_ = 2;
	const unsigned int n_outputs_ = 0;
}

bool dsm_write_state_machine_bin_process_cons(bprb_func_process& pro)
{
	using namespace dsm_write_state_machine_bin_process_globals;

	std::vector<std::string> input_types_(n_inputs_);

    input_types_[0] = vcl_string";
    input_types_[1] = "dsm_state_machine_base_sptr";

    if(!pro.set_input_types(input_types_))
        return false;
    
	return true;
}//end dsm_write_state_machine_bin_process_cons

bool dsm_write_state_machine_bin_process(bprb_func_process& pro)
{
	using namespace dsm_write_state_machine_bin_process_globals;

    //get input
    unsigned  i = 0;
    std::string filename = pro.get_input<std::string>(i++);
    dsm_state_machine_base_sptr sm_sptr = pro.get_input<dsm_state_machine_base_sptr>(i++);

    //create the output stream
    vsl_b_ofstream os(filename.c_str(), std::ios::out|std::ios::binary);
    
    switch(sm_sptr->ndims())
    {
    //case 1:
    //    {
    //        dsm_state_machine<1>* p = static_cast<dsm_state_machine<1>*>(sm_sptr.as_pointer());
    //        vsl_b_write(os,p);
    //    }//end case 1
    //    break;
    case 2:
        {
            dsm_state_machine<2>* p = static_cast<dsm_state_machine<2>*>(sm_sptr.as_pointer());
            vsl_b_write(os,p);
        }//end case 2
        break;
    case 3:
        {
            dsm_state_machine<3>* p = static_cast<dsm_state_machine<3>*>(sm_sptr.as_pointer());
            vsl_b_write(os,p);
        }//end case 3
        break;
    case 4:
        {
            dsm_state_machine<4>* p = static_cast<dsm_state_machine<4>*>(sm_sptr.as_pointer());
            vsl_b_write(os,p);
        }//end case4
        break;
    default:
        {
            std::cerr << "------ERROR DSM_WRITE_STATE_MACHINE_BIN_PROCESS: " << sm_sptr->ndims() 
                     << "UNSPECIFIED DIMENSION PLEASE ADD THIS CASE OR USE A DIFFERENT VALUE.---------" << std::flush;
            return false;   
        }//end default
    }//end switch
    os.close();

	return true;
}//end dsm_write_state_machine_bin_process