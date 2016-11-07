/*
 * elbow_get_largest_edge_cluster_process.cxx
 *
 *  Created on: Oct 18, 2011
 *      Author: firat
 */

#include "elbow_get_largest_edge_cluster_process.h"
#include <bpro1/bpro1_parameters.h>
#include "../storage/dbdet_third_order_3d_edge_storage.h"
#include "../storage/dbdet_third_order_3d_edge_storage_sptr.h"
#include "../storage/elbow_vil3d_storage_sptr.h"
#include "../storage/elbow_vil3d_storage.h"
#include "../algo/elbow_edge_clustering.h"
#include <bvis1/bvis1_manager.h>
#include <vul/vul_timer.h>


//: Constructor
elbow_get_largest_edge_cluster_process::
elbow_get_largest_edge_cluster_process()
{
	if( 	!parameters()->add( "Grid spacing (h)" , "-h", double(1))
			|| !parameters()->add( "Strength threshold" , "-str", double(10.0)))
	{
		vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
	}
}


//: Destructor
elbow_get_largest_edge_cluster_process::
~elbow_get_largest_edge_cluster_process()
{

}


//: Clone the process
bpro1_process* elbow_get_largest_edge_cluster_process::
clone() const
{
	return new elbow_get_largest_edge_cluster_process(*this);
}

//: Returns the name of this process
vcl_string elbow_get_largest_edge_cluster_process::
name()
{
	return "Compute largest edge cluster";
}

//: Provide a vector of required input types
vcl_vector< vcl_string > elbow_get_largest_edge_cluster_process::
get_input_type()
{
	vcl_vector< vcl_string > to_return;
	to_return.push_back("3d_edges");
	to_return.push_back("3d_dataset");
	return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > elbow_get_largest_edge_cluster_process::
get_output_type()
{
	vcl_vector<vcl_string > to_return;
	to_return.push_back("3d_edges");
	return to_return;
}

//: Return the number of input frames for this process
int elbow_get_largest_edge_cluster_process::
input_frames()
{
	bvis1_manager::instance()->first_frame();
	return 1;
}


//: Return the number of output frames for this process
int elbow_get_largest_edge_cluster_process::
output_frames()
{
	return 1;
}



//: Execute this process
bool elbow_get_largest_edge_cluster_process::
execute()
{
	vul_timer t;
	// 1. parse process parameters

	// filename

	double h, str;
	parameters()->get_value( "-h" , h );
	parameters()->get_value( "-str" , str );

	dbdet_third_order_3d_edge_storage_sptr in_edg_storage;
	in_edg_storage.vertical_cast(input_data_[0][0]);
	vcl_vector<dbdet_3d_edge_sptr>& in_edg = in_edg_storage->edgemap();
	vcl_vector<dbdet_3d_edge_sptr> in_edg_strong;
	dbdet_keep_strong_edges(in_edg,	in_edg_strong, str, false);

	elbow_vil3d_storage_sptr data_storage;
	data_storage.vertical_cast(input_data_[0][1]);
	vil3d_image_view<double>& image = data_storage->image();

	vcl_vector<dbdet_3d_edge_sptr> out_edg;
	elbow_get_largest_edge_cluster(in_edg_strong, out_edg,	image.ni(), image.nj(), image.nk(), h);

	dbdet_third_order_3d_edge_storage_sptr out_edge_storage = dbdet_third_order_3d_edge_storage_new();
	out_edge_storage->set_edgemap(out_edg);
	this->output_data_[0].push_back(out_edge_storage);

	double time_taken = t.real()/1000.0;
	t.mark();
	vcl_cout << "************ Time taken: "<< time_taken <<" sec" << vcl_endl;

	return true;
}

bool elbow_get_largest_edge_cluster_process::
finish()
{
	return true;
}




