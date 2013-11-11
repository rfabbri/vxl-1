//: This is lemsvxl/brcv/shp/dbmsh3d/tests/test_textured_mesh.cxx
//  Ming-Ching Chang  Nov 28, 2006.
//  To debug in Visual studio, put argument
//  all D:\Projects\BrownEyes\lemsvxlsrc\brcv\shp\dbmsh3d\tests\data


#include <vcl_cstdio.h>
#include <vcl_cstdlib.h>
#include <vcl_string.h>
#include <vcl_cassert.h>
#include <vnl/vnl_math.h>

#include <testlib/testlib_test.h>
#include <dbmsh3d/dbmsh3d_mesh.h>


MAIN (test_textured_mesh)
{
  testlib_test_start("dbmsh3d_test_face_geom.");

  //Create the test mesh.
  dbmsh3d_mesh* M = new dbmsh3d_mesh;

  dbmsh3d_vertex* v[3];

  v[0] = M->_new_vertex ();
  v[0]->set_pt (2,2,0);
  M->_add_vertex (v[0]);

  v[1] = M->_new_vertex ();
  v[1]->set_pt (172, 2, 0);
  M->_add_vertex (v[1]);

  v[2] = M->_new_vertex ();
  v[2]->set_pt (86, 174, 0);
  M->_add_vertex (v[2]);
  
  dbmsh3d_face* f0 = M->_new_face ();
  M->_add_face (f0);  
  f0->_ifs_add_bnd_V (v[0]); ///vgl_point_2d<double> (v[0]->pt().x(), v[0]->pt().y()));
  f0->_ifs_add_bnd_V (v[1]); ///vgl_point_2d<double> (v[1]->pt().x(), v[1]->pt().y()));
  f0->_ifs_add_bnd_V (v[2]); ///vgl_point_2d<double> (v[2]->pt().x(), v[2]->pt().y()));

  return testlib_test_summary();
}
