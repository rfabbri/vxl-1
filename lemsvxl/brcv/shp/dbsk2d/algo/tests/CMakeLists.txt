# This is brcv/shp/dbsk2d/algo/tests/CMakeLists.txt
# Tests for dbsk2d_algo library - specific algorithm for shock 2d computation

# Copy the test files to the LEMSVXL build folder
configure_file (
  ${LEMS_SOURCE_DIR}/brcv/shp/dbsk2d/algo/tests/test_bnd_files/test_file2.bnd
  ${LEMS_BINARY_DIR}/brcv/shp/dbsk2d/algo/tests/test_bnd_files/test_file2.bnd COPYONLY
)

add_executable( dbsk2d_algo_test_all
  test_driver.cxx
  
#test boundary preprocessing
  test_boundary_preprocess.cxx
  test_bnd_preprocess_common.cxx
  test_bnd_preprocess_points.cxx
  test_bnd_preprocess_lines.cxx
  test_bnd_preprocess_points_lines.cxx
  test_bnd_preprocess_arcs.cxx
  test_bnd_preprocess_points_arcs.cxx
  test_bnd_preprocess_lines_arcs.cxx
  test_extract_subgraph.cxx
  
  
#shock detection tests
  test_compute_ishock.cxx
  #test_compute_ishock_bkt.cxx

)

target_link_libraries( dbsk2d_algo_test_all dbsk2d_algo testlib )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/bonefishes.esf          ${CMAKE_CURRENT_BINARY_DIR}/bonefishes.esf )

# Boundary preprocessing test
add_test( NAME  dbsk2d_test_boundary_preprocess          COMMAND $<TARGET_FILE:dbsk2d_algo_test_all> test_boundary_preprocess )
add_test( NAME  dbsk2d_test_bnd_preprocess_common        COMMAND $<TARGET_FILE:dbsk2d_algo_test_all> test_bnd_preprocess_common )
add_test( NAME  dbsk2d_test_bnd_preprocess_points        COMMAND $<TARGET_FILE:dbsk2d_algo_test_all> test_bnd_preprocess_points )
add_test( NAME  dbsk2d_test_bnd_preprocess_lines         COMMAND $<TARGET_FILE:dbsk2d_algo_test_all> test_bnd_preprocess_lines )
add_test( NAME  dbsk2d_test_bnd_preprocess_points_lines  COMMAND $<TARGET_FILE:dbsk2d_algo_test_all> test_bnd_preprocess_points_lines )
add_test( NAME  dbsk2d_test_bnd_preprocess_arcs          COMMAND $<TARGET_FILE:dbsk2d_algo_test_all> test_bnd_preprocess_arcs )
add_test( NAME  dbsk2d_test_bnd_preprocess_points_arcs   COMMAND $<TARGET_FILE:dbsk2d_algo_test_all> test_bnd_preprocess_points_arcs )
add_test( NAME  dbsk2d_test_bnd_preprocess_lines_arcs    COMMAND $<TARGET_FILE:dbsk2d_algo_test_all> test_bnd_preprocess_lines_arcs )
add_test( NAME  dbsk2d_test_extract_subgraph COMMAND $<TARGET_FILE:dbsk2d_algo_test_all> test_extract_subgraph )


# Shock computation test
add_test( NAME  dbsk2d_test_compute_ishock        COMMAND $<TARGET_FILE:dbsk2d_algo_test_all> test_compute_ishock ${CMAKE_CURRENT_SOURCE_DIR})
#add_test( NAME  dbsk2d_test_compute_ishock_bkt    COMMAND $<TARGET_FILE:dbsk2d_algo_test_all> test_compute_ishock_bkt ${CMAKE_CURRENT_SOURCE_DIR})

add_executable( dbsk2d_algo_test_include test_include.cxx )
target_link_libraries( dbsk2d_algo_test_include dbsk2d_algo )
