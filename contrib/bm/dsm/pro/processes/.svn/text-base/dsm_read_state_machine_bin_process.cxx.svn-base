//this is /contrib/bm/dsm/pro/processes/dsm_write_state_machine_bin_process.cxx
#include<bprb/bprb_func_process.h>

#include<brdb/brdb_value.h>

#include<dsm/io/dsm_io_state_machine.h>

#include<vcl_iostream.h>
#include<vcl_vector.h>
#include<vcl_string.h>

namespace dsm_read_state_machine_bin_process_globals
{
	const unsigned int n_inputs_ = 2;
	const unsigned int n_outputs_ = 1;
}

bool dsm_read_state_machine_bin_process_cons(bprb_func_process& pro)
{
	using namespace dsm_read_state_machine_bin_process_globals;

	vcl_vector<vcl_string> input_types_(n_inputs_);
    vcl_vector<vcl_string> output_types_(n_outputs_);

    input_types_[0] = "vcl_string";
    input_types_[1] = "unsigned";
    output_types_[0] = "dsm_state_machine_base_sptr";

    if(!pro.set_output_types(output_types_))
        return false;

    if(!pro.set_input_types(input_types_))
        return false;
    
	return true;
}//end dsm_read_state_machine_bin_process_cons

bool dsm_read_state_machine_bin_process(bprb_func_process& pro)
{
	using namespace dsm_read_state_machine_bin_process_globals;

    //get input
    unsigned  i = 0;
    vcl_string filename = pro.get_input<vcl_string>(i++);
    unsigned ndims = pro.get_input<unsigned>(i++);

    //create the input stream
    vsl_b_ifstream is(filename.c_str(), vcl_ios::in|vcl_ios::binary);
    
    

    dsm_state_machine_base_sptr sm_sptr;

    switch(ndims)
    {
    //case 1:
    //    {
    //        dsm_state_machine<1>* p = new dsm_state_machine<1>;
    //        vsl_b_read(is,p);
    //        dsm_state_machine_base_sptr sm_sptr = p;   
    //    }//end case 1
    //    break;
    case 2:
        {
            dsm_state_machine<2>* p = new dsm_state_machine<2>;
            vsl_b_read(is,p);
            dsm_state_machine_base_sptr sm_sptr = p;   
        }//end case 2
        break;
    case 3:
        {
            dsm_state_machine<3>* p = new dsm_state_machine<3>;
            vsl_b_read(is,p);
            dsm_state_machine_base_sptr sm_sptr = p;   
        }//end case 3
        break;
    case 4:
        {
            dsm_state_machine<4>* p = new dsm_state_machine<4>;
            vsl_b_read(is,p);
            dsm_state_machine_base_sptr sm_sptr = p;  
        }//end case4
        break;
    default:
        {
            vcl_cerr << "------ERROR DSM_READ_STATE_MACHINE_BIN_PROCESS: " << ndims 
                     << "UNSPECIFIED DIMENSION PLEASE ADD THIS CASE OR USE A DIFFERENT VALUE.---------" << vcl_flush;
            return false;   
        }//end default
    }//end switch
    is.close();

	pro.set_output_val(0,sm_sptr);

	return true;
}//end dsm_write_state_machine_bin_process