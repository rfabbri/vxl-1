//:
// \file
// \brief The extract shock patch processor inherits from dborl_dataset_processor
//        parses an input file
//        input:
//               assumes .esf files are saved in directories of each object, object directories are read from the flat index
//               extracts patches 
//        option:  extract patches from a model shock graph (possible if shock graph is extracted from a simple closed curve --> hence its a tree)
//                 extract patches from a shock graph of a contour map
//
//        output: shock matching file is written if an shgm folder is suplied as an input
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

#if !defined(_dborl_extract_patch_processor_h)
#define _dborl_extract_patch_processor_h

#include <dborl/algo/mpi/dborl_dataset_processor.h>
#include <bxml/bxml_document.h>
#include <dbskr/algo/io/dbskr_extract_patch_params.h>
#include <dbsk2d/dbsk2d_shock_graph_sptr.h>

#include <vcl_utility.h> // --> for vcl_pair class
#include <vul/vul_timer.h>

class dborl_extract_patch_processor_input
{ public:
    dborl_extract_patch_processor_input(dbsk2d_shock_graph_sptr s, vcl_string n, vcl_string p) :
                                      sg(s), name(n), path(p) {}
    dbsk2d_shock_graph_sptr sg;
    vcl_string name;
    vcl_string path;
};


//: result class is a float point number which is the matching cost
//  to normalize this matching cost, finalize method will be used
class dborl_extract_patch_processor : public dborl_dataset_processor<dborl_extract_patch_processor_input, char>
{
public:
  dborl_extract_patch_processor() {}

  //: this method is run on each processor after lead processor broadcasts its command
  //  line arguments to all the processors since only on the lead processor is passed the command line arguments by mpirun
  virtual bool parse_command_line(vcl_vector<vcl_string>& argv);

  //: parse the index file
  virtual bool parse_index(vcl_string index_file);

  //: this method is run on each processor
  //  parse the input file into a bxml document and extract each parameter
  virtual bool parse(const char* param_file);

  //: this method prints an xml input file setting all the parameters to defaults
  //  run the algorithm to generate this file, then modify it
  void print_default_file(const char* default_file);

  //: this method is run on each processor
  virtual bool initialize(vcl_vector<dborl_extract_patch_processor_input>& t);

  //: this method is run in a distributed mode on each processor on the cluster
  virtual bool process(dborl_extract_patch_processor_input i, char& f);

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
  
  dbskr_extract_patch_params params_;
  vul_timer t_;
  unsigned total_processors_;
};

#endif  //_dborl_extract_patch_processor_h
