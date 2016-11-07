//:
// \file

#include "dborl_graph_construction_params.h"


//: Constructor
dborl_graph_construction_params::
dborl_graph_construction_params(vcl_string algo_name) : 
    dborl_algo_params(algo_name)
{ 

  // Should we print out graph 
  this->verbose_.set_values(
      this->param_list_, "graph_construction", 
      "verbose", 
      "-gc: print out graph information to std out", 
      false, false);

  // What type of graph to construct 
  this->graph_type_.set_values(
      this->param_list_, "graph_construction", 
      "graph_type", 
      "-gc: [0] Beta Graph [1] Knn_Graph", 
      0, 0);

  //: filename similarity matrix for dataset file
  this->exemplar_sim_matrix_.set_values
      (this->param_list_, 
       "io", 
       "exemplar_sim_matrix", 
       "-io: File Name for file that holds exemplar sim matrix ", 
       "exemplar_dataset.txt", "exemplar_dataset.txt");

  //: filename of labels for exemplars for dataset file
  this->exemplar_label_file_.set_values
      (this->param_list_, 
       "io", 
       "exemplar_label_file", 
       "-io: File Name for file that holds the name for each node ", 
       "exemplar.txt", "exemplar.txt");

  //: filename of labels for exemplars for dataset file
  this->exemplar_category_file_.set_values
      (this->param_list_, 
       "io", 
       "exemplar_category_file", 
       "-io: File Name for file that holds the category for each node ", 
       "exemplar_cat.txt", "exemplar_cat.txt");

  // Beta value for proximity graph construction 
  this->beta_.set_values(
      this->param_list_, "graph_construction", 
      "beta", 
      "-gc: Beta to be used for constructing proximity graphs ?", 
      1.0f, 1.0f);

  // Neighbors for Knn construction 
  this->neighbors_.set_values(
      this->param_list_, "graph_construction", 
      "neighbors", 
      "-gc: Neighbor to be used for Knn graph construction ?", 
      2,2);

  // Should we perform wilson editing
  this->wilson_edit_.set_values(
      this->param_list_, "graph_construction", 
      "wilson_edit", 
      "-gc: perform editing on the graph? ", 
      false, false);

  // Should we perform condensing of the graph
  this->thinning_.set_values(
      this->param_list_, "graph_construction", 
      "thinning", 
      "-gc: perform thining on the graph? ", 
      false, false);

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

