# dbrl/dbseg/dbbgm/tests/CMakeLists.txt
# Tests for bbgm library

ADD_EXECUTABLE( dbbgmo_pro_test_all
  test_driver.cxx
  test_io.cxx
  test_update_dist_image.cxx
#  test_neighborhood_ops.cxx
)

TARGET_LINK_LIBRARIES( dbbgmo_pro_test_all dbbgmo_pro dbbgmo dbstao_algo dbstao vil_pro brdb vnl vil vpl testlib )

ADD_TEST( dbbgmo_test_io  bbgmo_pro_test_all test_io )
ADD_TEST( dbbgmo_test_io  bbgmo_pro_test_all test_update_dist_image )
#ADD_TEST( bbgm_test_io  bbgm_pro_test_all test_neighborhood_ops )

ADD_EXECUTABLE( dbbgmo_pro_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( dbbgmo_pro_test_include dbbgmo_pro dbbgmo)
