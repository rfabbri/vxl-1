# contrib/mleotta/gui/CMakeLists.txt


if (VXL_VGUI_FOUND)

include_directories( ${VXL_VGUI_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_RPL_INCLUDE_DIR} )
include_directories( ${lems_SOURCE_DIR}/contrib )
include_directories( ${lems_SOURCE_DIR}/brcv/mvg )
include_directories( ${lems_SOURCE_DIR}/brcv/trk )
include_directories( ${lems_SOURCE_DIR}/brcv/seg )
include_directories( ${lems_SOURCE_DIR}/brcv/rec )
include_directories( ${lems_SOURCE_DIR}/brcv/shp )

include( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )


set(mleotta_gui_sources
   breye/mleotta_menu.cxx       breye/mleotta_menu.h
   breye/mleotta_main.cxx
)


aux_source_directory(Templates bvis1_sources)

add_executable(mleotta_gui WIN32 MACOSX_BUNDLE ${mleotta_gui_sources})

target_link_libraries(mleotta_gui bvis1 mleotta_pro dbgui dbvrl_vis dbetl_vis dbecl_vis dbdet_vis dbkpr_vis dbbgm_vis dbil_vis dbsta_pro vpgl vpgl_algo dvpgl_algo_pro dbil_pro modrec_pro dbul dbsol_pro)

if (HAS_BGUI3D)
  target_link_libraries(mleotta_gui bgui3d )
endif (HAS_BGUI3D)


if (HAS_BGUI3D)
  add_executable(bae_test_gui WIN32 MACOSX_BUNDLE bae_test_gui.cxx)
  target_link_libraries(bae_test_gui bgui3d dbul bgui )
endif (HAS_BGUI3D)   


if (HAS_BGUI3D)
  add_executable(mesh_test_gui WIN32 MACOSX_BUNDLE mesh_test_gui.cxx)
  target_link_libraries(mesh_test_gui bgui3d imesh_algo )
endif (HAS_BGUI3D)


#if (HAS_BGUI3D)
#  add_executable(mesh_camera_cal WIN32 MACOSX_BUNDLE mesh_camera_cal.cxx)
#  target_link_libraries(mesh_camera_cal bgui3d imesh vil)
#endif (HAS_BGUI3D)


if (HAS_BGUI3D)
  add_executable(pca_vehicle_gui WIN32 MACOSX_BUNDLE pca_vehicle_gui.cxx)
  target_link_libraries(pca_vehicle_gui bgui3d imesh modrec vil)
endif (HAS_BGUI3D)

add_subdirectory(pca_vehicle)


if (HAS_BGUI3D)
  set(lmco_gui_sources
    lmco_gui.cxx 
    lmco_manager.cxx       lmco_manager.h
    lmco_menus.cxx         lmco_menus.h
    lmco_compute_mi.cxx    lmco_compute_mi.h
  )
  add_executable(lmco_gui WIN32 MACOSX_BUNDLE ${lmco_gui_sources})
  target_link_libraries(lmco_gui bgui3d bgui imesh_algo )
endif (HAS_BGUI3D)


add_executable(live_bgm_gui WIN32 MACOSX_BUNDLE live_bgm_gui.cxx)
target_link_libraries( live_bgm_gui dbpro_vis dbvidl2_pro vidl_gui bsta_algo dbbgm_vis dbil_pro )

add_executable(live_bgm_vpdt_gui WIN32 MACOSX_BUNDLE live_bgm_vpdt_gui.cxx)
target_link_libraries( live_bgm_vpdt_gui dbpro_vis dbvidl2_pro vidl_gui bsta_algo dbbgm_vis dbil_pro )

add_executable(features_gui WIN32 MACOSX_BUNDLE features_gui.cxx)
target_link_libraries( features_gui vidreg dbpro_vis dbpro_xio dbvidl2_pro vidl_gui dbil_pro bgui)

if (HAS_BGUI3D)
  add_executable(mesh_map_gui WIN32 MACOSX_BUNDLE mesh_map_gui.cxx)
  target_link_libraries( mesh_map_gui dbpro_vis dbdet_vis dbvidl2_pro vidl_gui dbil_pro bgui bgui3d modrec_io dbcll)
  
  add_executable(codebook_gui WIN32 MACOSX_BUNDLE codebook_gui.cxx)
  target_link_libraries( codebook_gui dbdet_vis bgui bgui3d modrec_io dbcll)

  add_executable(flimap_gui WIN32 MACOSX_BUNDLE flimap_gui.cxx)
  target_link_libraries( flimap_gui bgui3d)
endif (HAS_BGUI3D)

endif (VXL_VGUI_FOUND)
