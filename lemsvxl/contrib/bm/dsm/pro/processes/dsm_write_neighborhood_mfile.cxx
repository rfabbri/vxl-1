//this is /contrib/bm/dsm/pro/processes/dsm_write_neighborhood_mfile.cxx
#include<bprb/bprb_func_process.h>

#include<brdb/brdb_value.h>

#include<dsm/dsm_ncn_sptr.h>

#include<vcl_iostream.h>
#include<vcl_vector.h>
#include<vcl_string.h>

//process which takes a dsm_ncn_sptr and writes its neighborhood as an mfile.

//:global variables
namespace dsm_write_neighborhood_mfile_globals
{
	const unsigned int n_inputs_ = 2;
	const unsigned int n_outputs_ = 0;
}

bool dsm_write_neighborhood_mfile_process_cons(bprb_func_process& pro)
{
	//set input/output types
	using namespace dsm_write_neighborhood_mfile_globals;

	vcl_vector<vcl_string> input_types_(n_inputs_);
	unsigned i = 0;
	input_types_[i++] = "vcl_string";
	input_types_[i++] = "dsm_ncn_sptr";

	if(!pro.set_input_types(input_types_))
		return false;
	
	return true;
}//end dsm_write_neighborhood_mfile_process_cons

bool dsm_write_neighborhood_mfile_process(bprb_func_process& pro)
{
	using namespace dsm_write_neighborhood_mfile_globals;

	if( pro.n_inputs() < n_inputs_ )
	{
		vcl_cerr << pro.name() << "dsm_write_neighborhood_mfile_process: The input number should be: " << n_inputs_ << vcl_flush;
		return false;
	}

	//get intputs
	unsigned i = 0;
	vcl_string filename = pro.get_input<vcl_string>(i++);
	dsm_ncn_sptr ncn_sptr = pro.get_input<dsm_ncn_sptr>(i++);

	return ncn_sptr->write_neighborhood_mfile(filename);

}//end dsm_write_neighborhood_mfile_process
