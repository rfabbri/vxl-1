# contrib/knee_cartilage/gui/CMakeLists.txt


IF (VXL_VGUI_FOUND)

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )


SET(knee_cartilage_gui_sources
   knee_cartilage_gui_menu.cxx       knee_cartilage_gui_menu.h
   knee_cartilage_gui_main.cxx 
   dbknee_dicom_image_displayer.h
   dbknee_dicom_image_displayer.cxx
)

AUX_SOURCE_DIRECTORY(Templates bvis1_sources)
ADD_EXECUTABLE(knee_cartilage_gui ${knee_cartilage_gui_sources})
TARGET_LINK_LIBRARIES( knee_cartilage_gui bvis1 dbknee dbknee_pro dbsol_algo dbsol_vis dbsol_pro dbgui dbdet_pro)


ENDIF (VXL_VGUI_FOUND)
