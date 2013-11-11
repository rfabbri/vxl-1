// This is rec/dborl/algo/mpi/symseg/dborl_sks_match_shape_processor.cxx
//:
// \file

#include "dborl_sks_match_shape_processor.h"
#include <bxml/bxml_read.h>
#include <bxml/bxml_write.h>
#include <bxml/bxml_find.h>
//
#include <dborl/algo/dborl_utilities.h>
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vil/vil_load.h>
#include <vil/vil_convert.h>
//
#include <dbsksp/xio/dbsksp_xio_shock_graph.h>
#include <dbsksp/dbsksp_shock_graph.h>
#include <dbsks/dbsks_dp_match.h>
#include <dbsks/dbsks_dp_match_sptr.h>

#include <bpro1/bpro1_process_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <dbsks/pro/dbsks_shapematch_storage.h>
#include <dbsksp/pro/dbsksp_shock_storage.h>
#include <dbsks/pro/dbsks_compute_arc_cost_process.h>
#include <dbsks/pro/dbsks_detect_shape_process.h>
#include <dbsks/pro/dbsks_local_match_process.h>
#include <dbsks/pro/dbsks_write_shapematch_to_ps_process.h>
#include <dbsks/dbsks_local_match.h>
#include <bpro1/bpro1_parameters.h>
#include <vul/vul_temp_filename.h>
#include <vul/vul_sprintf.h>




vcl_string dborl_sks_temp_data_folder(const vcl_string& processor_name)
{
  if (processor_name == "compute-1-0.local" ||
    processor_name == "compute-1-1.local" ||
    processor_name == "compute-1-2.local" ||
    processor_name == "compute-1-3.local" ||
    processor_name == "compute-1-4.local" ||
    processor_name == "compute-1-4.local" ||
    processor_name == "compute-1-5.local" ||
    processor_name == "compute-1-6.local" ||
    processor_name == "compute-1-7.local" ||
    processor_name == "compute-1-8.local" ||
    processor_name == "compute-1-9.local" ||
    processor_name == "compute-1-10.local" ||
    processor_name == "compute-1-11.local" ||
    processor_name == "compute-1-12.local" ||
    processor_name == "compute-1-13.local" ||
    processor_name == "compute-1-14.local" ||
    processor_name == "compute-0-0.local" ||
    processor_name == "compute-0-1.local" ||
    processor_name == "compute-0-2.local" ||
    processor_name == "tile-0-0.local" ||
    processor_name == "tile-1-0.local"
  )
  {
    return "/state/partition1/temp/";
  }
  else if (processor_name == "compute-1-15.local")
  {
    return "/export/temp/";
  }
  else
  {
    return vul_file::dirname(vul_temp_filename()) + "/";
  }
}



// -----------------------------------------------------------------------------
//: this method is run on each processor after lead processor broadcasts its command
//  line arguments to all the processors since only on the lead processor is passed the command line arguments by mpirun
bool dborl_sks_match_shape_processor::
parse_command_line(vcl_vector<vcl_string>& argv)
{
  vul_arg_info_list arg_list;
  vul_arg<vcl_string> input_xml_file(arg_list, "-inp", 
    "the input file in xml format that sets parameters of the algorithm","");
  vul_arg<bool> print_usage_only(arg_list, "-usage", "print usage info and exit", false);
  vul_arg<bool> print_help(arg_list, "-help", "print usage info and exit", false);
  vul_arg<bool> print_def_xml(arg_list, "-print-def-xml", 
    "print input.xml file with default params and exit",false);  
  vul_arg_include(arg_list);
  //vul_arg_parse(argc, argv, false); // warn_about_unrecognized_arguments = false

  for (unsigned i = 1; i < argv.size(); ++i) 
  {
    vcl_string arg = argv[i];
    if (arg == vcl_string ("-inp")) 
    { 
      input_xml_file.value_ = argv[++i]; 
    } 
    else if (arg == vcl_string ("-usage")) 
    { 
      print_usage_only.value_ = true; 
    }
    else if (arg == vcl_string ("-help")) 
    { 
      print_help.value_ = true; 
    }
    else if (arg == vcl_string ("-print-def-xml")) 
    { 
      print_def_xml.value_ = true; 
    }
  }

  if (print_help() || print_usage_only()) 
  {
    arg_list.display_help();
    return false; // --> to exit
  }

  if (print_def_xml()) 
  {
    vcl_string input = "input.xml";
    dborl_sks_match_shape_processor::print_default_file(input.c_str());
    return false;  // --> to exit
  }

  if (input_xml_file().compare("") == 0) 
  {
    arg_list.display_help();
    vcl_cout << "XML filename is empty \n";
    return false;  // --> to exit
  }

  this->param_file_ = input_xml_file();
  return true;  
}



