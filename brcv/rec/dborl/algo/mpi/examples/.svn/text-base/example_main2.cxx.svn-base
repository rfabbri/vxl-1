//:
// \file
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/30/07
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
//

#include "example_processor2.h"
#include <dborl/algo/mpi/dborl_cluster.h>
#include <vcl_iostream.h>

int main(int argc, char *argv[]) {

  // Initialize defaults
  int return_status = 0;
  example_processor2 e;

  dborl_cluster<int, example_processor2, ex_class> cluster(&e);

  try
  {
    //: must call this to initialize the MPI interface
    if (!cluster.mpi_initialize(argc, argv))
      throw dborl_cluster_exception (dborl_cluster_exception::MPIError, "MPI initialization failed", 0, 0);

    vcl_vector<int> things; 
    e.initialize(things);
  
    cluster.distribute(things);  // throws an exception if processing is Incomplete
                                 // this method calls finalize method of the process once results are collected from each processor

    things.clear();

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

