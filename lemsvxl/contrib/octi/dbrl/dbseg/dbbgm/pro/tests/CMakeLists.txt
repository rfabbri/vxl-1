# dbrl/dbseg/dbbgm/tests/CMakeLists.txt
# Tests for bbgm library

add_executable( dbbgmo_pro_test_all
  test_driver.cxx
  test_io.cxx
  test_update_dist_image.cxx
#  test_neighborhood_ops.cxx
)

target_link_libraries( dbbgmo_pro_test_all dbbgmo_pro dbbgmo dbstao_algo dbstao vil_pro brdb vnl vil vpl testlib )

add_test( dbbgmo_test_io  bbgmo_pro_test_all test_io )
add_test( dbbgmo_test_io  bbgmo_pro_test_all test_update_dist_image )
#add_test( bbgm_test_io  bbgm_pro_test_all test_neighborhood_ops )

add_executable( dbbgmo_pro_test_include test_include.cxx )
target_link_libraries( dbbgmo_pro_test_include dbbgmo_pro dbbgmo)
