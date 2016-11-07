//:
// \file
// \brief Base class for the processor to be used with dborl_cluster for distributed processing via MPI
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/25/07
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
// usage:
// inherit from dborl_processor_base and fill in initialize(), process() and finalize() methods
// initialize() is called from each processor on the cluster, i.e. as many times as the number of processors.
//              the method is meant to prepare the vector of "things" on each processor,
//              the "things" should not take too much memory not to eat up memory on the processors
//              if they do, then binary write and read methods should be used and only paths should be passed in the "things" vector
// finalize() is called only once from the lead processor.
//            the method is meant to get the results vector and do the final processing, e.g. process/print/save the overall results.
//
// create_datatype_for_R() the only method that requires use of MPI functions explicitly
//                         R is the type of the result class, this class is meant to be light and short length since
//                         this is the only class whose instances will be passed around as messages
//                         Sending instances of STL classes like vector, string etc. is complicated and 
//                         there is no example for this yet. (for now binary read/write methods could be utilized to pass around complicated class instances)
//                         but see example_processor2 which contains primitive type variables
//                         and example_processor3 which contains fixed sized arrays of primitives
// 

#if !defined(_DBORL_PROCESSOR_BASE_H)
#define _DBORL_PROCESSOR_BASE_H

// To compile on windows machine add the path to your local mpi.h
// (e.g. "C:\Program Files (x86)\MPICH\SDK\Include\")
// into:
// \lemsvxlsrc\config\cmake\Modules\FindMPI.cmake 
#include <mpi.h>
// Note: To build classes using this class on Windows, need to have MPICH2 installed on the machine and the
//       paths to the libraries specified properly in FindMPI.cmake


#include <vcl_string.h>
#include <vcl_vector.h>
//using namespace std;

template <class T, class R> 
class dborl_processor_base
{
public:

  //: this method is run on each processor after lead processor broadcasts its command 
  //  line arguments to all the processors since only on the lead processor is passed the command line arguments by mpirun
  //virtual bool parse_command_line(int argc, char *argv[]) = 0;
  virtual bool parse_command_line(vcl_vector<vcl_string>& argv) = 0;

  //: this method is run on each processor
  virtual bool parse(const char* param_file) = 0;

  //: this method is run on each processor
  virtual bool initialize(vcl_vector<T>& t) = 0;

  //: this method is run in a distributed mode on each processor on the cluster
  virtual bool process(T t, R& r) = 0;

  //: this method is run on the lead processor once after results are collected from each processor
  virtual bool finalize(vcl_vector<R>& results) = 0;

#ifdef MPI_CPP_BINDING
  virtual MPI::Datatype create_datatype_for_R() = 0;
#else
  virtual MPI_Datatype create_datatype_for_R() = 0;
#endif

  vcl_string param_file_;
};

#endif  //_DBORL_PROCESSOR_BASE_H
