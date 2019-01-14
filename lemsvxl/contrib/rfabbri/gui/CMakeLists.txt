# contrib/rfabbri/gui/CMakeLists.txt


if (VXL_VGUI_FOUND)

include_directories( ${LEMS_SOURCE_DIR}/breye1 )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories(${VXL_OXL_INCLUDE_DIR} )
include_directories(${VXL_MUL_INCLUDE_DIR} )
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )
include_directories( ${LEMS_SOURCE_DIR}/brcv/mvg)
include_directories( ${LEMS_SOURCE_DIR}/brcv/rec )
include_directories( ${LEMS_SOURCE_DIR}/basic )
include_directories(${VXD_BRLD_INCLUDE_DIR})


# Scrap application
set(misc_gui_sources
   misc_menu.cxx       misc_menu.h
   misc_main.cxx
)

aux_source_directory(Templates bvis1_sources)

add_executable(misc_gui ${misc_gui_sources})

target_link_libraries(misc_gui bvis1 dbgui vidpro1 bpro1)
endif (VXL_VGUI_FOUND)
