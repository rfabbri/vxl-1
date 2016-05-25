// This is basic/dbgl/algo/tests/test_diffgeom.cxx
#include <testlib/testlib_test.h>
#include <dbgl/algo/dbgl_curvature.h>


void test_curvature() {
  std::cout << "hello";
  // dbgl_compute_curvature(vertices, &k)
}

void test_normals() {
  // dbgl_compute_normals(vertices, &n)
}

//: Test differential geometry estimation functions
MAIN( test_diffgeom )
{
  START("dbgl curvature estimation");
  test_curvature();
  SUMMARY();
}
