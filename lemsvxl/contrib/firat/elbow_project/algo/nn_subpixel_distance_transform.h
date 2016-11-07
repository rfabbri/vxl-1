/*
 * nn_subpixel_distance_transform.h
 *
 *  Created on: Nov 14, 2011
 *      Author: firat
 */

#ifndef NN_SUBPIXEL_DISTANCE_TRANSFORM_H_
#define NN_SUBPIXEL_DISTANCE_TRANSFORM_H_

#include <vcl_vector.h>
#include <vil/vil_image_view.h>
#include "dbdet_third_order_3d_edge_detector.h"

bool nn_subpixel_distance_transform(vcl_vector<dbdet_3d_edge_sptr>& edges, vil_image_view<double>& DT,
		int ni, int nj, int slice_index, double hx, double hy, double dist_thresh);


#endif /* NN_SUBPIXEL_DISTANCE_TRANSFORM_H_ */
