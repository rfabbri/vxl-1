#include <testlib/testlib_register.h>

DECLARE( test_ishock_belm );

DECLARE( test_file_load_bnd );
DECLARE( test_file_load_txt );
DECLARE( test_file_load_con );
DECLARE( test_file_load_edg );
DECLARE( test_file_load_cem );
DECLARE( test_bnd_vertex );
DECLARE( test_bnd_edge );
DECLARE( test_bnd_contour );
DECLARE( test_bnd_utils );
DECLARE( test_boundary_inputs );
DECLARE( test_boundary_bounding_box );
DECLARE( test_boundary_partition );
DECLARE( test_dbsk2d_distance );
DECLARE( test_dbsk2d_closest_point );
DECLARE( test_dbsk2d_exceptions );

void
register_tests()
{
  REGISTER( test_ishock_belm );
  REGISTER( test_file_load_bnd );
  REGISTER( test_file_load_txt );
  REGISTER( test_file_load_con );
  REGISTER( test_file_load_edg );
  REGISTER( test_file_load_cem );
  REGISTER( test_bnd_vertex );
  REGISTER( test_bnd_edge );
  REGISTER( test_bnd_contour );
  REGISTER( test_bnd_utils );
  REGISTER( test_boundary_inputs );
  REGISTER( test_boundary_bounding_box );  
  REGISTER( test_boundary_partition );
  REGISTER( test_dbsk2d_distance );
  REGISTER( test_dbsk2d_closest_point );
  REGISTER( test_dbsk2d_exceptions );

}

DEFINE_MAIN;
