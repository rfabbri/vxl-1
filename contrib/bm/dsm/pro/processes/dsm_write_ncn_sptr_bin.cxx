//this is /contrib/bm/dsm/pro/processes/dsm_write_ncn_sptr_bin.cxx
//process to write an ncn sptr in the database to disk

#include<bprb/bprb_func_process.h>

#include<brdb/brdb_value.h>

#include<dsm/dsm_ncn_sptr.h>

#include<vbl/io/vbl_io_smart_ptr.h>

#include<vcl_iostream.h>
#include<vcl_vector.h>
#include<vcl_string.h>

//#include<vsl/vsl_quick_file.h>

//:Process takes a filename and dsm_ncn_sptr from database and writes it to disk
namespace dsm_write_ncn_sptr_bin_globals
{
	const unsigned int n_inputs_ = 2;
	const unsigned int n_outputs_ = 0;
}

//:sets input and output types
bool dsm_write_ncn_sptr_bin_process_cons(bprb_func_process& pro)
{
	using namespace dsm_write_ncn_sptr_bin_globals;

	vcl_vector<vcl_string> input_types_(n_inputs_);
	input_types_[0] = "vcl_string";
	input_types_[1] = "dsm_ncn_sptr";

	if( !pro.set_input_types(input_types_) )
		return false;

	return true;
}//end dsm_write_ncn_sptr_process_cons

bool dsm_write_ncn_sptr_bin_process(bprb_func_process& pro)
{
	using namespace dsm_write_ncn_sptr_bin_globals;

	if( pro.n_inputs() < n_inputs_ )
	{
		vcl_cout << pro.name() << "dsm_write_ncn_sptr_process: The input number should be " << n_inputs_ << vcl_endl;
		return false;
	}

	//get inputs
	unsigned i = 0;
	vcl_string filename = pro.get_input<vcl_string>(i++);
	dsm_ncn_sptr ncn_sptr = pro.get_input<dsm_ncn_sptr>(i++);

	vcl_cout << "Writing dsm_ncn_sptr to: " << filename;
	//vcl_ofstream of(filename.c_str(), vcl_ios::out);
	//vsl_b_ostream os(&of);
	vsl_b_ofstream os(filename.c_str(), vcl_ios::out|vcl_ios::binary);
	vsl_b_write(os,ncn_sptr);

	return true;
}//end dsm_write_ncn_sptr_process