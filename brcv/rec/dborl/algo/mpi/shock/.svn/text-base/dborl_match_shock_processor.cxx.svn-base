//:
// \file
// \brief
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/30/07

//
//

#include "dborl_match_shock_processor.h"
#include <bxml/bxml_read.h>
#include <bxml/bxml_write.h>
#include <bxml/bxml_find.h>
#include <dbskr/dbskr_tree_edit.h>
#include <dbskr/dbskr_tree.h>
#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>
#include <dborl/algo/dborl_utilities.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_algorithm.h>
#include <vul/vul_arg.h>
#include <vul/vul_file.h>

//: this method is run on each processor after lead processor broadcasts its command
//  line arguments to all the processors since only on the lead processor is passed the command line arguments by mpirun
bool dborl_match_shock_processor::parse_command_line(vcl_vector<vcl_string>& argv)
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
    dborl_match_shock_processor::print_default_file(input.c_str());
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
bool dborl_match_shock_processor::parse(const char* param_file)
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
  bxml_element * root = new bxml_element("dborl_match_shock_processor");
  doc.set_root_element(root);
  root->append_data(data1);
  root->append_text("\n");
  bxml_write("deneme.xml", doc);
#endif 

  return true;
}

//: this method prints an xml input file setting all the parameters to defaults
//  run the algorithm to generate this file, then modify it  
void dborl_match_shock_processor::print_default_file(const char* def_file)
{
  bxml_document doc;
  bxml_element * root = new bxml_element("dborl_match_shock_processor");
  doc.set_root_element(root);

  bxml_element * data1 = params_.create_default_document_data();
  root->append_data(data1);
  root->append_text("\n");
  
  bxml_write(vcl_string(def_file), doc);
}

//: this method is run on each processor
bool dborl_match_shock_processor::initialize(vcl_vector<dborl_match_shock_processor_input>& t)
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
  //vcl_cout << "db size: " << D1 << " train size: " << D2 << vcl_endl;
  if (params_.db_train_same_ && D1 == D2 && params_.db_dir_.compare(params_.train_dir_) == 0) {
    vcl_cout << "in db_train_same ON mode - will initialize " << D1 * D2 << " matchings\n";
    //: shuffle the elements of these vectors to maximize the speed up with MPI process distribution
    //  some class trees are way larger than other classes and thus when they are passed as chunks to the same processor
    //  it delays the others
    vcl_random_shuffle(database.begin(), database.end());
    training_set.clear();  // make sure it is ordered the same as database
    training_set.insert(training_set.begin(), database.begin(), database.end());

  } else {
    vcl_cout << "in db_train_same OFF mode - will initialize " << D1 * D2 + D2 * D1 << " matchings\n";
    //: reset params_.db_train_same_ to make sure
    params_.db_train_same_ = false;
    vcl_random_shuffle(database.begin(), database.end());
    vcl_random_shuffle(training_set.begin(), training_set.end());
  }
  
  dbsk2d_xshock_graph_fileio loader;
  vcl_vector<dbskr_tree_sptr> database_tree, training_set_tree;

  for (unsigned int i = 0; i<D1; i++) {
    vcl_string esf_file;
    if (params_.orl_format_folders_)
      esf_file = params_.db_dir_ + database[i] + "/" + database[i] +".esf";
    else
      esf_file = params_.db_dir_ + database[i] + ".esf";

    dbsk2d_shock_graph_sptr sg = loader.load_xshock_graph(esf_file.c_str());
    if (!sg) {
      vcl_cout << "Problems in getting graphs! "<<database[i]<<vcl_endl;
      return false;
    } //else
      //vcl_cout <<"loaded esf: " << esf_file <<vcl_endl;
    
    dbskr_tree_sptr tree = new dbskr_tree(params_.edit_params_.scurve_sample_ds_, params_.edit_params_.scurve_interpolate_ds_);
    tree->acquire(sg, params_.edit_params_.elastic_splice_cost_, params_.edit_params_.circular_ends_, params_.edit_params_.combined_edit_);
    database_tree.push_back(tree);
  }

  for (unsigned int i = 0; i<D2; i++) {
    vcl_string esf_file;
    if (params_.orl_format_folders_)
      esf_file = params_.train_dir_ + training_set[i] + "/" + training_set[i] +".esf";
    else
      esf_file = params_.train_dir_ + training_set[i] + ".esf";

    dbsk2d_shock_graph_sptr sg = loader.load_xshock_graph(esf_file.c_str());
    if (!sg) {
      vcl_cout << "Problems in getting graphs! "<<database[i]<<vcl_endl;
      return false;
    } //else
      //vcl_cout <<"loaded esf: " << esf_file <<vcl_endl;

    dbskr_tree_sptr tree = new dbskr_tree(params_.edit_params_.scurve_sample_ds_, params_.edit_params_.scurve_interpolate_ds_);
    tree->acquire(sg, params_.edit_params_.elastic_splice_cost_, params_.edit_params_.circular_ends_, params_.edit_params_.combined_edit_);
    training_set_tree.push_back(tree);
  }

  //: initialize the input vector for each pair
  for (unsigned int i = 0; i<D1; i++)
    for (unsigned int j = 0; j<D2; j++) {
      dborl_match_shock_processor_input inp(database_tree[i], training_set_tree[j], database[i], training_set[j]);
      t.push_back(inp);
    }

  if (!params_.db_train_same_) {  // insert the inverted matrix of matchings 
    for (unsigned int j = 0; j<D2; j++) 
      for (unsigned int i = 0; i<D1; i++) {
        dborl_match_shock_processor_input inp(training_set_tree[j], database_tree[i], training_set[j], database[i]);
        t.push_back(inp);
      }
  }

  return true;
}

