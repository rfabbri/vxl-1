# This is brcv/shp/dbsk2d/tests/CMakeLists.txt

# Copy the test files to the LEMSVXL build folder
configure_file (
  ${lems_SOURCE_DIR}/brcv/shp/dbsk2d/tests/file_io_data/test_file1.bnd
  ${lems_BINARY_DIR}/brcv/shp/dbsk2d/tests/file_io_data/test_file1.bnd COPYONLY
)  

add_executable( dbsk2d_test_all
  test_driver.cxx
  
  # test exception handling
  test_dbsk2d_exceptions.cxx
  
  #boundary element classes
  test_ishock_belm.cxx
  
  # file i/o tests
  test_file_load_bnd.cxx
  test_file_load_txt.cxx
  test_file_load_con.cxx
  test_file_load_edg.cxx
  test_file_load_cem.cxx
    
  # test functions of dbsk2d_bnd_vertex class
  test_bnd_vertex.cxx
  
  # test functions of dbsk2d_bnd_vertex class
  test_bnd_edge.cxx
  
  # test functions of dbsk2d_bnd_contour class
  test_bnd_contour.cxx
  
  # test functions of dbsk2d_bnd_utils
  test_bnd_utils.cxx
  
  # test different kinds of inputs to dbsk2d_boundary
  test_boundary_inputs.cxx
  
  # test computing bounding box of boundary
  test_boundary_bounding_box.cxx
  
  # test partitioning a boundary into rectangular cells
  test_boundary_partition.cxx
  
  # test computing min distance between different dbsk2d objects
  test_dbsk2d_distance.cxx
  
  # test computing closest point between different dbsk2d objects
  test_dbsk2d_closest_point.cxx
  
)

target_link_libraries( dbsk2d_test_all dbsk2d testlib )

# exceptions test
add_test( dbsk2d_test_dbsk2d_exceptions     dbsk2d_test_all test_dbsk2d_exceptions )

# file i/o tests

add_test( dbsk2d_test_file_load_bnd         dbsk2d_test_all test_file_load_bnd ${CMAKE_CURRENT_SOURCE_DIR})
add_test( dbsk2d_test_file_load_txt         dbsk2d_test_all test_file_load_txt ${CMAKE_CURRENT_SOURCE_DIR})
add_test( dbsk2d_test_file_load_con         dbsk2d_test_all test_file_load_con ${CMAKE_CURRENT_SOURCE_DIR})
add_test( dbsk2d_test_file_load_edg         dbsk2d_test_all test_file_load_edg ${CMAKE_CURRENT_SOURCE_DIR})
add_test( dbsk2d_test_file_load_cem         dbsk2d_test_all test_file_load_cem ${CMAKE_CURRENT_SOURCE_DIR})

add_test( dbsk2d_test_ishock_belm           dbsk2d_test_all test_ishock_belm )

add_test( dbsk2d_test_bnd_vertex            dbsk2d_test_all test_bnd_vertex )
add_test( dbsk2d_test_bnd_contour           dbsk2d_test_all test_bnd_contour )
add_test( dbsk2d_test_bnd_utils             dbsk2d_test_all test_bnd_utils )
add_test( dbsk2d_test_boundary_inputs       dbsk2d_test_all test_boundary_inputs )
add_test( dbsk2d_test_boundary_bounding_box dbsk2d_test_all test_boundary_bounding_box )
add_test( dbsk2d_test_boundary_partition    dbsk2d_test_all test_boundary_partition )
add_test( dbsk2d_test_dbsk2d_distance       dbsk2d_test_all test_dbsk2d_distance )
add_test( dbsk2d_test_dbsk2d_closest_pont   dbsk2d_test_all test_dbsk2d_closest_point )

add_executable( dbsk2d_test_include test_include.cxx )
target_link_libraries( dbsk2d_test_include dbsk2d )
