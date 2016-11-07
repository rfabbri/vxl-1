#include <testlib/testlib_register.h>


DECLARE( dbmsh3d_test_face_geom );
DECLARE( test_textured_mesh );
DECLARE( test_mesh_break_face );
DECLARE( dbmsh3d_mesh_tri_test );

void register_tests()
{
  REGISTER( dbmsh3d_test_face_geom );
  REGISTER( test_textured_mesh );
  REGISTER( test_mesh_break_face );
  // REGISTER( dbmsh3d_mesh_tri_test );
}

DEFINE_MAIN;


