//:
// \file
// \brief A wrapper class that brings later stages of recognition together
//        all the intermediate data are saved into an output folder optionally
//
//        input: a prototype and a query database with patches extracted
//           
//        for each prototype shape
//            for each query shape:
//               4) match the query patch set with the prototype patch set
//               5) run detection algorithm on the query
//               6) generate TP, FP, TN, FN 
//
//            return a cumulative TP, FP, TN, FN for the whole query database for this prototype
//        return a cumulative TP, FP, TN, FN for the whole prototypes
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

#if !defined(_dborl_patch_match_h)
#define _dborl_patch_match_h

#include <dborl/algo/mpi/dborl_processor_base.h>
#include "dborl_patch_match_params.h"

#include <vcl_utility.h> // --> for vcl_pair class
#include <vul/vul_timer.h>

#include <dborl/dborl_index_sptr.h>
#include <dborl/dborl_index_node_sptr.h>

class dborl_patch_match_input
{ public:
    dborl_patch_match_input(vcl_string pn, vcl_string qn, 
                            dborl_image_description_sptr pi, 
                            dborl_image_description_sptr qi,
                            vcl_string pstn, vcl_string qstn,
                            vcl_string pp, vcl_string qp) : proto_name(pn), query_name(qn), proto_id(pi), query_id(qi),
                                                                proto_st_name(pstn), query_st_name(qstn),
                                                                proto_path(pp), query_path(qp) {}

    dborl_image_description_sptr proto_id;
    dborl_image_description_sptr query_id;
    vcl_string proto_name;
    vcl_string query_name;
    vcl_string proto_st_name;
    vcl_string query_st_name;
    vcl_string proto_path;
    vcl_string query_path;
};

class dborl_patch_match_output
{
public:
  dborl_patch_match_output() : TP_(0), FP_(0), TN_(0), FN_(0) {}
  
  inline void set_values(int tp, int fp, int tn, int fn) { TP_ = tp, FP_ = fp, TN_ = tn, FN_ = fn; }
  void set_values(dborl_exp_stat& stat);
  dborl_exp_stat_sptr get_exp_stat();
  
  int TP_;
  int FP_;
  int TN_;
  int FN_;
};


//: result class is a float point number which is the matching cost
//  to normalize this matching cost, finalize method will be used
class dborl_patch_match : public dborl_processor_base<dborl_patch_match_input, dborl_patch_match_output>
{
public:
  dborl_patch_match() {}

  //: this method is run on each processor after lead processor broadcasts its command
  //  line arguments to all the processors since only on the lead processor is passed the command line arguments by mpirun
  virtual bool parse_command_line(vcl_vector<vcl_string>& argv);

  //: this method is run on each processor
  //  parse the input file into a bxml document and extract each parameter
  virtual bool parse(const char* param_file);

  virtual bool parse_index(vcl_string index_file, dborl_index_sptr& ind, dborl_index_node_sptr& root);

  //: this method prints an xml input file setting all the parameters to defaults
  //  run the algorithm to generate this file, then modify it
  void print_default_file(const char* default_file);

  //: this method is run on each processor
  virtual bool initialize(vcl_vector<dborl_patch_match_input>& t);

  //: this method is run in a distributed mode on each processor on the cluster
  virtual bool process(dborl_patch_match_input i, dborl_patch_match_output& f);

  //: this method is run on the lead processor once after results are collected from each processor
  virtual bool finalize(vcl_vector<dborl_patch_match_output>& results);

  void print_time();
  void set_total_processors(unsigned tot) { total_processors_ = tot; }

#ifdef MPI_CPP_BINDING
  virtual MPI::Datatype create_datatype_for_R();
#else
  virtual MPI_Datatype create_datatype_for_R();
#endif

protected:
  protected:
  
  dborl_patch_match_params params_;

  dborl_index_sptr proto_ind_, query_ind_;
  dborl_index_node_sptr proto_root_, query_root_;

  vcl_vector<dborl_image_description_sptr> query_img_d_;
  vcl_vector<dborl_image_description_sptr> proto_img_d_;

  vul_timer t_;
  unsigned total_processors_;
  
};

#endif  //_dborl_patch_match_h
