# contrib/knee_cartilage/gui/CMakeLists.txt


if (VXL_VGUI_FOUND)

include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR} )

include_directories( ${lems_SOURCE_DIR}/brcv/shp )
include_directories( ${lems_SOURCE_DIR}/brcv/seg )
include_directories( ${lems_SOURCE_DIR}/brcv/rec )
include_directories( ${lems_SOURCE_DIR}/basic )


set(knee_cartilage_gui_sources
   knee_cartilage_gui_menu.cxx       knee_cartilage_gui_menu.h
   knee_cartilage_gui_main.cxx 
   dbknee_dicom_image_displayer.h
   dbknee_dicom_image_displayer.cxx
)

aux_source_directory(Templates bvis1_sources)
add_executable(knee_cartilage_gui ${knee_cartilage_gui_sources})
target_link_libraries( knee_cartilage_gui bvis1 dbknee dbknee_pro dbsol_algo dbsol_vis dbsol_pro dbgui dbdet_pro)


endif (VXL_VGUI_FOUND)
