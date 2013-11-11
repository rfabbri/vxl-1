# brl/bseg/blem/contrib/itrack/tests/CMakeLists.txt
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/ )
ADD_EXECUTABLE( itrack_test_all
  test_driver.cxx
)

TARGET_LINK_LIBRARIES(itrack_test_all dbinfo vidpro bpro vsol vsl vpl vbl vul testlib)

ADD_TEST(itrack_test_track_storage  itrack_test_all   test_track_storage)

ADD_EXECUTABLE( itrack_test_include test_include.cxx )
#TARGET_LINK_LIBRARIES( itrack_test_include itrack )
