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
#include <vcl_vector.h>
#include <vcl_string.h>


//: Save a list of xshock detections to a folder
bool dbsks_save_detections_to_folder(const vcl_vector<dbsks_det_desc_xgraph_sptr >& det_list, 
                                     const vcl_string& image_name,
                                     const vcl_string& model_category,
                                     const vcl_string& det_group_id,
                                     const vil_image_view<vxl_byte >& source_image,
                                     const vcl_string& output_folder,
                                     const vcl_string& det_record_dirname);

//: Load detection results from an a folder
bool dbsks_load_detections_from_folder(const vcl_string& storage_folder,
                                       vcl_vector<dbsks_det_desc_xgraph_sptr >& records);


#endif
