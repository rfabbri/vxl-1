//:
// \file
// \brief Class that holds a dataset of objects
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/25/07
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
//

// To compile on windows machine add the path to your local mpi.h
// (e.g. "C:\Program Files (x86)\MPICH\SDK\Include\")
// into:
// \lemsvxlsrc\config\cmake\Modules\FindMPI.cmake 
#include <mpi.h>
// Note: To build classes using this class on Windows, need to have MPICH2 installed on the machine and the
//       paths to the libraries specified properly in FindMPI.cmake

#include "dborl_cluster.h"
#include <vcl_iostream.h>
//#include <new>
//using namespace std;

void helper_tokenizer(const vcl_string& str,
                      vcl_vector<vcl_string>& tokens,
                      const vcl_string& delimiters = " ")
{
    // Skip delimiters at beginning.
    vcl_string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    vcl_string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (vcl_string::npos != pos || vcl_string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}


template <class T, class P, class R> 
bool dborl_cluster<T, P, R>::mpi_initialize(int argc, char *argv[])
{
  // Initialize MPI interface
#ifdef MPI_CPP_BINDING
  MPI::Init(argc, argv);
#else
  MPI_Init(&argc,&argv);
#endif  // MPI_CPP_BINDING

#ifdef MPI_CPP_BINDING
 my_rank_ = MPI::COMM_WORLD.Get_rank();
#else
 MPI_Comm_rank(MPI_COMM_WORLD,&my_rank_);
#endif

#ifdef MPI_CPP_BINDING
 total_processors_ = MPI::COMM_WORLD.Get_size();
#else
 MPI_Comm_size(MPI_COMM_WORLD,&total_processors_);
#endif

 

 // processor name
  vcl_string name;
#ifdef MPI_CPP_BINDING
  int namelen;
  char *processor_name = new char[MPI_MAX_PROCESSOR_NAME];
  MPI::Get_processor_name(processor_name, namelen);
  name = processor_name;
  delete [] processor_name;
#else
  int  namelen;
  char processor_name[MPI_MAX_PROCESSOR_NAME];
  MPI_Get_processor_name(processor_name,&namelen);
  name = processor_name;
#endif
  this->processor_name_ = name;






  int size = 10; // first broadcast the size of argv to be safe
  int dummy_argc = argc;
  char *buffer;
  vcl_string dummy = "";

  //vcl_cout << "Hello World! I am " << name << " " << my_rank_ << " of " << total_processors_ << " size before Bcast: " << size << vcl_endl;

  //: lead processor should parse the input and update the size
  if (lead_processor()) {
    for (int i = 0; i < argc; i++) {
      dummy += vcl_string(argv[i]) + " ";
    }
    size = dummy.size();
    //vcl_cout << "lead processor will send: " << dummy << vcl_endl;
  }

  //: MPI_Bcast is such that the root (lead) processor broadcasts and all the others receive.
#ifdef MPI_CPP_BINDING
  MPI::COMM_WORLD.Bcast (&size, 1, MPI_INT, 0);  // last argumant is the rank of the receiver which is the LeadProcessor, i.e. 0
  MPI::COMM_WORLD.Bcast (&dummy_argc, 1, MPI_INT, 0);  // last argumant is the rank of the receiver which is the LeadProcessor, i.e. 0
#else
  MPI_Bcast (&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast (&dummy_argc, 1, MPI_INT, 0, MPI_COMM_WORLD);
#endif

  //vcl_cout << "Hello World! I am " << name << " size after Bcast: " << size << " " << vcl_endl;

  //now allocate space for buffer
  buffer = new char[size+1];
  if (lead_processor()) {
    for (int i = 0; i < size; i++) 
      buffer[i] = dummy[i];
  } else {
    for (int i = 0; i < size; i++) 
      buffer[i] = '%';
  }
  buffer[size] = '\0';
  //vcl_cout << "Hello World! I am " << name << " buffer before Bcast: " << buffer << " " << vcl_endl;

  //: MPI_Bcast is such that the root (lead) processor broadcasts and all the others receive.
#ifdef MPI_CPP_BINDING
  MPI::COMM_WORLD.Bcast (buffer, size+1, MPI_CHAR, 0);  // last argumant is the rank of the receiver which is the LeadProcessor, i.e. 0
#else
  MPI_Bcast (buffer, size+1, MPI_CHAR, 0, MPI_COMM_WORLD);
#endif
  //vcl_cout << "Hello World! I am " << name << " buffer after Bcast: " << buffer << " " << vcl_endl;

  vcl_vector<vcl_string> tokens;
  dummy = buffer; // put buffer into dummy in all processors
  helper_tokenizer(dummy, tokens);
  //vcl_cout << "Hello World! I am " << name << " tokenizer found: " << tokens.size() << " tokens: \n";
  //for (unsigned i = 0; i < tokens.size(); i++) {
  //  vcl_cout << "\t\ttokens[" << i << "]: " << tokens[i] << vcl_endl;
  //}
  
  //if (!p_.parse_command_line(tokens.size(), argv_new))
  if (!p_->parse_command_line(tokens)) {
    vcl_cout << "cannot parse command line!\n";
    return false;  // --> to exit
  }

  //vcl_cout << "param_file: " << p_->param_file_ << vcl_endl;
  if (!p_->parse(p_->param_file_.c_str())) { // param_file_ is set by the previous process
    vcl_cout << "cannot parse parameter file: " << p_->param_file_ << vcl_endl;
    return false;  // --> to exit
  }

  return true;
}

template <class T, class P, class R> 
void dborl_cluster<T, P, R>::mpi_finalize()
{
  // Terminate MPI
#ifdef MPI_CPP_BINDING
  MPI::Finalize ();
#else
  MPI_Finalize ();
#endif  // MPI_CPP_BINDING
}

//: collects results and calls finalize() method of p_ on the lead processor (i.e. once)
template <class T, class P, class R> 
//bool dborl_cluster<T, P, R>::distribute(T *t, int cnt_t, R *r, int& cnt_r)
bool dborl_cluster<T, P, R>::distribute(vcl_vector<T>& t)
{
    //: verify that the MPI interface has been initialized
#ifdef MPI_CPP_BINDING
  if (!MPI::Is_initialized ()) {
    throw dborl_cluster_exception (dborl_cluster_exception::MPIError, "MPI must be initialized in main", 0, 0);
    vcl_cout << "MPI must be initialized in main\n";
  }
#else
    {
      int inited = 0;
      MPI_Initialized (&inited);
      if (!inited) { 
        throw dborl_cluster_exception (dborl_cluster_exception::MPIError, "MPI must be initialized in main", 0, 0);
        vcl_cout << "MPI must be initialized in main\n";
      }
    }
#endif  // MPI_CPP_BINDING

 vcl_string name;

// //
//#ifdef MPI_CPP_BINDING
// int  namelen;
// char processor_name[MPI_MAX_PROCESSOR_NAME];
// MPI_Get_processor_name(processor_name,&namelen);
// name = processor_name;
//#else
// int  namelen;
// char processor_name[MPI_MAX_PROCESSOR_NAME];
// MPI::Get_processor_name(processor_name, &namelen);
// name = processor_name;
//#endif

 name = this->processor_name(); // added by Nhon
  
 unsigned cnt_t = t.size();
 vcl_cout << "Hello World! I am " << name << " " << my_rank_ << " of " << total_processors_ << " input size: " << cnt_t << " ";


 //: distribute the vector onto the processors
 unsigned my_start_point = get_start_point(cnt_t, my_rank_);
 unsigned my_number_points = get_number_points(cnt_t, my_rank_);
 unsigned my_end_point = my_start_point + my_number_points;
 vcl_cout << "start point: " << my_start_point << " # of points: " << my_number_points << vcl_endl;

 //: prepare the send buffer 
 //delete send_buf_;
 send_buf_ = new R [my_number_points];

 for (unsigned i = 0; i < cnt_t; i++)
 { 
   if (i < my_start_point || i >= my_end_point) 
    continue;

   R res;
   if (!p_->process(t[i], res))
     throw dborl_cluster_exception();  // throws an Incomplete error
   send_buf_[i - my_start_point] = res;  
 }

 // If we're the lead processor...
 if (lead_processor())
 {
    // Allocate the receive buffer to hold the merged responses and the gather map.
    //delete recv_buf_;
    recv_buf_ = new R [cnt_t];
    point_offsets_ = new int [total_processors_];
    point_counts_ = new int [total_processors_];
      
    // Build the gather map - the offset/count for each processor
    for (int rank = 0; rank < total_processors_; rank++)
    {
        // Compute the offset/count for this processor.
        point_offsets_[rank] = get_start_point(cnt_t, rank);
        point_counts_[rank] = get_number_points(cnt_t, rank);
        vcl_cout << "Process: " << rank << ", offset: " << point_offsets_[rank] << ", count: " << point_counts_[rank] << "\n";
    }
  }
  
  //: create the datatype for our results using the process 
  MPI_Datatype result_type = p_->create_datatype_for_R();

  // Gather results - This method is such that LeadProcessor receives and others send.
  if (lead_processor()) vcl_cout << "Gather start \n";

#ifdef MPI_CPP_BINDING
    MPI::COMM_WORLD.Gatherv (send_buf_, my_number_points, result_type, 
                             recv_buf_, point_counts_, point_offsets_, 
                             result_type, 0);  // last argumant is the rank of the receiver which is the LeadProcessor, i.e. 0
#else
    MPI_Gatherv (send_buf_, my_number_points, result_type, 
                 recv_buf_, point_counts_, point_offsets_, 
                 result_type, 0, MPI_COMM_WORLD);
#endif  // MPI_CPP_BINDING
    if (lead_processor()) vcl_cout << "Gather end \n";

    // Lead processor marshals back to response field
    if (lead_processor())
    {
      vcl_vector<R> r;
      for (unsigned i = 0; i < cnt_t; i++)
        r.push_back(recv_buf_[i]);
      p_->finalize(r);
    }

 return true;
}

#define DBORL_CLUSTER_INSTANTIATE(T, P, R) \
template class dborl_cluster<T, P, R>

