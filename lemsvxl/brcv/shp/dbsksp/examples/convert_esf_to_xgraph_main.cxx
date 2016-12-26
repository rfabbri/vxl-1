// This is shp/dbsksp/examples/convert_esf_to_xgraph_main.cxx

// \file


#include <dbsk2d/pro/dbsk2d_load_esf_process.h>
#include <dbsksp/pro/dbsksp_fit_sksp_xgraph_to_sk2d_xgraph_process.h>
#include <dbsksp/pro/dbsksp_save_xgraph_xml_process.h>
#include <dbsksp/pro/dbsksp_xgraph_storage_sptr.h>

#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_arg.h>
#include <vcl_iostream.h>
#include <vcl_cstdlib.h>




//: Read in esf files, convert them to dbsksp_xshock_graph structure, and then save to xml file
int main(int argc, char *argv[])
{
  // 1) Define the arguments.
  vul_arg_info_list arg_list;
  
  // Input files
  vul_arg<vcl_string > input_files(arg_list,"-in", "Input esf files", vcl_string(""));

  // Output folder
  vul_arg<vcl_string > output_folder(arg_list,"-out", "Output folder", vcl_string("."));

  // Distance threshold for fitting
  vul_arg<float > tol (arg_list,"-tol", "Fitting tolerance", 0.5f);

  
  
  // 2) Parse input arguments
  vul_arg_include(arg_list);
  vul_arg_parse(argc, argv);


  // 3) Process the data

  if (!vul_file::is_directory(output_folder()))
  {
    vcl_cout << "\nERROR: value for \"-out\" option is not a directory.\n";
    return EXIT_FAILURE;
  }

  if (vul_file::is_directory(input_files()))
  {
    vcl_cout << "\nERROR: value for \"-in\" option is not file name(s).\n";
    return EXIT_FAILURE;
  }
 
  

  for (vul_file_iterator fn = input_files(); fn; ++fn)
  {
    // a) Load input
    vcl_string esf_file = fn();

    vcl_cout << "\nLoading esf file: " << esf_file << "...";
    dbsk2d_load_esf_process load_esf_process;
    load_esf_process.parameters()->set_value("-esfinput", bpro1_filepath(esf_file));
    load_esf_process.clear_input();
    load_esf_process.clear_output();

    bool load_success = load_esf_process.execute();
    if (!load_success)
    {
      vcl_cout << "[ Failed ]\n";
      continue;
    }
    vcl_cout << "[ OK ]\n";

    dbsk2d_shock_storage_sptr sk2d_storage;
    sk2d_storage.vertical_cast(load_esf_process.get_output()[0]);

    // b) Do the conversion
    dbsksp_fit_sksp_xgraph_to_sk2d_xgraph_process fit_process;

    if (tol.set())
    {
      fit_process.parameters()->set_value("-distance-rms-error-threshold", tol());
    }
    
    
    fit_process.clear_input();
    fit_process.clear_output();
    fit_process.add_input(sk2d_storage.ptr());

    if (!fit_process.execute())
    {
      vcl_cout << "[ Failed ]\n"; 
      continue;
    }
    else
    {
      vcl_cout << "[ OK ]\n";
    }
    dbsksp_xgraph_storage_sptr xgraph_storage;
    xgraph_storage.vertical_cast(fit_process.get_output()[0]);
    
    // c) Save output
    vcl_string xml_fname = vul_file::strip_extension(vul_file::strip_directory(esf_file)) + ".xml";
    vcl_string xml_file = output_folder() + "/" + xml_fname;

    vcl_cout << "\nSaving xgraph to xml file: " << xml_file << "...";

    dbsksp_save_xgraph_xml_process save_xgraph_process;
    save_xgraph_process.parameters()->set_value("-xmlfile", bpro1_filepath(xml_file));
    
    save_xgraph_process.clear_input();
    save_xgraph_process.clear_output();
    save_xgraph_process.add_input(xgraph_storage.ptr());
    if (!save_xgraph_process.execute())
    {
      vcl_cout << "[ Failed ]\n";
      continue;
    }
    else
    {
      vcl_cout << "[ OK ]\n";
    }
  }

  return EXIT_SUCCESS;
}

