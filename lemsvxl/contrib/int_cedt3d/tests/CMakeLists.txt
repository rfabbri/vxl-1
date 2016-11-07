# basic/dbil/algo/tests/CMakeLists.txt

#set(VXL_DIR /shared/source/vxlbin)
#set(LEMS_DIR /shared/source/lemsbin)
#set(LEMS_SOURCE_DIR /shared/source/lemsvxlsrc)

#find_package(VXL)
#if(VXL_FOUND)
#include(${VXL_CMAKE_DIR}/UseVXL.cmake)

include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${lems_SOURCE_DIR}/contrib/ )
include_directories( ${VXL_CORE_INCLUDE_DIR} )

        add_executable( dbil3d_cedt_test_all
                        test_driver.cxx
                        test_dbil3d_cedt.cxx
                      )

target_link_libraries( dbil3d_cedt_test_all int_cedt3d testlib vnl )

add_test( dbil3d_test_cedt   dbil3d_cedt_test_all test_dbil3d_cedt )

add_executable( dbil3d_cedt_test_include test_include.cxx )
target_link_libraries( dbil3d_cedt_test_include int_cedt3d )

#endif(VXL_FOUND)
