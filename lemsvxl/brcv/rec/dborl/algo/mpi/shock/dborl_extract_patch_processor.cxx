//:
// \file
// \brief
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/30/07

//
//

#include "dborl_extract_patch_processor.h"
#include <bxml/bxml_read.h>
#include <bxml/bxml_write.h>
#include <bxml/bxml_find.h>
#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>
#include <dbskr/pro/dbskr_shock_patch_storage_sptr.h>
#include <dbskr/algo/dbskr_shock_patch_model_selector.h>
#include <dbskr/algo/dbskr_shock_patch_selector.h>
#include <dbskr/pro/dbskr_shock_patch_storage.h>
#include <dbskr/dbskr_shock_patch.h>
#include <dbskr/dbskr_tree.h>
#include <dborl/algo/dborl_utilities.h>
#include <dborl/dborl_index.h>
#include <dborl/dborl_index_node.h>
#include <dborl/algo/dborl_index_parser.h>
#include <dborl/dborl_index.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vul/vul_arg.h>
#include <vul/vul_file.h>

//: this method is run on each processor after lead processor broadcasts its command
//  line arguments to all the processors since only on the lead processor is passed the command line arguments by mpirun
bool dborl_extract_patch_processor::parse_command_line(vcl_vector<vcl_string>& argv)
{
  vul_arg_info_list arg_list;
  vul_arg<vcl_string> input_xml_file(arg_list,"-inp","the input file in xml format that sets parameters of the algorithm","");
  vul_arg<vcl_string> input_index_file(arg_list,"-ind","the flat index file of dataset, should contain full paths of objects","");
  vul_arg<bool> print_usage_only(arg_list,"-usage", "print usage info and exit",false);
  vul_arg<bool> print_help(arg_list,"-help", "print usage info and exit",false);
  vul_arg<bool> print_def_xml(arg_list,"-print-def-xml", "print input.xml file with default params and exit",false);  
  vul_arg_include(arg_list);
  //vul_arg_parse(argc, argv, false); // warn_about_unrecognized_arguments = false

  for (unsigned i = 1; i < argv.size(); i++) {
        vcl_string arg = argv[i];
        if (arg == vcl_string ("-inp")) { 
          input_xml_file.value_ = argv[++i]; 
        } else if (arg == vcl_string ("-ind")) { 
          input_index_file.value_ = argv[++i]; 
        }
        else if (arg == vcl_string ("-usage")) { print_usage_only.value_ = true; 
        }
        else if (arg == vcl_string ("-help")) { print_help.value_ = true; 
        }
        else if (arg == vcl_string ("-print-def-xml")) { 
          print_def_xml.value_ = true; 
        }
      }

  if (print_help() || print_usage_only()) {
    arg_list.display_help();
    return false; // --> to exit
  }

  if (print_def_xml()) {
    vcl_string input = "input_defaults.xml";
    dborl_extract_patch_processor::print_default_file(input.c_str());
    return false;  // --> to exit
  }

  if (input_xml_file().compare("") == 0 || input_index_file().compare("") == 0) {
    arg_list.display_help();
    return false;  // --> to exit
  }

  param_file_ = input_xml_file();
  index_file_ = input_index_file();
  return parse_index(index_file_);  // parse any extra file that needs to be parsed in this method
                                    // parse() is called by dborl_cluster but not these others
}

//: this method is run on each processor
bool dborl_extract_patch_processor::parse_index(vcl_string index_file)
{
  dborl_index_parser parser;
  parser.clear();

  vcl_FILE *xmlFile = vcl_fopen(index_file.c_str(), "r");
  if (xmlFile == NULL){
    vcl_cout << index_file << "-- error on opening" << vcl_endl;
    return false;
  }

  if (!parser.parseFile(xmlFile)) {
     vcl_cout << XML_ErrorString(parser.XML_GetErrorCode()) << " at line " <<
        parser.XML_GetCurrentLineNumber() << vcl_endl;
     return 0;
   }

  fclose(xmlFile);
  ind_ = parser.get_index();
  
  if (!ind_)
    return false;
  
  return true;
}

//: this method is run on each processor
bool dborl_extract_patch_processor::parse(const char* param_file)
{
  param_doc_ = bxml_read(param_file);
  if (!param_doc_.root_element())
    return false;
  
  if (param_doc_.root_element()->type() != bxml_data::ELEMENT) {
    vcl_cout << "params root is not ELEMENT\n";
    return false;
  }

  return params_.parse_from_data(param_doc_.root_element());

#if 0
  bxml_document doc;
  bxml_element * root = new bxml_element("dborl_extract_patch_processor");
  doc.set_root_element(root);
  root->append_data(data1);
  root->append_text("\n");
  bxml_write("deneme.xml", doc);
#endif 
}

//: this method prints an xml input file setting all the parameters to defaults
//  run the algorithm to generate this file, then modify it  
void dborl_extract_patch_processor::print_default_file(const char* def_file)
{
  bxml_document doc;
  bxml_element * root = new bxml_element("dborl_extract_patch_processor");
  doc.set_root_element(root);

  bxml_element * data1 = params_.create_default_document_data();
  root->append_data(data1);
  root->append_text("\n");
  
  bxml_write(vcl_string(def_file), doc);
}

