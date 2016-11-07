//:
// \file
// \brief
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 03/26/08

//
//

#include "dborl_patch_match.h"
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

#include <dborl/algo/dborl_index_parser.h>
#include <dborl/algo/dborl_image_desc_parser.h>
#include <dborl/dborl_index.h>
#include <dborl/dborl_index_node.h>
#include <dborl/dborl_image_description.h>
#include <dborl/dborl_evaluation.h>

dborl_exp_stat_sptr dborl_patch_match_output::get_exp_stat()
{
  dborl_exp_stat_sptr es = new dborl_exp_stat();
  es->increment_TP_by(TP_);
  es->increment_FP_by(FP_);
  es->increment_TN_by(TN_);
  es->increment_FN_by(FN_);
  return es;
}

void dborl_patch_match_output::set_values(dborl_exp_stat& stat)
{
  TP_ = stat.TP_; 
  FP_ = stat.FP_;
  TN_ = stat.TN_;
  FN_ = stat.FN_;
}

//: this method is run on each processor after lead processor broadcasts its command
//  line arguments to all the processors since only on the lead processor is passed the command line arguments by mpirun
bool dborl_patch_match::parse_command_line(vcl_vector<vcl_string>& argv)
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
bool dborl_patch_match::parse(const char* param_file)
{
  params_.input_param_filename_ = param_file; // just in case
  if (!params_.parse_input_xml())
    return false;

  if (params_.exit_with_no_processing() || params_.print_params_only())
    return false;

  return true;
}

//: this method is run on each processor
bool dborl_patch_match::parse_index(vcl_string index_file, dborl_index_sptr& ind, dborl_index_node_sptr& root)
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
  ind = parser.get_index();
  
  if (!ind)
    return false;

  vcl_cout << "parsed the index file with name: " << ind->name_ << vcl_endl;

  root = ind->root_->cast_to_index_node();
  if (root->names().size() != root->paths().size()) {
    vcl_cout << "dborl_patch_match::parse_index() -- " << index_file << "-- number of names not equal number of paths!!" << vcl_endl;
    return false;
  }

  return true;
}

//: this method prints an xml input file setting all the parameters to defaults
//  run the algorithm to generate this file, then modify it  
void dborl_patch_match::print_default_file(const char* def_file)
{
  params_.print_default_input_xml(vcl_string(def_file));
}

//: this method is run on each processor
bool dborl_patch_match::initialize(vcl_vector<dborl_patch_match_input>& t)
{
  //: parse the index file 
  if (!parse_index(params_.db_index_prototype_(), proto_ind_, proto_root_)) {
    vcl_cout << "cannot parse: " << params_.db_index_prototype_() << vcl_endl;
    return false;
  }

  if (!parse_index(params_.db_index_query_(), query_ind_, query_root_)) {
    vcl_cout << "cannot parse: " << params_.db_index_query_() << vcl_endl;
    return false;
  }
  vcl_cout << "parsed index files\n";
  vcl_cout.flush();

  //: initialize each proto-query pair
  
  unsigned Q = query_root_->names().size();
  unsigned P = proto_root_->names().size();

  for (unsigned i = 0; i < P; i++) {
    vcl_string proto_gt_file = proto_root_->paths()[i] + "/" + proto_root_->names()[i] + ".xml";
    if (!vul_file::exists(proto_gt_file)) {
      vcl_cout << "cannot find: " << proto_gt_file << vcl_endl;
      return false;
    }

    dborl_image_desc_parser parser;
    dborl_image_description_sptr proto_id = dborl_image_description_parse(proto_gt_file, parser);
    proto_img_d_.push_back(proto_id);
  }

  for (unsigned i = 0; i < Q; i++) {
    vcl_string query_gt_file = query_root_->paths()[i] + "/" + query_root_->names()[i] + ".xml";
    if (!vul_file::exists(query_gt_file)) {
      vcl_cout << "cannot find: " << query_gt_file << vcl_endl;
      return false;
    }

    dborl_image_desc_parser parser;
    dborl_image_description_sptr query_id = dborl_image_description_parse(query_gt_file, parser);
    query_img_d_.push_back(query_id);
  }

  vcl_cout << "parsed ground truth files\n";
  vcl_cout.flush();

  vcl_string det_dir = vul_file::strip_extension(params_.evaluation_file());

  for (unsigned i = 0; i < P; i++) {
    vcl_string proto_name = proto_root_->names()[i];
    vcl_string proto_st_file = params_.patch_folder_assoc_prototype_() + "/" + 
      params_.patch_folder_assoc_prototype_.file_type() + "/" + proto_name + "/" +
      proto_name + params_.patch_params_prototype_.output_file_postfix("p_e") + "/" + 
      proto_name + params_.patch_params_prototype_.output_file_postfix("p_e") + "-patch_strg.bin";

    for (unsigned j = 0; j < Q; j++) {
      vcl_string query_name = query_root_->names()[j];
      vcl_string query_st_file = params_.patch_folder_assoc_query_() + "/" + params_.patch_folder_assoc_query_.file_type() + "/" 
        + query_name + "/" + query_name + params_.patch_params_query_.output_file_postfix("p_e") + "/" 
        + query_name + params_.patch_params_query_.output_file_postfix("p_e") + "-patch_strg.bin";

      if (proto_st_file.compare(query_st_file) == 0) { // proto and query are identical continue
        vcl_cout << "skipping:  " << query_name << " for the prototype: " << proto_name << " they have identical patch sets!!!!!!!!!!!!\n";
        continue;
      }

      if (params_.compute_one_per_computer_() && params_.use_saved_detections_()) {
        vcl_string det_name = det_dir + "/" + proto_name + "/" + query_name + "_eval.out";
        if (vul_file::exists(det_name)) {  // skip this one already computed
          continue;
        }
      }

      dborl_patch_match_input inp(proto_name, 
                                  query_name, 
                                  proto_img_d_[i], 
                                  query_img_d_[j],
                                  proto_st_file, 
                                  query_st_file, proto_root_->paths()[i], query_root_->paths()[j]);
        t.push_back(inp);

        if (params_.compute_one_per_computer_() && t.size() == total_processors_)
          return true;
    }
  }

  return true;
}

