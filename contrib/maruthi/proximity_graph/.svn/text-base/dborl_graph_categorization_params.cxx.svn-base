//:
// \file

#include "dborl_graph_categorization_params.h"


//: Constructor
dborl_graph_categorization_params::
dborl_graph_categorization_params(vcl_string algo_name) : 
    dborl_algo_params(algo_name)
{ 

  //: filename similarity matrix for dataset file
  this->graph_file_.set_values
      (this->param_list_, 
       "io", 
       "graph_file", 
       "-io: File Name for proximity graph xml file ", 
       "beta_1_graph.xml", "beta_1_graph.xml");

  //: filename similarity matrix for dataset file
  this->exemplar_sim_matrix_.set_values
      (this->param_list_, 
       "io", 
       "exemplar_sim_matrix", 
       "-io: File Name for file that holds exemplar sim matrix ", 
       "exemplar_dataset.txt", "exemplar_dataset.txt");

  //: filename similarity matrix for dataset file
  this->query_sim_matrix_.set_values
      (this->param_list_, 
       "io", 
       "query_sim_matrix", 
       "-io: File Name for file that holds query sim matrix ", 
       "query_dataset.txt", "query_dataset.txt");

  //: filename of labels for queries for dataset file
  this->query_label_file_.set_values
      (this->param_list_, 
       "io", 
       "query_label_file", 
       "-io: File Name for file that holds the name of the query shapes ", 
       "queries.txt", "queries.txt");

  // Beta value for proximity graph categorization 
  this->beta_.set_values(
      this->param_list_, "graph_categorization", 
      "beta", 
      "-gc: Beta to be used for constructing proximity graphs ?", 
      1.0f, 1.0f);

  //: passes the folder of the input directory
  this->dataset_dir_.
      set_values(this->param_list_, 
                 "io", "model_object_dir",
                 "model object folder", "", 
                 "/home/mn/research/proximity_graphs/dataset/euclidian",
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