//: this method is run on each processor
bool dborl_extract_patch_processor::initialize(vcl_vector<dborl_extract_patch_processor_input>& t)
{
  if (!ind_) {
    vcl_cout << "Index file is not parsed! Exiting!\n";
    return false;
  }

  t_.mark();

  dborl_index_node_sptr root = ind_->root_->cast_to_index_node();

  dbsk2d_xshock_graph_fileio loader;

  for (unsigned int i = 0; i<root->names().size(); i++) {
    vcl_string esf_file = root->paths()[i] + root->names()[i] + ".esf";
    dbsk2d_shock_graph_sptr sg = loader.load_xshock_graph(esf_file.c_str());
    if (!sg || !sg->number_of_vertices()) {
      vcl_cout << "Problems in getting graph or zero vertices in:" << esf_file<< vcl_endl;
      return false;
    } //else
      //vcl_cout <<"loaded esf: " << esf_file <<vcl_endl;
    
    vcl_string output_name = root->paths()[i] + root->names()[i] + params_.output_file_postfix_ + "/" + root->names()[i] + params_.output_file_postfix_ + "-patch_strg.bin";
 
    if (!vul_file::exists(output_name.c_str())) {
      dborl_extract_patch_processor_input inp(sg, root->names()[i], root->paths()[i]);
      t.push_back(inp);
      //if (t.size() == total_processors_)
      //  return true;
    }
  }

  return true;
}

//: this method is run in a distributed mode on each processor on the cluster
bool dborl_extract_patch_processor::process(dborl_extract_patch_processor_input i, char& f)
{
  vcl_cout << "processing: " << i.name << " ";
  vcl_cout.flush();

  // create the output storage class
  dbskr_shock_patch_storage_sptr output = dbskr_shock_patch_storage_new();

  if (params_.extract_from_tree_) {
    if (params_.put_only_tree_) {
      
      //: use the trees dart count as an estimate of the max possible depth of this shock graph
      dbskr_tree_sptr dummy_tree = new dbskr_tree();
      dummy_tree->acquire(i.sg, false, false, false);  // options are meaningless

      dbskr_shock_patch_model_selector selector(i.sg);
      selector.extract(dummy_tree->size(), params_.circular_ends_);
      selector.prune_same_patches(dummy_tree->size());
      selector.add_to_storage(dummy_tree->size(), output);
      if (output->size() != 1)
        vcl_cout << "WARNING: trying to put ONLY tree but there are more than one patches in the storage!!!!!!!!!\n";

    } else {
      dbskr_shock_patch_model_selector selector(i.sg);
    
      for (int d = params_.start_depth_; d <= params_.end_depth_; d += params_.depth_interval_) {
        //vcl_cout << " depth: " << d << " ..";
        selector.extract(d, params_.circular_ends_);
        selector.prune_same_patches(d);
        //vcl_cout << " DONE! ";
      }
      selector.prune_same_patches_at_all_depths();

      for (int d = params_.start_depth_; d <= params_.end_depth_; d += params_.depth_interval_) {
        selector.add_to_storage(d, output);
      }
    }

    vcl_string output_name_prefix = i.path + i.name + params_.output_file_postfix_;
    vsl_b_ofstream bfs((output_name_prefix + "-patch_strg.bin").c_str());
    output->b_write(bfs);
    bfs.close();

    //vcl_cout << output->size() << " patches, saving shocks..";
    //: save esfs for each patch to load later
    for (unsigned i = 0; i < output->size(); i++) {
      dbskr_shock_patch_sptr sp = output->get_patch(i);
      vcl_ostringstream oss;
      oss << sp->id();
      vcl_string patch_esf_name = output_name_prefix + "-" + oss.str() + ".esf";
      dbsk2d_xshock_graph_fileio file_io;
      file_io.save_xshock_graph(sp->shock_graph(), patch_esf_name);
    }
    //vcl_cout << output->size() << " DONE!\n";

  } else {

    vcl_string image_file = i.path + i.name + "." + params_.image_ext_;
    vcl_string esf_file = i.path + i.name + ".esf";
    vcl_string boundary_file = i.path + i.name + "_boundary.bnd";
    vcl_string kept_dir_name = i.path + i.name + params_.output_file_postfix_ + "/kept/";
    vul_file::make_directory_path(kept_dir_name);
    vcl_string discarded_dir_name = i.path + i.name + params_.output_file_postfix_ + "/discarded/";
    vul_file::make_directory_path(discarded_dir_name);
    vcl_string output_name = i.path + i.name + params_.output_file_postfix_ + "/" + i.name + params_.output_file_postfix_ + "-patch_strg.bin";

    extract_subgraph_and_find_shock_patches(image_file, esf_file, boundary_file, kept_dir_name, discarded_dir_name, output_name,
      params_.contour_ratio_, params_.circular_ends_, params_.area_threshold_ratio_, params_.overlap_threshold_,
      params_.start_depth_, params_.end_depth_, params_.depth_interval_, params_.pruning_depth_, params_.sort_threshold_,
      params_.keep_pruned_, params_.save_images_, params_.save_discarded_images_);

  }

  f = 't';  // no way to pass a boolean since there is no MPI_BOOL, pass char just to be save
  return true;
}
void dborl_extract_patch_processor::print_time()
{
  vcl_cout << "\t\t\t total time: " << (t_.real()/1000.0f) << " secs.\n";
  vcl_cout << "\t\t\t total time: " << ((t_.real()/1000.0f)/60.0f) << " mins.\n";
}

//: this method is run on the lead processor once after results are collected from each processor
bool dborl_extract_patch_processor::finalize(vcl_vector<char>& results)
{
  //vcl_cout << "results size: " << results.size() << " not doing anything with them\n";
  //for (unsigned i = 0; i < results.size(); i++) {
  //  vcl_cout << results[i];
  //}
  //vcl_cout << vcl_endl;
  return true;
}

#ifdef MPI_CPP_BINDING
MPI::Datatype dborl_extract_patch_processor::create_datatype_for_R()
{
  return MPI::CHAR;
}
#else
MPI_Datatype dborl_extract_patch_processor::create_datatype_for_R()
{
  return MPI_CHAR;
}
#endif


