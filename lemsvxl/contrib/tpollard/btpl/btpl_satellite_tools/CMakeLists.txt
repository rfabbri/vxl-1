# Thomas Pollard's misc projects
# contrib/tpollard/btpl/btpl_satellite_tools/CMakeLists.txt

include_directories( ${VXLCORE_INCLUDE_DIR} )
include_directories( ${LEMS_SOURCE_DIR}/basic )
include_directories( ${LEMS_SOURCE_DIR}/contrib/tpollard )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )


add_library( btpl_satellite_tools
             btpl_satellite_tools.h btpl_satellite_tools.cxx
             btpl_imd_parser.h      btpl_imd_parser.cxx )

target_link_libraries( btpl_satellite_tools vcl vgl vnl vil vul vpgl vpgl_algo bgeo vpgl_file_formats)
set( BTPL_HDR_INCLUDE_DIR ${btpl_satellite_tools_SOURCE_DIR} )

# Misc apps for quick experiments.
add_executable ( btpl_satellite_app btpl_satellite_app.cxx )
target_link_libraries( btpl_satellite_app btpl_satellite_tools vcl vil vgl vnl vpgl vpgl_file_formats vpgl_algo vul bgeo )

