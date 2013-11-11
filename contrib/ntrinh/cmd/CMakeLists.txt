# contrib/ntrinh/cmd/CMakeLists.txt
# Command line programs for ntrinh



IF (VXL_VGUI_FOUND)

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/ntrinh )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )


SET(dbsksp_morph_image_sources
   dbsksp_morph_image.cxx
   dbsksp_morph_image_utils.h    dbsksp_morph_image_utils.cxx
)

ADD_EXECUTABLE(dbsksp_morph_image ${dbsksp_morph_image_sources})

TARGET_LINK_LIBRARIES(dbsksp_morph_image dbsksp vgui dbgui)


ENDIF (VXL_VGUI_FOUND)


