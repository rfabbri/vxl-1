# contrib/firat/dbsksp_gui/CMakeLists.txt
if (VXL_VGUI_FOUND)

include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )

include_directories( ${LEMS_SOURCE_DIR}/contrib/firat )
include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )
include_directories( ${LEMS_SOURCE_DIR}/brcv/rec )
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )
include_directories( ${LEMS_SOURCE_DIR}/basic )


link_directories( ${BOOST_LIBRARY_PATH} )
link_directories( ${VTK_LIBRARY_PATH})
link_directories( ${ANN_LIBRARY_PATH})

# Small GUI
set(elbow_segmentation_gui_sources
   elbow_segmentation_gui_menu.cxx       elbow_segmentation_gui_menu.h
   elbow_segmentation_gui_main.cxx
)
add_executable(elbow_segmentation_gui ${elbow_segmentation_gui_sources})
target_link_libraries( elbow_segmentation_gui bvis1 dbgui elbow_process elbow_storage vtkIO)


endif (VXL_VGUI_FOUND)
