# contrib/medial_livewire/gui/CMakeLists.txt


if (VXL_VGUI_FOUND)

include_directories( ${LEMS_SOURCE_DIR}/breye1 )
include_directories(${VXL_OXL_INCLUDE_DIR} )
include_directories(${VXL_MUL_INCLUDE_DIR} )

include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg/dbdet )
include_directories( ${LEMS_SOURCE_DIR}/brcv/rec )
include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )
include_directories( ${LEMS_SOURCE_DIR}/basic )
include_directories( ${LEMS_SOURCE_DIR}/breye1 )

include_directories( ${LEMS_SOURCE_DIR}/contrib)


set(medial_livewire_gui_sources
   medial_livewire_menu.cxx       medial_livewire_menu.h
   medial_livewire_main.cxx
   dbdet_draw_spheres_tool.cxx    dbdet_draw_spheres_tool.h
   dbdet_dlvwr_params.cxx         dbdet_dlvwr_params.h
   dbdet_dlvwr.cxx                dbdet_dlvwr.h
   dbdet_dlvwr_tool.cxx           dbdet_dlvwr_tool.h
   dbdet_dmedial_wire_tool.cxx    dbdet_dmedial_wire_tool.h
   dbacm_levelset_tool.cxx        dbacm_levelset_tool.h
   dbdet_image_seg_tool.cxx       dbdet_image_seg_tool.h
   dbdet_manual_seg_tool.cxx       dbdet_manual_seg_tool.h
   dbdet_image_seg_dlvwr_tool.cxx dbdet_image_seg_dlvwr_tool.h
   # dbdet_load_volume_seg_xml_process.cxx    dbdet_load_volume_seg_xml_process.h
   
   bvis1_dicom_image_displayer.cxx   bvis1_dicom_image_displayer.h
   dbknee_vsol2D_displayer.cxx       dbknee_vsol2D_displayer.h
   
)


aux_source_directory(Templates medial_livewire_gui_sources)

add_executable(medial_livewire_gui ${medial_livewire_gui_sources})

target_link_libraries(medial_livewire_gui bvis1 dbgui dbsol vidpro1 vidl1 dbdet_vis dbdet_xio dbsol_vis dbsol_pro)
#target_link_libraries(medial_livewire_gui bvis1 dbgui dbsol vidpro1 vidl1 dbdet_vis dbdet_xio dbmsh3d_algovtk_pro dbsol_vis dbsol_pro )


endif (VXL_VGUI_FOUND)
