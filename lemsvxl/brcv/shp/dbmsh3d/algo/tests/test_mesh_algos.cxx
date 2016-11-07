// This is dbmsh3d/tests/test_file_io.cxx
// \author Nhon Trinh
// \date June 13, 2006

#include <vcl_iostream.h>
#include <testlib/testlib_test.h>
#include <dbmsh3d/algo/dbmsh3d_mesh_algos.h>
#include <dbmsh3d/algo/dbmsh3d_fileio.h>

void test_mesh_boundary()
{
  // test extracting 2-manifold mesh boundary
  vcl_string cube_wo_base_ply = "C:/projects/lemsvxl/src/brcv/shp/dbmsh3d/tests/test_data/cube_without_bases.ply";

  dbmsh3d_mesh mesh;
  dbmsh3d_load_ply (&mesh, cube_wo_base_ply.c_str());
  mesh.IFS_to_MHE();
  ///mesh.build_IFS_mesh ();

  vcl_vector<vcl_vector<dbmsh3d_vertex* > > boundary_list;
  bool success = dbmsh3d_mesh_algos::find_boundary(mesh, boundary_list);
  for (unsigned int i=0; i<boundary_list.size(); ++i)
  {
    vcl_cout << "Boundary " << i << " vertices = ";
    vcl_vector<dbmsh3d_vertex* > boundary = boundary_list[i];
    for (unsigned int j=0; j<boundary.size(); ++j)
    {
      dbmsh3d_vertex* v = boundary[j];
      vcl_cout << "  " << v->id();
    }
    vcl_cout << "\n";
  }

  return;
}



//: Test functions of biarc class
MAIN( test_mesh_algos )
{
  START ("Test mesh algos");
  test_mesh_boundary();
  SUMMARY();
}
