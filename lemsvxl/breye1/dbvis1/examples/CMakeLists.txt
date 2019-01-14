# breyes/dbvis1/examples/CMakeLists.txt

if (VXL_VGUI_FOUND)


include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${VXL_OXL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${LEMS_SOURCE_DIR}/brcv/mvg )
include_directories( ${LEMS_SOURCE_DIR}/brcv/trk )
include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )
include_directories( ${LEMS_SOURCE_DIR}/brcv/rec )
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )

add_definitions ( -DCOIN_NOT_DLL )
include_directories( ${VXL_COIN3D_INCLUDE_DIR} )

set(dbrown_eyes_sources
   dbvis1_brown_eyes_menu.cxx       dbvis1_brown_eyes_menu.h
   dbvis1_brown_eyes_main.cxx
)

aux_source_directory(Templates dbrown_eyes_sources)

add_executable(brown_eyes_dev1 ${dbrown_eyes_sources})


target_link_libraries(brown_eyes_dev1 bvis1 dbgui dbvrl_vis dbctrk_vis dbetrk_vis dbetl_vis dbecl_vis dbsk2d_vis dbil_pro dbcvr_pro dbcvr_vis dbsol_algo dbru_vis dvpgl_pro dvpgl_vis)


if (VXL_BGUI3D_FOUND)
  target_link_libraries(brown_eyes_dev1 bgui3d )
endif (VXL_BGUI3D_FOUND)

endif (VXL_VGUI_FOUND)
