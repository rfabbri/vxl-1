/*
 * elbow_mesh_post_process.cxx
 *
 *  Created on: Oct 23, 2011
 *      Author: firat
 */

#include "elbow_mesh_post_process.h"
#include <bpro1/bpro1_parameters.h>
#include "../storage/elbow_mesh_file_storage.h"
#include "../storage/elbow_mesh_file_storage_sptr.h"
#include <bvis1/bvis1_manager.h>
#include <vul/vul_timer.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <cstdlib>


//: Constructor
elbow_mesh_post_process::
elbow_mesh_post_process()
{
	if( 	!parameters()->add( "Number of structures" , "-num", int(3))

	)
	{
		std::cerr << "ERROR: Adding parameters in " __FILE__ << std::endl;
	}
}


//: Destructor
elbow_mesh_post_process::
~elbow_mesh_post_process()
{

}


//: Clone the process
bpro1_process* elbow_mesh_post_process::
clone() const
{
	return new elbow_mesh_post_process(*this);
}

//: Returns the name of this process
std::string elbow_mesh_post_process::
name()
{
	return "Mesh post-processing for visualization";
}

//: Provide a vector of required input types
std::vector< std::string > elbow_mesh_post_process::
get_input_type()
{
	std::vector< std::string > to_return;
	to_return.push_back("mesh_file");
	return to_return;
}


//: Provide a vector of output types
std::vector< std::string > elbow_mesh_post_process::
get_output_type()
{
	std::vector<std::string > to_return;
	to_return.push_back("mesh_file");
	return to_return;
}

//: Return the number of input frames for this process
int elbow_mesh_post_process::
input_frames()
{
	bvis1_manager::instance()->first_frame();
	return 1;
}


//: Return the number of output frames for this process
int elbow_mesh_post_process::
output_frames()
{
	return 1;
}

void elbow_mesh_cc(const std::string& root_path, const std::string& in, int num)
{
	// run mesh_cc
	std::string* args[5];
	args[0] = new std::string("-t");
	std::stringstream ss1;
	ss1 << num;
	args[1] = new std::string(ss1.str());
	args[2] = new std::string("-s");
	args[3] = new std::string(in);
	args[4] = new std::string(in);

	std::string cmd = root_path + MESH_CC_COMMAND;
	for(int i = 0; i < 5; i++)
	{
		cmd = cmd + " " + *(args[i]);
		delete args[i];
	}
	std::cout << cmd << std::endl;
	std::system(cmd.c_str());
}

void elbow_mesh_shade(const std::string& root_path, const std::vector<std::string>& in)
{
	// run mesh_shade
	std::string colors[6] = {"FF0000", "00FF00", "0000FF", "FFFF00", "00FFFF", "FF00FF"};
	for(int i = 0; i < in.size(); i++)
	{
		std::string* args[4];
		args[0] = new std::string(in[i]);
		args[1] = new std::string("color");
		args[2] = new std::string(colors[i%6]);
		args[3] = new std::string(in[i]);

		std::string cmd = root_path + MESH_SHADE_COMMAND;
		for(int j = 0; j < 4; j++)
		{
			cmd = cmd + " " + *(args[j]);
			delete args[j];
		}
		std::cout << cmd << std::endl;
		std::system(cmd.c_str());
	}
}



//: Execute this process
bool elbow_mesh_post_process::
execute()
{
	vul_timer t;
	// 1. parse process parameters

	// filename

	int num;
	parameters()->get_value( "-num" , num );

	elbow_mesh_file_storage_sptr in_file_storage;
	in_file_storage.vertical_cast(input_data_[0][0]);
	std::string folder = in_file_storage->folder();
	std::vector<std::string> files = in_file_storage->files();
	std::string cwd = vul_file::get_cwd();
	vul_file::change_directory(folder);

	elbow_mesh_cc(cwd, files[0], num);
	std::vector<std::string> valid_files;
	for (vul_file_iterator fn = std::string("cc*")+files[0]; fn; ++fn)
	{
		valid_files.push_back(fn());
	}
	elbow_mesh_shade(cwd, valid_files);

	elbow_mesh_file_storage_sptr out_file_storage = elbow_mesh_file_storage_new();
	for(int i = 0; i < valid_files.size(); i++)
	{
		out_file_storage->add_file(valid_files[i]);
	}
	out_file_storage->set_folder(folder);
	this->output_data_[0].push_back(out_file_storage);

	vul_file::change_directory(cwd);

	double time_taken = t.real()/1000.0;
	t.mark();
	std::cout << "************ Time taken: "<< time_taken <<" sec" << std::endl;

	return true;
}

bool elbow_mesh_post_process::
finish()
{
	return true;
}





