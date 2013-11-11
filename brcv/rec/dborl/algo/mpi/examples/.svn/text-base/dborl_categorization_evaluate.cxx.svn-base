//:
// \file
// \brief 
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/25/07
//
//

#include "dborl_categorization_evaluate.h"
#include "dborl_categorization_evaluate_params.h"
#include <vcl_iostream.h>

#include <dborl/algo/dborl_index_parser.h>
#include <dborl/dborl_index.h>
#include <dborl/dborl_index_node.h>

#include <vul/vul_file.h>

#include <dborl/dborl_evaluation.h>


bool dborl_categorization_evaluate::parse_command_line(int argc, char* argv[])
{
  params_->parse_command_line_args(argc, argv);  // parses the input.xml file if its name is passed from command line
  
  //: always print the params file if an executable to work with ORL web interface
  if (!params_->print_params_xml(params_->print_params_file()))
    vcl_cout << "problems in writing params file to: " << params_->print_params_file() << vcl_endl;

  if (params_->exit_with_no_processing() || params_->print_params_only())
    return false;

  //: set the param_file_ variable just in case
  param_file_ = params_->input_param_filename_;

  return true;
}

//: this method is run on each processor
bool dborl_categorization_evaluate::parse_index(vcl_string index_file)
{
  dborl_index_parser parser;
  parser.clear();

  vcl_FILE *xmlFile = vcl_fopen(index_file.c_str(), "r");
  if (xmlFile == NULL){
    vcl_cout << "dborl_categorization_evaluate::parse_index() -- " << index_file << "-- error on opening" << vcl_endl;
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
bool dborl_categorization_evaluate::parse(const char* param_file)
{
  params_->parse_input_xml();  // the input parameter file name has already been parsed by the parse_command_line_args() method of params
  if (params_->exit_with_no_processing())
    return false;

  return parse_index(params_->index_filename());
}

//: this method prints an xml input file setting all the parameters to defaults
//  run the algorithm to generate this file, then modify it  
void dborl_categorization_evaluate::print_default_file(const char* def_file)
{
  params_->print_default_input_xml(vcl_string(def_file));
}

//: this method is run in a distributed mode on each processor on the cluster
//  load the image and trace contour
bool dborl_categorization_evaluate::process()
{
  vcl_map<vcl_string, dborl_exp_stat_sptr> category_statistics;

  dborl_index_node_sptr root = ind_->root_->cast_to_index_node();
  for (unsigned i = 0; i < root->paths().size(); i++) {
    vcl_string cat_file = root->paths()[i] + "categorization.xml";

     bxml_document param_doc = bxml_read(cat_file);
     if (!param_doc.root_element()) {
        vcl_cout << "dborl_categorization_evaluate::process() -- problems in reading: " << cat_file << vcl_endl;
        return false;
     }
  
     if (param_doc.root_element()->type() != bxml_data::ELEMENT) {
        vcl_cout << "params root is not ELEMENT\n";
        return false;
     }
     
     bxml_element query("category");
     bxml_data_sptr result = bxml_find_by_name(param_doc.root_element(), query);

     if (!result) {
        vcl_cout << "dbskr_match_shock_params::parse_from_match_data() - could not find the node category\n";
        return false;
     }
   
     bxml_element * data = static_cast<bxml_element*>(result.ptr());
     bxml_text * data_text = static_cast<bxml_text*>(data->data_begin()->ptr());
     if (!data_text || data_text->type() != bxml_data::TEXT) {
       vcl_cout << "dbskr_match_shock_params::parse_from_match_data() - could not find the node category\n";
       return false;
     }
     vcl_string category = data_text->data();

     bxml_element query2("gt_category");
     bxml_data_sptr result2 = bxml_find_by_name(param_doc.root_element(), query2);

     if (!result2) {
        vcl_cout << "dbskr_match_shock_params::parse_from_match_data() - could not find the node input_category\n";
        return false;
     }
   
     bxml_element * data2 = static_cast<bxml_element*>(result2.ptr());
     bxml_text * data2_text = static_cast<bxml_text*>(data2->data_begin()->ptr());
     if (!data2_text || data2_text->type() != bxml_data::TEXT) {
       vcl_cout << "dbskr_match_shock_params::parse_from_match_data() - could not find the node input_category\n";
       return false;
     }
     vcl_string gt_category = data2_text->data();

     //: find the exp stat counter for this category 
     vcl_map<vcl_string, dborl_exp_stat_sptr>::iterator cat_iter = category_statistics.find(category);
     dborl_exp_stat_sptr current_stat;
     if (cat_iter == category_statistics.end()) {  // encountered this category for the first time
       current_stat = new dborl_exp_stat();
       category_statistics[category] = current_stat;
     } else
       current_stat = cat_iter->second;

     if (category.compare(gt_category) == 0)   // TP
       current_stat->increment_TP();
     else
       current_stat->increment_FP();


     //: find the exp stat counter for this gt_category 
     cat_iter = category_statistics.find(gt_category);
     dborl_exp_stat_sptr gt_stat;
     if (cat_iter == category_statistics.end()) {  // encountered this category for the first time
       gt_stat = new dborl_exp_stat();
       category_statistics[gt_category] = gt_stat;
     } else
       gt_stat = cat_iter->second;

     gt_stat->increment_positive_cnt();
  }

  //: update the negative cnts for all the categories in this experiment
  for (vcl_map<vcl_string, dborl_exp_stat_sptr>::iterator temp_iter = category_statistics.begin(); temp_iter != category_statistics.end(); temp_iter++) {
    int neg_cnt = 0;
    for (vcl_map<vcl_string, dborl_exp_stat_sptr>::iterator temp_iter2 = category_statistics.begin(); temp_iter2 != category_statistics.end(); temp_iter2++) {
      if (temp_iter == temp_iter2)
        continue;
      neg_cnt += temp_iter2->second->positive_cnt_;
    }
    temp_iter->second->negative_cnt_ = neg_cnt;
  }

  //: print the statistics for all the categories
  for (vcl_map<vcl_string, dborl_exp_stat_sptr>::iterator temp_iter = category_statistics.begin(); temp_iter != category_statistics.end(); temp_iter++) {
    dborl_exp_stat_sptr current_stat = temp_iter->second;
    vcl_cout << "category: " << temp_iter->first << " neg cnt: " << current_stat->negative_cnt_ << " pos cnt: " << current_stat->positive_cnt_;
    vcl_cout << " TPs: " << current_stat->TP_ << " FP: " << current_stat->FP_ << "\n";
    float tpr = current_stat->TPR();
    float fpr = current_stat->FPR();
    params_->perf_map_insert(temp_iter->first, fpr, tpr);
    params_->perf_plot_set_type(dborl_evaluation_plot_type::ROC);
    params_->print_perf_xml("ROC plot: TPR vs FPR");
  }
  
  return true;
}




