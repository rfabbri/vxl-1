/*
 * dbdet_3d_edge_io.h
 *
 *  Created on: Oct 21, 2011
 *      Author: firat
 */

#ifndef DBDET_3D_EDGE_IO_H_
#define DBDET_3D_EDGE_IO_H_

#include "dbdet_third_order_3d_edge_detector.h"

bool dbdet_save_3d_edges(const std::vector<dbdet_3d_edge_sptr>& edgemap, const std::string& out_file, bool include_strength = true);
bool dbdet_load_3d_edges(const std::string& in_file, std::vector<dbdet_3d_edge_sptr>& edgemap);

#endif /* DBDET_3D_EDGE_IO_H_ */
