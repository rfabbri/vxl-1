//:
// \file
// \brief Class that handles the MPI functionalities
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
//
// This class is mainly intended to be built on clusters running linux
// Make necessary changes to 
// \lemsvxlsrc\config\cmake\Modules\FindMPI.cmake 
// to locate headers and libraries on the linux machine to compile and build classes using this class
// 
// One may wish just to compile classes using this class on a Windows machine, then add the path to your local mpi.h
// (e.g. "C:\Program Files (x86)\MPICH\SDK\Include\")
// into:
// \lemsvxlsrc\config\cmake\Modules\FindMPI.cmake 
// To build classes using this class on Windows, need to have MPICH2 installed on the machine and the
// paths to the libraries must be specified properly in FindMPI.cmake

#if !defined(_DBORL_CLUSTER_H)
#define _DBORL_CLUSTER_H

//#define MPI_CPP_BINDING

#include <vcl_algorithm.h>

#include <vcl_vector.h>
#include <vcl_string.h>
//using namespace std;

// Exception class definition
class dborl_cluster_exception
{
public:
    // Error types
    typedef enum { Incomplete, MPIError, SystemError } cluster_error;
private:
    // Bioproc error type
    cluster_error m_error;
    // Exception message
    vcl_string m_message;
    // MPI specific error.
    int m_mpierror;
    // System specific error code.
    int m_syserror;
public:
    // Constructors
    // Default is thrown when a container tries to fetch the filter response on non-LeadProcessor
    dborl_cluster_exception() : m_error (Incomplete), m_message ("Response field is only complete on processor 0"), m_mpierror (0), m_syserror (0) {}
    // Customizable exception instance
    dborl_cluster_exception(cluster_error err, vcl_string msg, int mpierr, int syserr) : m_error (err), m_message (msg), m_mpierror (mpierr), m_syserror (syserr) {}

    // Property accessors
    const vcl_string get_message() { return m_message.data (); }
    int get_error() { return m_error; }
    int get_MPI_error() { return m_mpierror; }
    int get_syserror() { return m_syserror; }
};

template <class T, class P, class R> 
class dborl_cluster
{
public:
  dborl_cluster(P* p) : p_(p) {}

  //: must call this to initialize the MPI interface
  bool mpi_initialize(int argc, char *argv[]);
  
  //: collects results and calls finalize() method of p_ on the lead processor (i.e. once)
  bool distribute(vcl_vector<T>& t);
  //bool distribute(T *t, int cnt_t, R *r, int& cnt_r);

  //: must call this to properly finalize the MPI interface
  void mpi_finalize();

  //: processor
  P* p_;

  // Whether this processor is the lead processor
  bool lead_processor() { return my_rank_ == 0; }
  unsigned total_processors() { return total_processors_; }
  vcl_string processor_name() const {return processor_name_; }

protected:
  int my_rank_;
  int total_processors_;
  vcl_string processor_name_;

  R *send_buf_;  // each processor will have a send buf 
  R *recv_buf_;  // lead processor will have a receive buf to collect results from each process

  // Definition of each processors partition for MPI gathering operation
  int *point_offsets_;
  int *point_counts_;

  // Number of pairs per processor and the number left over
  unsigned points_per_processor(unsigned npts) { return (npts / total_processors_); }
  unsigned points_left_over(unsigned npts) { return (npts % total_processors_); }

  // Return the start point and number of points for a given point count and processor rank
  unsigned get_start_point(unsigned npts, unsigned rank) { return (rank * points_per_processor(npts) + vcl_min (rank, points_left_over(npts))); }
  unsigned get_number_points(unsigned npts, unsigned rank) { return (points_per_processor(npts) + (rank < points_left_over (npts) ? 1 : 0)); }

};


#endif  //_DBORL_CLUSTER_H
