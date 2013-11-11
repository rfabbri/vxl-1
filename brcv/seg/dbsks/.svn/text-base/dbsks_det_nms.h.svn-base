// This is dbsks/pro/dbsks_det_nms.h
#ifndef dbsks_det_nms_h_
#define dbsks_det_nms_h_

//:
// \file
// \brief Non-max suppression for detection results
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Feb 4, 2009
//
// \verbatim
//  Modifications
// \endverbatim




#include <dbsks/dbsks_det_desc_xgraph_sptr.h>
#include <vcl_vector.h>




//: Non-max supression on list of detection based the polygon of the detected xgraphs
// Output is a list of detections that survive, sorted in descending confidence level.
bool dbsks_det_nms_using_polygon(const vcl_vector<dbsks_det_desc_xgraph_sptr >& input_det_list,
                                 vcl_vector<dbsks_det_desc_xgraph_sptr >& output_det_list,
                                 double min_overlap_ratio = 0.5);



#endif
