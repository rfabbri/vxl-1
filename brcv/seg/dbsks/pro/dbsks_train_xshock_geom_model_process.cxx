// This is dbsks/pro/dbsks_train_xshock_geom_model_process.cxx

//:
// \file

#include "dbsks_train_xshock_geom_model_process.h"

#include <bpro1/bpro1_parameters.h>
#include <dbsks/algo/dbsks_train_geom_model.h>
#include <vul/vul_file.h>

//------------------------------------------------------------------------------
//: Constructor
dbsks_train_xshock_geom_model_process::
dbsks_train_xshock_geom_model_process()
{
  if ( !parameters()->add("xgraph folder:" , "-xgraph-folder", bpro1_filepath("")) ||
    !parameters()->add("xgraph list file:" , "-xgraph-list-file", bpro1_filepath("")) ||
    !parameters()->add("Root vertex id:" , "-root-vid", unsigned(0)) ||
	!parameters()->add("Normalize xgraph? " , "-b-normalize", bool(true)) ||
    !parameters()->add("pseudo parent edge id:" , "-pseudo-parent-eid", unsigned(0)) ||
    !parameters()->add("Output file:" , "-output-file", bpro1_filepath(""))
    )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//------------------------------------------------------------------------------
//: Destructor
dbsks_train_xshock_geom_model_process::
~dbsks_train_xshock_geom_model_process()
{
}


//------------------------------------------------------------------------------
//: Clone the process
bpro1_process* dbsks_train_xshock_geom_model_process::
clone() const
{
  return new dbsks_train_xshock_geom_model_process(*this);
}


//------------------------------------------------------------------------------
//: Returns the name of this process
vcl_string dbsks_train_xshock_geom_model_process::
name()
{ 
  return "Train xshock geometry model"; 
}


//------------------------------------------------------------------------------
//: Provide a vector of required input types
vcl_vector< vcl_string > dbsks_train_xshock_geom_model_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.clear();
  return to_return;
}



//------------------------------------------------------------------------------
//: Provide a vector of output types
vcl_vector< vcl_string > dbsks_train_xshock_geom_model_process::
get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.clear();
  return to_return;
}


//------------------------------------------------------------------------------
//: Return the number of input frames for this process
int dbsks_train_xshock_geom_model_process::
input_frames()
{
  return 1;
}


//------------------------------------------------------------------------------
//: Return the number of output frames for this process
int dbsks_train_xshock_geom_model_process::
output_frames()
{
  return 1;
}



//------------------------------------------------------------------------------
//: Execute this process
bool dbsks_train_xshock_geom_model_process::
execute()
{
  // USER PARAMS --------------------------------------------------------------

  // folder containing exemplar xgraphs
  bpro1_filepath xgraph_folder;
  this->parameters()->get_value("-xgraph-folder", xgraph_folder);

  // list of names of available exemplar xgraphs
  bpro1_filepath xgraph_list_file;
  this->parameters()->get_value("-xgraph-list-file", xgraph_list_file);

  // id of root vertex
  unsigned root_vid = 0;
  this->parameters()->get_value("-root-vid", root_vid);
  
  // id of pseudo parent edge
  unsigned pseudo_parent_eid = 0;
  this->parameters()->get_value("-pseudo-parent-eid", pseudo_parent_eid);

  bool b_normalize = true;
  this->parameters()->get_value("-b-normalize", b_normalize);

  // name of output file
  bpro1_filepath output_file;
  this->parameters()->get_value("-output-file", output_file);



  // STORAGE CLASSES ----------------------------------------------------------
  
  
  // PROCESS DATA -------------------------------------------------------------

  // trainer parameter
  dbsks_train_geom_model_params params;
  params.normalized_xgraph_size = 100; // \todo make this a parameter
  params.b_normalize = b_normalize;
  params.output_file = output_file.path;
  params.pseudo_parent_eid = pseudo_parent_eid;
  params.root_vid = root_vid;
  params.xshock_folder = xgraph_folder.path;
  params.xshock_list_file = xgraph_list_file.path;


  // trainer
  dbsks_train_geom_model trainer;

  // execution
  trainer.set_training_info(params);
  trainer.collect_data();
  trainer.construct_geom_model();
  trainer.save_geom_model_to_file();
  
  
  // OUTPUT DATA ---------------------------------------------------------------

  return true;
}



// ----------------------------------------------------------------------------
bool dbsks_train_xshock_geom_model_process::
finish()
{
  return true;
}







