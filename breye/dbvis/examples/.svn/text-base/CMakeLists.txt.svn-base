# breyes/dbvis/examples/CMakeLists.txt

IF (VXL_VGUI_FOUND)


INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/mvg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye )

SET(dbrown_eyes_sources
   dbvis_brown_eyes_menu.cxx       dbvis_brown_eyes_menu.h
   dbvis_brown_eyes_main.cxx
)

AUX_SOURCE_DIRECTORY(Templates dbrown_eyes_sources)

ADD_EXECUTABLE(brown_eyes_dev ${dbrown_eyes_sources})


TARGET_LINK_LIBRARIES(brown_eyes_dev bvis dbgui dbvrl_vis dbctrk_vis dbetrk_vis dbetl_vis dbecl_vis dbsk2d_vis dbil_pro dbcvr_pro dbcvr_vis dbsol_algo dbru_vis dvpgl_pro dvpgl_vis)


IF (HAS_BGUI3D)
  TARGET_LINK_LIBRARIES(brown_eyes_dev bgui3d )
ENDIF (HAS_BGUI3D)

ENDIF (VXL_VGUI_FOUND)
