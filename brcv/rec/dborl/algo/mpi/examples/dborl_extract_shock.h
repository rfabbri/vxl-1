//:
// \file
// \brief An example processor inheriting from dborl_dataset_processor
//        Runs shock extraction algorithm on each image in the dataset
//        If the images are binary, the con files should be extracted beforehand,
//        if the images are natural images, the cem files should be extracted beforehand,
//          the related files should be saved in the object directories, and the extensions to concatanate
//          to the object name to locate those files is a parameter in this process
//
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 01/16/08
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
//

#if !defined(_dborl_extract_shock_h)
#define _dborl_extract_shock_h

#include <dborl/algo/mpi/dborl_dataset_processor.h>

#include "dborl_extract_shock_params_sptr.h"

class dborl_extract_shock : public dborl_dataset_processor<int, char>
{
public:

  dborl_extract_shock(dborl_extract_shock_params_sptr params) : params_(params) {}

  //: this method is run on each processor after lead processor broadcasts its command
  //  line arguments to all the processors since only on the lead processor is passed the command line arguments by mpirun
  virtual bool parse_command_line(vcl_vector<vcl_string>& argv);

  //: parse the index file
  virtual bool parse_index(vcl_string index_file);

  //: this method prints an xml input file setting all the parameters to defaults
  //  run the algorithm to generate this file, then modify it
  void print_default_file(const char* default_file);

  //: this method is run on the lead processor once
  virtual bool parse(const char* param_file);

  //: this method is run on each processor
  virtual bool initialize(vcl_vector<int>& t);

  //: this method is run in a distributed mode on each processor on the cluster
  virtual bool process(int t1, char& f);

  //: this method is run on the lead processor once after results are collected from each processor
  virtual bool finalize(vcl_vector<char>& results);

#ifdef MPI_CPP_BINDING
  virtual MPI::Datatype create_datatype_for_R();
#else
  virtual MPI_Datatype create_datatype_for_R();
#endif

protected:
  dborl_extract_shock_params_sptr params_;

};

#endif  //_dborl_extract_shock_h
