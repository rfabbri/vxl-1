// This is dbsks/algo/dbsks_vox_utils.h
#ifndef dbsks_vox_utils_h_
#define dbsks_vox_utils_h_

//:
// \file
// \brief 
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date July 3, 2008
//
// \verbatim
//  Modifications
// \endverbatim


#include <dbsks/dbsks_det_desc_xgraph_sptr.h>
#include <vil/vil_image_view.h>
#include <vector>
#include <string>
#include <dbsks/dbsks_xshock_utils.h>


//: Save a list of xshock detections to a folder
bool dbsks_save_detections_to_folder(const std::vector<dbsks_det_desc_xgraph_sptr >& det_list, 
                                     const std::string& image_name,
                                     const std::string& model_category,
                                     const std::string& det_group_id,
                                     const vil_image_view<vxl_byte >& source_image,
                                     const std::string& output_folder,
                                     const std::string& det_record_dirname);

//: Load detection results from an a folder
bool dbsks_load_detections_from_folder(const std::string& storage_folder,
                                       std::vector<dbsks_det_desc_xgraph_sptr >& records);



#endif
