include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories(${lems_SOURCE_DIR}/brcv/seg)

set(polygon_distance_transform_sources
       polygon_distance_transform.h polygon_distance_transform.cxx
    )
add_library(polygon_distance_transform  ${polygon_distance_transform_sources} )
target_link_libraries (polygon_distance_transform vsol vgl vnl)

set(compute_polygon_distance_transform_sources
       compute_polygon_distance_transform.cxx
    )
add_executable(compute_polygon_distance_transform  ${compute_polygon_distance_transform_sources} )
target_link_libraries (compute_polygon_distance_transform polygon_distance_transform)


#mex version
if(UNIX)
	find_package(Matlab)
	include_directories(${MATLAB_INCLUDE_DIR})
	add_definitions(-DMATLAB_MEX_FILE)
	add_library(mex_poly_dist_trans MODULE mex_poly_dist_trans.cxx)
	set_target_properties(mex_poly_dist_trans PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY /home/firat/lemsvxl/bin/mex PREFIX "" SUFFIX ".mexa64")
	target_link_libraries(mex_poly_dist_trans  polygon_distance_transform ${MATLAB_LIBRARIES})
endif(UNIX)