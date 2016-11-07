# contrib/ntrinh/gui/CMakeLists.txt


if (VXL_VGUI_FOUND)

include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR} )

set(ntrinh_gui_sources
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

aux_source_directory(Templates bvis1_sources)

add_executable(ntrinh_gui ${ntrinh_gui_sources})

target_link_libraries(ntrinh_gui bvis1 vidpro1 vidl1 eulerspiral shockshape)


endif (VXL_VGUI_FOUND)
