//:
// \file
// \brief command line utilities for ORL experiments
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/16/07
//      
// \verbatim
//   Modifications
//  
// \endverbatim


#if !defined(_DBORL_CMD_LINE_DATASET_PROCS_H)
#define _DBORL_CMD_LINE_DATASET_PROCS_H

#include <ctime>
#include <algorithm>
#include <iostream>
#include <string>
#include <dborl/dborl_dataset_sptr.h>

bool write_def_params(std::string bnd_param_file, std::string shock_param_file);

bool prepare_ethz_boundaries(std::string index_file, 
                             std::string image_ext, std::string bnd_param_xml, std::string sh_param_xml, bool check_existence);

bool prepare_match_html(std::string model_image_file, std::string model_st_file, 
                        std::string query_image_file, std::string query_st_file, 
                        std::string match_file, std::string detection_param_xml, std::string out_name, std::string width_str);

bool batch_convert(std::string input_dir);

bool detect_instance(std::string model_image_file, std::string model_st_file, 
                        std::string query_image_file, std::string query_st_file, 
                        std::string match_file, std::string detection_param_xml, std::string out_name, std::string width_str);

bool save_patch_images(std::string image_file, std::string st_file, std::string out_name);


#endif  //_DBORL_CMD_LINE_DATASET_PROCS_H