// -----------------------------------------------------------------------------
//: this method is run on each processor
bool dborl_sks_match_shape_processor::
parse(const char* param_file)
{
  this->param_doc_ = bxml_read(param_file);
  
  //
  if (!param_doc_.root_element())
  {
    vcl_cout << "ERROR reading parameter XML file: "
      << param_file << "\n";
    vcl_cout << "root = " << param_doc_.root_element() << "\n";
    return false;
  }
  
  if (param_doc_.root_element()->type() != bxml_data::ELEMENT) 
  {
    vcl_cout << "params root is not ELEMENT\n";
    return false;
  }

  return this->params_.parse_from_data(param_doc_.root_element());
}



// -----------------------------------------------------------------------------
//: this method prints an xml input file setting all the parameters to defaults
//  run the algorithm to generate this file, then modify it  
void dborl_sks_match_shape_processor::
print_default_file(const char* def_file)
{
  bxml_document doc;
  bxml_element * root = new bxml_element("dborl_sks_match_shape_processor");
  doc.set_root_element(root);

  bxml_element * data1 = params_.create_default_document_data();
  root->append_data(data1);
  root->append_text("\n");
  
  bxml_write(vcl_string(def_file), doc);
}


// -----------------------------------------------------------------------------
//: this method is run on each processor
bool dborl_sks_match_shape_processor::initialize(
  vcl_vector<dborl_sks_match_shape_processor_input>& t)
{
  t_.mark();

  dbsksp_shock_graph_sptr shock_graph = 0;
  if ( !x_read(params_.model_file_, shock_graph) )
  {
    vcl_cerr << "Loading shock graph XML file: " << params_.model_file_ << "  failed.\n";
    return false;
  }
  
  vcl_cout << "Loading shock graph XML file ... Done.\n";
  shock_graph->compute_all_dependent_params();

  vcl_vector<vcl_string> image_names;
  if (!parse_strings_from_file(params_.file_list_, image_names)) 
  {
    vcl_cout << "Unable to open list file! " << params_.file_list_ << "!\n";
    return false;
  }

  bxml_document doc;
  bxml_element * root = new bxml_element("dborl_sks_match_shape_processor");
  doc.set_root_element(root);
  bxml_element * data1 = params_.create_document_data();
  root->append_data(data1);
  root->append_text("\n");
  bxml_write(params_.out_dir_ + this->processor_name() + "_using_params.xml", doc);
   
  for (unsigned int i = 0; i<image_names.size(); i++) 
  {
    dborl_sks_match_shape_processor_input inp(shock_graph, image_names[i]);
    t.push_back(inp);
  }

  return true;
}



