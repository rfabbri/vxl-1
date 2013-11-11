//:
// \file
// \brief An example categorization algorithm inheriting from dborl_dataset_processor
//        optionally uses shock matching or curve matching to find the nearest neighbor to an input object
//        input: a flat index file  (the nearest neighbor among the objects in this index is found)
//               a binary image to be categorized (uses .esf files in the case of shock categorization, .con files in the case of curve matching, these files are assumed to be extracted previously and saved in the image folder)
//        matches the input esf/con files to the esfs/cons in the only node of the index file
//        distributes job groupings at the root node of the index (so does only one round of distribution as opposed to a categorization that 
//          does many rounds of distributions. see the class dborl_categorization in this examples directory, it is meant to do many rounds of distribution, given a complex index file.)
//
//        the categorization is done by finding the nearest neighbor in the root node
//        then declaring the category to be the category of the nearest neighbor and categorization.xml file is written to the output folder of the job
//
//        for each object, categorization algo writes a categorization.xml file to the output folder similar in format to the ground truth descriptions of the objects
//        this file reports TP, FP, TN, FN for each category encountered. VOX parses such files for a collection of input objects and sums up TP, FP, etc to plot an ROC for each category
//
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 01/30/08
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
//

#if !defined(_dborl_categorization_simple_h)
#define _dborl_categorization_simple_h

#include <dborl/algo/mpi/dborl_dataset_processor.h>

#include "dborl_categorization_simple_params_sptr.h"
#include <dbskr/dbskr_tree_sptr.h>
#include <dborl/dborl_index_node_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>

class dborl_categorization_simple : public dborl_dataset_processor<vcl_string, float>
{
public:

  dborl_categorization_simple(dborl_categorization_simple_params_sptr params) : params_(params) {}

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
  //  initialize the input vector with names of input esfs in the current node of the index file
  virtual bool initialize(vcl_vector<vcl_string>& t);

  //: this method is run in a distributed mode on each processor on the cluster
  virtual bool process(vcl_string name, float& f);

  //: this method is run on the lead processor once after results are collected from each processor
  //  this method decides to make another round of process distribution or not
  //  and if so picks the next job grouping by updating the current index node pointer
  virtual bool finalize(vcl_vector<float>& results);

#ifdef MPI_CPP_BINDING
  virtual MPI::Datatype create_datatype_for_R();
#else
  virtual MPI_Datatype create_datatype_for_R();
#endif

protected:
  dborl_categorization_simple_params_sptr params_;

  //: initialized to root
  dborl_index_node_sptr current_node_;

  dbskr_tree_sptr input_shock_tree_;  // used in the case of shock matching 
  vsol_polygon_2d_sptr input_poly_;   // used in the case of curve matching
  vsol_polygon_2d_sptr input_poly_flipped_;   // used in the case of curve matching

  vcl_string current_category_;
  vcl_string input_gt_category_;
};

#endif  //_dborl_categorization_simple_h
