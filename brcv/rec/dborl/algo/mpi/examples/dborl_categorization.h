//:
// \file
// \brief An example categorization algorithm inheriting from dborl_dataset_processor
//        optionally uses shock matching or curve matching to find the nearest neighbor to an input object
//        input: an index file  (the nearest neighbor among the objects in this index is found)
//               a binary image to be categorized (uses .esf files in the case of shock categorization, .con files in the case of curve matching, these files are assumed to be extracted previously and saved in the image folder)
//        matches the input esf/con files to the esfs/cons in the index file following the hierarchy of the index
//        distributes job groupings at each node of the index 
//        and does many rounds of such distribution untill the object is categorized
//        after each distribution, the current results are examined to decide the flow of execution
//
//        the categorization is done by finding the nearest neighbor following the order induced by the index
//        then declares the category to be the category of the nearest neighbor and writes categorization.xml file into the output folder of the job
//
//        for each object, categorization algo writes a categorization.xml file to the output folder similar in format to the ground truth descriptions of the objects
//        then an evaluation algorithm should be run on a collection of output folders for a collection of input objects to generate
//        perf.xml file summarizing the performance on that test collection
//
//        if the index has intermediate nodes to follow, the name of the node to be followed from a parent node
//         should be the same as the nearest object's name in the parent node, i.e.:
//                       node_name: root, object names: obj1 obj2 obj3
//                   child_name: obj1, obj names: obj22, obj32 ; child_name: obj2, obj names: obj34, obj25 ; child_name: obj3, obj names: obj6, obj7
//                   etc.
//        this algorithm assumes there is at most one path to follow in the index, i.e. each parent node has children with unique node names
//
//        TODO:      Debug this application. Currently there is a bug and it does not work as its supposed to work.
//                   The problem is that finalize() method is only called by the lead processor which has gathered all the results after the first round 
//                   of distribution and in finalize() the lead processor decides which node in the index is the next node.
//                   However this information needs to be sent to all other processors via MPI_SEND just like the command line arguments were sent
//                   so that all the other processors pick the next node in the index and initialize() initializes using the correct "current_node_"
//                   I cannot debug this now, cause I don't have any more time left. Hope to come back to this example after March 17, 2008.
//                   Also all the categorization result file generation should change as in dborl_categorization_simple
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 01/14/08
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
//

#if !defined(_dborl_categorization_h)
#define _dborl_categorization_h

#include <dborl/algo/mpi/dborl_dataset_processor.h>

#include "dborl_categorization_params_sptr.h"
#include <dbskr/dbskr_tree_sptr.h>
#include <dborl/dborl_index_node_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>

class dborl_categorization : public dborl_dataset_processor<vcl_string, float>
{
public:

  dborl_categorization(dborl_categorization_params_sptr params) : params_(params) {}

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
  dborl_categorization_params_sptr params_;

  //: initialized to root
  dborl_index_node_sptr current_node_;

  dbskr_tree_sptr input_shock_tree_;  // used in the case of shock matching 
  vsol_polygon_2d_sptr input_poly_;   // used in the case of curve matching
  vsol_polygon_2d_sptr input_poly_flipped_;   // used in the case of curve matching

  vcl_string current_category_;
  vcl_string input_gt_category_;
};

#endif  //_dborl_categorization_h
