#include <rsdl/rsdl_bins_2d.txx>
#include <vcl_vector.txx>
#include <vbl/vbl_array_2d.txx>

RSDL_BINS_2D_INSTANTIATE( double, double );

typedef rsdl_bins_2d_entry< double, double > bin_T;
typedef vcl_vector< bin_T > vector_T;
VBL_ARRAY_2D_INSTANTIATE( vector_T );
