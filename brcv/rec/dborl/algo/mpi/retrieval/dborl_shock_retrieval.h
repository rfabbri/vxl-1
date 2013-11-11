//:
// \file
// \brief Shape retrieval using shock matching algorithm via the leave-one-out strategy
//
//        input: a shape database with N shapes and c categories with N_i (i \in [1,c]) shapes from each
//               the shapes are assumed to be simple closed curves so that their shock graphs are actually trees
//               and they are readily available in the object directories
//
//        main parameter: rho: the radius in terms of similarity values, 
//                        or k: number of nearest neighbors to use
//           
//        first create the NxN similarity matrix
//
//        for each image/shape in the database:
//               4) sort its N-1 matches 
//               5) pick top k matches (declared positives) count how many are from the category of the query (TP) and how many are not (FP)
//                    in the remaining N-1-k images, count how many are from the category of the query (FN) and how many are not (TN)               
//
//        return a cumulative TP, FP, TN, FN for the whole query database
//        
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 03/27/08
//      
// \verbatim
//   Modifications
//  
// \endverbatim
//
//

#if !defined(_dborl_shock_retrieval_h)
#define _dborl_shock_retrieval_h

#include <dborl/algo/mpi/dborl_processor_base.h>
#include "dborl_shock_retrieval_params.h"
#include <dborl/dborl_index_sptr.h>

#include <vcl_utility.h> // --> for vcl_pair class
#include <vul/vul_timer.h>

class dborl_shock_retrieval_input
{ public:
    dborl_shock_retrieval_input(dbskr_tree_sptr t1, dbskr_tree_sptr t2, vcl_string n1, vcl_string n2) :
                                      tree1(t1), tree2(t2), name1(n1), name2(n2) {}
    dbskr_tree_sptr tree1;
    dbskr_tree_sptr tree2;
    vcl_string name1;
    vcl_string name2;
};


//: result class is a float point number which is the matching cost
//  to normalize this matching cost, finalize method will be used
class dborl_shock_retrieval : public dborl_processor_base<dborl_shock_retrieval_input, float>
{
public:
  dborl_shock_retrieval() {}

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
  virtual bool initialize(vcl_vector<dborl_shock_retrieval_input>& t);

  //: this method is run in a distributed mode on each processor on the cluster
  virtual bool process(dborl_shock_retrieval_input i, float& f);

  //: this method is run on the lead processor once after results are collected from each processor
  virtual bool finalize(vcl_vector<float>& results);

  void print_time();

  bool load_sim_matrix(vcl_string sim_file);
  void write_sim_matrix(vcl_string sim_file);
  void initialize_sim_matrix(unsigned D);

#ifdef MPI_CPP_BINDING
  virtual MPI::Datatype create_datatype_for_R();
#else
  virtual MPI_Datatype create_datatype_for_R();
#endif

protected:
  protected:
  
  dborl_shock_retrieval_params params_;

  dborl_index_sptr ind_;

  vcl_string match_folder_;
  vcl_vector<unsigned> database_indices_;

  //: the following 2D sim matrix, will carry for a given i: row, j: col, sim of i and j and the description of j
  //  since when we sort the rows wrt sim, then we want to keep columns' descriptions
  vcl_vector<vcl_vector<vcl_pair<float, dborl_image_description_sptr> >* > sim_matrix_;

  vul_timer t_;
  
};

#endif  //_dborl_shock_retrieval_h
