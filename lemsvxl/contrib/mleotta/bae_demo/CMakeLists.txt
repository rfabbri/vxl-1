# contrib/mleotta/bae_demo/CMakeLists.txt


if (VXL_VGUI_FOUND)

include_directories( ${VXL_VGUI_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${LEMS_SOURCE_DIR}/brcv/mvg )
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )

include( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )

set(bae_demo_sources
   bae_demo_menu.cxx       bae_demo_menu.h
   bae_demo_main.cxx
)


#aux_source_directory(Templates bvis1_sources)

add_executable(bae_demo ${bae_demo_sources})

target_link_libraries(bae_demo bvis1 dbbgm_vis dbsta_pro dvpgl_algo_pro  modrec_pro dbul dbsol_pro)


endif (VXL_VGUI_FOUND)
