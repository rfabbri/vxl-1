#INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib )

SET(vol3d_test_src
  test_driver.cxx 
  test_vol3d_radius.cxx
)

ADD_EXECUTABLE( biovol3d_test_all ${vol3d_test_src})

ADD_TEST( biovol3d_test_radius_filter
          biovol3d_test_all
          test_vol3d_radius
        )

TARGET_LINK_LIBRARIES(biovol3d_test_all testlib vcl vbl vol3d)


