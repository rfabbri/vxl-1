/*
 * elbow_show_3d_mesh_process.cxx
 *
 *  Created on: Oct 23, 2011
 *      Author: firat
 */

#include "elbow_show_3d_mesh_process.h"
#include <bpro1/bpro1_parameters.h>
#include "../storage/elbow_mesh_file_storage.h"
#include "../storage/elbow_mesh_file_storage_sptr.h"
#include <bvis1/bvis1_manager.h>
#include <vul/vul_timer.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vcl_cstdlib.h>


//: Constructor
elbow_show_3d_mesh_process::
elbow_show_3d_mesh_process()
{

}


//: Destructor
elbow_show_3d_mesh_process::
~elbow_show_3d_mesh_process()
{

}


//: Clone the process
bpro1_process* elbow_show_3d_mesh_process::
clone() const
{
	return new elbow_show_3d_mesh_process(*this);
}

//: Returns the name of this process
vcl_string elbow_show_3d_mesh_process::
name()
{
	return "Show 3D mesh";
}

//: Provide a vector of required input types
vcl_vector< vcl_string > elbow_show_3d_mesh_process::
get_input_type()
{
	vcl_vector< vcl_string > to_return;
	to_return.push_back("mesh_file");
	return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > elbow_show_3d_mesh_process::
get_output_type()
{
	vcl_vector<vcl_string > to_return;
	return to_return;
}

//: Return the number of input frames for this process
int elbow_show_3d_mesh_process::
input_frames()
{
	bvis1_manager::instance()->first_frame();
	return 1;
}


//: Return the number of output frames for this process
int elbow_show_3d_mesh_process::
output_frames()
{
	return 0;
}

void elbow_mesh_view(const vcl_string& root_path, const vcl_vector<vcl_string>& in)
{
	vcl_string cmd = root_path + MESH_VIEW_COMMAND;
	for(int i = 0; i < in.size(); i++)
	{
		cmd = cmd + " " + in[i];
	}
	vcl_cout << cmd << vcl_endl;
	vcl_system(cmd.c_str());
}


//: Execute this process
bool elbow_show_3d_mesh_process::
execute()
{
	vul_timer t;
	// 1. parse process parameters

	// filename

	elbow_mesh_file_storage_sptr in_file_storage;
	in_file_storage.vertical_cast(input_data_[0][0]);
	vcl_string folder = in_file_storage->folder();
	vcl_vector<vcl_string> files = in_file_storage->files();
	vcl_string cwd = vul_file::get_cwd();
	vul_file::change_directory(folder);

	elbow_mesh_view(cwd, files);

	vul_file::change_directory(cwd);

	double time_taken = t.real()/1000.0;
	t.mark();
	vcl_cout << "************ Time taken: "<< time_taken <<" sec" << vcl_endl;

	return true;
}

bool elbow_show_3d_mesh_process::
finish()
{
	return true;
}





