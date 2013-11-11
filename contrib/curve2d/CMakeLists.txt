# contrib/curve2d/gui/CMakeLists.txt
PROJECT( curve2d_gui )

IF (VXL_VGUI_FOUND)

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_MUL_INCLUDE_DIR} )

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )
SET(curve2d_gui_sources
   curve2d_gui_menu.cxx       curve2d_gui_menu.h
   geno_app.cxx               geno_app.h
   curve2d_gui_main.cxx
)

AUX_SOURCE_DIRECTORY(Templates curve2d_gui_sources)

ADD_EXECUTABLE(curve2d_gui ${curve2d_gui_sources})

TARGET_LINK_LIBRARIES(curve2d_gui vidpro1 bvis1 dbdet_vis dbcvr_vis dbnl_pro  dbgl_algo dbsol_pro dbdet_pro dbgui dbru_vis dbru_pro)

ENDIF (VXL_VGUI_FOUND)
