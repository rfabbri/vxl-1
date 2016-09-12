# Thomas Pollard's misc projects
# contrib/tpollard/btpl/CMakeLists.txt

include_directories( ${VXLCORE_INCLUDE_DIR} )
include_directories( ${lems_SOURCE_DIR}/basic )
include_directories( ${lems_SOURCE_DIR}/contrib/tpollard )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${VXL_GEL_INCLUDE_DIR})
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )


# Visualization code for change detection results.
subdirs( btpl_change_vis )

# High dynamic range code.
subdirs( btpl_hdr )

# Code for managing a large database of satellite images.
subdirs( btpl_satellite_tools )

# Misc apps for quick experiments.
add_executable ( btpl_experiments btpl_experiments.cxx )
target_link_libraries(btpl_experiments vcl vil vgl vnl vpgl vpgl_file_formats vpgl_algo vul vidl1 btpl_hdr baml bvaml btpl_satellite_tools )

add_executable (btpl_micd bptl_micd.cxx)
target_link_libraries(btpl_micd vcl vil vgl vnl vsol vpgl vpgl_file_formats vpgl_algo vul vidl1 btpl_hdr baml bvaml btpl_satellite_tools brip)
