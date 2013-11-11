// This is dbsksp/pro/dbsksp_average_two_xgraphs_process.cxx

//:
// \file

#include "dbsksp_average_two_xgraphs_process.h"

#include <dbsksp/pro/dbsksp_xgraph_storage.h>
#include <dbsksp/pro/dbsksp_xgraph_storage_sptr.h>
#include <dbsksp/pro/dbsksp_average_xgraph_storage.h>
#include <dbsksp/pro/dbsksp_average_xgraph_storage_sptr.h>

#include <dbsksp/algo/dbsksp_edit_distance.h>
#include <dbsksp/algo/dbsksp_xshock_directed_tree.h>
#include <dbsksp/algo/dbsksp_xshock_directed_tree_sptr.h>
#include <dbsksp/algo/dbsksp_average_two_xgraphs.h>
#include <dbsksp/algo/dbsksp_weighted_average_two_xgraphs.h>

#include <bpro1/bpro1_parameters.h>

// ----------------------------------------------------------------------------
//: Constructor
dbsksp_average_two_xgraphs_process::
dbsksp_average_two_xgraphs_process()
{
  if( !parameters()->add("Weight of xgraph1: " , "-weight1" , 1.0f) ||
    !parameters()->add("Weight of xgraph2: " , "-weight2" , 1.0f) ||
    !parameters()->add("Scurve matching R: " , "-scurve-matching-R" , 6.0f) ||
    !parameters()->add("Scurve sample ds: " , "-scurve-sample-ds" , 1.0f) ||
    !parameters()->add("Prefix for intermediate results (empty for not saving):" , "-base_name", vcl_string("")) 
    )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


// ----------------------------------------------------------------------------
//: Destructor
dbsksp_average_two_xgraphs_process::
~dbsksp_average_two_xgraphs_process()
{
  
}


// ----------------------------------------------------------------------------
//: Clone the process
bpro1_process* dbsksp_average_two_xgraphs_process::
clone() const
{
  return new dbsksp_average_two_xgraphs_process(*this);
}


// ----------------------------------------------------------------------------
//: Returns the name of this process
vcl_string dbsksp_average_two_xgraphs_process::
name()
{ 
  return "Average two xshock graphs"; 
}


// ----------------------------------------------------------------------------
//: Provide a vector of required input types
vcl_vector< vcl_string > dbsksp_average_two_xgraphs_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back("dbsksp_xgraph");
  to_return.push_back("dbsksp_xgraph");
  return to_return;
}



// ----------------------------------------------------------------------------
//: Provide a vector of output types
vcl_vector< vcl_string > dbsksp_average_two_xgraphs_process::
get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "dbsksp_average_xgraph" );  
  //to_return.push_back( "dbsksp_xgraph" );  
  //to_return.push_back( "dbsksp_xgraph" );  
  return to_return;
}



// ----------------------------------------------------------------------------
//: Return the number of input frames for this process
int dbsksp_average_two_xgraphs_process::
input_frames()
{
  return 1;
}


// ----------------------------------------------------------------------------
//: Return the number of output frames for this process
int dbsksp_average_two_xgraphs_process::
output_frames()
{
  return 1;
}

//: Execute this process
bool dbsksp_average_two_xgraphs_process::
execute()
{

  // Parse parameter values
  float weight1 = 1.0f;
  this->parameters()->get_value("-weight1", weight1);

  float weight2 = 1.0f;
  this->parameters()->get_value("-weight2", weight2);

  float scurve_matching_R = 6.0f;
  this->parameters()->get_value("-scurve-matching-R", scurve_matching_R);

  float scurve_sample_ds = 3.0f;
  this->parameters()->get_value("-scurve-sample-ds" , scurve_sample_ds);

  vcl_string base_name = "";
  this->parameters()->get_value("-base_name", base_name);


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

  vcl_cout << "\n>Compute average of the two xgraphs ...\n";

  ////
  //dbsksp_average_two_xgraphs engine;
  //engine.set_parent_xgraph(0, xgraph1);
  //engine.set_parent_xgraph(1, xgraph2);
  //engine.set_scurve_matching_R(scurve_matching_R);
  //engine.set_scurve_sample_ds(scurve_sample_ds);
  //engine.set_base_name(base_name);
  //engine.compute();

  dbsksp_weighted_average_two_xgraphs engine(xgraph1, xgraph2, weight1, weight2, 
    scurve_matching_R, scurve_sample_ds, 0.5, 0.01, base_name);
  engine.compute();

  vcl_cout << "\nDone.\n";

  dbsksp_xshock_graph_sptr average_xgraph = engine.average_xgraph();
  if (average_xgraph)
  {
       
    vcl_cout << "\nFinal edit cost xgraph1 - average_xgraph = " << engine.distance_to_parent(0) << "\n"
      << "Final edit cost average_xgraph - xgraph2 = " << engine.distance_to_parent(1) << "\n";

    // create a storage and push this tree in

    dbsksp_average_xgraph_storage_sptr output_average_xgraph = dbsksp_average_xgraph_storage_new();
    output_data_[0].push_back(output_average_xgraph);
    output_average_xgraph->set_xgraph(average_xgraph);
    output_average_xgraph->set_distance_to_parent(0, engine.distance_to_parent(0));
    output_average_xgraph->set_distance_to_parent(1, engine.distance_to_parent(1));
    output_average_xgraph->set_distance_btw_parents(engine.distance_btw_parents());
    output_average_xgraph->set_parent_xgraph(0, engine.parent_xgraph(0));
    output_average_xgraph->set_parent_xgraph(1, engine.parent_xgraph(1));

    ////
    //dbsksp_xgraph_storage_sptr output_xgraph1 = dbsksp_xgraph_storage_new();
    //output_data_[0].push_back(output_xgraph1);
    //output_xgraph1->set_xgraph(engine.get_common_xgraph(0));
    //
    //dbsksp_xgraph_storage_sptr output_xgraph2 = dbsksp_xgraph_storage_new();
    //output_data_[0].push_back(output_xgraph2);
    //output_xgraph2->set_xgraph(engine.get_common_xgraph(1));

    return true;
  }
  return false;
}


//------------------------------------------------------------------------------
//:
bool dbsksp_average_two_xgraphs_process::
finish()
{
  return true;
}





