//:
// \file
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
//
//         non-mpi version of the command line similarity matrix computation algorithm given two shape databases
//         
// \date 10/25/07
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
// inputs to be passed from input.xml  see dborl_match_shock_processor::parse(const char* param_file)
// training_set_dir, "traindir","the folder that contains training set",
// database_dir      "dbdir","the folder that contains the database"
// training_list     "-trainlist","the file that contains the list of the elements in the training set"
// database_list     "-dblist","the file that contains the list of the elements in the database"
// orl_format (bool) "orl-style","if this flag is on, the dataset folders are assumed to contain a subfolder for each object in the lists"
// shgm_folder       "-shgms","the output folder to save shgms"
// output_file       "-out","the output file to write results to"

#include "dborl_match_shock_processor.h"
//#include <dborl/algo/mpi/dborl_cluster.h>
#include <vcl_iostream.h>
#include <vul/vul_arg.h>
#include <vul/vul_timer.h>

int main(int argc, char *argv[]) {

  // Initialize defaults
  int return_status = 0;

  dborl_match_shock_processor e;
  //dborl_cluster<dborl_match_shock_processor_input, dborl_match_shock_processor, float> cluster(&e);
  vcl_vector<vcl_string> argv_str;
  for (int i = 0; i < argc; i++)
    argv_str.push_back(vcl_string(argv[i]));

  if (!e.parse_command_line(argv_str))
    return false;

  if (!e.parse(e.param_file_.c_str()))
    return false;

  vcl_vector<dborl_match_shock_processor_input> things;
  if (!e.initialize(things)) {
    vcl_cout << "Initialization failed\n";
    return -1;
  } else 
    vcl_cout << "initialized..";
  
  vcl_vector<float> results;
  for (unsigned i = 0; i < things.size(); i++) {
    float f;
    e.process(things[i], f);
    results.push_back(f);
  }

  vcl_cout << " processed..";
  e.finalize(results);
  vcl_cout << " finalized..\n";

  return 0;
}

