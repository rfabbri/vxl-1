# basic/dbgl/algo/tests/CMakeLists.txt
# Tests for dbgl_algo library

INCLUDE_DIRECTORIES( ${LIBRARY_OUTPUT_PATH})

ADD_EXECUTABLE( dbgl_algo_test_all
  test_driver.cxx
  test_circ_arc.cxx
  test_conic_arc.cxx
  test_biarc.cxx
  test_eulerspiral.cxx
  test_arc_intersect.cxx
  test_intersect.cxx
  test_distance.cxx
  test_closest_point.cxx
  test_pt_tri_dist_3d.cxx
  test_similarity_3d.cxx
  test_fit_circ_arc_spline.cxx
  test_compute_symmetry_point.cxx
)

TARGET_LINK_LIBRARIES( dbgl_algo_test_all vsl vnl_algo dbgl_algo dbgl dbnl testlib )

ADD_TEST( dbgl_algo_test_circ_arc        dbgl_algo_test_all test_circ_arc )
ADD_TEST( dbgl_algo_test_biarc           dbgl_algo_test_all test_biarc )
ADD_TEST( dbgl_algo_test_eulerspiral     dbgl_algo_test_all test_eulerspiral )
ADD_TEST( dbgl_algo_test_conic_arc       dbgl_algo_test_all test_conic_arc )
ADD_TEST( dbgl_algo_test_arc_intersect   dbgl_algo_test_all test_arc_intersect )
ADD_TEST( dbgl_algo_test_intersect       dbgl_algo_test_all test_intersect )
ADD_TEST( dbgl_algo_test_distance        dbgl_algo_test_all test_distance )
ADD_TEST( dbgl_algo_test_closest_point   dbgl_algo_test_all test_closest_point )
ADD_TEST( dbgl_algo_test_pt_tri_dist_3d  dbgl_algo_test_all test_pt_tri_dist_3d )
ADD_TEST( dbgl_algo_test_similarity_3d   dbgl_algo_test_all test_similarity_3d )
ADD_TEST( dbgl_algo_test_fit_circ_arc_spline  dbgl_algo_test_all test_fit_circ_arc_spline )
ADD_TEST( dbgl_algo_test_compute_symmetry_point dbgl_algo_test_all test_compute_symmetry_point )

ADD_EXECUTABLE( dbgl_algo_test_include test_include.cxx )

TARGET_LINK_LIBRARIES( dbgl_algo_test_include dbgl_algo dbgl dbnl ray_tri )
