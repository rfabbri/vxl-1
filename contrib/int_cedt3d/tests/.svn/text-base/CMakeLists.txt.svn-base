# basic/dbil/algo/tests/CMakeLists.txt

#SET(VXL_DIR /shared/source/vxlbin)
#SET(LEMS_DIR /shared/source/lemsbin)
#SET(LEMS_SOURCE_DIR /shared/source/lemsvxlsrc)

#FIND_PACKAGE(VXL)
#IF(VXL_FOUND)
#INCLUDE(${VXL_CMAKE_DIR}/UseVXL.cmake)

INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/ )
INCLUDE_DIRECTORIES( ${VXL_CORE_INCLUDE_DIR} )

        ADD_EXECUTABLE( dbil3d_cedt_test_all
                        test_driver.cxx
                        test_dbil3d_cedt.cxx
                      )

TARGET_LINK_LIBRARIES( dbil3d_cedt_test_all int_cedt3d testlib vnl )

ADD_TEST( dbil3d_test_cedt   dbil3d_cedt_test_all test_dbil3d_cedt )

ADD_EXECUTABLE( dbil3d_cedt_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( dbil3d_cedt_test_include int_cedt3d )

#ENDIF(VXL_FOUND)
