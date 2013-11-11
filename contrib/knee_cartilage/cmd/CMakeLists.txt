# This is contrib/knee_cartilage/cmd/CMakeLists.txt
# Command-line programs for knee cartilage project
#

INCLUDE_DIRECTORIES(${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas )

INCLUDE_DIRECTORIES ( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES ( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p )

# ADD_EXECUTABLE(kc_extract_boundary kc_extract_boundary.cxx)
# TARGET_LINK_LIBRARIES( kc_extract_boundary dbmsh3d_algo vul )
