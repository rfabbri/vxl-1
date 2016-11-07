// This is dbmsh3d/tests/test_pt_set_correspondence.cxx
// \author Nhon Trinh
// \date August 14, 2006

#include <vcl_iostream.h>
#include <testlib/testlib_test.h>
#include <dbmsh3d/algo/dbmsh3d_fileio.h>
#include <dbmsh3d/algo/dbmsh3d_pt_set_correspondence.h>

#include <vul/vul_timer.h>


void test_pt_set_correspondence()
{
  vcl_string reference_ply = "P:/results/3d-scans/knee_1133RT/wc7_nc7/knee_1133RT_wc7_nc7_cartilage_nc_left.ply";
  vcl_string data_ply = "P:/results/3d-scans/knee_1133RT/wc6_nc6/knee_1133RT_wc6_nc6_cartilage_nc_left.ply";

  dbmsh3d_mesh reference_mesh;
  dbmsh3d_mesh data_mesh;


  vcl_cout << "Starting loading 2 ply files ...\n";
  vul_timer timer;
  timer.mark();

  // load the meshes
  dbmsh3d_load_ply(&reference_mesh, reference_ply.c_str());
  dbmsh3d_load_ply(&data_mesh, data_ply.c_str());

  vcl_cout << "Done. Time taken = \n";
  timer.print(vcl_cout);


  vcl_cout << "Starting performing correspondence\n";
  timer.mark();

    //dbmsh3d_load_ply (&mesh, loaded_file.c_str());

  vcl_map<int, int > cmap;
  vcl_map<int, double > dmap;
  /*dbmsh3d_mesh_correspondence_using_shortest_distance(
    reference_mesh, 
    data_mesh,
    cmap,
    dmap);*/

  

  for (int i=0; i < 1000000; ++i)
  {}

  vcl_cout << "Done. Time taken = \n";
  timer.print(vcl_cout);



}



//: Test functions of biarc class
MAIN( test_pt_set_correspondence )
{
  START ("Test point set correspondence functions");
  test_pt_set_correspondence();
  SUMMARY();
}
