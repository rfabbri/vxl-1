# This is contrib/knee_cartilage/cmd/CMakeLists.txt
# Command-line programs for knee cartilage project
#

include_directories(${VXL_OXL_INCLUDE_DIR} )
include_directories(${VXL_MUL_INCLUDE_DIR} )
include_directories(${VXL_GEL_INCLUDE_DIR} )
include_directories(${VXL_BRL_INCLUDE_DIR} )
include_directories(${VXL_BRL_INCLUDE_DIR}/bbas )

include_directories ( ${LEMS_SOURCE_DIR}/brcv/shp )
include_directories ( ${LEMS_SOURCE_DIR}/basic )
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )
include_directories( ${LEMS_SOURCE_DIR}/brcv/rec )
include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )
include_directories( ${LEMS_SOURCE_DIR}/db3p )

# add_executable(kc_extract_boundary kc_extract_boundary.cxx)
# target_link_libraries( kc_extract_boundary dbmsh3d_algo vul )
