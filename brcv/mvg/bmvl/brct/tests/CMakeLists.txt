# testing code for brct library
# brl/bmvl/brct/tests/CMakeLists.txt

include_directories(${GEL_INCLUDE_DIR})
include_directories(${OXL_INCLUDE_DIR})
include_directories(${BRL_INCLUDE_DIR})

set(BUILD_VGUI_TEST "NO")
if(BUILD_VGUI_TEST)
# determines if vgui is built and opengl is available
include( ${VXL_CMAKE_DIR}/UseVGUI.cmake )

if(HAS_VGUI)

set(brct_test_sources
    brct_menus.cxx          brct_menus.h
    brct_windows_frame.cxx  brct_windows_frame.h
    test_brct_kalman.cxx
   )

add_executable(brct_test_vgui ${brct_test_sources})
target_link_libraries(brct_test_vgui vgl_algo vgl vil1 vnl vgui brct bugl bgui)

endif(HAS_VGUI)
endif(BUILD_VGUI_TEST)

add_executable(brct_test_all
  test_driver.cxx
  brct_test_compute_P.cxx
  brct_test_synthetic_data.cxx
)
target_link_libraries(brct_test_all brct vsol vdgl vgl vil1 vnl testlib)

add_test(brct_test_synthetic_data ${EXECUTABLE_OUTPUT_PATH}/brct_test_all brct_test_synthetic_data)
add_test(brct_test_compute_P      ${EXECUTABLE_OUTPUT_PATH}/brct_test_all brct_test_compute_P)

add_executable(brct_test_include test_include.cxx)
target_link_libraries(brct_test_include brct)

add_executable( generate_S3  generate_S3.cxx )
target_link_libraries(generate_S3 brct)
