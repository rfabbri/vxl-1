//:
// \file
// \brief
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/30/07

//
//

#include "dborl_detect_patch_processor.h"
#include <bxml/bxml_read.h>
#include <bxml/bxml_write.h>
#include <bxml/bxml_find.h>
#include <dborl/dborl_index.h>
#include <dborl/dborl_index_node.h>
#include <dborl/dborl_image_description.h>
#include <dbskr/algo/dbskr_rec_algs.h>
#include <dbskr/algo/dbskr_shock_patch_match.h>
#include <dbskr/pro/dbskr_shock_patch_storage.h>
#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>
#include <dborl/algo/dborl_utilities.h>
#include <dborl/algo/dborl_index_parser.h>
#include <dborl/algo/dborl_image_desc_parser.h>
#include <dborl/dborl_evaluation.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_algorithm.h>
#include <vul/vul_arg.h>
#include <vul/vul_file.h>

//: this method is run on each processor after lead processor broadcasts its command
//  line arguments to all the processors since only on the lead processor is passed the command line arguments by mpirun
bool dborl_detect_patch_processor::parse_command_line(vcl_vector<vcl_string>& argv)
{
  vul_arg_info_list arg_list;
  vul_arg<vcl_string> input_xml_file(arg_list,"-x","the input file in xml format that sets parameters of the algorithm","");
  vul_arg<bool> print_usage_only(arg_list,"-usage", "print usage info and exit",false);
  vul_arg<bool> print_help(arg_list,"-help", "print usage info and exit",false);
  vul_arg<bool> print_def_xml(arg_list,"-print-def-xml", "print input.xml file with default params and exit",false);  
  vul_arg_include(arg_list);

  for (unsigned i = 1; i < argv.size(); i++) {
        vcl_string arg = argv[i];
        if (arg == vcl_string ("-x")) { 
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
    dborl_detect_patch_processor::print_default_file(input.c_str());
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
bool dborl_detect_patch_processor::parse(const char* param_file)
{
  param_doc_ = bxml_read(param_file);
  if (!param_doc_.root_element())
    return false;
  
  if (param_doc_.root_element()->type() != bxml_data::ELEMENT) {
    vcl_cout << "params root is not ELEMENT\n";
    return false;
  }

  if (!params_.parse_from_data(param_doc_.root_element()))
    return false;

  index_file_ = params_.index_file_;

  size_t_ = 0;
  for (float t = params_.min_thres_; t < params_.max_thres_; t += params_.thres_inc_) {
    vcl_cout << " ";  // dummy line because of the compiler error on smarteyes side, when there is a single line here, it computes size_t_ one more than it should be
    size_t_ = size_t_ + 1;
  }

  vcl_cout << "there are " << size_t_ << " thresholds in the interval given\n";
  
  return parse_index(index_file_);
}

//: this method is run on each processor
bool dborl_detect_patch_processor::parse_index(vcl_string index_file)
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

//: this method prints an xml input file setting all the parameters to defaults
//  run the algorithm to generate this file, then modify it  
void dborl_detect_patch_processor::print_default_file(const char* def_file)
{
  bxml_document doc;
  bxml_element * root = new bxml_element("dborl_detect_patch_processor");
  doc.set_root_element(root);

  bxml_element * data1 = params_.create_default_document_data();
  root->append_data(data1);
  root->append_text("\n");
  
  bxml_write(vcl_string(def_file), doc);
}

//: this method is run on each processor
bool dborl_detect_patch_processor::initialize(vcl_vector<dborl_detect_patch_processor_input>& input_vec)
{
  vcl_string storage_end = "patch_strg.bin";
  vcl_string model_storage_file = params_.model_dir_ + params_.model_name_ + params_.model_patch_params_.output_file_postfix_ + "-" + storage_end;

  if (!vul_file::exists(model_storage_file)) {
    vcl_cout << "dborl_detect_patch_processor::initialize() - " << model_storage_file << " not found\n";
    return false;
  }

  //: load the model
  dbskr_shock_patch_storage_sptr model_st = dbskr_shock_patch_storage_new();
  vsl_b_ifstream ifs(model_storage_file.c_str());
  model_st->b_read(ifs);
  ifs.close();

  //: load the shock graphs --> assumes each shock graph for each patch is saved in the same folder
  //vcl_cout << model_st->size() << " patches in model storage, reading shocks..\n";
  dbsk2d_xshock_graph_fileio loader;
  //: load esfs for each patch
  for (unsigned iii = 0; iii < model_st->size(); iii++) {
    dbskr_shock_patch_sptr sp = model_st->get_patch(iii);
    vcl_string patch_esf_name = model_storage_file.substr(0, model_storage_file.length()-storage_end.size());
    vcl_ostringstream oss;
    oss << sp->id();
    patch_esf_name = patch_esf_name+oss.str()+".esf";
    dbsk2d_shock_graph_sptr sg = loader.load_xshock_graph(patch_esf_name);
    sp->set_shock_graph(sg);
    sp->set_tree_parameters(params_.shock_match_params_.elastic_splice_cost_, 
                            params_.shock_match_params_.circular_ends_,
                            params_.shock_match_params_.combined_edit_, 
                            params_.shock_match_params_.scurve_sample_ds_, 
                            params_.shock_match_params_.scurve_interpolate_ds_); // so that prepare_tree() and tree() methods of patch have the needed params
  }
  //vcl_string model_gt_file = params_.model_dir_ + "groundtruth.xml";
  vcl_string model_gt_file = params_.model_dir_ + params_.model_name_ + ".xml";
  if (!vul_file::exists(model_gt_file)) {
    vcl_cout << "dborl_detect_patch_processor::initialize() - " << model_gt_file << " not found\n";
    return false;
  }
    
  dborl_image_desc_parser parser;
  dborl_image_description_sptr model_id = dborl_image_description_parse(model_gt_file, parser);
  if (!model_id->has_single_category()) {
    vcl_cout << "dborl_detect_patch_processor::initialize() - model image description has more than one category! image was not a valid map for a model\n";
    return false;
  }
  model_category_ = model_id->get_first_category();
  
  //: load each storage given in the root node of the index
  vcl_vector<vcl_string>& names = ind_->root_->cast_to_index_node()->names();
  vcl_vector<vcl_string>& paths = ind_->root_->cast_to_index_node()->paths();
  if (!names.size() || names.size() != paths.size()) {
    vcl_cout << "dborl_detect_patch_processor::initialize() - " << params_.index_file_ << " has problems\n";
  }

  vcl_vector<dbskr_shock_patch_storage_sptr> test_str;
  vcl_vector<dborl_image_description_sptr> test_desc;
  //: find the number of positive and negative instances in the test set
  positive_cnt_ = 0;
  negative_cnt_ = 0;
  for (unsigned i = 0; i < names.size(); i++) {  
    //if (names[i].compare(params_.model_name_) == 0 && paths[i].compare(params_.model_dir_) == 0) {
    if (names[i].compare(params_.model_name_) == 0) {
      vcl_cout << "skipping model instance\n";
      continue;
    }

    vcl_string st_file = paths[i] + "/" + names[i] + params_.query_patch_params_.output_file_postfix_ + "/" + names[i] + params_.query_patch_params_.output_file_postfix_ + "-" + storage_end;
    //vcl_string st_file = paths[i] + "/" + names[i] + params_.query_patch_params_.output_file_postfix_ + "-" + storage_end;
    if (!vul_file::exists(st_file)) {
      vcl_cout << "dborl_detect_patch_processor::initialize() - " << st_file << " not found\n";
      return false;
    }
    
    dbskr_shock_patch_storage_sptr st = dbskr_shock_patch_storage_new();
    vsl_b_ifstream ifs(st_file.c_str());
    st->b_read(ifs);
    ifs.close();

    //: load the shock graphs --> assumes each shock graph for each patch is saved in the same folder
    //vcl_cout << st->size() << " patches in test storage, reading shocks..\n";
    //: load esfs for each patch
    for (unsigned iii = 0; iii < st->size(); iii++) {
      dbskr_shock_patch_sptr sp = st->get_patch(iii);
      vcl_string patch_esf_name = st_file.substr(0, st_file.length()-storage_end.size());
      vcl_ostringstream oss;
      oss << sp->id();
      patch_esf_name = patch_esf_name+oss.str()+".esf";
      dbsk2d_shock_graph_sptr sg = loader.load_xshock_graph(patch_esf_name);
      sp->set_shock_graph(sg);
      sp->set_tree_parameters(params_.shock_match_params_.elastic_splice_cost_, 
                              params_.shock_match_params_.circular_ends_,
                              params_.shock_match_params_.combined_edit_, 
                              params_.shock_match_params_.scurve_sample_ds_, 
                              params_.shock_match_params_.scurve_interpolate_ds_);  // so that prepare_tree() and tree() methods of patch have the needed params
    }
    
    test_str.push_back(st);

    //vcl_string gt_file = paths[i] + "groundtruth.xml"; 
    vcl_string gt_file = paths[i] + names[i] + ".xml"; 
    if (!vul_file::exists(gt_file)) {
      vcl_cout << "dborl_detect_patch_processor::initialize() - " << gt_file << " not found\n";
      return false;
    }
    
    //vcl_cout << "query description file: " << gt_file << vcl_endl;
    dborl_image_desc_parser parser;
    dborl_image_description_sptr id = dborl_image_description_parse(gt_file, parser);
    if (id->category_exists(model_category_)) {
      positive_cnt_++;
      if (id->get_category_cnt(model_category_) != 1) {
        vcl_cout << "WARNING: there are more than one instance of the model category in this test image, but increased the positive_cnt only by one.\n";
        vcl_cout << "MODIFY this section if you want different behavior in counting the number of positive instances\n";
      }
    } else 
      negative_cnt_++;
    test_desc.push_back(id);
    //if (id->has_single_category())
    //  vcl_cout << "query description has a single category: " << id->get_first_category() << vcl_endl;
  }

  vcl_cout << "there are " << positive_cnt_ << " pos instances and " << negative_cnt_ << " neg instances\n";

  query_set_size_ = positive_cnt_ + negative_cnt_;

  //: initialize the input vector for each pair
  unsigned str_cnt = 0;
  for (unsigned int i = 0; i<names.size(); i++) { 
    //if (names[i].compare(params_.model_name_) == 0 && paths[i].compare(params_.model_dir_) == 0) 
    if (names[i].compare(params_.model_name_) == 0) 
      continue;

    for (float t = params_.min_thres_; t < params_.max_thres_; t += params_.thres_inc_) {
      dborl_detect_patch_processor_input inp(model_st, 
                                             test_str[str_cnt],
                                             params_.model_name_ + params_.model_patch_params_.output_file_postfix_, 
                                             names[i] + params_.query_patch_params_.output_file_postfix_, t, test_desc[str_cnt]);
      input_vec.push_back(inp);
    }
    str_cnt++;
  }

  return true;
}

//: this method is run in a distributed mode on each processor on the cluster
bool dborl_detect_patch_processor::process(dborl_detect_patch_processor_input inp, dborl_detect_patch_processor_output& f)
{
  //vcl_cout << "query: " << inp.query_name << "\n";
  //: load the match instance which contains model vs query patch matches
  vcl_string match_name = params_.matches_dir_ + inp.model_name + "-" + inp.query_name + params_.shock_match_params_.output_file_postfix_ + ".bin";
  if (!vul_file::exists(match_name.c_str())) {
    vcl_cout << "dborl_detect_patch_processor::process() - the file:" << match_name << " not found!\n";
    return false;
  }
  //vcl_cout << "\t loaded match.. ";

  dbskr_shock_patch_match_sptr match = new dbskr_shock_patch_match();
  vsl_b_ifstream ifs(match_name.c_str());
  match->b_read(ifs);
  ifs.close();

  //: prepare id maps for this match
  vcl_map<int, dbskr_shock_patch_sptr> model_map;
  for (unsigned ii = 0; ii < inp.model_st->size(); ii++) 
    model_map[inp.model_st->get_patch(ii)->id()] = inp.model_st->get_patch(ii);
  match->set_id_map1(model_map);

  vcl_map<int, dbskr_shock_patch_sptr> query_map;
  for (unsigned ii = 0; ii < inp.query_st->size(); ii++) 
    query_map[inp.query_st->get_patch(ii)->id()] = inp.query_st->get_patch(ii);
  match->set_id_map2(query_map);

  if (params_.use_normalized_costs_) {
    if (params_.use_reconst_boundary_length_)
      if (!match->compute_length_norm_costs_of_cors()) { // sorts the matches after recomputation of the normalized cost
        vcl_cout << "dborl_detect_patch_processor::process() - sorting error for the match file:" << match_name << "!!!!\n";
        return false;
      }
    else if (params_.use_total_splice_cost_)
      if (!match->compute_splice_norm_costs_of_cors()) {// sorts the matches after recomputation of the normalized cost
        vcl_cout << "dborl_detect_patch_processor::process() - sorting error for the match file:" << match_name << "!!!!\n";
        return false;
      }
  }
  //vcl_cout << " prepared id maps.. ";

  dborl_exp_stat instance_stat;
  
  vsol_box_2d_sptr box;
  if (!match->detect_instance(box, params_.det_params_.top_N_, params_.det_params_.k_, inp.threshold)) {
    vcl_cout << "dborl_detect_patch_processor::process() - detection error for the match file:" << match_name << "!!!!\n";
    return false;
  }

  //vcl_cout << " ran detection algo.. \n";
  
  //if (box)
  //  vcl_cout << "\t\t model: " << inp.model_name << " detected box: " << *box  << "\n";
  //else
  //  vcl_cout << "\t\t model: " << inp.model_name << " did not detect a box\n";

  if (params_.do_categorization_) {  // if a detection is declared assign category of the model to this query, check TP and FP in this case
    if (box) {  // algorithm said positive
      if (inp.query_desc->category_exists(model_category_)) // ground truth is positive
        f.set_values(1, 0, 0, 0);
      else 
        f.set_values(0, 1, 0, 0);
    } else {    // algorithm said negative
      if (inp.query_desc->category_exists(model_category_)) // ground truth is positive
        f.set_values(0, 0, 0, 1);
      else
        f.set_values(0, 0, 1, 0);
    }
    
  } else {  // detection, we do not know N- in this case. so ROC cannot be plotted
    //: check each gt_box of the model_category in this query image
    vsol_box_2d_sptr gt_box = dborl_evaluation_evaluate_detection(instance_stat, model_category_, 
                          inp.query_desc, box, params_.box_overlap_ratio_threshold_);

    //if (gt_box)
     // vcl_cout << "\t\t gt_box: " << *gt_box << vcl_endl;

    //instance_stat.print_only_stats();

    
    f.set_values(instance_stat.TP_, instance_stat.FP_, instance_stat.TN_, instance_stat.FN_);
  }

  return true;
}

void dborl_detect_patch_processor::print_time()
{
  vcl_cout << "\t\t\t total time: " << (t_.real()/1000.0f) << " secs.\n";
  vcl_cout << "\t\t\t total time: " << ((t_.real()/1000.0f)/60.0f) << " mins.\n";
}

//: this method is run on the lead processor once after results are collected from each processor
bool dborl_detect_patch_processor::finalize(vcl_vector<dborl_detect_patch_processor_output>& results)
{
  vcl_cout << "in finalize method!!!!!!!!!!!!!!!\n";
  vcl_cout.flush();

  vcl_cout << "size_t_: " << size_t_ << " query_set_size_: " << query_set_size_ << " results size: " << results.size() << "\n";
  vcl_cout << "size_t_*query_set_size_ = " << size_t_*query_set_size_ << vcl_endl;
  vcl_cout.flush();

  if (! ((size_t_*query_set_size_) == results.size()) )
    return false;

  vcl_cout << "finalizing..results have: " << results.size() << vcl_endl;
  vcl_cout.flush();

  //: collect statistics for each threshold
  vcl_vector<dborl_exp_stat_sptr> thres_stats(size_t_, 0);

  unsigned result_ind = 0;
  for (unsigned int i = 0; i<query_set_size_; i++) { 
    for (unsigned t = 0; t < size_t_; t++) {
      if (!thres_stats[t])
        thres_stats[t] = new dborl_exp_stat(positive_cnt_, negative_cnt_);

      if (results[result_ind].FP_)
        thres_stats[t]->increment_FP();
      else if (results[result_ind].TP_)
        thres_stats[t]->increment_TP();
      else if (results[result_ind].FN_)
        thres_stats[t]->increment_FN();
      else if (results[result_ind].TN_)
        thres_stats[t]->increment_TN();

      result_ind++;
    }
  }

  float thres = params_.min_thres_;
  for (unsigned t = 0; t < size_t_; thres += params_.thres_inc_, t++) {
    vcl_cout << "stats for threshold: " << thres << vcl_endl;
    thres_stats[t]->print_stats();
    vcl_cout << "-------------------------------\n";
  }

  vcl_string out_file_name = params_.out_file_ + params_.output_file_postfix_ + params_.det_params_.output_file_postfix_ + ".out";
  vcl_ofstream of(out_file_name.c_str(), vcl_ios::app);
  if (!of) {
    vcl_cout << "out file: " << out_file_name << " could not be opened!\n";
    return false;
  }

  of << params_.model_name_ << ": ";

  thres = params_.min_thres_;
  for (unsigned t = 0; t < size_t_; thres += params_.thres_inc_, t++) {
    of << "stats for threshold: " << thres << vcl_endl;
    thres_stats[t]->print_stats(of);
    of << "-------------------------------\n";
  }
  of.close();

  vcl_string out_file_plot_name = params_.out_file_ + params_.output_file_postfix_ + params_.det_params_.output_file_postfix_ + "-plots.out";
  vcl_ofstream ofp(out_file_plot_name.c_str(), vcl_ios::app);
  if (!ofp) {
    vcl_cout << "out file plot: " << out_file_plot_name << " could not be opened!\n";
    return false;
  }

  //ofp << "# model name: " << params_.model_name_ << " category: " << model_category_ << vcl_endl;
  ofp << params_.model_name_ << vcl_endl;
  ofp << size_t_ << vcl_endl;
  //ofp << "# ROC data, thres range (" << params_.min_thres_ << ", " << params_.max_thres_ << ") inc: " << params_.thres_inc_ << " cnt: " << size_t_ << "\n";
  dborl_evaluation_print_ROC_data(thres_stats, ofp);
  ofp << dborl_evaluation_ROC_EER(thres_stats) << vcl_endl;
  //ofp << "# -------------------------------\n";
  //ofp << "# RPC data, thres range (" << params_.min_thres_ << ", " << params_.max_thres_ << ") inc: " << params_.thres_inc_ << " cnt: " << size_t_ << "\n";
  dborl_evaluation_print_RPC_data(thres_stats, ofp);
  ofp << dborl_evaluation_RPC_EER(thres_stats) << vcl_endl;
  //ofp << "#-------------------------------\n";
  //ofp << "# PRC data, thres range (" << params_.min_thres_ << ", " << params_.max_thres_ << ") inc: " << params_.thres_inc_ << " cnt: " << size_t_ << "\n";
  dborl_evaluation_print_PRC_data(thres_stats, ofp);
  //ofp << "#-------------------------------\n";
  
  ofp.close();

  return true;
}

#ifdef MPI_CPP_BINDING
MPI::Datatype dborl_detect_patch_processor::create_datatype_for_R()
{
  MPI::Datatype dborl_detect_patch_processor_output_type;
  //dborl_detect_patch_processor_output_type = MPI::FLOAT.Create_contiguous(size_t_*4);
  

  MPI::Datatype type[5] = {MPI::INT, MPI::INT, MPI::INT, MPI::INT, MPI::UB};  // MPI_UB should be added at the end for each type to be safe
  int blocklen[5] = {1, 1, 1, 1, 1};  // number of items from each type
  MPI::Aint disp[5];

  // compute byte displacements of each component, create a dummy instance array
  dborl_detect_patch_processor_output dummy[2];
  disp[0] = MPI::Aint(&dummy);
  disp[1] = MPI::Aint(&dummy[0].FP_);
  disp[2] = MPI::Aint(&dummy[0].TN_);
  disp[3] = MPI::Aint(&dummy[0].FN_);
  disp[4] = MPI::Aint(&dummy[1]);
  int base = disp[0];
  for (int i = 0; i < 5; i++) disp[i] -= base;  // get rid of initial address, we only need displacements
  dborl_detect_patch_processor_output_type = MPI::Datatype::Create_struct ( 5, blocklen, disp, type);

  dborl_detect_patch_processor_output_type.Commit();
  return dborl_detect_patch_processor_output_type;
}
#else
MPI_Datatype dborl_detect_patch_processor::create_datatype_for_R()
{

  MPI_Datatype dborl_detect_patch_processor_output_type;

//  MPI_Type_contiguous(size_t_*4, MPI_FLOAT, &dborl_detect_patch_processor_output_type);

  MPI_Datatype type[5] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_UB};  // MPI_UB should be added at the end for each type to be safe
  int blocklen[5] = {1, 1, 1, 1, 1};  // number of items from each type
  MPI_Aint disp[5];

  // compute byte displacements of each component, create a dummy instance array
  dborl_detect_patch_processor_output dummy[2];
  disp[0] = MPI_Aint(&dummy);
  disp[1] = MPI_Aint(&dummy[0].FP_);
  disp[2] = MPI_Aint(&dummy[0].TN_);
  disp[3] = MPI_Aint(&dummy[0].FN_);
  disp[4] = MPI_Aint(&dummy[1]);
  int base = disp[0];
  for (int i = 0; i < 5; i++) disp[i] -= base;  // get rid of initial address, we only need displacements
  MPI_Type_struct ( 5, blocklen, disp, type, &dborl_detect_patch_processor_output_type);

  MPI_Type_commit ( &dborl_detect_patch_processor_output_type );
  return dborl_detect_patch_processor_output_type;
}
#endif


