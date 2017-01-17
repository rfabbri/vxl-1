# breyes/dbvis/examples/CMakeLists.txt

if (VXL_VGUI_FOUND)


include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${VXL_OXL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${lems_SOURCE_DIR}/brcv/mvg )
include_directories( ${lems_SOURCE_DIR}/brcv/trk )
include_directories( ${lems_SOURCE_DIR}/brcv/shp )
include_directories( ${lems_SOURCE_DIR}/brcv/rec )
include_directories( ${lems_SOURCE_DIR}/brcv/seg )
include_directories( ${lems_SOURCE_DIR}/breye1 )
include_directories( ${lems_SOURCE_DIR}/breye )

set(dbrown_eyes_sources
   dbvis_brown_eyes_menu.cxx       dbvis_brown_eyes_menu.h
   dbvis_brown_eyes_main.cxx
)

aux_source_directory(Templates dbrown_eyes_sources)

add_executable(brown_eyes_dev ${dbrown_eyes_sources})


target_link_libraries(brown_eyes_dev bvis dbgui dbvrl_vis dbctrk_vis dbetrk_vis dbetl_vis dbecl_vis dbsk2d_vis dbil_pro dbcvr_pro dbcvr_vis dbsol_algo dbru_vis dvpgl_pro dvpgl_vis)


if (HAS_BGUI3D)
  target_link_libraries(brown_eyes_dev bgui3d )
endif (HAS_BGUI3D)

endif (VXL_VGUI_FOUND)
