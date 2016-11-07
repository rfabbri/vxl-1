#include <testlib/testlib_register.h>

DECLARE( test_file_io );
DECLARE( test_mesh_algos );

DECLARE( test_mesh_tri );
DECLARE( test_mesh_triangulate );
DECLARE( test_graph_sh_path );
DECLARE( test_mesh_merge_face);

DECLARE( test_pt_set_correspondence );
DECLARE( test_pt_mesh_dist );

void register_tests()
{
  REGISTER( test_file_io );
  REGISTER( test_mesh_algos );

  REGISTER( test_mesh_tri );
  REGISTER( test_mesh_triangulate );
  REGISTER( test_graph_sh_path );
  REGISTER( test_mesh_merge_face );

  REGISTER( test_pt_set_correspondence );
  REGISTER( test_pt_mesh_dist );
}

DEFINE_MAIN;


