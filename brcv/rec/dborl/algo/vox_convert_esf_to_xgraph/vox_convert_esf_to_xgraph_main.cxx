// This is rec/dborl/algo/vox_convert_esf_to_xgraph/vox_convert_esf_to_xgraph_main.cxx

//:
// \file
// \author Nhon Trinh ( ntrinh@lems.brown.edu )
// \date Mar 19, 2010
// \brief An algorithm to compute average of two shock graphs
//  
// \verbatim
// \endverbatim


#include "vox_convert_esf_to_xgraph_params.h"
#include "vox_convert_esf_to_xgraph_params_sptr.h"
#include <vul/vul_file.h>
#include <vul/vul_timer.h>





#include <dbsk2d/pro/dbsk2d_shock_storage_sptr.h>
#include <dbsk2d/pro/dbsk2d_load_esf_process.h>
#include <dbsksp/pro/dbsksp_xgraph_storage_sptr.h>
#include <dbsksp/pro/dbsksp_xgraph_storage.h>
#include <dbsksp/pro/dbsksp_fit_sksp_xgraph_to_sk2d_xgraph_process.h>
#include <dbsksp/xio/dbsksp_xio_xshock_graph.h>

#include <vul/vul_file.h>
#include <vcl_cstdlib.h>

// Convert esf file to an xgraph
bool convert_esf_to_xgraph(const vcl_string& esf_file, float tol,  
                           dbsksp_xshock_graph_sptr& xgraph);

int main(int argc, char *argv[]) 
{
  // Let time how long this takes
  // Start timer
  vul_timer t;

  // construct parameters with the default values;
  vox_convert_esf_to_xgraph_params_sptr params = 
    new vox_convert_esf_to_xgraph_params("vox_convert_esf_to_xgraph");  

  // parse the command line arguments
  if (!params->parse_command_line_args(argc, argv))
    return 1;

  //: always print the params file if an executable to work with ORL web 
  // interface
  if (!params->print_params_xml(params->print_params_file()))
  {
    vcl_cerr << "problems in writing params file to: " 
      << params->print_params_file() << vcl_endl;
  }

  // exit if there is nothing else to do
  if (params->exit_with_no_processing() || params->print_params_only())
  {
    return 0;
  }

  //: always call this method to actually parse the input parameter file 
  // whose name is extracted from the command line
  if (!params->parse_input_xml())
  {
    return 1;
  }

  // Averaging two xgraphs
  vcl_cout<<"************  Convert esf to xgraph  *************\n";

 
  vcl_string esf_file = params->object_dir_() + "/" + params->object_name_() + ".esf";
  float tol = params->convert_sk2d_to_sksp_tol_();

  vcl_cout 
    << "\nObject name = " << params->object_name_()
    << "\n  esf file  = " << esf_file
    << "\n  err tol   = " << tol << "\n";

  vcl_cout << "\nConvering esf file to xgraph ...";
  dbsksp_xshock_graph_sptr xgraph = 0;
  bool success = convert_esf_to_xgraph(esf_file, tol, xgraph);

  if (!success)
  {
    vcl_cout << "ERROR: conversion failed.\n";
    
    // Update status results
    params->percent_completed = 0.0f;
    params->exit_code = 1;
    params->exit_message = "esf file conversion failed !";
    params->print_status_xml();

    return EXIT_FAILURE;
  }
  vcl_cout << "[ OK ]\n";
  
  // save xgraph to folder

  vcl_string xgraph_folder = params->save_to_object_folder_() ? 
    params->object_dir_() : params->output_folder_();

  // create folder if it doesn't exist yet
  if (!vul_file::is_directory(xgraph_folder))
  {
    vul_file::make_directory(xgraph_folder);
  }

  vcl_string xgraph_file = xgraph_folder + "/" + params->object_name_() + params->xgraph_extension_();
  vcl_cout << "\nSaving xgraph to: " << xgraph_file << "\n";
  success = x_write(xgraph_file, xgraph);

  if (!success)
  {
    vcl_cout << "\nERROR: Saving xgraph file failed.\n";
    // Update status results
    params->percent_completed = 0.0f;
    params->exit_code = 1;
    params->exit_message = "Saving -xgraph.xml file failed!";
    params->print_status_xml();

    return EXIT_FAILURE;
  }

  // Finish off by updating status results
  params->percent_completed = 100.0f;
  params->exit_code = 0;
  params->exit_message = "Converting esf to xgraph finished succesfully";
  params->print_status_xml();

  double vox_time = t.real()/1000.0;
  t.mark();
  vcl_cout<<vcl_endl;
  vcl_cout<<"************ Time taken: "<<vox_time<<" sec"<<vcl_endl;

  return 0;
}




//------------------------------------------------------------------------------
// Convert esf file to an xgraph
bool convert_esf_to_xgraph(const vcl_string& esf_file, float tol,  
                           dbsksp_xshock_graph_sptr& xgraph)
{
  // sanitize output container
  xgraph = 0;

  //1) Load esf file

  dbsk2d_load_esf_process load_process;
  load_process.parameters()->set_value("-esfinput" , bpro1_filepath(esf_file));
  load_process.clear_input();
  load_process.clear_output();

  bool success = load_process.execute();
  if (!success)
    return false;

  // retrieve output
  dbsk2d_shock_storage_sptr sk2d_storage = 0;
  sk2d_storage.vertical_cast(load_process.get_output()[0]);


  //2) Convert dbsk2d_shock_graph to dbsksp_xshock_graph

  // set parameters for storage
  dbsksp_fit_sksp_xgraph_to_sk2d_xgraph_process fit_process;
  fit_process.parameters()->set_value("-distance-rms-error-threshold", tol);
  fit_process.clear_input();
  fit_process.clear_output();
  fit_process.add_input(sk2d_storage.ptr());
  success = fit_process.execute();

  if (!success)
    return false;

  // retrieve output
  dbsksp_xgraph_storage_sptr sksp_storage;
  sksp_storage.vertical_cast(fit_process.get_output()[0]);  

  //3) Save output
  xgraph = sksp_storage->xgraph();
  return true;  
}

