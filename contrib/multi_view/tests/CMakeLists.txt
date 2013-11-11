# brl/bseg/blem/contrib/multi_view/tests/CMakeLists.txt
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/ )
#ADD_EXECUTABLE( mview2_test_all
#  test_driver.cxx
#)

#TARGET_LINK_LIBRARIES(mview2_test_all dbinfo vidpro bpro vsol vsl vpl vbl vul testlib)

#ADD_TEST(mview2_test_track_storage  mview2_test_all   test_track_storage)

#ADD_EXECUTABLE( mview2_test_include test_include.cxx )
#TARGET_LINK_LIBRARIES( mview2_test_include mview2 )
