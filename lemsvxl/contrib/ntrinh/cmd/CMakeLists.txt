# contrib/ntrinh/cmd/CMakeLists.txt
# Command line programs for ntrinh



if (VXL_VGUI_FOUND)

include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR} )

include_directories( ${LEMS_SOURCE_DIR}/contrib/ntrinh )
include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )
include_directories( ${LEMS_SOURCE_DIR}/brcv/rec )
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )
include_directories( ${LEMS_SOURCE_DIR}/basic )


set(dbsksp_morph_image_sources
   dbsksp_morph_image.cxx
   dbsksp_morph_image_utils.h    dbsksp_morph_image_utils.cxx
)

add_executable(dbsksp_morph_image ${dbsksp_morph_image_sources})

target_link_libraries(dbsksp_morph_image dbsksp vgui dbgui)


endif (VXL_VGUI_FOUND)


