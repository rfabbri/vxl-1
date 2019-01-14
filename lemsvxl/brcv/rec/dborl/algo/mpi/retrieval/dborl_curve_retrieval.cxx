//:
// \file
// \brief
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 03/26/08

//
//

#include "dborl_curve_retrieval.h"
#include <dborl/algo/dborl_utilities.h>
#include <dborl/algo/dborl_index_parser.h>
#include <dborl/algo/dborl_image_desc_parser.h>
#include <dborl/dborl_index.h>
#include <dborl/dborl_index_node.h>
#include <borld/borld_image_description.h>
#include <dbcvr/dbcvr_clsd_cvmatch.h>
#include <dbcvr/dbcvr_clsd_cvmatch_sptr.h>
#include <bsold/bsold_file_io.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <borld/borld_evaluation.h>
#include <vsol/vsol_polyline_2d.h>

//: this method is run on each processor after lead processor broadcasts its command
//  line arguments to all the processors since only on the lead processor is passed the command line arguments by mpirun
bool dborl_curve_retrieval::parse_command_line(std::vector<std::string>& argv)
{

  if (!params_.parse_command_line_args(argv))
    return false;

  //: always print the params file if an executable to work with ORL web interface
  if (!params_.print_params_xml(params_.print_params_file()))
    std::cout << "problems in writing params file to: " << params_.print_params_file() << std::endl;

  param_file_ = params_.input_param_filename_;
  return true;
}

//: this method is run on each processor
bool dborl_curve_retrieval::parse(const char* param_file)
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
void dborl_curve_retrieval::print_default_file(const char* def_file)
{
  params_.print_default_input_xml(std::string(def_file));
}

//: this method is run on each processor
bool dborl_curve_retrieval::parse_index(std::string index_file)
{
  dborl_index_parser parser;
  parser.clear();

  std::FILE *xmlFile = std::fopen(index_file.c_str(), "r");
  if (xmlFile == NULL){
    std::cout << "dborl_curve_retrieval::parse_index() -- " << index_file << "-- error on opening" << std::endl;
    return false;
  }

  if (!parser.parseFile(xmlFile)) {
     std::cout << XML_ErrorString(parser.XML_GetErrorCode()) << " at line " <<
        parser.XML_GetCurrentLineNumber() << std::endl;
     return 0;
   }

  fclose(xmlFile);
  ind_ = parser.get_index();
  
  if (!ind_)
    return false;

  std::cout << "parsed the index file with name: " << ind_->name_ << std::endl;
  return true;
}

//: this method is run on each processor
bool dborl_curve_retrieval::initialize(std::vector<dborl_curve_retrieval_input>& t)
{
  //: parse the index file 
  if (!parse_index(params_.db_index_()))
    return false;

  dborl_index_node_sptr root = ind_->root_->cast_to_index_node();
  if (root->names().size() != root->paths().size())
    return false;

  unsigned D = root->names().size();
  std::cout << "db size: " << D << std::endl;
  
  for (unsigned i = 0; i < D; i++)
    database_indices_.push_back(i);

  //: shuffle the elements of this vector to maximize the speed up with MPI process distribution
  //  some class trees are way larger than other classes and thus when they are passed as chunks to the same processor
  //  it delays the others
  std::random_shuffle(database_indices_.begin(), database_indices_.end());
  
  std::vector<vsol_polygon_2d_sptr> database_cons;

  for (unsigned i = 0; i<D; i++) {
    std::string name = (root->names())[database_indices_[i]];
    std::string path = (root->paths())[database_indices_[i]];
    std::string con_file = path + "/" + name + params_.input_ext_();
    if (!vul_file::exists(con_file)) {
      std::cout << "In dborl_curve_retrieval::initialize() -- cannot find con file: " << con_file << std::endl;
      return false;
    }
    
    //: load con
    vsol_spatial_object_2d_sptr ob = bsold_load_con_file(con_file.c_str());
    vsol_polygon_2d_sptr poly;
    if (!ob) {
      std::cout << "In dborl_curve_retrieval::initialize() -- cannot load con file as a polygon: " << con_file << std::endl;
      return false;
    }

    if ((ob->cast_to_curve() && ob->cast_to_curve()->cast_to_polyline())) {
      std::vector<vsol_point_2d_sptr> pts;
      for (unsigned ii = 0; ii < ob->cast_to_curve()->cast_to_polyline()->size(); ii++)
        pts.push_back(ob->cast_to_curve()->cast_to_polyline()->vertex(ii));
      poly = new vsol_polygon_2d(pts);
    } else if ((ob->cast_to_region() && ob->cast_to_region()->cast_to_polygon())) {
      poly = ob->cast_to_region()->cast_to_polygon();
    } else {
      std::cout << "In dborl_curve_retrieval::initialize() -- cannot load con file as a polygon: " << con_file << std::endl;
      return false;
    }
    
    if (!poly->size()) {
      std::cout << "In dborl_curve_retrieval::initialize() -- polygon has zero vertices: " << con_file << std::endl;
      return false;
    } 
    database_cons.push_back(poly);
  }

  //: initialize the input vector for each pair
  for (unsigned i = 0; i<D; i++) {
    for (unsigned j = i; j<D; j++) {
      dborl_curve_retrieval_input inp(database_cons[i], database_cons[j], (root->names())[database_indices_[i]], (root->names())[database_indices_[j]]);
      t.push_back(inp);
    }
  }  

  params_.percent_completed = 1.0f;
  params_.print_status_xml();
  return true;
}

