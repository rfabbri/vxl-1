// This is sel_examine_params.h
#ifndef sel_examine_params_h
#define sel_examine_params_h
//:
//\file
//\brief 
//\author Amir Tamrakar
//\date 11/19/06
//
//\verbatim
//  Modifications
//\endverbatim

#include <vector>
#include <string>

#include <vil/vil_image_view.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>

#include <dbdet/sel/dbdet_edgel.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>

bool saveEDG(std::string filename, dbdet_edgemap_sptr edgemap);

bool saveCEM (std::string filename, std::vector< vsol_spatial_object_2d_sptr > & vsol_list);

#endif // sel_examine_params_h
