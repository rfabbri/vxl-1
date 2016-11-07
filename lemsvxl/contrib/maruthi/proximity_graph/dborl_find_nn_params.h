// \file
// \brief parameter set for construction of graph types
//
// \author Maruthi Narayanan (mn@lems.brown.edu)
// \date Feb 26,2010
//      
// \verbatim
//   Modifications
//  
// \endverbatim

// update by 
//

#ifndef dborl_find_nn_params_h_
#define dborl_find_nn_params_h_

#include <dborl/algo/dborl_algo_params.h>


//: put all the additional params into this class, and add them 
//  to the parameter list in the constructor so that 
//  all the files related to the parameter set of the algorithm are 
//  generated automatically via the methods of the base class
//  no need to overwrite any of the methods in the base class thanks 
//  to the parameter list
class dborl_find_nn_params : public dborl_algo_params
{
public:

  //: Constructor
  dborl_find_nn_params(vcl_string algo_name);

  // MEMBER VARIABLES
 
  // What type of processing
  dborl_parameter<unsigned int> action_type_;

  // Should we be verbose
  dborl_parameter<bool> verbose_;

  //: Name of file that holds a list of strings that represent queries
  dborl_parameter<vcl_string> query_label_file_;

  //: Name of seeds file that takes in separate seeds
  //: for all the various types of graphs
  dborl_parameter<vcl_string> batch_seeds_file_;

  //: Name of search type
  dborl_parameter<unsigned int> search_type_;

  //: Extended search neighborhood
  dborl_parameter<double> tau_;

  //: Passes a file that holds the names of all the graph files
  //: Assumed to be in dataset directory
  dborl_parameter<vcl_string> batch_files_graphs_list_;    

  //: Passes the folder that holds the names of all the batch processing files
  //: Assumed to be in dataset directory hold matrices
  //: Could be query data matrix of exemplar data matrix
  dborl_parameter<vcl_string> batch_files_matrices_list_;    

  //: Passes the folder of the dataset directory
  //: will write straight to the dataset folder
  dborl_parameter<vcl_string> dataset_dir_;    

  //: Passes the folder of the seed directory
  //: will write straight to the dataset folder
  dborl_parameter<vcl_string> seeds_dir_;    

  //: Passes the folder of the output directory
  //: of where results should go
  dborl_parameter<vcl_string> output_dir_;    

};

#endif  //_dborl_find_nn_params_h
