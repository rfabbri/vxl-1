//:
// \file
// \brief An example processor inheriting from dborl_processor_base
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

#if !defined(_dborl_example_h)
#define _dborl_example_h

#include <dborl/algo/mpi/dborl_processor_base.h>

class example_processor : public dborl_processor_base<int, float>
{
public:
  //: this method is run on each processor after lead processor broadcasts its command
  //  line arguments to all the processors since only on the lead processor is passed the command line arguments by mpirun
  //virtual bool parse_command_line(int argc, char *argv[])
  virtual bool parse_command_line(vcl_vector<vcl_string>& argv);

  //: this method is run on the lead processor once
  virtual bool parse(const char* param_file);

  //: this method is run on each processor
  virtual bool initialize(vcl_vector<int>& t);

  //: this method is run in a distributed mode on each processor on the cluster
  virtual bool process(int t1, float& f);

  //: this method is run on the lead processor once after results are collected from each processor
  virtual bool finalize(vcl_vector<float>& results);

#ifdef MPI_CPP_BINDING
  virtual MPI::Datatype create_datatype_for_R();
#else
  virtual MPI_Datatype create_datatype_for_R();
#endif

protected:
  float b_;
};

#endif  //_dborl_example_h
