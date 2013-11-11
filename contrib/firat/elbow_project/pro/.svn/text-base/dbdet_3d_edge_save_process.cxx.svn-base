/*
 * dbdet_3d_edge_save_process.cxx
 *
 *  Created on: Oct 24, 2011
 *      Author: firat
 */

#include "dbdet_3d_edge_save_process.h"
#include <bpro1/bpro1_parameters.h>
#include "../storage/dbdet_third_order_3d_edge_storage.h"
#include "../storage/dbdet_third_order_3d_edge_storage_sptr.h"
#include "../algo/dbdet_3d_edge_io.h"
#include <vul/vul_timer.h>


//: Constructor
dbdet_3d_edge_save_process::
dbdet_3d_edge_save_process()
{

	if( !parameters()->add( "Edge file" , "-edgefile",
			bpro1_filepath("")) )
	{
		vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
	}
}


//: Destructor
dbdet_3d_edge_save_process::
~dbdet_3d_edge_save_process()
{

}


//: Clone the process
bpro1_process* dbdet_3d_edge_save_process::
clone() const
{
	return new dbdet_3d_edge_save_process(*this);
}

//: Returns the name of this process
vcl_string dbdet_3d_edge_save_process::
name()
{
	return "Save 3D edges";
}

//: Provide a vector of required input types
vcl_vector< vcl_string > dbdet_3d_edge_save_process::
get_input_type()
{
	vcl_vector< vcl_string > to_return;
	to_return.push_back("3d_edges");
	return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbdet_3d_edge_save_process::
get_output_type()
{
	vcl_vector<vcl_string > to_return;
	return to_return;
}

//: Return the number of input frames for this process
int dbdet_3d_edge_save_process::
input_frames()
{
	return 1;
}


//: Return the number of output frames for this process
int dbdet_3d_edge_save_process::
output_frames()
{
	return 0;
}

//: Execute this process
bool dbdet_3d_edge_save_process::
execute()
{

	vul_timer t;
	// 1. parse process parameters

	// filename

	bpro1_filepath edgefile;
	parameters()->get_value( "-edgefile" , edgefile );

	dbdet_third_order_3d_edge_storage_sptr in_edg_storage;
	in_edg_storage.vertical_cast(input_data_[0][0]);
	vcl_vector<dbdet_3d_edge_sptr>& in_edg = in_edg_storage->edgemap();

	dbdet_save_3d_edges(in_edg, edgefile.path);

	double time_taken = t.real()/1000.0;
	t.mark();
	vcl_cout << "************ Time taken: "<< time_taken <<" sec" << vcl_endl;

	return true;
}


bool dbdet_3d_edge_save_process::
finish()
{
	return true;
}



