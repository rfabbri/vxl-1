//:
// \file
// \brief The match shock processor inherits from dborl_processor base
//        parses an input file
//        input: train dir & train list and database dir & database list
//               assumes .esf files are saved in train dir and database dir
//               matches each esf in train list to each esf in database list
//        output: shock matching file is written if an shgm folder is suplied as an input
//
//        CAUTION: if the user passes the training set and the database (test set) to be the same set (same size, from same directory, with the same list)
//                 and sets the option of db_train_same_ on
//                 then the algorithm works in a special mode, and creates a single matrix which is made into a symmetric matrix in the finalize method
//                 if db_train_same_ is set to off then it is assumed that the train and test sets are different then two sets of computations are performed to make sure
//                 that both edit(t1, t2) and edit(t2, t1) are performed and the min cost is reported in the final matrix
//
//                 to run in db_train_same_ ON mode and only make D1*D2 matchings, make sure D1 == D2 && params_.db_dir_.compare(params_.train_dir_) == 0)
//                 but the program does not make a check on the element by element equality of db_list_ and train_list_, even if they are not, it will still run in db_train_same=ON mode
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

#if !defined(_dborl_match_shock_processor_h)
#define _dborl_match_shock_processor_h

#include <dborl/algo/mpi/dborl_processor_base.h>
#include <bxml/bxml_document.h>
#include <dbskr/dbskr_tree_sptr.h>
#include <dbskr/algo/io/dbskr_match_shock_params.h>

#include <vcl_utility.h> // --> for vcl_pair class
#include <vul/vul_timer.h>

class dborl_match_shock_processor_input
{ public:
    dborl_match_shock_processor_input(dbskr_tree_sptr t1, dbskr_tree_sptr t2, vcl_string n1, vcl_string n2) :
                                      tree1(t1), tree2(t2), name1(n1), name2(n2) {}
    dbskr_tree_sptr tree1;
    dbskr_tree_sptr tree2;
    vcl_string name1;
    vcl_string name2;
};


//: result class is a float point number which is the matching cost
//  to normalize this matching cost, finalize method will be used
class dborl_match_shock_processor : public dborl_processor_base<dborl_match_shock_processor_input, float>
{
public:
  dborl_match_shock_processor() {}

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
  virtual bool initialize(vcl_vector<dborl_match_shock_processor_input>& t);

  //: this method is run in a distributed mode on each processor on the cluster
  virtual bool process(dborl_match_shock_processor_input i, float& f);

  //: this method is run on the lead processor once after results are collected from each processor
  virtual bool finalize(vcl_vector<float>& results);

  void print_time();

#ifdef MPI_CPP_BINDING
  virtual MPI::Datatype create_datatype_for_R();
#else
  virtual MPI_Datatype create_datatype_for_R();
#endif

protected:
  protected:
  bxml_document param_doc_;  // parse the input file into a document
  dbskr_match_shock_params params_;

  vcl_vector<vcl_string> database, training_set;
  unsigned D1, D2;

  vul_timer t_;
  
};

#endif  //_dborl_match_shock_processor_h
