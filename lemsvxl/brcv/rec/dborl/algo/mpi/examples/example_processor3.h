//:
// \file
// \brief An example processor inheriting from dborl_processor_base.
//        Uses the simple ex_class3 as the result type R,
//        ex_class3 contains primitive type members and fixed length arrays of primitive types,
//        see MPI_Datatype create_datatype_for_R() on how to create a new datatype using MPI functions
//        in order to be able to pass around instances of ex_class3 as messages

//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/30/07
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
//

#if !defined(_dborl_example3_h)
#define _dborl_example3_h

#include <dborl/algo/mpi/dborl_processor_base.h>

#define EX_CLASS3_B_SIZE  100
#define EX_CLASS3_C_SIZE  5

/*
class ex_class3 
{ public:
    int a;
    char b[EX_CLASS3_B_SIZE];
    float c[EX_CLASS3_C_SIZE];
};
*/

class ex_class3
{ public:
    int a;
    char *b;
    float *c;
};

class example_processor3 : public dborl_processor_base<int, ex_class3>
{
public:

  //: this method is run on each processor after lead processor broadcasts its command
  //  line arguments to all the processors since only on the lead processor is passed the command line arguments by mpirun
  //virtual bool parse_command_line(int argc, char *argv[]);
  virtual bool parse_command_line(vcl_vector<vcl_string>& argv);

  //: this method is run on the lead processor once
  virtual bool parse(const char* param_file);

  //: this method is run on each processor
  virtual bool initialize(vcl_vector<int>& t);

  //: this method is run in a distributed mode on each processor on the cluster
  virtual bool process(int t1, ex_class3& f);

  //: this method is run on the lead processor once after results are collected from each processor
  virtual bool finalize(vcl_vector<ex_class3>& results);

#ifdef MPI_CPP_BINDING
  virtual MPI::Datatype create_datatype_for_R();
#else
  virtual MPI_Datatype create_datatype_for_R();
#endif

protected:
  float var_;
  unsigned b_size_;
  unsigned c_size_;
};

#endif  //_dborl_example3_h
