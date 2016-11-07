# int_cedt3d Semi-converted legacy C-code contour based euclidean 3D distance transform 
# lemsvxlsrc/contrib/int_cedt3d/CMakeLists.txt

set (CMAKE_CXX_FLAGS -g)

include_directories( ${VXL_CORE_INCLUDE_DIR}     )
include_directories( ${VXL_MUL_INCLUDE_DIR}      )

        set(int_cedt3d_sources
                        cedt3d_utils.C dt_by_3d_cedt.C min_heap.C  propagate3d.C cedt3d.C
                        cedt3d_utils.h dt_by_3d_cedt.h propagate3d.h cedt3d.h
                        dbil3d_cedt.h dbil3d_cedt.hxx
           )

aux_source_directory(Templates int_cedt3d_sources)

add_library(int_cedt3d  ${int_cedt3d_sources})

target_link_libraries(int_cedt3d vil3d )

if( BUILD_TESTING )
  add_subdirectory(tests)
endif( BUILD_TESTING )
