// This is dbsks/pro/dbsks_compute_shock_graph_stats_process.cxx

//:
// \file

#include "dbsks_compute_shock_graph_stats_process.h"

#include <bpro1/bpro1_parameters.h>

#include <dbsks/dbsks_shock_graph_stats.h>
#include <dbsksp/pro/dbsksp_shock_storage.h>
#include <dbsksp/xio/dbsksp_xio_shock_graph.h>
#include <dbsks/xio/dbsks_xio_stats.h>





//: Constructor
dbsks_compute_shock_graph_stats_process::
dbsks_compute_shock_graph_stats_process()
{

  if ( !parameters()->add("Shock graph folder: " , "shock_graph_folder", bpro1_filepath("")) ||
    !parameters()->add("Shock graph list: " , "shock_graph_list", bpro1_filepath("")) ||
    !parameters()->add("Save result to file: " , "out_stats_file", bpro1_filepath(""))
    )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbsks_compute_shock_graph_stats_process::
~dbsks_compute_shock_graph_stats_process()
{
}


//: Clone the process
bpro1_process* dbsks_compute_shock_graph_stats_process::
clone() const
{
  return new dbsks_compute_shock_graph_stats_process(*this);
}

//: Returns the name of this process
vcl_string dbsks_compute_shock_graph_stats_process::
name()
{ 
  return "Compute shock graph statistics"; 
}

//: Provide a vector of required input types
vcl_vector< vcl_string > dbsks_compute_shock_graph_stats_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back("dbsksp_shock");
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbsks_compute_shock_graph_stats_process::
get_output_type()
{
  vcl_vector<vcl_string > to_return;
  return to_return;
}

//: Return the number of input frames for this process
int dbsks_compute_shock_graph_stats_process::
input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int dbsks_compute_shock_graph_stats_process::
output_frames()
{
  return 1;
}

//: Execute this process
bool dbsks_compute_shock_graph_stats_process::
execute()
{  
  if ( input_data_.size() != 1 )
  {
    vcl_cerr << "In dbsks_compute_shock_graph_stats_process::execute() - "
      << "not exactly one input frame" << vcl_endl;
    return false;
  }


  // USER PARAMS --------------------------------------------------------------

  bpro1_filepath shock_graph_folder;
  parameters()->get_value("shock_graph_folder", shock_graph_folder);

  bpro1_filepath shock_graph_list;
  parameters()->get_value("shock_graph_list", shock_graph_list);

  bpro1_filepath out_stats_file;
  parameters()->get_value("out_stats_file", out_stats_file);

  // INPUT STORAGE CLASSES -----------------------------------------------------

  // shock graph
  dbsksp_shock_storage_sptr shock_storage;
  shock_storage.vertical_cast(input_data_[0][0]);
  

  // PROCESS DATA --------------------------------------------------------------
  
  // Reference shock graph
  dbsksp_shock_graph_sptr ref_graph = shock_storage->shock_graph();

  // Load list of shock graphs used as data to compute statistics
  vcl_vector<vcl_string > file_list;
  this->parse_lines_from_file(shock_graph_list.path, file_list);

  // list of loaded shock graphs
  vcl_vector<dbsksp_shock_graph_sptr > loaded_shock_graphs;
  for (unsigned i =0; i < file_list.size(); ++i)
  {
    vcl_string shock_graph_file_path = shock_graph_folder.path + "\\" + file_list[i];
    dbsksp_shock_graph_sptr graph;
    if (!x_read(shock_graph_file_path, graph))
    {
      vcl_cout << "ERROR: Unable to loaded shock graph file: " 
        << shock_graph_file_path << ".\n";
    }
    loaded_shock_graphs.push_back(graph);
  }


  // Compute statistics
  dbsks_shock_graph_stats graph_stats(ref_graph);
  graph_stats.compute_stats(loaded_shock_graphs);


  // OUTPUT STORAGE CLASSES -----------------------------------------------------
  
  // Save result to a file
  x_write(out_stats_file.path, graph_stats);


  // DEBUG ////////
  dbsks_shock_graph_stats stats2;
  x_read(out_stats_file.path, graph_stats.shock_graph(), stats2);

  // write back
  x_write(out_stats_file.path + ".bak", stats2);

  return true;
}


// ----------------------------------------------------------------------------
bool dbsks_compute_shock_graph_stats_process::
finish()
{
  return true;
}





bool dbsks_compute_shock_graph_stats_process::
parse_lines_from_file(vcl_string fname, vcl_vector<vcl_string>& strings)
{
  vcl_ifstream fp(fname.c_str());
  if (!fp) {
    vcl_cout<<" In dborl_utilities - parse_file(): Unable to Open " << fname << vcl_endl;
    return false;
  }

  while (!fp.eof()) {
    char buffer[1000];
    fp.getline(buffer, 1000);
    vcl_string name = buffer;
    if (name.size() > 0) {
      strings.push_back(name);
    }
  }
  fp.close();
  return true;
}

