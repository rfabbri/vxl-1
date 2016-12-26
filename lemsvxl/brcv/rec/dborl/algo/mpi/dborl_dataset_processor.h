//:
// \file
// \brief The dataset processor base class (abstract)
//        should be used when the same process will be run on each member of a dataset
//              
//        inputs: the xml file that is the flat index of a dataset, input.xml (parameter file) 
//        job   : distribute the processing of each element in the index root node onto the cluster
//        result type should be defined by the inheriting process
//
//        this base class only implements index parsing
//        inherited classes should define the rest of the methods
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

#if !defined(_dborl_dataset_processor_h)
#define _dborl_dataset_processor_h

#include <dborl/algo/mpi/dborl_processor_base.h>
#include <dborl/dborl_index_sptr.h>
#include <bxml/bxml_document.h>
#include <vcl_string.h>

//: T will actually always be one of the dborl_object's I could have inherited from dborl_processor_base<dborl_object_base_sptr, R>
//  but then need a check in runtime to determine which class inherited from dborl_object_base
//  instead I make a template so inheritance is resolved during compile time

template <class T, class R> 
class dborl_dataset_processor : public dborl_processor_base<T, R>
{
public:
  //: this method is run on each processor after lead processor broadcasts its command
  //  line arguments to all the processors since only on the lead processor is passed the command line arguments by mpirun
  //virtual bool parse_command_line(int argc, char *argv[]) = 0;
  virtual bool parse_command_line(vcl_vector<vcl_string>& argv) = 0;

  //: parse the index file
  virtual bool parse_index(vcl_string index_file) = 0;

  //: this method is run on each processor
  virtual bool parse(const char* param_file) = 0;

  //: this method is run on each processor
  virtual bool initialize(vcl_vector<T>& t) = 0;

  //: this method is run in a distributed mode on each processor on the cluster
  virtual bool process(T t1, R& f) = 0;

  //: this method is run on the lead processor once after results are collected from each processor
  virtual bool finalize(vcl_vector<R>& results) = 0;

#ifdef MPI_CPP_BINDING
  virtual MPI::Datatype create_datatype_for_R() = 0;
#else
  virtual MPI_Datatype create_datatype_for_R() = 0;
#endif

protected:
  dborl_index_sptr ind_;
  bxml_document param_doc_;
  vcl_string index_file_;
};

#endif  //_dborl_example3_h
