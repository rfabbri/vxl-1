# int_cedt3d Semi-converted legacy C-code contour based euclidean 3D distance transform 
# lemsvxlsrc/contrib/int_cedt3d/CMakeLists.txt

SET (CMAKE_CXX_FLAGS -g)

INCLUDE_DIRECTORIES( ${VXL_CORE_INCLUDE_DIR}     )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR}      )

        SET(int_cedt3d_sources
                        cedt3d_utils.C dt_by_3d_cedt.C min_heap.C  propagate3d.C cedt3d.C
                        cedt3d_utils.h dt_by_3d_cedt.h propagate3d.h cedt3d.h
                        dbil3d_cedt.h dbil3d_cedt.txx
           )

AUX_SOURCE_DIRECTORY(Templates int_cedt3d_sources)

ADD_LIBRARY(int_cedt3d  ${int_cedt3d_sources})

TARGET_LINK_LIBRARIES(int_cedt3d vil3d )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
