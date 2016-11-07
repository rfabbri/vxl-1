//:
// \file

#include "dborl_find_nn_params.h"


//: Constructor
dborl_find_nn_params::
dborl_find_nn_params(vcl_string algo_name) : 
    dborl_algo_params(algo_name)
{ 

  // What type of searching
  this->action_type_.set_values(
      this->param_list_, "find_nn", 
      "action_type", 
      "-fnn: [0] Find NN, [1] Test Reachability, [2] Optimal Seeds", 
      0, 0);

  // Should we print out graph 
  this->verbose_.set_values(
      this->param_list_, "find_nn", 
      "verbose", 
      "-fnn: print out search information to std out", 
      false, false);

  //: filename of labels for queries for dataset file
  this->query_label_file_.set_values
      (this->param_list_, 
       "io", 
       "query_label_file", 
       "-io: File Name for file that holds the name of the query shapes ", 
       "queries.txt", "queries.txt");

  //: filename of seeds file
  this->batch_seeds_file_.set_values
      (this->param_list_, 
       "io", 
       "batch_seeds_file", 
       "-io: String for file that holds the seeds for nn search ", 
       "batch_seeds.txt", "batch_seeds.txt");

  // What type of searching
  this->search_type_.set_values(
      this->param_list_, "find_nn", 
      "search_type", 
      "-fnn: [0] Gradient Descent, [1] Wave Propagate", 
      0, 0);

  //: Tau for extended neighborhood 
  this->tau_.set_values(
      this->param_list_, 
      "find_nn", 
      "tau", 
      "-fnn: Tau for extended neighborhood?", 
      1.0f, 1.0f);

  //: filename of batch list of data
  this->batch_files_graphs_list_.set_values
      (this->param_list_, 
       "io", 
       "batch_files_graphs_list", 
       "-io: string for holding batch files list of graphs", 
       "batch_files_graphs_list.txt", "batch_files_graphs_list.txt");

  //: filename of batch list of exemplar data matrices
  this->batch_files_matrices_list_.set_values
      (this->param_list_, 
       "io", 
       "batch_files_matrices_list", 
       "-io: string for holding batch files list of matrices", 
       "batch_files_matrices_list.txt", "batch_files_matrices_list.txt");

  //: passes the folder of the input directory
  this->dataset_dir_.
      set_values(this->param_list_, 
                 "io", "dataset_dir",
                 "dataset object folder", "", 
                 "/home/mn/research/proximity_graphs/dataset",
                 0, // for 1th input object
                 dborl_parameter_system_info::INPUT_OBJECT_DIR);

  //: passes the folder of the input directory
  this->seeds_dir_.
      set_values(this->param_list_, 
                 "io", "seeds_dir",
                 "seeds object folder", "", 
                 "/home/mn/research/proximity_graphs/dataset",
                 0, // for 1th input object
                 dborl_parameter_system_info::INPUT_OBJECT_DIR);

  //: passes the folder of the output directory
  this->output_dir_.
      set_values(this->param_list_, 
                 "io", "output_dir",
                 "output directory folder", "", 
                 "/home/mn/research/proximity_graphs/dataset/euclidian",
                 0, // for 1th input object
                 dborl_parameter_system_info::OUTPUT_FILE);

}

