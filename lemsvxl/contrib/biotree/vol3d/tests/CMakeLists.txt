#include_directories( ${LEMS_SOURCE_DIR}/contrib )

set(vol3d_test_src
  test_driver.cxx 
  test_vol3d_radius.cxx
)

add_executable( biovol3d_test_all ${vol3d_test_src})

add_test( NAME  NAME  biovol3d_test_radius_filter COMMAND $<TARGET_FILE:biovol3d_test_all>
          test_vol3d_radius
        )

target_link_libraries(biovol3d_test_all testlib vcl vbl vol3d)


