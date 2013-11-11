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

#ifndef dborl_graph_construction_params_h_
#define dborl_graph_construction_params_h_

#include <dborl/algo/dborl_algo_params.h>


//: put all the additional params into this class, and add them 
//  to the parameter list in the constructor so that 
//  all the files related to the parameter set of the algorithm are 
//  generated automatically via the methods of the base class
//  no need to overwrite any of the methods in the base class thanks 
//  to the parameter list
class dborl_graph_construction_params : public dborl_algo_params
{
public:

  //: Constructor
  dborl_graph_construction_params(vcl_string algo_name);

  // MEMBER VARIABLES

  // Should we be verbose
  dborl_parameter<bool> verbose_;

  // Should pruning be performed
  dborl_parameter<unsigned int>  graph_type_;

  //: Name of file that holds similarity matrix
  dborl_parameter<vcl_string> exemplar_sim_matrix_;
  
  //: Name of file that holds a list of strings that represent exemplars
  dborl_parameter<vcl_string> exemplar_label_file_;

  //: Name of file that holds category names
  dborl_parameter<vcl_string> exemplar_category_file_;

  //: Beta needed for beta skeletons
  dborl_parameter<double> beta_;
  
  //: Neighbors needed for knn graph
  dborl_parameter<unsigned int> neighbors_;

  //: Perform wilson editing
  dborl_parameter<bool> wilson_edit_;

  //: Perform thinning of graph
  dborl_parameter<bool> thinning_;

  //: Passes the folder of the dataset directory
  //: will write straight to the dataset folder
  dborl_parameter<vcl_string> dataset_dir_;    

  //: Passes the folder of the output directory
  //: of where results should go
  dborl_parameter<vcl_string> output_dir_;    

};

#endif  //_dborl_graph_construction_params_h