//: this method is run in a distributed mode on each processor on the cluster
bool dborl_match_shock_processor::process(dborl_match_shock_processor_input i, float& f)
{
  //vcl_cout << "processing: " << i.name1 << " tree n size: " << i.tree1->node_size() << " and " << i.name2 << " tree n size: " << i.tree2->node_size() << vcl_endl;
  //vcl_cout.flush();

  vcl_string out_name = params_.shgm_folder_ + i.name1 + "-" + i.name2 + params_.edit_params_.output_file_postfix_ + ".shgm";
  if (!vul_file::exists(out_name)) {

    dbskr_tree_edit edit(i.tree1, i.tree2, params_.edit_params_.circular_ends_, params_.edit_params_.localized_edit_);
    edit.save_path(true);    
    edit.set_curvematching_R(params_.edit_params_.curve_matching_R_);
    if (!edit.edit()) {
      vcl_cout << "Problems in editing trees\n";
      return false;
    }

    f = edit.final_cost();

    if (params_.save_shgms_) {
  //    vcl_string out_name = params_.shgm_folder_ + i.name1 + "-" + i.name2 + params_.edit_params_.output_file_postfix_ + ".shgm";
      edit.write_shgm(out_name);
    }

  } else {
    dbskr_sm_cor_sptr sm_cor = new dbskr_sm_cor();
    sm_cor->read_and_construct_from_shgm(out_name, false);
    //vcl_cout << out_name << " exists .. skipped\n";
    f = sm_cor->final_cost();
  }

  return true;
}

void dborl_match_shock_processor::print_time()
{
  vcl_cout << "\t\t\t total time: " << (t_.real()/1000.0f) << " secs.\n";
  vcl_cout << "\t\t\t total time: " << ((t_.real()/1000.0f)/60.0f) << " mins.\n";
}

//: this method is run on the lead processor once after results are collected from each processor
bool dborl_match_shock_processor::finalize(vcl_vector<float>& results)
{

  if (params_.db_train_same_) {
    if (results.size() != D1*D2) {
      vcl_cout << "result vectors size is: " << results.size() << " should have been: " << D1*D2 << vcl_endl;
      return false;
    } 
  } else {
    if (results.size() != (D1*D2 + D2*D1)) {
      vcl_cout << "result vectors size is: " << results.size() << " should have been: " << (D1*D2 + D2*D1) << vcl_endl;
      return false;
    }
  }

  vcl_vector<vcl_vector<float> > shock_database_cost1 ( D1,vcl_vector<float> ( D2 ));
  for(unsigned int i = 0; i<D1; i++){
    for(unsigned int j = 0; j<D2; j++){
      shock_database_cost1[i][j]= results[i*D2 + j];  // number of columns is D2
    }
  }

  if (!params_.db_train_same_) {  // use the second half of results to correct shock_database_cost1
    for(unsigned int j = 0; j<D2; j++){
      for(unsigned int i = 0; i<D1; i++){
        shock_database_cost1[i][j]= shock_database_cost1[i][j] < results[D1*D2 + j*D1 + i] ? shock_database_cost1[i][j] : results[D1*D2 + j*D1 + i] ;  // number of columns is D2
      }
    }
  } else { //: make the matrix symmetric and the diagonal very big so that nearest match is not itself
    for(unsigned int i = 0; i<D1; i++){
      for(unsigned int j = i; j<D2; j++){
        if (i == j)
          shock_database_cost1[i][i] = 1000.0f;  // don't consider matching with itself
        else {
          shock_database_cost1[i][j]= shock_database_cost1[i][j] < shock_database_cost1[j][i] ? shock_database_cost1[i][j] : shock_database_cost1[j][i];  // number of columns is D2
          shock_database_cost1[j][i] = shock_database_cost1[i][j];
        }
      }
    }
  }

  if (params_.generate_output_) {
    vcl_ofstream of((params_.output_file_ + params_.edit_params_.output_file_postfix_ + ".out").c_str());

    of << "# db vs train matrix: \n";
    for(unsigned int i = 0; i<D1; i++){
      of << database[i] << "\t";
      for(unsigned int j = 0; j<D2; j++){
        of << results[i*D2 + j] << "\t";
      }
      of << "\n";
    }
    of << "\n\n";

    if (!params_.db_train_same_) {
      of << "# train vs db matrix: \n";
      for(unsigned int j = 0; j<D2; j++){
        of << training_set[j] << "\t";
        for(unsigned int i = 0; i<D1; i++){
          of << results[D1*D2 + j*D1 + i] << "\t";
        }
        of << "\n";
      }
      of << "\n\n";
    }

    of << "# db vs train matrix with corrected costs (used to find best matches): \n";
    for(unsigned int i = 0; i<D1; i++){
      of << database[i] << "\t";
      for(unsigned int j = 0; j<D2; j++){
        of << shock_database_cost1[i][j] << "\t";
      }
      of << "\n";
    }
    of << "\n\n";

    of << "\n\n";
   
    for(unsigned int i = 0; i<D1; i++){
      of << database[i]<<" best match: ";
      int bj = 0;
      float min = shock_database_cost1[i][0];
      for(unsigned int j = 0; j<D2; j++){
        if (shock_database_cost1[i][j] < min) {
          min = shock_database_cost1[i][j];
          bj = j;
        }
      }
      of << training_set[bj] << " " << shock_database_cost1[i][bj] << vcl_endl;
    }

    of.close();
  }

  return true;
}

#ifdef MPI_CPP_BINDING
MPI::Datatype dborl_match_shock_processor::create_datatype_for_R()
{
  return MPI::FLOAT;
}
#else
MPI_Datatype dborl_match_shock_processor::create_datatype_for_R()
{
  return MPI_FLOAT;
}
#endif


