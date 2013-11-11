//this is /contrib/bm/dsm/pro/processes/dsm_create_ncn_sptr.cxx
#include<bprb/bprb_func_process.h>
#include<bprb/bprb_parameters.h>

#include<brdb/brdb_value.h>

#include<dsm/dsm_ncn_sptr.h>

#include<vcl_iostream.h>
#include<vcl_vector.h>
#include<vcl_string.h>

//:global variables
//:2 inputs, the filename to write to and dsm_ncn_sptr from database
namespace dsm_create_ncn_sptr_globals
{
	const unsigned int n_inputs_ = 0;
	const unsigned int n_outputs_ = 1;
}

bool dsm_create_ncn_sptr_process_cons(bprb_func_process& pro)
{
	//set input/output types
	using namespace dsm_create_ncn_sptr_globals;
	vcl_vector<vcl_string> output_types_(n_outputs_);
	output_types_[0] = "dsm_ncn_sptr";

	if(!pro.set_output_types(output_types_))
		return false;

	return true;
}//end dsm_create_ncn_sptr_process_cons

//:creates the dsm_ncn_sptr and ncn object to be commited to db
bool dsm_create_ncn_sptr_process(bprb_func_process& pro)
{
	using namespace dsm_create_ncn_sptr_globals;

	vcl_string video_glob("");
	vcl_string targets_xml_path("");
	unsigned num_neighbors(0);
	unsigned num_pivot_pixels(0);
	unsigned num_particles(0);
	pro.parameters()->get_value("video_glob", video_glob);
	pro.parameters()->get_value("targets_xml_path", targets_xml_path);
	pro.parameters()->get_value("num_neighbors", num_neighbors);
	pro.parameters()->get_value("num_pivot_pixels", num_pivot_pixels);
	pro.parameters()->get_value("num_particles", num_particles);

	dsm_ncn_sptr ncn_sptr = new dsm_ncn(video_glob,targets_xml_path, num_neighbors, num_pivot_pixels, num_particles);

	pro.set_output_val(0,ncn_sptr);

	return true;
}//end dsm_create_ncn_sptr_process