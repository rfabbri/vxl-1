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

#include <ctime>
#include <algorithm>
#include <iostream>
#include <string>

//: also prepares the command file
bool prepare_cameras_for_eth80(std::string output_prefix);

bool prepare_joint_hist(std::string image_name, std::string changes_file);

bool collect_statistics_for_glitch_area(std::string mask_name, std::string prob_map_name, std::string glitch_img_name,
                                        float measurement_max, int measurement_bin_cnt, int neighborhood_size, bool not, bool fit_weibull);

#endif  //_DBVXM_COMMAND_LINE_H
