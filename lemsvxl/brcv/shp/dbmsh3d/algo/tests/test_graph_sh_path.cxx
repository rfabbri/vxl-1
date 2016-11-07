//: This is dbmsh3d/algo/tests/test_mesh_triangulate.cxx
//  To debug in Visual studio, put argument
//  test_graph_sh_path D:\Projects\BrownEyes\lemsvxlsrc\brcv\shp\dbmsh3d\algo\tests\data
//  all D:\Projects\BrownEyes\lemsvxlsrc\brcv\shp\dbmsh3d\algo\tests\data

#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <testlib/testlib_test.h>

#include <dbmsh3d/algo/dbmsh3d_graph_sh_path.h>
#include <dbmsh3d/algo/dbmsh3d_fileio.h>

MAIN_ARGS (test_graph_sh_path)
{
  START ("Graph Shortest-Path finding via Dijkstra's Algo. on a Mesh.");  

  //Load testM from the test file.
  vcl_string dir_base;
  if (argc >= 2) {
    dir_base = argv[1];
    #ifdef VCL_WIN32
      dir_base += "\\";
    #else
      dir_base += "/";
    #endif
  }
  vcl_string data_prefix = dir_base + "test_sh_path_mesh.ply2";
  dbmsh3d_mesh* testM = new dbmsh3d_mesh;  
  dbmsh3d_load_ply2 (testM, data_prefix.c_str());
  testM->IFS_to_MHE ();

  int src = 0;
  dbmsh3d_vertex* srcV = testM->vertexmap (src);
  int dest = 6;
  dbmsh3d_vertex* destV = testM->vertexmap (dest);

  //Test shortest-path finding.
  vcl_vector<dbmsh3d_edge*> Edges;
  bool result = find_shortest_Es_on_M (testM, srcV, destV, Edges);

  TEST ("find_shortest_Es_on_M successfully finished.", result, true);
  TEST ("shortest path edges E[0].", Edges[0]->id(), 8);
  TEST ("shortest path edges E[1].", Edges[1]->id(), 5);
  TEST ("shortest path edges E[2].", Edges[2]->id(), 0);

  delete testM;

  SUMMARY();
}