//: this method is run in a distributed mode on each processor on the cluster
bool dborl_curve_retrieval::process(dborl_curve_retrieval_input i, float& f)
{
  
  dbcvr_clsd_cvmatch_sptr d1 = new dbcvr_clsd_cvmatch(i.cont1, i.cont2, params_.matching_R_(), params_.line_fit_rms_(), params_.DP_template_size_());
  d1->setStretchCostFlag(false);   // cost: |ds1-ds2| + R|d_theta1-d_theta2|
  d1->Match();

  int minIndex;
  double curve_matching_cost = d1->finalBestCost(minIndex, params_.normalize_());
  //std::printf("%9.6f\n",curve_matching_cost);
  //sil_cor= d1->get_cv_cor(minIndex);
  
  f = (float)curve_matching_cost;

  return true;
}

void dborl_curve_retrieval::print_time()
{
  std::cout << "\t\t\t total time: " << (t_.real()/1000.0f) << " secs.\n";
  std::cout << "\t\t\t total time: " << ((t_.real()/1000.0f)/60.0f) << " mins.\n";
}

//: For sorting pairs by their second elements cost
inline bool
final_cost_less( const std::pair<float, borld_image_description_sptr>& left,
                 const std::pair<float, borld_image_description_sptr>& right )
{
  return left.first < right.first;
}

