// This is shp/dbsksp/examples/average_two_xgraphs_main.cxx

// \file


#include <dbsksp/pro/dbsksp_save_xgraph_xml_process.h>
#include <dbsksp/pro/dbsksp_xgraph_storage_sptr.h>
#include <dbsksp/pro/dbsksp_load_xgraph_xml_process.h>
#include <dbsksp/pro/dbsksp_average_two_xgraphs_process.h>

#include <bpro1/bpro1_parameters.h>

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
  vul_arg<vcl_string > xgraph1_file(arg_list,"-xgraph1", "Input file for xgraph1", vcl_string(""));

  vul_arg<vcl_string > xgraph2_file(arg_list,"-xgraph2", "Input file for xgraph2", vcl_string(""));

  // Output file
  vul_arg<vcl_string > output_file(arg_list,"-out", "Output xgraph file", vcl_string(""));

  // Averaging parameters
  vul_arg<float > scurve_matching_R(arg_list, "-scurve-matching-R", "Parameter R in matching scurve", 6.0f);
  vul_arg<float > scurve_sample_ds(arg_list, "-scurve-sample-ds", "Sample ds in matching scurve", 1.0f);

  
  // 2) Parse input arguments
  vul_arg_include(arg_list);
  vul_arg_parse(argc, argv);


  // 3) Process the data

  if (!xgraph1_file.set() || !xgraph2_file.set())
  {
    vcl_cout << "\nERROR: missing arguments for '-xgraph1' or '-xgraph2'.\n";
    return EXIT_FAILURE;
  }

  // a) Load xgraph1
  vcl_cout << "\nLoading xgraph1 xml file: " << xgraph1_file() << "...";
  dbsksp_load_xgraph_xml_process load_xgraph_process;
  
  load_xgraph_process.parameters()->set_value("-xmlfile", xgraph1_file());
  load_xgraph_process.clear_input();
  load_xgraph_process.clear_output();

  bool success = load_xgraph_process.execute();
  if (!success)
  {
    vcl_cout << "[ Failed ]\n";
    return EXIT_FAILURE;
  }
  vcl_cout << "[ OK ]\n";

  // retrieve storage for xgraph1
  dbsksp_xgraph_storage_sptr xgraph1_storage;
  xgraph1_storage.vertical_cast(load_xgraph_process.get_output().front());

  // b) Load xgraph2
  vcl_cout << "\nLoading xgraph2 xml file: " << xgraph1_file() << "...";
  load_xgraph_process.parameters()->set_value("-xmlfile", xgraph2_file());
  load_xgraph_process.clear_input();
  load_xgraph_process.clear_output();

  success = load_xgraph_process.execute();
  if (!success)
  {
    vcl_cout << "[ Failed ]\n";
    return EXIT_FAILURE;
  }
  vcl_cout << "[ OK ]\n";
  // retrieve storage for xgraph2
  dbsksp_xgraph_storage_sptr xgraph2_storage;
  xgraph2_storage.vertical_cast(load_xgraph_process.get_output().front());

  // c) Compute average of two xgraphs
  dbsksp_average_two_xgraphs_process averaging_process;

  if (scurve_matching_R())
  {
    averaging_process.parameters()->set_value("-scurve-matching-R", scurve_matching_R());
  }

  if (scurve_sample_ds())
  {
    averaging_process.parameters()->set_value("-scurve-sample-ds", scurve_sample_ds());
  }


  averaging_process.clear_input();
  averaging_process.clear_output();
  averaging_process.add_input(xgraph1_storage);
  averaging_process.add_input(xgraph2_storage);

  vcl_cout << "\nComputing average of two loaded xgraphs ... ";
  success = averaging_process.execute();
  if (!success)
  {
    vcl_cout << "[ Failed ]\n";
    return EXIT_FAILURE;
  }
  vcl_cout << "[ OK ]\n";

  dbsksp_xgraph_storage_sptr average_xgraph_storage;
  average_xgraph_storage.vertical_cast(averaging_process.get_output()[1]);

  dbsksp_xgraph_storage_sptr xgraph1_trimmed_storage;
  xgraph1_trimmed_storage.vertical_cast(averaging_process.get_output()[0]);

  dbsksp_xgraph_storage_sptr xgraph2_trimmed_storage;
  xgraph2_trimmed_storage.vertical_cast(averaging_process.get_output()[2]);

  // d) Save output

  vcl_string basename = vul_file::strip_extension(output_file());
  if (!output_file.set())
  {
    vcl_string name1 = vul_file::strip_extension(vul_file::strip_directory(xgraph1_file()));
    vcl_string name2 = vul_file::strip_extension(vul_file::strip_directory(xgraph2_file()));
    basename = "./" + name1 + "+" + name2;
  }

  vcl_string average_xgraph_file = basename + ".xml";
  vcl_string xgraph1_trimmed_file  = basename + "_1.xml";
  vcl_string xgraph2_trimmed_file  = basename + "_2.xml";

  // d.1) save average xgraph
  vcl_cout << "\nSaving average xgraph to xml file: " << average_xgraph_file << "...";

  dbsksp_save_xgraph_xml_process save_xgraph_process;
  save_xgraph_process.parameters()->set_value("-xmlfile", bpro1_filepath(average_xgraph_file));
  save_xgraph_process.clear_input();
  save_xgraph_process.clear_output();
  save_xgraph_process.add_input(average_xgraph_storage);
  if (!save_xgraph_process.execute())
  {
    vcl_cout << "[ Failed ]\n";
    EXIT_FAILURE;
  }
  else
  {
    vcl_cout << "[ OK ]\n";
  }


  // d.2) save trimmed xgraph 1
  vcl_cout << "\nSaving trimmed version of xgraph1: " << xgraph1_trimmed_file << "...";
  save_xgraph_process.parameters()->set_value("-xmlfile", bpro1_filepath(xgraph1_trimmed_file));
  save_xgraph_process.clear_input();
  save_xgraph_process.clear_output();
  save_xgraph_process.add_input(xgraph1_trimmed_storage);
  if (!save_xgraph_process.execute())
  {
    vcl_cout << "[ Failed ]\n";
    EXIT_FAILURE;
  }
  else
  {
    vcl_cout << "[ OK ]\n";
  }


  // d.3) save trimmed xgraph 2
  vcl_cout << "\nSaving trimmed version of xgraph2: " << xgraph1_trimmed_file << "...";
  save_xgraph_process.parameters()->set_value("-xmlfile", bpro1_filepath(xgraph2_trimmed_file));
  save_xgraph_process.clear_input();
  save_xgraph_process.clear_output();
  save_xgraph_process.add_input(xgraph2_trimmed_storage);
  if (!save_xgraph_process.execute())
  {
    vcl_cout << "[ Failed ]\n";
    EXIT_FAILURE;
  }
  else
  {
    vcl_cout << "[ OK ]\n";
  }

  return EXIT_SUCCESS;
}

