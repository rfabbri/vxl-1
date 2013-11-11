// This is dbsks/dbsks_io_shapelet_grid.h
#ifndef dbsks_io_shapelet_grid_h
#define dbsks_io_shapelet_grid_h
//:
// \file
// \brief Binary I/O functions for dbsks_dp_match
// \author Nhon Trinh
// \date Dec 1, 2007

#include <vsl/vsl_fwd.h>
#include <dbsks/dbsks_shapelet_grid.h>

// ============================================================================
// shapelet_grid_params
// ============================================================================

//: Binary save an array of matrices to a a stream
void vsl_b_write(vsl_b_ostream & os, const dbsks_shapelet_grid_params& params);

//: Binary load an array of matrices from a stream 
void vsl_b_read(vsl_b_istream & is, dbsks_shapelet_grid_params& params);

// ============================================================================
// shapelet_grid
// ============================================================================

//: Binary save a shapelet_grid to a a stream
void vsl_b_write(vsl_b_ostream & os, const dbsks_shapelet_grid& grid);

//: Binary load a shapelet_grid from a stream 
void vsl_b_read(vsl_b_istream & is, dbsks_shapelet_grid& grid);




#endif // dbsks_io_shapelet_grid_h
