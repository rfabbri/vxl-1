# basic/dbpro/vis/CMakeLists.txt
# Development Brown Processing Visualization Extention Library


IF (VXL_VGUI_FOUND)

SET(dbpro_vis_sources

  dbpro_run_tableau.cxx             dbpro_run_tableau.h           dbpro_run_tableau_sptr.h
  dbpro_basic_gui_observers.h

)


#AUX_SOURCE_DIRECTORY(Templates dbpro_vis_sources)

ADD_LIBRARY(dbpro_vis  ${dbpro_vis_sources})

TARGET_LINK_LIBRARIES(dbpro_vis dbpro vgui)

ENDIF (VXL_VGUI_FOUND)
