# Thomas Pollard's misc projects
# contrib/tpollard/btpl/btpl_satellite_tools/CMakeLists.txt

INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/tpollard )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )


ADD_LIBRARY( btpl_satellite_tools
             btpl_satellite_tools.h btpl_satellite_tools.cxx
             btpl_imd_parser.h      btpl_imd_parser.cxx )

TARGET_LINK_LIBRARIES( btpl_satellite_tools vcl vgl vnl vil vul vpgl vpgl_algo bgeo vpgl_file_formats)
SET( BTPL_HDR_INCLUDE_DIR ${btpl_satellite_tools_SOURCE_DIR} )

# Misc apps for quick experiments.
ADD_EXECUTABLE ( btpl_satellite_app btpl_satellite_app.cxx )
TARGET_LINK_LIBRARIES( btpl_satellite_app btpl_satellite_tools vcl vil vgl vnl vpgl vpgl_file_formats vpgl_algo vul bgeo )

