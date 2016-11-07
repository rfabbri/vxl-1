/*
 * poisson_reconstruction_process.cxx
 *
 *  Created on: Oct 21, 2011
 *      Author: firat
 */

#include "poisson_reconstruction_process.h"
#include <bpro1/bpro1_parameters.h>
#include "../storage/dbdet_third_order_3d_edge_storage.h"
#include "../storage/dbdet_third_order_3d_edge_storage_sptr.h"
#include "../storage/elbow_mesh_file_storage.h"
#include "../storage/elbow_mesh_file_storage_sptr.h"
#include "../algo/dbdet_3d_edge_io.h"
#include <bvis1/bvis1_manager.h>
#include <vul/vul_timer.h>
#include <dbul/dbul_random.h>
#include <vul/vul_file.h>
#include <vcl_cstdlib.h>


//: Constructor
poisson_reconstruction_process::
poisson_reconstruction_process()
{
	if( 	!parameters()->add( "Reconstruction depth" , "-depth", int(11))
			|| !parameters()->add( "Solver subdivision depth" , "-solverDivide", int(8))
			|| !parameters()->add("Output folder:", "-out_folder", bpro1_filepath(""))
			|| !parameters()->add("Output file name:", "-out_file", vcl_string("output.ply"))
	)
	{
		vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
	}
}


//: Destructor
poisson_reconstruction_process::
~poisson_reconstruction_process()
{

}


//: Clone the process
bpro1_process* poisson_reconstruction_process::
clone() const
{
	return new poisson_reconstruction_process(*this);
}

//: Returns the name of this process
vcl_string poisson_reconstruction_process::
name()
{
	return "3D reconstruction from edges (Poisson)";
}

//: Provide a vector of required input types
vcl_vector< vcl_string > poisson_reconstruction_process::
get_input_type()
{
	vcl_vector< vcl_string > to_return;
	to_return.push_back("3d_edges");
	return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > poisson_reconstruction_process::
get_output_type()
{
	vcl_vector<vcl_string > to_return;
	to_return.push_back("mesh_file");
	return to_return;
}

//: Return the number of input frames for this process
int poisson_reconstruction_process::
input_frames()
{
	bvis1_manager::instance()->first_frame();
	return 1;
}


//: Return the number of output frames for this process
int poisson_reconstruction_process::
output_frames()
{
	return 1;
}

void poisson_reconstruction(const vcl_string& root_path, const vcl_string& in, const vcl_string& out, int recons_depth, int solver_divide)
{
	vcl_string* args[8];
	args[0] = new vcl_string("--in");
	args[1] = new vcl_string(in);
	args[2] = new vcl_string("--out");
	args[3] = new vcl_string(out);
	args[4] = new vcl_string("--depth");
	vcl_stringstream ss1, ss2;
	ss1 << recons_depth;
	args[5] = new vcl_string(ss1.str());
	args[6] = new vcl_string("--solverDivide");
	ss2 << solver_divide;
	args[7] = new vcl_string(ss2.str());

	vcl_string cmd = root_path + POISSON_RECONSTRUCTION_COMMAND;
	for(int i = 0; i < 8; i++)
	{
		cmd = cmd + " " + *(args[i]);
		delete args[i];
	}
	vcl_cout << cmd << vcl_endl;
	vcl_system(cmd.c_str());
}



//: Execute this process
bool poisson_reconstruction_process::
execute()
{
	vul_timer t;
	// 1. parse process parameters

	// filename

	int depth, solverDivide;
	bpro1_filepath folder_path;
	vcl_string out_file;
	vcl_string out_folder;
	parameters()->get_value( "-depth" , depth );
	parameters()->get_value( "-solverDivide" , solverDivide );
	parameters()->get_value( "-out_file" , out_file );
	out_file = vul_file::strip_directory(vul_file::expand_tilde(out_file));
	this->parameters()->get_value("-out_folder", folder_path);
	out_folder = vul_file::expand_tilde(folder_path.path);

	dbdet_third_order_3d_edge_storage_sptr in_edg_storage;
	in_edg_storage.vertical_cast(input_data_[0][0]);
	vcl_vector<dbdet_3d_edge_sptr>& in_edg = in_edg_storage->edgemap();

	vcl_cout << "Saving edges in a temporary file..." << vcl_endl;
	vcl_string cwd = vul_file::get_cwd();
	vul_file::make_directory_path(out_folder);
	vul_file::change_directory(out_folder);
	vcl_string rand_file = dbul_get_random_alphanumeric_string(10)+".xyz";

	if(!dbdet_save_3d_edges(in_edg, rand_file, false))
	{
		vcl_cout << "Cannot save edges!" << vcl_endl;
		return false;
	}

	poisson_reconstruction(cwd, rand_file, out_file, depth, solverDivide);

	elbow_mesh_file_storage_sptr out_file_storage = elbow_mesh_file_storage_new();
	out_file_storage->add_file(out_file);
	out_file_storage->set_folder(out_folder);
	this->output_data_[0].push_back(out_file_storage);

	vul_file::delete_file_glob(rand_file);
	vul_file::change_directory(cwd);

	double time_taken = t.real()/1000.0;
	t.mark();
	vcl_cout << "************ Time taken: "<< time_taken <<" sec" << vcl_endl;

	return true;
}

bool poisson_reconstruction_process::
finish()
{
	return true;
}


