/*
 * elbow_edge_clustering.h
 *
 *  Created on: Oct 18, 2011
 *      Author: firat
 */

#ifndef ELBOW_EDGE_CLUSTERING_H_
#define ELBOW_EDGE_CLUSTERING_H_

#include <vcl_vector.h>
#include "dbdet_third_order_3d_edge_detector.h"

bool elbow_get_largest_edge_cluster(const vcl_vector<dbdet_3d_edge_sptr>& edgemap, vcl_vector<dbdet_3d_edge_sptr>& cluster,
		int ni, int nj, int nk, int h);

#endif /* ELBOW_EDGE_CLUSTERING_H_ */