//: this method is run on the lead processor once after results are collected from each processor
bool dborl_curve_retrieval::finalize(std::vector<float>& results)
{
  params_.percent_completed = 95.0f;
  params_.print_status_xml();

  dborl_index_node_sptr root = ind_->root_->cast_to_index_node();
  unsigned D = database_indices_.size();

  //: the following 2D sim matrix, will carry for a given i: row, j: col, sim of i and j and the description of j
  //  since when we sort the rows wrt sim, then we want to keep columns' descriptions
  std::vector<std::vector<std::pair<float, borld_image_description_sptr> >* > sim_matrix;
  for (unsigned i = 0; i < database_indices_.size(); i++) {
    std::pair<float, borld_image_description_sptr> p(10000.0f, 0);
    std::vector<std::pair<float, borld_image_description_sptr> >* tmp = new std::vector<std::pair<float, borld_image_description_sptr> >(database_indices_.size(), p);
    sim_matrix.push_back(tmp);
  }
    
  //: load ground truth files of the objects
  std::vector<borld_image_description_sptr> ids;
  for (unsigned i = 0; i < D; i++) {
    std::string name = (root->names())[database_indices_[i]];
    std::string full = (root->paths())[database_indices_[i]] + "/" + (root->names())[database_indices_[i]] + ".xml";
    if (!vul_file::exists(full)) {
      std::cout << "dborl_curve_retrieval::finalize() -- cannot find the ground truth description: " << full << "\n";
      return false;
    }

    //: assuming image description
    dborl_image_desc_parser parser;
    borld_image_description_sptr id = borld_image_description_parse(full, parser);
    if (!id->has_single_category()) {
      std::cout << "dborl_curve_retrieval::finalize() -- image description has more than one category! this image is not valid for this retrieval\n";
      return false;
    }
    ids.push_back(id);
  }

  //: initialize the input vector for each pair
  unsigned k = 0;
  for (unsigned i = 0; i < D; i++) {
    for (unsigned j = i; j < D; j++) {
      (*sim_matrix[i])[j].first = results[k];
      (*sim_matrix[i])[j].second = ids[j];
      k++;
      (*sim_matrix[j])[i].first = (*sim_matrix[i])[j].first;
      (*sim_matrix[j])[i].second = ids[i];
    }
  }

  std::ofstream of((params_.output_folder_() + "sim_matrix.out").c_str());

  of << "# db vs db matrix: \n";
  for(unsigned int i = 0; i<D; i++){
    of << (root->names())[database_indices_[i]] << "\t";
    for(unsigned int j = 0; j<D; j++){
      of << (*sim_matrix[i])[j].first << "\t";
    }
    of << "\n";
  }
  of << "\n\n";
  //of.close();
  
  //: sort each row
  for (unsigned i = 0; i < D; i++) {
    std::vector<std::pair<float, borld_image_description_sptr> >* v = sim_matrix[i];
    std::sort(v->begin(), v->end(), final_cost_less);
  }

  of << "# db vs db matrix categories after sorting: \n";
  for(unsigned int i = 0; i<D; i++){
    of << (root->names())[database_indices_[i]] << "\t";
    for(unsigned int j = 0; j<D; j++){
      of << "(" << (*sim_matrix[i])[j].first << ", " << (*sim_matrix[i])[j].second->get_first_category() << ")\t";
    }
    of << "\n";
  }
  of << "\n\n";
  of.close();

  std::string algo_prefix = "Method II";
  std::map<std::string, buld_exp_stat_sptr> stat_map;
  for (unsigned i = 0; i < D; i++) {
    std::vector<std::pair<float, borld_image_description_sptr> >* v = sim_matrix[i];
    std::string category = ids[i]->get_first_category();
    std::map<std::string, buld_exp_stat_sptr>::iterator it = stat_map.find(algo_prefix + category);
    buld_exp_stat_sptr s;
    if (it == stat_map.end()) {
      s = new buld_exp_stat();
      stat_map[algo_prefix + category] = s;
    } else {
      s = it->second;
    } 
    
    //: skip the first one, assuming is itself
    if ((*v)[0].first > 0.1) {
      std::cout << "dborl_curve_retrieval::finalize() -- similarity to itself is larger than 0.1!!\n";
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
        if (((*sim_matrix[i])[jj].second->get_first_category()).compare(category) == 0)   // same cat
          s->increment_TP();
        else 
          s->increment_FP();
      }
      for (unsigned jj = j; jj < D; jj++) {
        if (((*sim_matrix[i])[jj].second->get_first_category()).compare(category) == 0)   // same cat == missed this one, FN
          s->increment_FN();
        else
          s->increment_TN();
      }
      
    } else {  // use top k
      unsigned k = params_.k_() < int(D) ? params_.k_() : D-1;
      for (unsigned jj = 1; jj <= k; jj++) {
        if (((*sim_matrix[i])[jj].second->get_first_category()).compare(category) == 0)   // same cat
          s->increment_TP();
        else 
          s->increment_FP();
      }
      for (unsigned jj = k + 1; jj < D; jj++) {
        if (((*sim_matrix[i])[jj].second->get_first_category()).compare(category) == 0)   // same cat == missed this one, FN
          s->increment_FN();
        else
          s->increment_TN();
      }

    }

  }

  //: find the cumulative statistics of all categories
  buld_exp_stat_sptr cum = new buld_exp_stat();
  std::map<std::string, buld_exp_stat_sptr>::iterator it = stat_map.begin();
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
MPI::Datatype dborl_curve_retrieval::create_datatype_for_R()
{
  return MPI::FLOAT;
}
#else
MPI_Datatype dborl_curve_retrieval::create_datatype_for_R()
{
  return MPI_FLOAT;
}
#endif
