//:
// \file
// \brief
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 03/26/08

//
//

#include "dborl_shock_retrieval.h"
#include <bxml/bxml_read.h>
#include <bxml/bxml_write.h>
#include <bxml/bxml_find.h>
#include <dbskr/dbskr_tree_edit.h>
#include <dbskr/algo/dbskr_rec_algs.h>
#include <dbskr/algo/dbskr_shock_patch_match.h>
#include <dbskr/pro/dbskr_shock_patch_storage.h>
#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>
#include <dborl/algo/dborl_utilities.h>
#include <dborl/algo/dborl_index_parser.h>
#include <dborl/algo/dborl_image_desc_parser.h>
#include <dborl/dborl_index.h>
#include <dborl/dborl_index_node.h>
#include <dborl/dborl_image_description.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_algorithm.h>
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <dborl/dborl_evaluation.h>

//: this method is run on each processor after lead processor broadcasts its command
//  line arguments to all the processors since only on the lead processor is passed the command line arguments by mpirun
bool dborl_shock_retrieval::parse_command_line(vcl_vector<vcl_string>& argv)
{

  if (!params_.parse_command_line_args(argv))
    return false;

  //: always print the params file if an executable to work with ORL web interface
  if (!params_.print_params_xml(params_.print_params_file()))
    vcl_cout << "problems in writing params file to: " << params_.print_params_file() << vcl_endl;

  param_file_ = params_.input_param_filename_;
  return true;
}

//: this method is run on each processor
bool dborl_shock_retrieval::parse(const char* param_file)
{
  params_.input_param_filename_ = param_file; // just in case
  if (!params_.parse_input_xml())
    return false;

  if (params_.exit_with_no_processing() || params_.print_params_only())
    return false;

  return true;
}

//: this method prints an xml input file setting all the parameters to defaults
//  run the algorithm to generate this file, then modify it  
void dborl_shock_retrieval::print_default_file(const char* def_file)
{
  params_.print_default_input_xml(vcl_string(def_file));
}

