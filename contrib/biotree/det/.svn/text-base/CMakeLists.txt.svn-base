# /contrib/biotree/det/CMakeLists.txt

SET(CMAKE_CXX_FLAGS -g)

INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )

SET(det_src
det_map_entry.h
det_cylinder_map.h  det_cylinder_map.cxx
det_edge_map.h  det_edge_map.cxx
det_nonmaxium_suppression.h det_nonmaxium_suppression.cxx
det_edge_nonmaximum_suppression.h det_edge_nonmaximum_suppression.cxx
det_nonmax_sup_helper.h det_nonmax_sup_helper.cxx
det_cylinder_detect.h det_cylinder_detect.cxx
det_suppression_and_interpolation.h det_suppression_and_interpolation.cxx

)

AUX_SOURCE_DIRECTORY(Templates det_src)
ADD_LIBRARY(biodet ${det_src})
TARGET_LINK_LIBRARIES(biodet vcl vgl vnl vnl_algo vbl biob xmvg vil3d)

#IF(BUILD_EXAMPLES)
#  SUBDIRS(examples)
#ENDIF(BUILD_EXAMPLES)

#IF(BUILD_TESTING)
#  SUBDIRS(tests)
#ENDIF(BUILD_TESTING)
