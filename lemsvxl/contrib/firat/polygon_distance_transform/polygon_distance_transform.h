// This is /lemsvxl/contrib/firat/polygon_distance_transform/polygon_distance_transform.h.

// \file
// \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
// \date Aug 14, 2011

#ifndef POLYGON_DISTANCE_TRANSFORM_H_
#define POLYGON_DISTANCE_TRANSFORM_H_

#include <vector>
#include <vnl/vnl_matrix.h>
#include <string>

bool compute_polygon_signed_distance_transform(std::vector<double* >& xv, std::vector<double* >& yv, std::vector<int> num_points, vnl_matrix<double>& phi, int height, int width, double hx, double hy);
bool read_polygon_file(const std::string& polygon_file, std::vector<double* >& xv, std::vector<double* >& yv, std::vector<int>& num_points);

#endif /* POLYGON_DISTANCE_TRANSFORM_H_ */
