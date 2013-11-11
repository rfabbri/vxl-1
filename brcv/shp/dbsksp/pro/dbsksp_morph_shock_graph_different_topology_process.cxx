// This is dbsksp/pro/dbsksp_morph_shock_graph_different_topology_process.cxx

//:
// \file

#include "dbsksp_morph_shock_graph_different_topology_process.h"

#include <bpro1/bpro1_parameters.h>
#include <dbsksp/dbsksp_shock_graph.h>
#include <dbsksp/dbsksp_shock_graph_sptr.h>
#include <dbsksp/pro/dbsksp_shock_storage.h>
#include <dbsksp/pro/dbsksp_shock_storage_sptr.h>

#include <dbskr/pro/dbskr_shock_match_storage.h>
#include <dbskr/pro/dbskr_shock_match_storage_sptr.h>

#include <dbsksp/algo/dbsksp_morph_shock_graph_different_topology.h>
#include <dbsksp/algo/dbsksp_morph_shock_graph_same_topology.h>





// ----------------------------------------------------------------------------
//: Constructor
dbsksp_morph_shock_graph_different_topology_process::
dbsksp_morph_shock_graph_different_topology_process()
{
  if( 
    !parameters()->add("scurve_sample_ds: " , "-scurve_sample_ds", 
      double(1) ) ||
    !parameters()->add("shock_interp_sample_ds: " , "-shock_interp_sample_ds", 
      double(6) )
  )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}

// ----------------------------------------------------------------------------
//: Destructor
dbsksp_morph_shock_graph_different_topology_process::
~dbsksp_morph_shock_graph_different_topology_process()
{
  
}


// ----------------------------------------------------------------------------
//: Clone the process
bpro1_process* dbsksp_morph_shock_graph_different_topology_process::
clone() const
{
  return new dbsksp_morph_shock_graph_different_topology_process(*this);
}


// ----------------------------------------------------------------------------
//: Returns the name of this process
vcl_string dbsksp_morph_shock_graph_different_topology_process::
name()
{ 
  return "Morph Shock Graph - different topology"; 
}




// ----------------------------------------------------------------------------
void dbsksp_morph_shock_graph_different_topology_process::
clear_output(int resize)
{
  this->set_num_frames(0);
  bpro1_process::clear_output(resize);
}




// ----------------------------------------------------------------------------
//: Provide a vector of required input types
vcl_vector< vcl_string > dbsksp_morph_shock_graph_different_topology_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back("shock_match");
  return to_return;
}



// ----------------------------------------------------------------------------
//: Provide a vector of output types
vcl_vector< vcl_string > dbsksp_morph_shock_graph_different_topology_process::
get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "dbsksp_shock" );
  to_return.push_back( "dbsksp_shock" );
  return to_return;
}



// ----------------------------------------------------------------------------
//: Return the number of input frames for this process
int dbsksp_morph_shock_graph_different_topology_process::
input_frames()
{
  return 1;
}


// ----------------------------------------------------------------------------
//: Return the number of output frames for this process
int dbsksp_morph_shock_graph_different_topology_process::
output_frames()
{
  return this->num_frames();
}

