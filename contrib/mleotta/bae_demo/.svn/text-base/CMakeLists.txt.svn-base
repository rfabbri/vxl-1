# contrib/mleotta/bae_demo/CMakeLists.txt


IF (VXL_VGUI_FOUND)

INCLUDE_DIRECTORIES( ${VXL_VGUI_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/mvg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )

INCLUDE( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )

SET(bae_demo_sources
   bae_demo_menu.cxx       bae_demo_menu.h
   bae_demo_main.cxx
)


#AUX_SOURCE_DIRECTORY(Templates bvis1_sources)

ADD_EXECUTABLE(bae_demo ${bae_demo_sources})

TARGET_LINK_LIBRARIES(bae_demo bvis1 dbbgm_vis dbsta_pro dvpgl_algo_pro  modrec_pro dbul dbsol_pro)


ENDIF (VXL_VGUI_FOUND)
