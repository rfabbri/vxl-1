// \file
// \brief parameter set for categorization of graph types
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

#ifndef dborl_graph_categorization_params_h_
#define dborl_graph_categorization_params_h_

#include <dborl/algo/dborl_algo_params.h>


//: put all the additional params into this class, and add them 
//  to the parameter list in the constructor so that 
//  all the files related to the parameter set of the algorithm are 
//  generated automatically via the methods of the base class
//  no need to overwrite any of the methods in the base class thanks 
//  to the parameter list
class dborl_graph_categorization_params : public dborl_algo_params
{
public:

  //: Constructor
  dborl_graph_categorization_params(vcl_string algo_name);

  // MEMBER VARIABLES

  //: Name of graph file that holds proximity graph
  dborl_parameter<vcl_string> graph_file_;

  //: Name of file that holds similarity matrix
  dborl_parameter<vcl_string> exemplar_sim_matrix_;
  
  //: Name of file that holds similarity matrix
  dborl_parameter<vcl_string> query_sim_matrix_;
  
  //: Name of file that holds category names
  dborl_parameter<vcl_string> query_label_file_;

  //: Beta needed for beta skeletons
  dborl_parameter<double> beta_;
  
  //: Passes the folder of the dataset directory
  //: will write straight to the dataset folder
  dborl_parameter<vcl_string> dataset_dir_;    

  //: Passes the folder of the output directory
  //: of where results should go
  dborl_parameter<vcl_string> output_dir_;    

};

#endif  //_dborl_graph_categorization_params_h
