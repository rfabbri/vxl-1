// This is brcv/rec/dbskr/algo/dborl_utilities.h
#ifndef dborl_utilities_h_
#define dborl_utilities_h_
//:
// \file
// \brief Utilities for dborl applications
// \author Ozge C. Ozcanli
//
// \verbatim
//  Modifications
//   O.C.Ozcanli  10/15/2007   
//
// \endverbatim 

#include <vcl_map.h>
#include <vcl_vector.h>
#include <vcl_utility.h>
#include <vcl_string.h>

#include <dborl/dborl_category_info_sptr.h>
#include <dborl/dborl_evaluation.h>
#include <dborl/algo/dborl_algo_params.h>

#include <bpro1/bpro1_parameters.h>
#include <dborl/dborl_parameter.h>
#include <bpro1/bpro1_process.h>

//: simple parse: put each string in the file into the strings vector
bool parse_strings_from_file(vcl_string fname, vcl_vector<vcl_string>& strings);
bool parse_lines_from_file(vcl_string fname, vcl_vector<vcl_string>& strings);

//: return the id in the categories vector for the category one of whose prefixes matches the object name
int dborl_get_category(vcl_string object_name, vcl_vector<dborl_category_info_sptr>& cats);

bool parse_evaluation_file(vcl_string fname, vcl_map<vcl_string, dborl_exp_stat_sptr>& category_statistics, vcl_string& algo_name);

//: print the evaluation result of an instance along with the detected boxes
bool print_obj_evaluation(vcl_string out_file, vcl_string obj_name, vcl_vector<vsol_box_2d_sptr>& detected_boxes, vcl_vector<vcl_string>& categories, dborl_exp_stat& stat);
bool parse_obj_evaluation(vcl_string out_file, vcl_string& obj_name, vcl_vector<vsol_box_2d_sptr>& detected_boxes, vcl_vector<vcl_string>& categories, dborl_exp_stat& stat);

dborl_parameter_base* convert_parameter_from_bpro1(vcl_string prefix, vcl_string prefix_desc, bpro1_param* param);

void set_process_parameters_of_bpro1(dborl_algo_params& algo_params, bpro1_process& pro, vcl_string algo_abbreviation);

//: Set parameters of a process using a list of parameters
void dborl_set_params_of_bpro1_process(const vcl_vector<dborl_parameter_base* >& param_list, 
                                       const vcl_string param_group_name,  
                                       const bpro1_parameters_sptr& bpro1_params);

#endif // dborl_utilities_h_

