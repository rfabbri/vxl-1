/*
 * dbdet_third_order_3d_edge_detector_process_utils.h
 *
 *  Created on: Oct 24, 2011
 *      Author: firat
 */

#ifndef DBDET_THIRD_ORDER_3D_EDGE_DETECTOR_PROCESS_UTILS_H_
#define DBDET_THIRD_ORDER_3D_EDGE_DETECTOR_PROCESS_UTILS_H_

#include<vil3d/vil3d_image_view.h>
#include<vcl_vector.h>

void dbdet_compute_effective_block_size_and_number(int n, int bs, int& num, int*& cumsum_ebs_vec);
bool dbdet_compute_rois(const vil3d_image_view<double>& image, vcl_vector<vcl_vector<int> >& rois, int block_size);


#endif /* DBDET_THIRD_ORDER_3D_EDGE_DETECTOR_PROCESS_UTILS_H_ */
