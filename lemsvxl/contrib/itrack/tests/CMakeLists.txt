# brl/bseg/blem/contrib/itrack/tests/CMakeLists.txt
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${LEMS_SOURCE_DIR}/brcv/trk )
include_directories( ${LEMS_SOURCE_DIR}/contrib/ )
add_executable( itrack_test_all
  test_driver.cxx
)

target_link_libraries(itrack_test_all dbinfo vidpro bpro vsol vsl vpl vbl vul testlib)

add_test( NAME itrack_test_track_storage  COMMAND $<TARGET_FILE:itrack_test_all>   test_track_storage)

add_executable( itrack_test_include test_include.cxx )
#target_link_libraries( itrack_test_include itrack )
