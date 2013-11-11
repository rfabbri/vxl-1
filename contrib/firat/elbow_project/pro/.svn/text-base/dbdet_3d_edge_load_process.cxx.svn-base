/*
 * dbdet_3d_edge_load_process.cxx
 *
 *  Created on: Oct 24, 2011
 *      Author: firat
 */

#include "dbdet_3d_edge_load_process.h"
#include <bpro1/bpro1_parameters.h>
#include "../storage/dbdet_third_order_3d_edge_storage.h"
#include "../storage/dbdet_third_order_3d_edge_storage_sptr.h"
#include "../algo/dbdet_3d_edge_io.h"
#include <vul/vul_timer.h>

//: Constructor
dbdet_3d_edge_load_process::
dbdet_3d_edge_load_process()
{

	if( !parameters()->add( "Edge file" , "-edgefile",
			bpro1_filepath("")) )
	{
		vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
	}
}


//: Destructor
dbdet_3d_edge_load_process::
~dbdet_3d_edge_load_process()
{

}


//: Clone the process
bpro1_process* dbdet_3d_edge_load_process::
clone() const
{
	return new dbdet_3d_edge_load_process(*this);
}

//: Returns the name of this process
vcl_string dbdet_3d_edge_load_process::
name()
{
	return "Load 3D edges";
}

//: Provide a vector of required input types
vcl_vector< vcl_string > dbdet_3d_edge_load_process::
get_input_type()
{
	vcl_vector< vcl_string > to_return;
	return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbdet_3d_edge_load_process::
get_output_type()
{
	vcl_vector<vcl_string > to_return;
	to_return.push_back("3d_edges");
	return to_return;
}

//: Return the number of input frames for this process
int dbdet_3d_edge_load_process::
input_frames()
{
	return 0;
}


//: Return the number of output frames for this process
int dbdet_3d_edge_load_process::
output_frames()
{
	return 1;
}

//: Execute this process
bool dbdet_3d_edge_load_process::
execute()
{
	// 1. parse process parameters
	vul_timer t;
	// filename

	bpro1_filepath edgefile;
	parameters()->get_value( "-edgefile" , edgefile );
	vcl_vector<dbdet_3d_edge_sptr> edgemap;
	if(!dbdet_load_3d_edges(edgefile.path, edgemap))
	{
		vcl_cout << "Cannot load edges!" << vcl_endl;
		return false;
	}

	dbdet_third_order_3d_edge_storage_sptr edge_storage = dbdet_third_order_3d_edge_storage_new();
	edge_storage->set_edgemap(edgemap);
	this->output_data_[0].push_back(edge_storage);

	double time_taken = t.real()/1000.0;
	t.mark();
	vcl_cout << "************ Time taken: "<< time_taken <<" sec" << vcl_endl;
}


bool dbdet_3d_edge_load_process::
finish()
{
	return true;
}