//: this method is run in a distributed mode on each processor on the cluster
bool dborl_patch_match::process(dborl_patch_match_input inp, dborl_patch_match_output& f)
{
  vcl_vector<vsol_box_2d_sptr> det_boxes;
  vcl_string det_dir = vul_file::strip_extension(params_.evaluation_file());
  
  if (params_.use_saved_detections_()) {
    vcl_string det_name = det_dir + "/" + inp.proto_name + "/" + inp.query_name + "_eval.out";
    if (vul_file::exists(det_name)) {  // if not exists then compute
      vcl_vector<vcl_string> names;
      vcl_string obj_name;
      dborl_exp_stat stat;
      if (parse_obj_evaluation(det_name, obj_name, det_boxes, names, stat)) {
        vcl_cout << "parsed: " << det_name << "\n";
        f.set_values(stat);
        return true;
      }
    }
  }

  //: load the proto patches
  vcl_string storage_end = "patch_strg.bin";

  if (!vul_file::exists(inp.proto_st_name)) {
    vcl_cout << "proto st file: " << inp.proto_st_name << " does not exist!!!\n";
    return false;
  }
  vcl_cout << "proto: " << inp.proto_name << " query: " << inp.query_name << " ";
  vcl_cout.flush();

  dbskr_shock_patch_storage_sptr proto_st = dbskr_shock_patch_storage_new();
  vsl_b_ifstream ifs(inp.proto_st_name.c_str());
  proto_st->b_read(ifs);
  ifs.close();

  //: load the shock graphs --> assumes each shock graph for each patch is saved in the same folder
  //vcl_cout << proto_st->size() << " patches in proto storage, reading shocks..\n";

  if (!proto_st->size()) {
    vcl_cout << "zero patches in proto st!!!: " << inp.proto_st_name << vcl_endl;
  }

  if (!proto_st->load_patch_shocks_and_create_trees(inp.proto_st_name, storage_end, 
                            params_.edit_params_.elastic_splice_cost_(), 
                            params_.edit_params_.circular_ends_(),
                            params_.edit_params_.combined_edit_(), 
                            params_.edit_params_.scurve_sample_ds_(), 
                            params_.edit_params_.scurve_interpolate_ds_()))
      return false;

  //: load the query patches
  if (!vul_file::exists(inp.query_st_name)) {
    vcl_cout << "query st file: " << inp.query_st_name << " does not exist!!!\n";
    return false;
  }

  dbskr_shock_patch_storage_sptr query_st = dbskr_shock_patch_storage_new();
  vsl_b_ifstream ifsq(inp.query_st_name.c_str());
  query_st->b_read(ifsq);
  ifsq.close();

  //: load the shock graphs --> assumes each shock graph for each patch is saved in the same folder
  //vcl_cout << query_st->size() << " patches in query storage, reading shocks..\n";

  vcl_cout << "p p#: " << proto_st->size() << " q p#: " << query_st->size() << "..";
  vcl_cout.flush();

  if (!query_st->size()) {
    vcl_cout << "zero patches in proto st!!!: " << inp.query_st_name << vcl_endl;
  }

  if (!query_st->load_patch_shocks_and_create_trees(inp.query_st_name, storage_end, 
                            params_.edit_params_.elastic_splice_cost_(), 
                            params_.edit_params_.circular_ends_(),
                            params_.edit_params_.combined_edit_(), 
                            params_.edit_params_.scurve_sample_ds_(), 
                            params_.edit_params_.scurve_interpolate_ds_()))
      return false;


  //: match them if needed
  dbskr_shock_patch_match_sptr match = new dbskr_shock_patch_match();
  patch_cor_map_type& map = match->get_map();

  dbskr_tree_edit_params edit_params;
  params_.edit_params_.get_edit_params_instance(edit_params);
  match->edit_params_ = edit_params;

  vcl_string out_name;
  if (params_.use_assoc_match_folder_()) {
    out_name = params_.match_folder_assoc_() + 
                          "/" + params_.match_folder_to_create_.file_type() + 
                          "/" + inp.proto_name + 
                          "/" + inp.proto_name + "-" + inp.query_name + params_.edit_params_.output_file_postfix() + ".bin";
    
    if (!vul_file::exists(out_name)) {
      vcl_cout << "can not find: " << out_name << vcl_endl;
      return false;
    }
    vsl_b_ifstream ifs(out_name.c_str());
    match->b_read(ifs);
    ifs.close();
    vcl_cout << "found!..";
    vcl_cout.flush();

  } else {

    out_name = params_.match_folder_to_create_();
    if (!vul_file::exists(out_name))
      vul_file::make_directory(out_name);
    out_name = out_name + "/" + params_.match_folder_to_create_.file_type() + "/";
    if (!vul_file::exists(out_name))
      vul_file::make_directory(out_name);
    out_name = out_name + inp.proto_name + "/";
    if (!vul_file::exists(out_name))
      vul_file::make_directory(out_name);

    out_name = out_name + inp.proto_name + "-" + inp.query_name + params_.edit_params_.output_file_postfix() + ".bin";
    if (vul_file::exists(out_name)) {
      vsl_b_ifstream ifs(out_name.c_str());
      match->b_read(ifs);
      ifs.close();
      vcl_cout << "exists!..";
      vcl_cout.flush();
    } else {
      vcl_vector<dbskr_shock_patch_sptr>& pv1 = proto_st->get_patches();
      vcl_vector<dbskr_shock_patch_sptr>& pv2 = query_st->get_patches();
  
      //vcl_cout << pv1.size() << " patches: ";
      for (unsigned i = 0; i < pv1.size(); i++) {
        //vcl_cout << i << " ";
        find_patch_correspondences(pv1[i], pv2, map, match->edit_params_);
        pv1[i]->kill_tree();
      }
      //vcl_cout << "\n match map size: " << map.size() << " map[pv1[0]->id()] size: " << map[pv1[0]->id()]->size() << vcl_endl;
      
      vsl_b_ofstream bfs(out_name.c_str());
      match->b_write(bfs);
      bfs.close();
      vcl_cout << "Done!..";
      vcl_cout.flush();
    }
  }


  //: prepare id maps for this match
  vcl_map<int, dbskr_shock_patch_sptr> model_map;
  for (unsigned ii = 0; ii < proto_st->size(); ii++) 
    model_map[proto_st->get_patch(ii)->id()] = proto_st->get_patch(ii);
  match->set_id_map1(model_map);

  vcl_map<int, dbskr_shock_patch_sptr> query_map;
  for (unsigned ii = 0; ii < query_st->size(); ii++) 
    query_map[query_st->get_patch(ii)->id()] = query_st->get_patch(ii);
  match->set_id_map2(query_map);


  if (params_.detection_params_.use_normalized_costs_()) {
    if (params_.detection_params_.use_reconst_boundary_length_())
      match->compute_length_norm_costs_of_cors();
    else
      match->compute_splice_norm_costs_of_cors();
  } 

  

  if (params_.detection_params_.use_only_patches_()) {  // use BMVC algo to detect shapes
     
    vsol_box_2d_sptr box;
    if (!match->detect_instance(box, params_.detection_params_.top_N_(), params_.detection_params_.k_(), params_.detection_params_.sim_threshold_())) {
     vcl_cout << "dborl_patch_match::process() - detection error for the match file:" << out_name << "!!!!\n";
     return false;
    }
    det_boxes.push_back(box);

  } else {   // use patch quad algorithm
    
    //: load proto shock
    vcl_string proto_shock_file;
    if (params_.use_object_shock_proto_()) {
      proto_shock_file = inp.proto_path + "/" + inp.proto_name + ".esf";
    } else {
      proto_shock_file = params_.shock_folder_assoc_prototype_() + "/" + 
        params_.shock_folder_assoc_prototype_.file_type() + "/" + inp.proto_name + "/" + inp.proto_name + ".esf";
    }
    
    if (!vul_file::exists(proto_shock_file)) {
      vcl_cout << "dborl_patch_match::process() - cannot find shock: " << proto_shock_file << vcl_endl;
      return false;
    }

    vcl_string query_shock_file;
    if (params_.use_object_shock_query_()) {
      query_shock_file = inp.query_path + "/" + inp.query_name + ".esf";
    } else {
      query_shock_file = params_.shock_folder_assoc_query_() + "/" + 
        params_.shock_folder_assoc_query_.file_type() + "/" + inp.query_name + "/" + inp.query_name + ".esf";
    }

    if (!vul_file::exists(query_shock_file)) {
      vcl_cout << "dborl_patch_match::process() - cannot find shock: " << query_shock_file << vcl_endl;
      return false;
    }

    //: load shock graphs and create the path finders
    dbsk2d_xshock_graph_fileio file_io;
    dbsk2d_shock_graph_sptr proto_sg = file_io.load_xshock_graph(proto_shock_file);
    if (!proto_sg) {
      vcl_cout << "dborl_patch_match::process() - cannot load shock: " << proto_shock_file << vcl_endl;
      return false;
    }

    dbskr_shock_path_finder proto_f(proto_sg);
    if (!proto_f.construct_v()) {
      vcl_cout << "dborl_patch_match::process() - cannot construct v from shock: " << proto_shock_file << vcl_endl;
      return false;
    }

    dbsk2d_shock_graph_sptr query_sg = file_io.load_xshock_graph(query_shock_file);
    if (!query_sg) {
      vcl_cout << "dborl_patch_match::process() - cannot load shock: " << query_shock_file << vcl_endl;
      return false;
    }

    dbskr_shock_path_finder query_f(query_sg);
    if (!query_f.construct_v()) {
      vcl_cout << "dborl_patch_match::process() - cannot construct v from shock: " << query_shock_file << vcl_endl;
      return false;
    }

    vsol_box_2d_sptr box;

    if (params_.detection_params_.use_paths_()) {
      if (!match->detect_instance_using_paths(box, proto_f, query_f, 
        params_.detection_params_.sim_threshold_(), 
        params_.detection_params_.upper_sim_threshold_(), 1.0f, 1.0f, edit_params, 
        true, params_.detection_params_.use_approx_path_cost_(), 
        params_.detection_params_.impose_geom_cons_(), 
        params_.detection_params_.geom_threshold_(),
        params_.detection_params_.alpha_())) {
        vcl_cout << "dborl_patch_match::process() - detection error for the match file:" << out_name << "!!!!\n";
        return false;
      }
    } else {
      if (!match->detect_instance(box, proto_f, query_f, 
        params_.detection_params_.sim_threshold_(), 
        params_.detection_params_.upper_sim_threshold_(), 1.0f, 1.0f, edit_params, true, params_.detection_params_.alpha_())) {
        vcl_cout << "dborl_patch_match::process() - detection error for the match file:" << out_name << "!!!!\n";
        return false;
      }
    }
    det_boxes.push_back(box);
  }

  if (params_.evaluate_params_.do_categorization_()) {  // if a detection is declared assign category of the model to this query, check TP and FP in this case
    
    vsol_box_2d_sptr box;
    if (det_boxes.size()>0)
      box = det_boxes[0];
    
    if (box) {  // algorithm said positive
      if (inp.query_id->category_exists(inp.proto_id->get_first_category())) {// ground truth is positive
        f.set_values(1, 0, 0, 0);
      } else { 
        f.set_values(0, 1, 0, 0);
      }
    } else {    // algorithm said negative
      if (inp.query_id->category_exists(inp.proto_id->get_first_category())) {// ground truth is positive
        f.set_values(0, 0, 0, 1);
      } else {
        f.set_values(0, 0, 1, 0);
      }
    }
    
  } else {  // detection, we do not know N- in this case. so ROC cannot be plotted
    //: check each gt_box of the model_category in this query image
    vsol_box_2d_sptr box;
    if (det_boxes.size()>0)
      box = det_boxes[0];

    dborl_exp_stat instance_stat;
    vsol_box_2d_sptr gt_box = dborl_evaluation_evaluate_detection(instance_stat, inp.proto_id->get_first_category(), 
      inp.query_id, box, params_.evaluate_params_.box_overlap_ratio_threshold_());

    //if (gt_box)
     // vcl_cout << "\t\t gt_box: " << *gt_box << vcl_endl;

    //instance_stat.print_only_stats();

    
    f.set_values(instance_stat.TP_, instance_stat.FP_, instance_stat.TN_, instance_stat.FN_);
  }

  if (params_.save_detections_()) {
    if (!vul_file::exists(det_dir))
      vul_file::make_directory(det_dir);
    vcl_string det_name = det_dir + "/" + inp.proto_name + "/";
    if (!vul_file::exists(det_name))
      vul_file::make_directory(det_name);
    det_name = det_name + inp.query_name + "_eval.out";

    dborl_exp_stat_sptr es = f.get_exp_stat();
    vcl_vector<vcl_string> categories(det_boxes.size(), inp.proto_id->get_first_category());
    print_obj_evaluation(det_name, inp.query_name, det_boxes, categories, *es);
  }

  vcl_cout << "TP: " << f.TP_ << " FP: " << f.FP_ << " TN: " << f.TN_ << " FN: " << f.FN_ << vcl_endl;
  vcl_cout.flush();
  return true;
}

