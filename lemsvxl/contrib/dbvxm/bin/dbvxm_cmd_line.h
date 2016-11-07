//:
// \file
// \brief command line utilities for dbvxm experiments
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 04/29/08
//      
// \verbatim
//   Modifications
//  
// \endverbatim


#if !defined(_DBVXM_CMD_LINE_H)
#define _DBVXM_CMD_LINE_H

#include <vcl_ctime.h>
#include <vcl_algorithm.h>
#include <vcl_iostream.h>
#include <vcl_string.h>

//: also prepares the command file
bool prepare_cameras_for_eth80(vcl_string output_prefix);

bool prepare_joint_hist(vcl_string image_name, vcl_string changes_file);

bool collect_statistics_for_glitch_area(vcl_string mask_name, vcl_string prob_map_name, vcl_string glitch_img_name,
                                        float measurement_max, int measurement_bin_cnt, int neighborhood_size, bool not, bool fit_weibull);

#endif  //_DBVXM_COMMAND_LINE_H
