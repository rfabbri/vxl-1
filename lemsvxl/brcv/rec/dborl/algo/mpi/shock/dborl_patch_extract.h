//:
// \file
// \brief A wrapper class that brings initial stages of recognition together
//        all the intermediate data are saved into an output folder optionally
//
//        input: a database 
//           
//        for each image/shape in the database:
//               1) prepare boundary set (method depends on the type of the input)
//               2) extract shock graph (repeat step 1 till a valid shock graph is extracted)
//               3) extract patch set
//
//        
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 03/26/08
//      
// \verbatim
//   Modifications
//  
// \endverbatim
//
//

#if !defined(_dborl_patch_extract_h)
#define _dborl_patch_extract_h

#include <dborl/algo/mpi/dborl_processor_base.h>
#include "dborl_patch_extract_params.h"

#include <vcl_utility.h> // --> for vcl_pair class
#include <vul/vul_timer.h>
#include <dborl/dborl_index_sptr.h>
#include <dborl/dborl_index_node_sptr.h>

class dborl_patch_extract_input
{ public:
    dborl_patch_extract_input(vcl_string n, vcl_string full_n) : name(n), full_name(full_n) {}
    vcl_string name;
    vcl_string full_name;
};


//: result class is a float point number which is the matching cost
//  to normalize this matching cost, finalize method will be used
class dborl_patch_extract : public dborl_processor_base<dborl_patch_extract_input, char>
{
public:
  dborl_patch_extract() {}

  //: this method is run on each processor after lead processor broadcasts its command
  //  line arguments to all the processors since only on the lead processor is passed the command line arguments by mpirun
  virtual bool parse_command_line(vcl_vector<vcl_string>& argv);

  //: this method is run on each processor
  //  parse the input file into a bxml document and extract each parameter
  virtual bool parse(const char* param_file);

  virtual bool parse_index(vcl_string index_file);

  //: this method prints an xml input file setting all the parameters to defaults
  //  run the algorithm to generate this file, then modify it
  void print_default_file(const char* default_file);

  //: this method is run on each processor
  virtual bool initialize(vcl_vector<dborl_patch_extract_input>& t);

  //: this method is run in a distributed mode on each processor on the cluster
  virtual bool process(dborl_patch_extract_input i, char& f);

  //: this method is run on the lead processor once after results are collected from each processor
  virtual bool finalize(vcl_vector<char>& results);

  void print_time();
  void set_total_processors(unsigned tot) { total_processors_ = tot; }

#ifdef MPI_CPP_BINDING
  virtual MPI::Datatype create_datatype_for_R();
#else
  virtual MPI_Datatype create_datatype_for_R();
#endif

protected:
  protected:
  
  dborl_patch_extract_params params_;
  dborl_index_sptr ind_;
  dborl_index_node_sptr root_;

  
  vul_timer t_;
  unsigned total_processors_;
  
};

#endif  //_dborl_patch_extract_h