void dborl_patch_match::print_time()
{
  vcl_cout << "\t\t\t total time: " << (t_.real()/1000.0f) << " secs.\n";
  vcl_cout << "\t\t\t total time: " << ((t_.real()/1000.0f)/60.0f) << " mins.\n";
}

//: this method is run on the lead processor once after results are collected from each processor
bool dborl_patch_match::finalize(vcl_vector<dborl_patch_match_output>& results)
{

  params_.percent_completed = 10.0f;
  params_.print_status_xml();

  vcl_map<vcl_string, dborl_exp_stat_sptr> stat_map;
  vcl_string algo_prefix;
  if (params_.detection_params_.use_only_patches_())
    algo_prefix = "patches ";
  else
    algo_prefix = "quads ";

  unsigned Q = query_root_->names().size();
  unsigned P = proto_root_->names().size();

  vcl_string det_dir = vul_file::strip_extension(params_.evaluation_file());

  unsigned cnt = 0;
  for (unsigned i = 0; i < P; i++) {

    vcl_string proto_name = proto_root_->names()[i];
    vcl_string proto_st_file = params_.patch_folder_assoc_prototype_() + "/" + 
      params_.patch_folder_assoc_prototype_.file_type() + "/" + proto_name + "/" +
      proto_name + params_.patch_params_prototype_.output_file_postfix("p_e") + "/" + 
      proto_name + params_.patch_params_prototype_.output_file_postfix("p_e") + "-patch_strg.bin";

    //: find the stats for this proto
    vcl_string name = algo_prefix + proto_root_->names()[i];
    dborl_exp_stat_sptr s = new dborl_exp_stat();
    stat_map[name] = s;

    for (unsigned j = 0; j < Q; j++) {

      vcl_string query_name = query_root_->names()[j];
      vcl_string query_st_file = params_.patch_folder_assoc_query_() + "/" + params_.patch_folder_assoc_query_.file_type() + "/" 
        + query_name + "/" + query_name + params_.patch_params_query_.output_file_postfix("p_e") + "/" 
        + query_name + params_.patch_params_query_.output_file_postfix("p_e") + "-patch_strg.bin";

      if (proto_st_file.compare(query_st_file) == 0) { // proto and query are identical continue
        continue;
      }

      if (params_.compute_one_per_computer_() && params_.use_saved_detections_()) {
        vcl_string det_name = det_dir + "/" + proto_name + "/" + query_name + "_eval.out";
        if (vul_file::exists(det_name)) {  // skip this one already computed
          continue;
        }
      }


      s->increment_TP_by(results[cnt].TP_);
      s->increment_FP_by(results[cnt].FP_);
      s->increment_TN_by(results[cnt].TN_);
      s->increment_FN_by(results[cnt].FN_);
      cnt++;
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

  for (unsigned i = 0; i < P; i++) {
    vcl_string cat = proto_img_d_[i]->get_first_category();
    vcl_string name = algo_prefix + cat;
    vcl_map<vcl_string, dborl_exp_stat_sptr>::iterator it = stat_map.find(name);
    if (it == stat_map.end()) {
      dborl_exp_stat_sptr s_cum = new dborl_exp_stat();
      stat_map[name] = s_cum;
      for (unsigned ii = 0; ii < P; ii++) {
        if (proto_img_d_[ii]->get_first_category().compare(cat) != 0)
          continue;

        vcl_string name_ii = algo_prefix + proto_root_->names()[ii];
        dborl_exp_stat_sptr s = stat_map[name_ii];
        s_cum->increment_TP_by(s->TP_);
        s_cum->increment_FP_by(s->FP_);
        s_cum->increment_TN_by(s->TN_);
        s_cum->increment_FN_by(s->FN_);
      }
    }

  }
 
  params_.print_evaluation_xml(stat_map, true);

  params_.percent_completed = 100.0f;
  params_.print_status_xml();

  return true;
}

#ifdef MPI_CPP_BINDING
MPI::Datatype dborl_patch_match::create_datatype_for_R()
{
  MPI::Datatype dborl_patch_match_output_type;
  //dborl_patch_match_output_type = MPI::FLOAT.Create_contiguous(size_t_*4);
  

  MPI::Datatype type[5] = {MPI::INT, MPI::INT, MPI::INT, MPI::INT, MPI::UB};  // MPI_UB should be added at the end for each type to be safe
  int blocklen[5] = {1, 1, 1, 1, 1};  // number of items from each type
  MPI::Aint disp[5];

  // compute byte displacements of each component, create a dummy instance array
  dborl_patch_match_output dummy[2];
  disp[0] = MPI::Aint(&dummy);
  disp[1] = MPI::Aint(&dummy[0].FP_);
  disp[2] = MPI::Aint(&dummy[0].TN_);
  disp[3] = MPI::Aint(&dummy[0].FN_);
  disp[4] = MPI::Aint(&dummy[1]);
  int base = disp[0];
  for (int i = 0; i < 5; i++) disp[i] -= base;  // get rid of initial address, we only need displacements
  dborl_patch_match_output_type = MPI::Datatype::Create_struct ( 5, blocklen, disp, type);

  dborl_patch_match_output_type.Commit();
  return dborl_patch_match_output_type;
}
#else
MPI_Datatype dborl_patch_match::create_datatype_for_R()
{

  MPI_Datatype dborl_patch_match_output_type;

//  MPI_Type_contiguous(size_t_*4, MPI_FLOAT, &dborl_patch_match_output_type);

  MPI_Datatype type[5] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_UB};  // MPI_UB should be added at the end for each type to be safe
  int blocklen[5] = {1, 1, 1, 1, 1};  // number of items from each type
  MPI_Aint disp[5];

  // compute byte displacements of each component, create a dummy instance array
  dborl_patch_match_output dummy[2];
  disp[0] = MPI_Aint(&dummy);
  disp[1] = MPI_Aint(&dummy[0].FP_);
  disp[2] = MPI_Aint(&dummy[0].TN_);
  disp[3] = MPI_Aint(&dummy[0].FN_);
  disp[4] = MPI_Aint(&dummy[1]);
  int base = disp[0];
  for (int i = 0; i < 5; i++) disp[i] -= base;  // get rid of initial address, we only need displacements
  MPI_Type_struct ( 5, blocklen, disp, type, &dborl_patch_match_output_type);

  MPI_Type_commit ( &dborl_patch_match_output_type );
  return dborl_patch_match_output_type;
}
#endif

