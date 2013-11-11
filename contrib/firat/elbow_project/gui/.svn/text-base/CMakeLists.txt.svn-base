# contrib/firat/dbsksp_gui/CMakeLists.txt
IF (VXL_VGUI_FOUND)

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/firat )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )


LINK_DIRECTORIES( ${BOOST_LIBRARY_PATH} )
LINK_DIRECTORIES( ${VTK_LIBRARY_PATH})
LINK_DIRECTORIES( ${ANN_LIBRARY_PATH})

# Small GUI
SET(elbow_segmentation_gui_sources
   elbow_segmentation_gui_menu.cxx       elbow_segmentation_gui_menu.h
   elbow_segmentation_gui_main.cxx
)
ADD_EXECUTABLE(elbow_segmentation_gui ${elbow_segmentation_gui_sources})
TARGET_LINK_LIBRARIES( elbow_segmentation_gui bvis1 dbgui elbow_process elbow_storage vtkIO)


ENDIF (VXL_VGUI_FOUND)
