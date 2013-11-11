// This is dbmsh3d/algo/tests/test_file_io.cxx
//
// \author Nhon Trinh, Ming-Ching Chang
// \date June 13, 2006
//    Test reading Shape Grabber .3pi raw scan file.
//
//  To debug in Visual studio, put argument
//  test_file_io D:\Projects\BrownEyes\lemsvxlsrc\brcv\shp\dbmsh3d\algo\tests\data

#include <vcl_iostream.h>
#include <testlib/testlib_test.h>
#include <dbmsh3d/algo/dbmsh3d_fileio.h>
#include <dbmsh3d/algo/dbmsh3d_sg3pi_algo.h>

void test_ply_io()
{
  // test writing
  TEST("Read .PLY file", true, true);

  vcl_string in_file[12];
  //
  in_file[0] = "C:/projects/ply-tools/Nbunny/BAMbunny.ply";
  //in_file[0] = "C:/projects/ply-tools/Nbunny/cube.ply";
  in_file[1] = "C:/projects/ply-tools/Nbunny/BAPbunny.ply";
  in_file[2] = "C:/projects/ply-tools/Nbunny/BAUbunny.ply";
  in_file[3] = "C:/projects/ply-tools/Nbunny/BBMbunny.ply";
  in_file[4] = "C:/projects/ply-tools/Nbunny/BBPbunny.ply"; // failed
  in_file[5] = "C:/projects/ply-tools/Nbunny/BBUbunny.ply";
  in_file[6] = "C:/projects/ply-tools/Nbunny/lambunny.ply";
  in_file[7] = "C:/projects/ply-tools/Nbunny/lapbunny.ply";
  in_file[8] = "C:/projects/ply-tools/Nbunny/laubunny.ply";
  in_file[9] = "C:/projects/ply-tools/Nbunny/lbmbunny.ply";
  in_file[10] = "C:/projects/ply-tools/Nbunny/lbpbunny.ply"; // failed
  in_file[11] = "C:/projects/ply-tools/Nbunny/lbubunny.ply";
  //vcl_string in_file = "C:/projects/ply-tools/Nbunny/dodecahedron.ply";
  //vcl_string in_file = "C:/projects/ply-tools/Nbunny/triangle.ply";


for (int i = 0; i<12; ++i)
{
  vcl_cout << "\n\nFile = " << in_file[i] << vcl_endl;

  vcl_string loaded_file = in_file[i];


  dbmsh3d_mesh mesh;

  //dbmsh3d_load_ply (&mesh, in_file.c_str());
  dbmsh3d_load_ply (&mesh, loaded_file.c_str());


  vcl_cout << "Print out about mesh" << vcl_endl;
  vcl_cout << "Number of vertices = " << mesh.vertexmap().size() << vcl_endl;
  vcl_cout << "Number of faces = " << mesh.facemap().size() << vcl_endl;

  // now test saving the file
  vcl_string out_file = "C:/projects/ply-tools/Nbunny/test_saving.ply";
  dbmsh3d_save_ply(& mesh, out_file.c_str(), false);


  // Load the file again
  dbmsh3d_mesh mesh2;
  dbmsh3d_load_ply (&mesh2, out_file.c_str());

  vcl_cout << "Print out about mesh 2" << vcl_endl;
  vcl_cout << "Number of vertices = " << mesh2.vertexmap().size() << vcl_endl;
  vcl_cout << "Number of faces = " << mesh2.facemap().size() << vcl_endl;
}
  
  return;
}

void test_ply_io_richmesh()
{
  vcl_string in_file = "C:/projects/lemsvxl/src/brcv/shp/dbmsh3d/tests/test_data/cube_w_verror.ply";
 

  // load rich mesh
  dbmsh3d_richmesh mesh;
  vcl_vector<vcl_string > vertex_property_list;
  vertex_property_list.push_back("verror");
  
  vcl_vector<vcl_string > face_property_list;
  face_property_list.clear();

  dbmsh3d_load_ply(&mesh, in_file.c_str(), vertex_property_list, face_property_list);

  // print out the properties of each vertex
  mesh.print_summary(vcl_cout);

  // save the mesh again to compare results
  vcl_string out_file =
    "C:/projects/lemsvxl/src/brcv/shp/dbmsh3d/tests/test_data/resaved_cube_w_verror.ply";
  dbmsh3d_save_ply(&mesh, out_file.c_str(), 
    vertex_property_list, face_property_list, ASCII);
}




