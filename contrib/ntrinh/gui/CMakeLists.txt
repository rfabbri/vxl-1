# contrib/ntrinh/gui/CMakeLists.txt


IF (VXL_VGUI_FOUND)

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )

SET(ntrinh_gui_sources
   ntrinh_menu.cxx       ntrinh_menu.h
   ntrinh_main.cxx
   
   vidpro1_shockshape_storage.cxx      
   vidpro1_shockshape_storage.h     
   vidpro1_shockshape_storage_sptr.h
   
   dbgui_shockshape_tableau.cxx       
   dbgui_shockshape_tableau.h      
   dbgui_shockshape_tableau_sptr.h
   
   dbvis1_shockshape_displayer.cxx     
   dbvis1_shockshape_displayer.h
   
   dbvis1_shockshape_tool.cxx          
   dbvis1_shockshape_tool.h
   
   dbvis1_triangularblob_tool.cxx      
   dbvis1_triangularblob_tool.h
   
#   dbvis1_a3branch_tool.cxx          
#   dbvis1_a3branch_tool.h
)

AUX_SOURCE_DIRECTORY(Templates bvis1_sources)

ADD_EXECUTABLE(ntrinh_gui ${ntrinh_gui_sources})

TARGET_LINK_LIBRARIES(ntrinh_gui bvis1 vidpro1 vidl1 eulerspiral shockshape)


ENDIF (VXL_VGUI_FOUND)
