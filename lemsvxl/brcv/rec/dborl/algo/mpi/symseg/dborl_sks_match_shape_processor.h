// This is rec/dborl/algo/mpi/symseg/dborl_sks_match_shape_processor.h
#ifndef dborl_sks_match_shape_processor_h
#define dborl_sks_match_shape_processor_h

//:
// \file
// \brief The detect shape processor inherits from dborl_dataset_processor
//        input:
//        option:  
//        output: 
//
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date 1/04/2008
// \verbatim
//    Modifications
//  
// \endverbatim


#include <dborl/algo/mpi/dborl_dataset_processor.h>
#include <bxml/bxml_document.h>
#include "dborl_sks_match_shape_params.h"
#include <dbsksp/dbsksp_shock_graph_sptr.h>
#include <vul/vul_timer.h>


//: input for detect_shape_processor
class dborl_sks_match_shape_processor_input
{ 
public:
  dborl_sks_match_shape_processor_input(dbsksp_shock_graph_sptr s, vcl_string n):
      shock_graph_(s), image_name_(n) 
  {}
      
  ~dborl_sks_match_shape_processor_input(){};
  
  // member variables
  dbsksp_shock_graph_sptr shock_graph_;
  vcl_string image_name_;
};


//: result class is a float point number which is the matching cost
//  to normalize this matching cost, finalize method will be used
class dborl_sks_match_shape_processor : 
  public dborl_processor_base<dborl_sks_match_shape_processor_input, float>
{
public:
  dborl_sks_match_shape_processor() {}

  // set processor name
  vcl_string processor_name() const {return this->processor_name_; }
  void set_processor_name(const vcl_string& processor_name)
  { this->processor_name_ = processor_name; }

  //: this method is run on each processor after lead processor broadcasts its command
  //  line arguments to all the processors since only on the lead processor is passed the command line arguments by mpirun
  virtual bool parse_command_line(vcl_vector<vcl_string>& argv);

  //: this method is run on each processor
  //  parse the input file into a bxml document and extract each parameter
  virtual bool parse(const char* param_file);

  //: this method prints an xml input file setting all the parameters to defaults
  //  run the algorithm to generate this file, then modify it
  void print_default_file(const char* default_file);

  //: this method is run on each processor
  virtual bool initialize(vcl_vector<dborl_sks_match_shape_processor_input>& t);

  //: this method is run in a distributed mode on each processor on the cluster
  virtual bool process(dborl_sks_match_shape_processor_input i, float& f);

  //: this method is run on the lead processor once after results are collected from each processor
  virtual bool finalize(vcl_vector<float>& results);

  void print_time();

  void set_total_processors(unsigned tot) { total_processors_ = tot; }

#ifdef MPI_CPP_BINDING
  virtual MPI::Datatype create_datatype_for_R();
#else
  virtual MPI_Datatype create_datatype_for_R();
#endif

protected:
  bxml_document param_doc_;  // parse the input file into a document
  dborl_sks_match_shape_params params_;
  vul_timer t_;
  unsigned total_processors_;
  vcl_string processor_name_;
};

#endif  //_dborl_sks_match_shape_processor_h
