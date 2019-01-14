//this is /contrib/bm/dsm/pro/processes/dsm_write_neighborhood_xml.cxx
#include<bprb/bprb_func_process.h>

#include<brdb/brdb_value.h>

#include<dsm/dsm_ncn_sptr.h>

#include<iostream>
#include<vector>
#include<string>

//process which takes a dsm_ncn_sptr and path and writes a neighborhood xml file.

//:global variables
namespace dsm_write_neighborhood_xml_globals
{
	const unsigned int n_inputs_ = 2;
	const unsigned int n_outputs_ = 0;
}

bool dsm_write_neighborhood_xml_process_cons(bprb_func_process& pro)
{
	//set input/output types
	using namespace dsm_write_neighborhood_xml_globals;

	std::vector<std::string> input_types_(n_inputs_);
	unsigned i = 0;
	input_types_[i++] = vcl_string";
	input_types_[i++] = "dsm_ncn_sptr";

	if(!pro.set_input_types(input_types_))
		return false;

	return true;
}//end dsm_write_neighborhood_xml_process_cons

bool dsm_write_neighborhood_xml_process(bprb_func_process& pro)
{
	using namespace dsm_write_neighborhood_xml_globals;

	if( pro.n_inputs() < n_inputs_ )
	{
		std::cerr << pro.name() << "dsm_write_neighborhood_xml_process: The input number should be: " << n_inputs_ << std::flush;
		return false;
	}

	//get inputs
	unsigned i = 0;
	std::string filename = pro.get_input<std::string>(i++);
	dsm_ncn_sptr ncn_sptr = pro.get_input<dsm_ncn_sptr>(i++);

	return ncn_sptr->write_neighborhood_xml(filename);

}//end dsm_write_neighborhood_xml_process
