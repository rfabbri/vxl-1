# contrib/mleotta/gui/CMakeLists.txt


IF (VXL_VGUI_FOUND)

INCLUDE_DIRECTORIES( ${VXL_VGUI_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_RPL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/mvg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )

INCLUDE( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )


SET(mleotta_gui_sources
   breye/mleotta_menu.cxx       breye/mleotta_menu.h
   breye/mleotta_main.cxx
)


AUX_SOURCE_DIRECTORY(Templates bvis1_sources)

ADD_EXECUTABLE(mleotta_gui WIN32 MACOSX_BUNDLE ${mleotta_gui_sources})

TARGET_LINK_LIBRARIES(mleotta_gui bvis1 mleotta_pro dbgui dbvrl_vis dbetl_vis dbecl_vis dbdet_vis dbkpr_vis dbbgm_vis dbil_vis dbsta_pro vpgl vpgl_algo dvpgl_algo_pro dbil_pro modrec_pro dbul dbsol_pro)

IF (HAS_BGUI3D)
  TARGET_LINK_LIBRARIES(mleotta_gui bgui3d )
ENDIF (HAS_BGUI3D)


IF (HAS_BGUI3D)
  ADD_EXECUTABLE(bae_test_gui WIN32 MACOSX_BUNDLE bae_test_gui.cxx)
  TARGET_LINK_LIBRARIES(bae_test_gui bgui3d dbul bgui )
ENDIF (HAS_BGUI3D)   


IF (HAS_BGUI3D)
  ADD_EXECUTABLE(mesh_test_gui WIN32 MACOSX_BUNDLE mesh_test_gui.cxx)
  TARGET_LINK_LIBRARIES(mesh_test_gui bgui3d imesh_algo )
ENDIF (HAS_BGUI3D)


#IF (HAS_BGUI3D)
#  ADD_EXECUTABLE(mesh_camera_cal WIN32 MACOSX_BUNDLE mesh_camera_cal.cxx)
#  TARGET_LINK_LIBRARIES(mesh_camera_cal bgui3d imesh vil)
#ENDIF (HAS_BGUI3D)


IF (HAS_BGUI3D)
  ADD_EXECUTABLE(pca_vehicle_gui WIN32 MACOSX_BUNDLE pca_vehicle_gui.cxx)
  TARGET_LINK_LIBRARIES(pca_vehicle_gui bgui3d imesh modrec vil)
ENDIF (HAS_BGUI3D)

SUBDIRS(pca_vehicle)


IF (HAS_BGUI3D)
  SET(lmco_gui_sources
    lmco_gui.cxx 
    lmco_manager.cxx       lmco_manager.h
    lmco_menus.cxx         lmco_menus.h
    lmco_compute_mi.cxx    lmco_compute_mi.h
  )
  ADD_EXECUTABLE(lmco_gui WIN32 MACOSX_BUNDLE ${lmco_gui_sources})
  TARGET_LINK_LIBRARIES(lmco_gui bgui3d bgui imesh_algo )
ENDIF (HAS_BGUI3D)


ADD_EXECUTABLE(live_bgm_gui WIN32 MACOSX_BUNDLE live_bgm_gui.cxx)
TARGET_LINK_LIBRARIES( live_bgm_gui dbpro_vis dbvidl2_pro vidl_gui bsta_algo dbbgm_vis dbil_pro )

ADD_EXECUTABLE(live_bgm_vpdt_gui WIN32 MACOSX_BUNDLE live_bgm_vpdt_gui.cxx)
TARGET_LINK_LIBRARIES( live_bgm_vpdt_gui dbpro_vis dbvidl2_pro vidl_gui bsta_algo dbbgm_vis dbil_pro )

ADD_EXECUTABLE(features_gui WIN32 MACOSX_BUNDLE features_gui.cxx)
TARGET_LINK_LIBRARIES( features_gui vidreg dbpro_vis dbpro_xio dbvidl2_pro vidl_gui dbil_pro bgui)

IF (HAS_BGUI3D)
  ADD_EXECUTABLE(mesh_map_gui WIN32 MACOSX_BUNDLE mesh_map_gui.cxx)
  TARGET_LINK_LIBRARIES( mesh_map_gui dbpro_vis dbdet_vis dbvidl2_pro vidl_gui dbil_pro bgui bgui3d modrec_io dbcll)
  
  ADD_EXECUTABLE(codebook_gui WIN32 MACOSX_BUNDLE codebook_gui.cxx)
  TARGET_LINK_LIBRARIES( codebook_gui dbdet_vis bgui bgui3d modrec_io dbcll)

  ADD_EXECUTABLE(flimap_gui WIN32 MACOSX_BUNDLE flimap_gui.cxx)
  TARGET_LINK_LIBRARIES( flimap_gui bgui3d)
ENDIF (HAS_BGUI3D)

ENDIF (VXL_VGUI_FOUND)
