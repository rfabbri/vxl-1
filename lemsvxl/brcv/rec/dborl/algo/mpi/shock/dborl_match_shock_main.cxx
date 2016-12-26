//:
// \file
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
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
#include <dborl/algo/mpi/dborl_cluster.h>
#include <vcl_iostream.h>
#include <vul/vul_arg.h>
#include <vul/vul_timer.h>

int main(int argc, char *argv[]) {

  // Initialize defaults
  int return_status = 0;

  dborl_match_shock_processor e;
  dborl_cluster<dborl_match_shock_processor_input, dborl_match_shock_processor, float> cluster(&e);

  try
  {
    //: must call this to initialize the MPI interface
    if (!cluster.mpi_initialize(argc, argv))  //-> in this method the command line arguments are broadcasted to all others by the lead processor
      throw dborl_cluster_exception (dborl_cluster_exception::MPIError, "MPI initialization failed", 0, 0);
  
    vcl_vector<dborl_match_shock_processor_input> things; 
    if (!e.initialize(things)) {
      vcl_cout << "Problems during initialization! Exiting!\n";
      throw dborl_cluster_exception (dborl_cluster_exception::MPIError, "data initialization failed", 0, 0);
    } 

    cluster.distribute(things);  // throws an exception if processing is Incomplete
                                 // this method calls finalize method of the process once results are collected from each processor

    things.clear();

    if (cluster.lead_processor())
      e.print_time();
          
  } catch (int i) {
        // Error detected in this method
        return_status = i;
  } catch (dborl_cluster_exception& e) {
        // Error thrown by dborl_cluster class
        if (e.get_error() != e.Incomplete)
        {
            vcl_cout << "dborl_cluster_exception: " << e.get_message()
            << " err: " << e.get_error() << " mpi: " 
            << e.get_MPI_error() << " sys: " 
            << e.get_syserror() << vcl_endl;

            return_status = e.get_error();
        }
  }

  //: must call this to properly finalize the MPI interface
  //  collects results and calls finalize() method of p_ on the lead processor (i.e. once)
  cluster.mpi_finalize();

  return (return_status);
}

