INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES(${lems_SOURCE_DIR}/brcv/seg)

SET(polygon_distance_transform_sources
       polygon_distance_transform.h polygon_distance_transform.cxx
    )
ADD_LIBRARY(polygon_distance_transform  ${polygon_distance_transform_sources} )
TARGET_LINK_LIBRARIES (polygon_distance_transform vsol vgl vnl)

SET(compute_polygon_distance_transform_sources
       compute_polygon_distance_transform.cxx
    )
ADD_EXECUTABLE(compute_polygon_distance_transform  ${compute_polygon_distance_transform_sources} )
TARGET_LINK_LIBRARIES (compute_polygon_distance_transform polygon_distance_transform)


#mex version
IF(UNIX)
	FIND_PACKAGE(Matlab)
	INCLUDE_DIRECTORIES(${MATLAB_INCLUDE_DIR})
	ADD_DEFINITIONS(-DMATLAB_MEX_FILE)
	ADD_LIBRARY(mex_poly_dist_trans MODULE mex_poly_dist_trans.cxx)
	SET_TARGET_PROPERTIES(mex_poly_dist_trans PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY /home/firat/lemsvxl/bin/mex PREFIX "" SUFFIX ".mexa64")
	TARGET_LINK_LIBRARIES(mex_poly_dist_trans  polygon_distance_transform ${MATLAB_LIBRARIES})
ENDIF(UNIX)