//: this method is run on each processor
bool dborl_shock_retrieval::parse_index(vcl_string index_file)
{
  dborl_index_parser parser;
  parser.clear();

  vcl_FILE *xmlFile = vcl_fopen(index_file.c_str(), "r");
  if (xmlFile == NULL){
    vcl_cout << "dborl_shock_retrieval::parse_index() -- " << index_file << "-- error on opening" << vcl_endl;
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

  vcl_cout << "parsed the index file with name: " << ind_->name_ << vcl_endl;
  return true;
}

//: this method is run on each processor
bool dborl_shock_retrieval::initialize(vcl_vector<dborl_shock_retrieval_input>& t)
{
  //: parse the index file 
  if (!parse_index(params_.db_index_()))
    return false;

  dborl_index_node_sptr root = ind_->root_->cast_to_index_node();
  if (root->names().size() != root->paths().size())
    return false;

  unsigned D = root->names().size();
  vcl_cout << "db size: " << D << vcl_endl;

  for (unsigned i = 0; i < D; i++)
    database_indices_.push_back(i);

  //: shuffle the elements of this vector to maximize the speed up with MPI process distribution
  //  some class trees are way larger than other classes and thus when they are passed as chunks to the same processor
  //  it delays the others
  vcl_random_shuffle(database_indices_.begin(), database_indices_.end());

  if (!params_.use_associated_sim_file_()) {
    
    dbsk2d_xshock_graph_fileio loader;
    vcl_vector<dbskr_tree_sptr> database_tree;

    for (unsigned i = 0; i<D; i++) {
      vcl_string name = (root->names())[database_indices_[i]];
      vcl_string path = (root->paths())[database_indices_[i]];
      vcl_string esf_file = path + "/" + name + params_.input_ext_();
      if (!vul_file::exists(esf_file)) {
        vcl_cout << "In dborl_shock_retrieval::initialize() -- cannot find file esf file: " << esf_file << vcl_endl;
        return false;
      }
      
      dbsk2d_shock_graph_sptr sg = loader.load_xshock_graph(esf_file.c_str());
      if (!sg || !sg->number_of_edges() || !sg->number_of_vertices() || sg->has_cycle()) {
        vcl_cout << "In dborl_shock_retrieval::initialize() -- Problems in loading graph: " << esf_file << vcl_endl;
        return false;
      } //else
        //vcl_cout <<"loaded esf: " << esf_file <<vcl_endl;
      
      dbskr_tree_sptr tree = new dbskr_tree(params_.edit_params_.scurve_sample_ds_(), params_.edit_params_.scurve_interpolate_ds_());
      if (!tree || !tree->acquire(sg, params_.edit_params_.elastic_splice_cost_(), params_.edit_params_.circular_ends_(), params_.edit_params_.combined_edit_())) {
        vcl_cout << "In dborl_shock_retrieval::initialize() -- Problems in constructing tree: " << esf_file << vcl_endl;
        return false;
      }

      database_tree.push_back(tree);
    }

    //: initialize the input vector for each pair
    for (unsigned i = 0; i<D; i++) {
      for (unsigned j = i; j<D; j++) {
        dborl_shock_retrieval_input inp(database_tree[i], database_tree[j], (root->names())[database_indices_[i]], (root->names())[database_indices_[j]]);
        t.push_back(inp);
      }
    }  

  }

  match_folder_ = params_.output_folder_() + "/shgms/";
  if (params_.save_matches_() && !vul_file::exists(match_folder_)) {
    vul_file::make_directory(match_folder_);
  }

  params_.percent_completed = 1.0f;
  params_.print_status_xml();
  return true;
}

//: load the sim matrix and re-arrange according to the order in database_indices_
//: assumes the sim matrix is initialized with the name order in database_indices_
bool dborl_shock_retrieval::load_sim_matrix(vcl_string sim_file) 
{
  unsigned D = database_indices_.size();

  if (!vul_file::exists(sim_file)) {
    vcl_cout << "In dborl_shock_retrieval::load_sim_matrix() -- cannot find: " << sim_file << vcl_endl;
    return false;
  }

  vcl_ifstream ifs(sim_file.c_str());
  if (!ifs) {
    vcl_cout << "In dborl_shock_retrieval::load_sim_matrix() -- problems in loading: " << sim_file << vcl_endl;
    return false;
  }

  char buffer[1000];
  ifs.getline(buffer, 1000);   // of << "# db vs db matrix: \n";

  vcl_map<vcl_string, vcl_pair<unsigned, vcl_vector<float>* > > tmp_map;
  
  for(unsigned int i = 0; i<D; i++){
    vcl_pair<unsigned, vcl_vector<float>* > p;
    vcl_vector<float>* tmp = new vcl_vector<float>();
    p.first = i;
    p.second = tmp;

    vcl_string name;
    ifs >> name;  // of << (root->names())[database_indices_[i]] << "\t";

    for(unsigned int j = 0; j<D; j++){
      float sim;
      ifs >> sim; // of << (*sim_matrix[i])[j].first << "\t";
      tmp->push_back(sim);
    }
    //of << "\n";
    tmp_map[name] = p;
  }
  //of << "\n\n";
  ifs.close(); //of.close();

  //: now fill in the sim_matrix_;
  dborl_index_node_sptr root = ind_->root_->cast_to_index_node();
  for (unsigned i = 0; i < D; i++) {
    vcl_string name = (root->names())[database_indices_[i]];
    vcl_map<vcl_string, vcl_pair<unsigned, vcl_vector<float>* > >::iterator it = tmp_map.find(name);
    if (it == tmp_map.end())
      return false;

    vcl_pair<unsigned, vcl_vector<float>* > p = it->second;
    for (unsigned j = 0; j < D; j++) {
      vcl_string name_j = (root->names())[database_indices_[j]];

      vcl_map<vcl_string, vcl_pair<unsigned, vcl_vector<float>* > >::iterator it_j = tmp_map.find(name_j);
      if (it_j == tmp_map.end())
        return false;

      (*sim_matrix_[i])[j].first = (*p.second)[(it_j->second).first];
    }

  }

  return true;
}

//: For sorting pairs by their second elements cost
inline bool
final_cost_less( const vcl_pair<float, dborl_image_description_sptr>& left,
                 const vcl_pair<float, dborl_image_description_sptr>& right )
{
  return left.first < right.first;
}

void dborl_shock_retrieval::write_sim_matrix(vcl_string sim_file)
{
  unsigned D = database_indices_.size();
  dborl_index_node_sptr root = ind_->root_->cast_to_index_node();

  vcl_ofstream of(sim_file.c_str());

  of << "# db vs db matrix: \n";
  for(unsigned int i = 0; i<D; i++){
    of << (root->names())[database_indices_[i]] << "\t";
    for(unsigned int j = 0; j<D; j++){
      of << (*sim_matrix_[i])[j].first << "\t";
    }
    of << "\n";
  }
  of << "\n\n";
  //of.close();
      
  of << "# db vs db matrix categories after sorting: \n";
  for(unsigned int i = 0; i<D; i++){
    //: sort each row
    vcl_vector<vcl_pair<float, dborl_image_description_sptr> >* v = new vcl_vector<vcl_pair<float, dborl_image_description_sptr> >();
    
    for (unsigned kk = 0; kk < (*sim_matrix_[i]).size(); kk++) 
      v->push_back((*sim_matrix_[i])[kk]);

    vcl_sort(v->begin(), v->end(), final_cost_less);

    of << (root->names())[database_indices_[i]] << "\t";
    for(unsigned int j = 0; j<D; j++){
      if ((*v)[j].second)
        of << "(" << (*v)[j].first << ", " << (*v)[j].second->get_first_category() << ")\t";
      else
        of << "(" << (*v)[j].first << ", unknown)\t";
    }
    of << "\n";
  }
  of << "\n\n";
  of.close();
}


//: this method is run in a distributed mode on each processor on the cluster
bool dborl_shock_retrieval::process(dborl_shock_retrieval_input i, float& f)
{
  vcl_string out_name1 = match_folder_ + i.name1 + "-" + i.name2 + params_.edit_params_.output_file_postfix() + ".shgm";
  vcl_string out_name2 = match_folder_ + i.name2 + "-" + i.name1 + params_.edit_params_.output_file_postfix() + ".shgm";

  if (!vul_file::exists(out_name1)) {

    dbskr_tree_edit edit(i.tree1, i.tree2, params_.edit_params_.circular_ends_(), params_.edit_params_.localized_edit_());
    edit.save_path(true);    
    edit.set_curvematching_R(params_.edit_params_.curve_matching_R_());
    if (!edit.edit()) {
      vcl_cout << "In dborl_shock_retrieval::process() -- Problems in editing trees\n";
      return false;
    }

    f = edit.final_cost();

    if (params_.save_matches_()) {
      edit.write_shgm(out_name1);
    }

  } else {
    dbskr_sm_cor_sptr sm_cor = new dbskr_sm_cor();
    sm_cor->read_and_construct_from_shgm(out_name1, false);
    //vcl_cout << out_name << " exists .. skipped\n";
    f = sm_cor->final_cost();
  }

  if (!vul_file::exists(out_name2)) {

    dbskr_tree_edit edit(i.tree2, i.tree1, params_.edit_params_.circular_ends_(), params_.edit_params_.localized_edit_());
    edit.save_path(true);    
    edit.set_curvematching_R(params_.edit_params_.curve_matching_R_());
    if (!edit.edit()) {
      vcl_cout << "In dborl_shock_retrieval::process() -- Problems in editing trees\n";
      return false;
    }

    if (edit.final_cost() < f)
      f = edit.final_cost();

    if (params_.save_matches_()) {
      edit.write_shgm(out_name2);
    }

  } else {
    dbskr_sm_cor_sptr sm_cor = new dbskr_sm_cor();
    sm_cor->read_and_construct_from_shgm(out_name2, false);
    //vcl_cout << out_name << " exists .. skipped\n";
    f = sm_cor->final_cost();
    if (sm_cor->final_cost() < f)
      f = sm_cor->final_cost();
  }

  if (params_.normalize_()) {
    if (params_.norm_reconst_bound_()) 
      f = f / (i.tree1->total_reconstructed_boundary_length() + i.tree2->total_reconstructed_boundary_length());
    else if (params_.norm_tot_splice_cost_())
      f = f / (i.tree1->total_splice_cost() + i.tree2->total_splice_cost());
    else if (params_.norm_con_arclength_()) {
      vcl_cout << "In dborl_shock_retrieval::process() -- normalization wrt con length is not implemented yet\n";
      return false;
    }
  }

  return true;
}

void dborl_shock_retrieval::print_time()
{
  vcl_cout << "\t\t\t total time: " << (t_.real()/1000.0f) << " secs.\n";
  vcl_cout << "\t\t\t total time: " << ((t_.real()/1000.0f)/60.0f) << " mins.\n";
}



void dborl_shock_retrieval::initialize_sim_matrix(unsigned D)
{
  for (unsigned i = 0; i < D; i++) {
    vcl_pair<float, dborl_image_description_sptr> p(10000.0f, 0);
    vcl_vector<vcl_pair<float, dborl_image_description_sptr> >* tmp = new vcl_vector<vcl_pair<float, dborl_image_description_sptr> >(database_indices_.size(), p);
    sim_matrix_.push_back(tmp);
  }
}

//: this method is run on the lead processor once after results are collected from each processor
bool dborl_shock_retrieval::finalize(vcl_vector<float>& results)
{
  params_.percent_completed = 95.0f;
  params_.print_status_xml();

  dborl_index_node_sptr root = ind_->root_->cast_to_index_node();
  unsigned D = database_indices_.size();

  //: load ground truth files of the objects
  vcl_vector<dborl_image_description_sptr> ids;
  for (unsigned i = 0; i < D; i++) {
    vcl_string name = (root->names())[database_indices_[i]];
    vcl_string full = (root->paths())[database_indices_[i]] + "/" + (root->names())[database_indices_[i]] + ".xml";
    if (!vul_file::exists(full)) {
      vcl_cout << "dborl_shock_retrieval::finalize() -- cannot find the ground truth description: " << full << "\n";
      return false;
    }

    //: assuming image description
    dborl_image_desc_parser parser;
    dborl_image_description_sptr id = dborl_image_description_parse(full, parser);
    if (!id->has_single_category()) {
      vcl_cout << "dborl_shock_retrieval::finalize() -- image description has more than one category! this image is not valid for this retrieval\n";
      return false;
    }
    ids.push_back(id);
  }
  
  vcl_cout << "ids has: " << ids.size() << " elemensts, initializing matrix..\n";
  vcl_cout.flush();

  initialize_sim_matrix(D);

  //: correct image description pointers
  for (unsigned i = 0; i < D; i++) {
    for (unsigned j = i; j < D; j++) {
      (*sim_matrix_[i])[j].second = ids[j];
      (*sim_matrix_[j])[i].second = ids[i];
    }
  }

  if (params_.use_associated_sim_file_()) {
    vcl_string sim_file_name = params_.sim_matrix_folder_() + "/sim_matrix_" + params_.edit_params_.output_file_postfix() + ".out";
    if (!load_sim_matrix(sim_file_name))
      return false;
  } else {
    //: initialize the input vector for each pair
    if (results.size() != (((D*D - D)/2) + D) ) {
      vcl_cout << "In dborl_shock_retrieval::finalize() -- results vector is not prepared ok!\n";
      return false;
    }

    vcl_cout << "results vector size: " << results.size() << "..\n";
    vcl_cout.flush();

    unsigned k = 0;
    for (unsigned i = 0; i < D; i++) {
      for (unsigned j = i; j < D; j++) {
        (*sim_matrix_[i])[j].first = results[k];
        (*sim_matrix_[j])[i].first = (*sim_matrix_[i])[j].first;
        k++;
      }
    }

    //: write the matrix since it is created for the first time
    vcl_string sim_file_name = params_.sim_matrix_folder_to_create_() + "/";
    
    if (!vul_file::exists(sim_file_name))
      vul_file::make_directory(sim_file_name);

    sim_file_name = sim_file_name + "sim_matrix_" + params_.edit_params_.output_file_postfix() + ".out";
    vcl_cout << "writing: " << sim_file_name << "..\n";
    vcl_cout.flush();
    write_sim_matrix(sim_file_name);
  }
    
  //: sort the sim_matrix rows
  for (unsigned i = 0; i < D; i++) {
    vcl_vector<vcl_pair<float, dborl_image_description_sptr> >* v = sim_matrix_[i];
    vcl_sort(v->begin(), v->end(), final_cost_less);
  }

  vcl_map<vcl_string, dborl_exp_stat_sptr> stat_map;
  vcl_string algo_prefix = "Method I";
  for (unsigned i = 0; i < D; i++) {
    vcl_vector<vcl_pair<float, dborl_image_description_sptr> >* v = sim_matrix_[i];
    vcl_string category = ids[i]->get_first_category();
    vcl_map<vcl_string, dborl_exp_stat_sptr>::iterator it = stat_map.find(algo_prefix + category);
    dborl_exp_stat_sptr s;
    if (it == stat_map.end()) {
      s = new dborl_exp_stat();
      stat_map[algo_prefix + category] = s;
    } else {
      s = it->second;
    } 
    
    //: skip the first one, assuming is itself
    if ((*v)[0].first > 0.1) {
      vcl_cout << "dborl_shock_retrieval::finalize() -- similarity to itself is larger than 0.1!!\n";
      return false;
    }
    
    //: find TP, FP, TN, FN
    if (params_.use_rho_()) {
      unsigned j = 1;  // skip 0, it is supposed to be itself
      for ( ; j < D; j++) {
        if ((*v)[j].first > params_.rho_())
          break;
      }
      for (unsigned jj = 1; jj < j; jj++) {
        if (((*sim_matrix_[i])[jj].second->get_first_category()).compare(category) == 0)   // same cat
          s->increment_TP();
        else 
          s->increment_FP();
      }
      for (unsigned jj = j; jj < D; jj++) {
        if (((*sim_matrix_[i])[jj].second->get_first_category()).compare(category) == 0)   // same cat == missed this one, FN
          s->increment_FN();
        else
          s->increment_TN();
      }
      
    } else {  // use top k
      unsigned k = params_.k_() < int(D) ? params_.k_() : D-1;
      for (unsigned jj = 1; jj <= k; jj++) {
        if (((*sim_matrix_[i])[jj].second->get_first_category()).compare(category) == 0)   // same cat
          s->increment_TP();
        else 
          s->increment_FP();
      }
      for (unsigned jj = k + 1; jj < D; jj++) {
        if (((*sim_matrix_[i])[jj].second->get_first_category()).compare(category) == 0)   // same cat == missed this one, FN
          s->increment_FN();
        else
          s->increment_TN();
      }

    }

  }

  //: find the cumulative statistics of all categories
  dborl_exp_stat_sptr cum = new dborl_exp_stat();
  vcl_map<vcl_string, dborl_exp_stat_sptr>::iterator it = stat_map.begin();
  for ( ; it != stat_map.end(); it++) {
    cum->increment_TP_by(it->second->TP_);
    cum->increment_FP_by(it->second->FP_);
    cum->increment_TN_by(it->second->TN_);
    cum->increment_FN_by(it->second->FN_);
  }

  stat_map[algo_prefix] = cum;  // cumulative result for algo
  params_.print_evaluation_xml(stat_map, true);

  params_.percent_completed = 100.0f;
  params_.print_status_xml();

  return true;
}

#ifdef MPI_CPP_BINDING
MPI::Datatype dborl_shock_retrieval::create_datatype_for_R()
{
  return MPI::FLOAT;
}
#else
MPI_Datatype dborl_shock_retrieval::create_datatype_for_R()
{
  return MPI_FLOAT;
}
#endif
