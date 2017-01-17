// This is dbsksp/pro/dbsksp_edit_distance_process.cxx

//:
// \file

#include "dbsksp_edit_distance_process.h"

#include <dbsksp/pro/dbsksp_xgraph_storage.h>
#include <dbsksp/pro/dbsksp_xgraph_storage_sptr.h>

#include <dbsksp/algo/dbsksp_edit_distance.h>
#include <dbsksp/algo/dbsksp_xshock_directed_tree.h>
#include <dbsksp/algo/dbsksp_xshock_directed_tree_sptr.h>
#include <dbsksp/algo/dbsksp_average_two_xgraphs.h>
#include <bpro1/bpro1_parameters.h>

// ----------------------------------------------------------------------------
//: Constructor
dbsksp_edit_distance_process::
dbsksp_edit_distance_process()
{
  if( !parameters()->add("Scurve matching R: " , "-scurve-matching-R" , 6.0f) ||
    !parameters()->add("Scurve sample ds: " , "-scurve-sample-ds" , 3.0f)
    )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


// ----------------------------------------------------------------------------
//: Destructor
dbsksp_edit_distance_process::
~dbsksp_edit_distance_process()
{
  
}


// ----------------------------------------------------------------------------
//: Clone the process
bpro1_process* dbsksp_edit_distance_process::
clone() const
{
  return new dbsksp_edit_distance_process(*this);
}


// ----------------------------------------------------------------------------
//: Returns the name of this process
vcl_string dbsksp_edit_distance_process::
name()
{ 
  return "Edit distance between two xgraphs"; 
}


// ----------------------------------------------------------------------------
//: Provide a vector of required input types
vcl_vector< vcl_string > dbsksp_edit_distance_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back("dbsksp_xgraph");
  to_return.push_back("dbsksp_xgraph");
  return to_return;
}



// ----------------------------------------------------------------------------
//: Provide a vector of output types
vcl_vector< vcl_string > dbsksp_edit_distance_process::
get_output_type()
{
  vcl_vector<vcl_string > to_return;
  //to_return.push_back( "dbsksp_xgraph" );  
  //to_return.push_back( "dbsksp_xgraph" );  
  return to_return;
}



// ----------------------------------------------------------------------------
//: Return the number of input frames for this process
int dbsksp_edit_distance_process::
input_frames()
{
  return 1;
}


// ----------------------------------------------------------------------------
//: Return the number of output frames for this process
int dbsksp_edit_distance_process::
output_frames()
{
  return 1;
}

//: Execute this process
bool dbsksp_edit_distance_process::
execute()
{

  float scurve_matching_R = 6.0f;
  this->parameters()->get_value("-scurve-matching-R", scurve_matching_R);

  float scurve_sample_ds = 3.0f;
  this->parameters()->get_value("-scurve-sample-ds" , scurve_sample_ds);


  vcl_cout << "\nRetrieve two xgraphs from repository ...";

  // input shock graphs
  dbsksp_xgraph_storage_sptr input_xgraph1;
  input_xgraph1.vertical_cast(input_data_[0][0]);
  
  dbsksp_xgraph_storage_sptr input_xgraph2;
  input_xgraph2.vertical_cast(input_data_[0][1]);

  // Retrieve the two xgraphs
  dbsksp_xshock_graph_sptr xgraph1 = input_xgraph1->xgraph();
  dbsksp_xshock_graph_sptr xgraph2 = input_xgraph2->xgraph();

  

  if (!xgraph1 || !xgraph2)
  {
    vcl_cout << "failed. Quit now.\n";
    return false;
  }
  else
  {
    vcl_cout << "done.\n";
  }

  // Call edit distance
  dbsksp_edit_distance edit_distance;
  edit_distance.set(xgraph1, xgraph2, scurve_matching_R, scurve_sample_ds);
  edit_distance.save_path(true);
  edit_distance.edit();

  vcl_vector<pathtable_key > corr;
  edit_distance.get_final_correspondence(corr);
  double deform_cost = edit_distance.get_deform_cost(corr);

  // Print out cost
  vcl_cout 
    << "\nFinal editing cost = " << edit_distance.final_cost()
    << "\n  Deformation cost = " << deform_cost   
    << "\n  Total splice cost - tree1 = " << edit_distance.tree1()->total_splice_cost()
    << "\n  Total splice cost - tree2 = " << edit_distance.tree2()->total_splice_cost() << "\n";

  //// Edited xgraphs to match the common coarse graph topology
  //dbsksp_xshock_graph_sptr common_xgraph1 = edit_distance.edit_xgraph1_to_common_topology();
  //dbsksp_xshock_graph_sptr common_xgraph2 = edit_distance.edit_xgraph2_to_common_topology();
  //
 
  //if (common_xgraph1 && common_xgraph2)
  //{
  //  // create a storage and push this tree in
  //  dbsksp_xgraph_storage_sptr output_xgraph1 = dbsksp_xgraph_storage_new();
  //  output_data_[0].push_back(output_xgraph1);
  //  output_xgraph1->set_xgraph(common_xgraph1);

  //  dbsksp_xgraph_storage_sptr output_xgraph2 = dbsksp_xgraph_storage_new();
  //  output_data_[0].push_back(output_xgraph2);
  //  output_xgraph2->set_xgraph(common_xgraph2);
  //  return true;
  //}

  return false;
}

bool dbsksp_edit_distance_process::
finish()
{
  return true;
}





