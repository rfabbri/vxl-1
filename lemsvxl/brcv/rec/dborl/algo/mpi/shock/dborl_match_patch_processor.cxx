//:
// \file
// \brief
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/30/07

//
//

#include "dborl_match_patch_processor.h"
#include <bxml/bxml_read.h>
#include <bxml/bxml_write.h>
#include <bxml/bxml_find.h>
#include <dbskr/dbskr_tree_edit.h>
#include <dbskr/algo/dbskr_rec_algs.h>
#include <dbskr/algo/dbskr_shock_patch_match.h>
#include <dbskr/pro/dbskr_shock_patch_storage.h>
#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>
#include <dborl/algo/dborl_utilities.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_algorithm.h>
#include <vul/vul_arg.h>
#include <vul/vul_file.h>

//: this method is run on each processor after lead processor broadcasts its command
//  line arguments to all the processors since only on the lead processor is passed the command line arguments by mpirun
bool dborl_match_patch_processor::parse_command_line(vcl_vector<vcl_string>& argv)
{
  vul_arg_info_list arg_list;
  vul_arg<vcl_string> input_xml_file(arg_list,"-inp","the input file in xml format that sets parameters of the algorithm","");
  vul_arg<bool> print_usage_only(arg_list,"-usage", "print usage info and exit",false);
  vul_arg<bool> print_help(arg_list,"-help", "print usage info and exit",false);
  vul_arg<bool> print_def_xml(arg_list,"-print-def-xml", "print input.xml file with default params and exit",false);  
  vul_arg_include(arg_list);

  for (unsigned i = 1; i < argv.size(); i++) {
        vcl_string arg = argv[i];
        if (arg == vcl_string ("-inp")) { 
          input_xml_file.value_ = argv[++i]; 
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
    vcl_string input = "input.xml";
    dborl_match_patch_processor::print_default_file(input.c_str());
    return false;  // --> to exit
  }

  if (input_xml_file().compare("") == 0) {
    arg_list.display_help();
    return false;  // --> to exit
  }

  param_file_ = input_xml_file();
  return true;
}

//: this method is run on each processor
bool dborl_match_patch_processor::parse(const char* param_file)
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
  bxml_element * root = new bxml_element("dborl_match_patch_processor");
  doc.set_root_element(root);
  root->append_data(data1);
  root->append_text("\n");
  bxml_write("deneme.xml", doc);
#endif 

  return true;
}

//: this method prints an xml input file setting all the parameters to defaults
//  run the algorithm to generate this file, then modify it  
void dborl_match_patch_processor::print_default_file(const char* def_file)
{
  bxml_document doc;
  bxml_element * root = new bxml_element("dborl_match_patch_processor");
  doc.set_root_element(root);

  bxml_element * data1 = params_.create_default_document_data();
  root->append_data(data1);
  root->append_text("\n");
  
  bxml_write(vcl_string(def_file), doc);
}

