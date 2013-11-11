// This is /lemsvxl/contrib/firat/dbdet_contour_tracer/dbdet_trace_contours_on_grid.h.

// \file
// \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
// \date Aug 12, 2011

#ifndef DBDET_TRACE_CONTOURS_ON_GRID_H_
#define DBDET_TRACE_CONTOURS_ON_GRID_H_

#include "dbdet_contour_tracer_grid.h"
#include <vsol/vsol_point_2d_sptr.h>

void dbdet_trace_all_contours_on_grid(dbdet_contour_tracer_grid& grid, vcl_vector<vcl_vector<vsol_point_2d_sptr> >& contours);
dbdet_contour_tracer_xing* dbdet_trace_contour_given_first_two_points(dbdet_contour_tracer_grid& grid, dbdet_contour_tracer_xing* prev, dbdet_contour_tracer_xing* curr, vcl_vector<vsol_point_2d_sptr>& contour);

#endif /* DBDET_TRACE_CONTOURS_ON_GRID_H_ */
