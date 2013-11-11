//this is /contrib/bm/dsm/pro/processes/dsm_state_machine.cxx
#include<bprb/bprb_func_process.h>
//#include<bprb/bprb_parameters.h>

#include<brdb/brdb_value.h>

#include<dsm/dsm_state_machine.h>

#include<vcl_iostream.h>
#include<vcl_vector.h>
#include<vcl_string.h>


namespace dsm_state_machine_process_globals
{
	const unsigned int n_inputs_ = 7;
	const unsigned int n_outputs_ = 1;
}

bool dsm_state_machine_process_cons(bprb_func_process& pro)
{
	using namespace dsm_state_machine_process_globals;

	vcl_vector<vcl_string> input_types_(n_inputs_);
	vcl_vector<vcl_string> output_types_(n_outputs_);

	unsigned i = 0;
    input_types_[i++] = "unsigned"; //state machine id
	input_types_[i++] = "unsigned"; //ndims
	input_types_[i++] = "unsigned"; //t_forget
	input_types_[i++] = "double"; //prob_thresh
	input_types_[i++] = "double"; //mahalan_factor
	input_types_[i++] = "double"; //init_covar
	input_types_[i++] = "double"; //min_covar

	output_types_[0] = "dsm_state_machine_base_sptr";

    if(!pro.set_input_types(input_types_))
        return false;
    
	if(!pro.set_output_types(output_types_))
		return false;

	return true;
}//end dsm_state_machine_process_cons

bool dsm_state_machine_process(bprb_func_process& pro)
{
	using namespace dsm_state_machine_process_globals;

    //get input
    unsigned  i = 0;
    unsigned id = pro.get_input<unsigned>(i++);

    unsigned ndims = pro.get_input<unsigned>(i++);
	unsigned t_forget = pro.get_input<unsigned>(i++);
    double prob_thresh = pro.get_input<unsigned>(i++);
    double mahalan_factor = pro.get_input<unsigned>(i++);
    double dinit_covar = pro.get_input<double>(i++);
    double dmin_covar = pro.get_input<double>(i++);



    switch(ndims)
    {
    case 1:
        {
            vcl_cout << "ERROR: dsm_state_machine_process ---------Cannot instantiate a state machine with dimension 1. Not yet implemented.--------- \n";
            return false;
        }//end case 1
        break;
    case 2:
        {
            vnl_matrix<double> init_covar = vnl_diag_matrix<double>(2,dinit_covar);
            vnl_matrix<double> min_covar(2,2,dmin_covar);
            dsm_state_machine_base_sptr sm_sptr = new dsm_state_machine<2>(id,t_forget, prob_thresh,mahalan_factor,init_covar,min_covar);
            pro.set_output_val(0,sm_sptr);
            return true;
        }//end case 2
        break;
    case 3:
        {
            vnl_matrix<double> init_covar = vnl_diag_matrix<double>(3,dinit_covar);
            vnl_matrix<double> min_covar(3,3,dmin_covar);
            dsm_state_machine_base_sptr sm_sptr = new dsm_state_machine<3>(id,t_forget, prob_thresh,mahalan_factor,init_covar,min_covar);
            pro.set_output_val(0,sm_sptr);
            return true;
        }//end case 3
        break;
    case 4:
        {
            vnl_matrix<double> init_covar = vnl_diag_matrix<double>(4,dinit_covar);
            vnl_matrix<double> min_covar(4,4,dmin_covar);
            dsm_state_machine_base_sptr sm_sptr = new dsm_state_machine<4>(id,t_forget, prob_thresh,mahalan_factor,init_covar,min_covar);
            pro.set_output_val(0,sm_sptr);
            return true;
        }//end case4
        break;
    default:
        {
            vcl_cerr << "------ERROR PROCESS DSM_STATE_MACHINE_PROCESS: " << ndims 
                     << "UNSPECIFIED DIMENSION PLEASE ADD THIS CASE OR USE A DIFFERENT VALUE.---------" << vcl_flush;
            return false;   
        }//end default
    }//end switch


	return false;
}//end dsm_state_machine_process