void test_ply_io_richmesh2()
{
  vcl_string richmesh_file = "sample_richmesh.ply";
  
 
  // save rich mesh
  dbmsh3d_richmesh richmesh;
  vcl_vector<vcl_string > vertex_property_list;
  vertex_property_list.push_back("vertex_weight");
  
  vcl_vector<vcl_string > face_property_list;
  face_property_list.push_back("face_weight");


  // topology
  dbmsh3d_richvertex* v0 = (dbmsh3d_richvertex*) richmesh._new_vertex();
  dbmsh3d_richvertex* v1 = (dbmsh3d_richvertex*) richmesh._new_vertex();
  dbmsh3d_richvertex* v2 = (dbmsh3d_richvertex*) richmesh._new_vertex();
  richmesh._add_vertex(v0);
  richmesh._add_vertex(v1);
  richmesh._add_vertex(v2);

  dbmsh3d_richface* f0 = (dbmsh3d_richface*) richmesh._new_face();
  f0->_ifs_add_bnd_V(v0);
  f0->_ifs_add_bnd_V(v1);
  f0->_ifs_add_bnd_V(v2);

  richmesh._add_face(f0);

  // vertex data
  v0->set_pt(vgl_point_3d<double > (1, 0, 0));
  v0->add_scalar_property("vertex_weight", 0.1);

  v1->set_pt(vgl_point_3d<double > (0, 1, 0));
  v1->add_scalar_property("vertex_weight", 0.5);
  
  v2->set_pt(vgl_point_3d<double > (0, 0, 1));
  v2->add_scalar_property("vertex_weight", 0.7);

  // face data
  f0->add_scalar_property("face_weight", 3.7);

  // print out the properties of each vertex
  richmesh.print_summary(vcl_cout);


  // save the constructed mesh to a file
  dbmsh3d_save_ply(&richmesh, richmesh_file.c_str(), 
    vertex_property_list, face_property_list, ASCII);


  //
  // load the same mesh in
  dbmsh3d_richmesh richmesh2;
  dbmsh3d_load_ply(&richmesh2, richmesh_file.c_str(), 
    vertex_property_list, face_property_list);

  // print out the properties of each vertex
  richmesh2.print_summary(vcl_cout);

  // write the file out again
  vcl_string richmesh2_file = "sample_richmesh_saved_again.ply";
  dbmsh3d_save_ply(&richmesh2, richmesh2_file.c_str(), 
    vertex_property_list, face_property_list, ASCII);

  // 
  bool success = (richmesh.num_vertices() == richmesh2.num_vertices()) &&
    (richmesh.facemap().size() == richmesh2.facemap().size());
    
  TEST("Save and Load richmesh file", success, true);


}



void test_off_read ()
{
}

//: Test reading Shape Grabber raw scan data file
//  into vectors of scanlines of points.
void test_3pi_read (vcl_string file_3pi)
{
  dbmsh3d_sg3pi* sg3pi = new dbmsh3d_sg3pi;
  bool result = dbmsh3d_load_sg3pi (sg3pi, file_3pi.c_str());
  TEST("read ShapeGrabber .3PI raw scan file", result, 1);

  delete sg3pi;
}


MAIN_ARGS (test_file_io)
{
  //read data file from specified position.
  vcl_string dir_base;
  if ( argc >= 2 ) {
      dir_base = argv[1];
      #ifdef VCL_WIN32
        dir_base += "\\";
      #else
        dir_base += "/";
      #endif
  }

  testlib_test_start ("Mesh file I/O");
  vcl_string file_3pi = dir_base + "test1.3pi";
  test_3pi_read (file_3pi);

  
  test_ply_io_richmesh2();

  ///test_ply_io();
  ///test_off_read ();

  return testlib_test_summary();   
}