//: this method is run on each processor
bool dborl_match_patch_processor::initialize(vcl_vector<dborl_match_patch_processor_input>& t)
{

  if (!parse_strings_from_file(params_.db_list_, database)) {
    vcl_cout << "Unable to open database file! " << params_.db_list_ << "!\n";
    return false;
  }

  if (!parse_strings_from_file(params_.train_list_, training_set)) {
    vcl_cout << "Unable to open training file: " << params_.train_list_ << "!\n";
    return false;
  }

  D1 = database.size(); 
  D2 = training_set.size();
  vcl_cout << "db size: " << D1 << " train size: " << D2 << vcl_endl;
  vcl_random_shuffle(database.begin(), database.end());
  vcl_random_shuffle(training_set.begin(), training_set.end());
  
  dbsk2d_xshock_graph_fileio loader;
  vcl_vector<dbskr_shock_patch_storage_sptr> database_str, training_str;

  vcl_string storage_end = "patch_strg.bin";
  for (unsigned int i = 0; i<D1; i++) {
    vcl_string st_file;
    if (params_.orl_format_folders_)
      st_file = params_.db_dir_ + database[i] + "/" + database[i] + params_.query_patch_params_.output_file_postfix_ + "/" + database[i] + params_.query_patch_params_.output_file_postfix_ + "-" + storage_end;
    else
      st_file = params_.db_dir_ + database[i] + params_.query_patch_params_.output_file_postfix_ + "-" + storage_end;

    if (!vul_file::exists(st_file)) {
      vcl_cout << "st file: " << st_file << " does not exist!!!\n";
      return false;
    }
    dbskr_shock_patch_storage_sptr st = dbskr_shock_patch_storage_new();
    vsl_b_ifstream ifs(st_file.c_str());
    st->b_read(ifs);
    ifs.close();

    //: load the shock graphs --> assumes each shock graph for each patch is saved in the same folder
    vcl_cout << st->size() << " patches in query storage, reading shocks..\n";

    if (!st->size()) {
      vcl_cout << "problems in query st: " << st_file << vcl_endl;
    }

    //: load esfs for each patch
    for (unsigned iii = 0; iii < st->size(); iii++) {
      dbskr_shock_patch_sptr sp = st->get_patch(iii);
      vcl_string patch_esf_name = st_file.substr(0, st_file.length()-storage_end.size());
      vcl_ostringstream oss;
      oss << sp->id();
      patch_esf_name = patch_esf_name+oss.str()+".esf";
      dbsk2d_shock_graph_sptr sg = loader.load_xshock_graph(patch_esf_name);
      if (!sg || !sg->number_of_vertices() || !sg->number_of_edges())
      {
        vcl_cout << database[i] << " shock graph has ZERO nodes or edges!!!!!!!!!\n";
        return false;
      }
      sp->set_shock_graph(sg);
      sp->set_tree_parameters(params_.shock_match_params_.elastic_splice_cost_, 
                              params_.shock_match_params_.circular_ends_,
                              params_.shock_match_params_.combined_edit_, 
                              params_.shock_match_params_.scurve_sample_ds_, 
                              params_.shock_match_params_.scurve_interpolate_ds_); // so that prepare_tree() and tree() methods of patch have the needed params
    }
    
    database_str.push_back(st);
  }

  for (unsigned int i = 0; i<D2; i++) {
    vcl_string st_file;
    if (params_.orl_format_folders_)
      st_file = params_.train_dir_ + training_set[i] + "/" + training_set[i] + params_.model_patch_params_.output_file_postfix_ + "-" + storage_end;
    else
      st_file = params_.train_dir_ + training_set[i] + params_.model_patch_params_.output_file_postfix_ + "-" + storage_end;

    if (!vul_file::exists(st_file)) {
      vcl_cout << "st file: " << st_file << " does not exist!!!\n";
      return false;
    }
    
    //vcl_cout << "model storage file: " << st_file << vcl_endl;
    dbskr_shock_patch_storage_sptr st = dbskr_shock_patch_storage_new();
    vsl_b_ifstream ifs(st_file.c_str());
    st->b_read(ifs);
    ifs.close();

    //: load the shock graphs --> assumes each shock graph for each patch is saved in the same folder
    vcl_cout << st->size() << " patches in model storage, reading shocks..\n";
    
    if (!st->size()) {
      vcl_cout << "problems in model st: " << st_file << vcl_endl;
    }

    //: load esfs for each patch
    for (unsigned iii = 0; iii < st->size(); iii++) {
      dbskr_shock_patch_sptr sp = st->get_patch(iii);
      vcl_string patch_esf_name = st_file.substr(0, st_file.length()-storage_end.size());
      vcl_ostringstream oss;
      oss << sp->id();
      patch_esf_name = patch_esf_name+oss.str()+".esf";
      dbsk2d_shock_graph_sptr sg = loader.load_xshock_graph(patch_esf_name);
      if (!sg || !sg->number_of_vertices() || !sg->number_of_edges())
      {
        vcl_cout << database[i] << " shock graph has ZERO nodes or edges!!!!!!!!!\n";
        return false;
      }
      
      sp->set_shock_graph(sg);
      
      sp->set_tree_parameters(params_.shock_match_params_.elastic_splice_cost_, 
                              params_.shock_match_params_.circular_ends_,
                              params_.shock_match_params_.combined_edit_, 
                              params_.shock_match_params_.scurve_sample_ds_, 
                              params_.shock_match_params_.scurve_interpolate_ds_);  // so that prepare_tree() and tree() methods of patch have the needed params
    }
    
    training_str.push_back(st);
  }

  //: initialize the input vector for each pair
  for (unsigned int j = 0; j<D2; j++) {
    for (unsigned int i = 0; i<D1; i++) { 
      dborl_match_patch_processor_input inp(training_str[j], 
                                            database_str[i],
                                            training_set[j] + params_.model_patch_params_.output_file_postfix_, 
                                            database[i] + params_.query_patch_params_.output_file_postfix_);
      t.push_back(inp);
    }
  }

  return true;
}

//: this method is run in a distributed mode on each processor on the cluster
bool dborl_match_patch_processor::process(dborl_match_patch_processor_input inp, char& f)
{
  vcl_vector<dbskr_shock_patch_sptr>& pv1 = inp.st1->get_patches();
  vcl_vector<dbskr_shock_patch_sptr>& pv2 = inp.st2->get_patches();

  dbskr_shock_patch_match_sptr match = new dbskr_shock_patch_match();
  patch_cor_map_type& map = match->get_map();

  match->edit_params_ = params_.shock_match_params_;
    
  //vcl_cout << pv1.size() << " patches: ";
  for (unsigned i = 0; i < pv1.size(); i++) {
    vcl_cout << i << " ";
    find_patch_correspondences(pv1[i], pv2, map, params_.shock_match_params_);
    pv1[i]->kill_tree();
  }
  //vcl_cout << "\n match map size: " << map.size() << " map[pv1[0]->id()] size: " << map[pv1[0]->id()]->size() << vcl_endl;
  
  if (params_.save_matches_) {
    vcl_string out_name = params_.match_folder_ + inp.name1 + "-" + inp.name2 + params_.shock_match_params_.output_file_postfix_ + ".bin";
    vsl_b_ofstream bfs(out_name.c_str());
    match->b_write(bfs);
    bfs.close();
  }

  f = 't';
  return true;
}

void dborl_match_patch_processor::print_time()
{
  vcl_cout << "\t\t\t total time: " << (t_.real()/1000.0f) << " secs.\n";
  vcl_cout << "\t\t\t total time: " << ((t_.real()/1000.0f)/60.0f) << " mins.\n";
}

//: this method is run on the lead processor once after results are collected from each processor
bool dborl_match_patch_processor::finalize(vcl_vector<char>& results)
{
  return true;
}

#ifdef MPI_CPP_BINDING
MPI::Datatype dborl_match_patch_processor::create_datatype_for_R()
{
  return MPI::CHAR;
}
#else
MPI_Datatype dborl_match_patch_processor::create_datatype_for_R()
{
  return MPI_CHAR;
}
#endif