//: Execute this process
bool dbsksp_morph_shock_graph_different_topology_process::
execute()
{
  

  // simple checks
  if ( this->input_data_.size() != 1 ){
    vcl_cerr << "ERROR: executing " __FILE__ "not exactly one input frame.\n";
    return false;
  }

  double scurve_sample_ds = 0;
  parameters()->get_value( "-scurve_sample_ds" , scurve_sample_ds);

  double shock_interp_sample_ds = 0;
  parameters()->get_value( "-shock_interp_sample_ds" , shock_interp_sample_ds);

  dbskr_shock_match_storage_sptr shock_match_storage;
  shock_match_storage.vertical_cast(input_data_[0][0]);

  // set up the morphing
  dbsksp_morph_shock_graph_different_topology morpher;
  morpher.set_sm_cor(shock_match_storage->get_sm_cor());
  morpher.shock_interp_sample_ds_ = shock_interp_sample_ds;
  morpher.scurve_sample_ds_ = scurve_sample_ds;

  morpher.morph();


  //// transition graphs
  //vcl_vector<dbsksp_shock_graph_sptr > transition_graphs_1 = 
  //  morpher.transitions_tree1();
  //vcl_vector<dbsksp_shock_graph_sptr > transition_graphs_2 = 
  //  morpher.transitions_tree2();


  //// compute the intermediate graphs between the last of the two lists
  //// we need a same-topology morpher
  //dbsksp_morph_shock_graph_same_topology sametop_morpher;
  //sametop_morpher.set_source_graph(transition_graphs_1.back());
  //sametop_morpher.set_target_graph(transition_graphs_2.back());

  //bool success = sametop_morpher.compute_correspondence();


  //// merge the two list, back to front
  //vcl_vector<dbsksp_shock_graph_sptr > transition_graphs = transition_graphs_1;

  //// insert this the mean graph in between
  //if (success)
  //{
  //  vcl_cout << "The final two graphs have the same topology.\n";
  //  sametop_morpher.morph();
  //  dbsksp_shock_graph_sptr mean_graph = 
  //    sametop_morpher.get_intermediate_graph(0.5);
  //  transition_graphs.push_back(mean_graph);

  //}
  //else
  //{
  //  vcl_cout << "The final two graphs do not have the same topology.\n";
  //}

  //// transition graphs for tree 1
  //for (vcl_vector<dbsksp_shock_graph_sptr >::reverse_iterator itr = 
  //  transition_graphs_2.rbegin(); itr != transition_graphs_2.rend(); ++itr)
  //{
  //  transition_graphs.push_back(*itr);
  //}

  //assert(transition_graphs.size() > 1);

  //// save transition graphs to output data
  //// go backward
  //for (vcl_vector<dbsksp_shock_graph_sptr >::reverse_iterator itr = 
  //  transition_graphs.rbegin(); itr != transition_graphs.rend(); ++itr)
  //{
  //  // new storage
  //  dbsksp_shock_storage_sptr output_shock = dbsksp_shock_storage_new();
  //  
  //  // new shock to put in the storage
  //  dbsksp_shock_graph_sptr graph = *itr;
  //  output_shock->set_shock_graph(graph);

  //  // storage vector
  //  vcl_vector<bpro1_storage_sptr > frame_storage;
  //  frame_storage.push_back(output_shock);
  //  output_data_.push_back(frame_storage);
  //}
  //this->set_num_frames(transition_graphs.size());




  //// insert the final common graph too
  //if (morpher.tree1_mean_graph_)
  //{
  //  transition_graphs.push_back(morpher.tree1_mean_graph_);
  //}

  //if (morpher.tree2_mean_graph_)
  //{
  //  transition_graphs.push_back(morpher.tree2_mean_graph_);
  //}



  // storage vector
  vcl_vector<bpro1_storage_sptr > frame1_storage;
  vcl_vector<bpro1_storage_sptr > frame2_storage;

  // frame 1: tree1 --> mean shape
  dbsksp_shock_storage_sptr tree1_init = dbsksp_shock_storage_new();
  tree1_init->set_shock_graph(morpher.tree1_init_graph_);

  dbsksp_shock_storage_sptr tree1_final = dbsksp_shock_storage_new();
  tree1_final->set_shock_graph(morpher.tree1_mean_graph_);

  frame1_storage.push_back(tree1_init);
  frame1_storage.push_back(tree1_final);
  


  // frame 2: mean shape --> tree2
  dbsksp_shock_storage_sptr tree2_init = dbsksp_shock_storage_new();
  tree2_init->set_shock_graph(morpher.tree2_init_graph_);

  dbsksp_shock_storage_sptr tree2_final = dbsksp_shock_storage_new();
  tree2_final->set_shock_graph(morpher.tree2_mean_graph_);
  
  frame2_storage.push_back(tree2_final);
  frame2_storage.push_back(tree2_init);
  
  // Save the frames
  output_data_.push_back(frame2_storage);
  output_data_.push_back(frame1_storage);
  this->set_num_frames(2);
  

  



  return true;
}

bool dbsksp_morph_shock_graph_different_topology_process::
finish()
{
  return true;
}





