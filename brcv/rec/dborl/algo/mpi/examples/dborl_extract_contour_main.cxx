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
//


#include "dborl_extract_contour.h"
#include "dborl_extract_contour_params.h"
#include <dborl/algo/mpi/dborl_cluster.h>
#include <vcl_iostream.h>

int main(int argc, char *argv[]) {

  // Initialize defaults
  int return_status = 0;
  dborl_extract_contour_params_sptr params = new dborl_extract_contour_params("dborl_extract_contour");
  dborl_extract_contour e(params);  // just pass with the defaults to the constructor
  
  dborl_cluster<int, dborl_extract_contour, char> cluster(&e);

  try
  {
    //: must call this to initialize the MPI interface
    if (!cluster.mpi_initialize(argc, argv)) { // --> calls e.parse_commanline() and e.parse() 
      vcl_cout << "problems in cluster.mpi_initialize(), parameter parsing and broadcasting\n";
      throw dborl_cluster_exception (dborl_cluster_exception::MPIError, "MPI initialization failed", 0, 0);
    }

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