// -----------------------------------------------------------------------------
//: this method is run in a distributed mode on each processor on the cluster
bool dborl_sks_match_shape_processor::
process(dborl_sks_match_shape_processor_input i, float& f)
{
  // Think of this as converting the process to an executable

  vcl_cout << "processing: " << i.image_name_ << " ";

  // Process I : compute_circ_arc_cost

  
  bpro1_process_sptr arc_cost_process = new dbsks_compute_arc_cost_process();
  arc_cost_process->clear_input();
  arc_cost_process->clear_output();

  vcl_cout << "Process : " << arc_cost_process->name() << "\n";

  // >> Input storage //////////////////////////////////////////////////////////

  // input 1: "image"
  
  // load the edgemap and put into a storage
  vcl_string edgemap_path = this->params_.file_dir_ + 
    i.image_name_ + 
    this->params_.extension_;

  if (!vul_file::exists(edgemap_path)) 
  {
    vcl_cout << "ERROR: image: " << edgemap_path << " does not exist!!\n";
    return false;
  }
  vil_image_resource_sptr edgemap_resource = vil_load_image_resource(edgemap_path.c_str());

  if (!edgemap_resource)
  {
    vcl_cout << "ERROR: couldn't load image: " << edgemap_path << "\n";
    return false;
  }

  vcl_cout << "Loaded edgemap. " 
    << " ni: " << edgemap_resource->ni() 
    << " nj: " << edgemap_resource->nj() << "\n";

  vidpro1_image_storage_sptr edgemap_storage = vidpro1_image_storage_new();
  edgemap_storage->set_image(edgemap_resource);

  // save the storage as input of the process
  arc_cost_process->add_input(edgemap_storage, 0);

  
  // >> Process parameters /////////////////////////////////////////////////////
  // load parameters from a file
  arc_cost_process->set_parameters(this->params_.dbsks_compute_arc_cost_params_);

  // >> Execute the process ////////////////////////////////////////////////////
  arc_cost_process->execute();

  // >> Clear output storage ///////////////////////////////////////////////////
  
  // first we save the output storage to some variables
  // shapematch storage
  dbsks_shapematch_storage_sptr shapematch_storage;
  shapematch_storage.vertical_cast(arc_cost_process->get_output(0)[0]);

  //// DT of edge map
  //vidpro1_image_storage_sptr dt_image_storage;
  //dt_image_storage.vertical_cast(arc_cost_process->get_output(0)[1]);
  
  arc_cost_process->clear_input();
  arc_cost_process->clear_output();

  //////////////////////////////////////////////////////////////////////////////
  // Process II: detect shape
  bpro1_process_sptr detect_shape_process = new dbsks_detect_shape_process();
  detect_shape_process->clear_input();
  detect_shape_process->clear_output();

  vcl_cout << "Process : " << detect_shape_process->name() << "\n";

  // >> Input storages /////////////////////////////////////////////////////////
  // ( "dbsks_shapematch" );
  detect_shape_process->add_input(shapematch_storage, 0);
  
  // ( "dbsksp_shock" );
  dbsksp_shock_storage_sptr model_shock_storage = dbsksp_shock_storage_new();
  model_shock_storage->set_shock_graph(i.shock_graph_);
  detect_shape_process->add_input(model_shock_storage);

  // ( "image" );
  detect_shape_process->add_input(edgemap_storage);

  // >> Process parameters /////////////////////////////////////////////////////
  // load parameters from file
  // create a new instances of the parameters
  bpro1_parameters_sptr params = new bpro1_parameters(this->params_.dbsks_detect_shape_params_);
  detect_shape_process->set_parameters(params);

  // modified the temp prefix to include processor name
  vcl_string tmp_prefix;
  detect_shape_process->parameters()->get_value("tmp_prefix", tmp_prefix);
  detect_shape_process->parameters()->set_value("tmp_prefix", 
    tmp_prefix + "_" +  this->processor_name() + "_" + i.image_name_);

  vcl_cout << "tmp_prefix = " << tmp_prefix << "\n";

  vcl_string temp_data_folder = dborl_sks_temp_data_folder(this->processor_name());
  vcl_cout << "temp_data_folder = " << temp_data_folder << "\n";

  detect_shape_process->parameters()->set_value("temp_data_folder", temp_data_folder);

  // >> Execute the process ////////////////////////////////////////////////////
  detect_shape_process->execute();

  // >> Clear output storage ///////////////////////////////////////////////////
  // first we save the output storage to some variables
  
  //// shapematch storage
  //dbsksp_shock_storage_sptr output_shock_storage;
  //output_shock_storage.vertical_cast(detect_shape_process->get_output(0)[0]);
  
  detect_shape_process->clear_input();
  detect_shape_process->clear_output();






  //////////////////////////////////////////////////////////////////////////////
  // Process II: local_match

  bool use_sub_optimal = false;
  this->params_.dbsks_local_match_params_->get_value("use_sub_optimal", use_sub_optimal);

  if (!use_sub_optimal)
  {
    bpro1_process_sptr local_match_process = new dbsks_local_match_process();
    local_match_process->clear_input();
    local_match_process->clear_output();

    vcl_cout << "Process : " << local_match_process->name() << "\n";

    // >> Input storages /////////////////////////////////////////////////////////
    // ( "dbsks_shapematch" );
    local_match_process->add_input(shapematch_storage, 0);
    
    // >> Process parameters /////////////////////////////////////////////////////
    // load parameters from file
    local_match_process->set_parameters(this->params_.dbsks_local_match_params_);

    // >> Execute the process ////////////////////////////////////////////////////
    local_match_process->execute();

    // >> Clear output storage ///////////////////////////////////////////////////
    
    //// first we save the output storage to some variables
    //// shock storage
    //dbsksp_shock_storage_sptr output_shock_storage;
    //output_shock_storage.vertical_cast(detect_shape_process->get_output(0)[0]);
    
    local_match_process->clear_input();
    local_match_process->clear_output();

    vcl_cout << "Done with " << local_match_process->name() << ".\n";

    /////////////////////////////////////////////////////////////////////////////
    // Save final results to files

    // a. Write out summary file

    vcl_string shapematch_file = params_.out_dir_ + i.image_name_ + "_shapematch_out.txt";
    vcl_cout << "Shapematch_file = " << shapematch_file << "\n";
    
    // open file
    vcl_ofstream os(shapematch_file.c_str());
    if (!os.good())
    {
      vcl_cout << "ERROR opening file. Quit now.\n";
      return false;
    }

    // write common info
    os << "image_name " << i.image_name_ << "\n"
      << "model_file " << vul_file::strip_directory(this->params_.model_file_) << "\n";

    // write summary from dp_engine
    vcl_cout << "Print summary from dp_engine.\n";
    shapematch_storage->dp_engine()->print_summary(os);

    // write summary from local match
    vcl_cout << "Print summary from lm_engine.\n";
    shapematch_storage->lm_engine()->print_summary(os);

    // close file
    os.close();
    vcl_cout << "Done writing out summary.\n";

    // 2. write out the segmentation to ps file

    vcl_string shapematch_ps_file = params_.out_dir_ + i.image_name_ + "_shapematch_out.ps";
    vcl_cout << "Shapematch_PS_file = " << shapematch_ps_file << "\n";

    vcl_string image_file = this->params_.file_dir_ + i.image_name_ + ".jpg";
    dbsksp_shock_graph_sptr graph = shapematch_storage->lm_engine()->graph();

    dbsks_write_shapematch_to_ps_process::save_ps_file(shapematch_ps_file, 
      shapematch_file, image_file, graph);
  }
  else
  {
    // RUN this 4 times for 4 sub-optimal solution
    for (int sub_optimal_index =0; sub_optimal_index < 4; ++sub_optimal_index)
    {
      bpro1_process_sptr local_match_process = new dbsks_local_match_process();
      local_match_process->clear_input();
      local_match_process->clear_output();

      vcl_cout << "Process : " << local_match_process->name() << "\n";

      // >> Input storages /////////////////////////////////////////////////////////
      // ( "dbsks_shapematch" );
      local_match_process->add_input(shapematch_storage, 0);
      
      // >> Process parameters /////////////////////////////////////////////////////
      // load parameters from file
      local_match_process->set_parameters(this->params_.dbsks_local_match_params_);

      // change the index of the sub-optimal solution
      local_match_process->parameters()->set_value("sub_optimal_index", sub_optimal_index);

      // make sure the process does not reuse previous results
      local_match_process->parameters()->set_value("force_start_from_DP_match", true);


      // >> Execute the process ////////////////////////////////////////////////////
      local_match_process->execute();

      // >> Clear output storage ///////////////////////////////////////////////////
      
      local_match_process->clear_input();
      local_match_process->clear_output();

      vcl_cout << "Done with " << local_match_process->name() << ".\n";

      /////////////////////////////////////////////////////////////////////////////
      // Save final results to files

      // a. Write out summary file

      vcl_string shapematch_file = vul_sprintf("%s%s_shapematch_out_%d.txt",
        params_.out_dir_.c_str(), i.image_name_.c_str(), sub_optimal_index);
      vcl_cout << "Shapematch_file = " << shapematch_file << "\n";
      
      // open file
      vcl_ofstream os(shapematch_file.c_str());
      if (!os.good())
      {
        vcl_cout << "ERROR opening file. Quit now.\n";
        return false;
      }

      // write common info
      os << "image_name " << i.image_name_ << "\n"
        << "model_file " << vul_file::strip_directory(this->params_.model_file_) << "\n"
        << "sub_optimal_index " << sub_optimal_index << "\n";

      // write summary from dp_engine
      vcl_cout << "Print summary from dp_engine.\n";
      shapematch_storage->dp_engine()->print_summary(os);

      // write summary from local match
      vcl_cout << "Print summary from lm_engine.\n";
      shapematch_storage->lm_engine()->print_summary(os);

      // close file
      os.close();
      vcl_cout << "Done writing out summary.\n";

      // 2. write out the segmentation to ps file
      

      vcl_string shapematch_ps_file = 
        vul_file::strip_extension(shapematch_file) + ".ps";
      vcl_cout << "Shapematch_PS_file = " << shapematch_ps_file << "\n";

      vcl_string image_file = this->params_.file_dir_ + i.image_name_ + ".jpg";
      dbsksp_shock_graph_sptr graph = shapematch_storage->lm_engine()->graph();

      dbsks_write_shapematch_to_ps_process::save_ps_file(shapematch_ps_file, 
        shapematch_file, image_file, graph);
    }   

}




  




  // result to send back to root node
  // no way to pass a boolean since there is no MPI_BOOL, pass char just to be save
  f = 0.0f;  
  return true;
}


// -----------------------------------------------------------------------------
//: 
void dborl_sks_match_shape_processor::
print_time()
{
  vcl_cout << "\t\t\t total time: " << (t_.real()/1000.0f) << " secs.\n";
  vcl_cout << "\t\t\t total time: " << ((t_.real()/1000.0f)/60.0f) << " mins.\n";
}


// -----------------------------------------------------------------------------
//: this method is run on the lead processor once after results are collected from each processor
bool dborl_sks_match_shape_processor::
finalize(vcl_vector<float>& results)
{
  //vcl_cout << "results size: " << results.size() << " not doing anything with them\n";
  //for (unsigned i = 0; i < results.size(); i++) {
  //  vcl_cout << results[i];
  //}
  //vcl_cout << vcl_endl;
  return true;
}

#ifdef MPI_CPP_BINDING
MPI::Datatype dborl_sks_match_shape_processor::create_datatype_for_R()
{
  return MPI::FLOAT;
}
#else
MPI_Datatype dborl_sks_match_shape_processor::create_datatype_for_R()
{
  return MPI_FLOAT;
}
#endif


