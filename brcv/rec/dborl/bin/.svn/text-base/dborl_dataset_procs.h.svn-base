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

#include <vcl_ctime.h>
#include <vcl_algorithm.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <dborl/dborl_dataset_sptr.h>

bool write_def_params(vcl_string bnd_param_file, vcl_string shock_param_file);

bool prepare_ethz_boundaries(vcl_string index_file, 
                             vcl_string image_ext, vcl_string bnd_param_xml, vcl_string sh_param_xml, bool check_existence);

bool prepare_match_html(vcl_string model_image_file, vcl_string model_st_file, 
                        vcl_string query_image_file, vcl_string query_st_file, 
                        vcl_string match_file, vcl_string detection_param_xml, vcl_string out_name, vcl_string width_str);

bool batch_convert(vcl_string input_dir);

bool detect_instance(vcl_string model_image_file, vcl_string model_st_file, 
                        vcl_string query_image_file, vcl_string query_st_file, 
                        vcl_string match_file, vcl_string detection_param_xml, vcl_string out_name, vcl_string width_str);

bool save_patch_images(vcl_string image_file, vcl_string st_file, vcl_string out_name);


#endif  //_DBORL_CMD_LINE_DATASET_PROCS_H
