# Thomas Pollard's misc projects
# contrib/tpollard/btpl/CMakeLists.txt

INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/tpollard )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )


# Visualization code for change detection results.
SUBDIRS( btpl_change_vis )

# High dynamic range code.
SUBDIRS( btpl_hdr )

# Code for managing a large database of satellite images.
SUBDIRS( btpl_satellite_tools )

# Misc apps for quick experiments.
ADD_EXECUTABLE ( btpl_experiments btpl_experiments.cxx )
TARGET_LINK_LIBRARIES(btpl_experiments vcl vil vgl vnl vpgl vpgl_file_formats vpgl_algo vul vidl1 btpl_hdr baml bvaml btpl_satellite_tools )

ADD_EXECUTABLE (btpl_micd bptl_micd.cxx)
TARGET_LINK_LIBRARIES(btpl_micd vcl vil vgl vnl vsol vpgl vpgl_file_formats vpgl_algo vul vidl1 btpl_hdr baml bvaml btpl_satellite_tools brip)
