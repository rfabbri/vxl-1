# bbasd/bgld/algo/tests/CMakeLists.txt
# Tests for bgld_algo library

include_directories( ${LIBRARY_OUTPUT_PATH})

add_executable( bgld_algo_test_all
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
  test_diffgeom.cxx
)

target_link_libraries( bgld_algo_test_all vsl vnl_algo bgld_algo bgld bnld testlib )

add_test( NAME  bgld_algo_test_circ_arc        COMMAND $<TARGET_FILE:bgld_algo_test_all> test_circ_arc )
add_test( NAME  bgld_algo_test_biarc           COMMAND $<TARGET_FILE:bgld_algo_test_all> test_biarc )
add_test( NAME  bgld_algo_test_eulerspiral     COMMAND $<TARGET_FILE:bgld_algo_test_all> test_eulerspiral )
add_test( NAME  bgld_algo_test_conic_arc       COMMAND $<TARGET_FILE:bgld_algo_test_all> test_conic_arc )
add_test( NAME  bgld_algo_test_arc_intersect   COMMAND $<TARGET_FILE:bgld_algo_test_all> test_arc_intersect )
add_test( NAME  bgld_algo_test_intersect       COMMAND $<TARGET_FILE:bgld_algo_test_all> test_intersect )
add_test( NAME  bgld_algo_test_distance        COMMAND $<TARGET_FILE:bgld_algo_test_all> test_distance )
add_test( NAME  bgld_algo_test_closest_point   COMMAND $<TARGET_FILE:bgld_algo_test_all> test_closest_point )
add_test( NAME  bgld_algo_test_pt_tri_dist_3d  COMMAND $<TARGET_FILE:bgld_algo_test_all> test_pt_tri_dist_3d )
add_test( NAME  bgld_algo_test_similarity_3d   COMMAND $<TARGET_FILE:bgld_algo_test_all> test_similarity_3d )
add_test( NAME  bgld_algo_test_fit_circ_arc_spline  COMMAND $<TARGET_FILE:bgld_algo_test_all> test_fit_circ_arc_spline )
add_test( NAME  bgld_algo_test_compute_symmetry_point COMMAND $<TARGET_FILE:bgld_algo_test_all> test_compute_symmetry_point )
add_test( NAME  bgld_algo_test_diffgeom COMMAND $<TARGET_FILE:bgld_algo_test_all> test_diffgeom )

add_executable( bgld_algo_test_include test_include.cxx )

target_link_libraries( bgld_algo_test_include bgld_algo bgld bnld ray_tri )
