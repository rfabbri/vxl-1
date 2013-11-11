# breyes/bvis1/CMakeLists.txt

INCLUDE_DIRECTORIES( ${VXL_VGUI_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_OXL_INCLUDE_DIR} )


IF (VXL_VGUI_FOUND)

SET(bvis1_sources 

#basic bvis1 classes
   bvis1_macros.h
   bvis1_manager.cxx               bvis1_manager.h 
   bvis1_view_tableau.cxx          bvis1_view_tableau.h             bvis1_view_tableau_sptr.h 
                                  bvis1_displayer.h                bvis1_displayer_sptr.h
   bvis1_tool.cxx                  bvis1_tool.h                     bvis1_tool_sptr.h
   bvis1_gl_codec.cxx              bvis1_gl_codec.h                 bvis1_gl_codec_sptr.h

# Displayers
   displayer/bvis1_image_displayer.cxx    displayer/bvis1_image_displayer.h
   displayer/bvis1_vsol2D_displayer.cxx   displayer/bvis1_vsol2D_displayer.h
   bvis1_mapper.cxx                       bvis1_mapper.h
                                         bvis1_mapper_sptr.h
   displayer/bvis1_vtol_displayer.cxx     displayer/bvis1_vtol_displayer.h
# New Menus
   menu/bvis1_file_menu.cxx               menu/bvis1_file_menu.h
   menu/bvis1_video_menu.cxx              menu/bvis1_video_menu.h
   menu/bvis1_repository_menu.cxx         menu/bvis1_repository_menu.h
   menu/bvis1_process_macro_menu.cxx      menu/bvis1_process_macro_menu.h
   menu/bvis1_tableau_menu.cxx            menu/bvis1_tableau_menu.h
   menu/bvis1_view_menu.cxx               menu/bvis1_view_menu.h
   menu/bvis1_add_menu.cxx                menu/bvis1_add_menu.h
   menu/bvis1_command.cxx                 menu/bvis1_command.h

#tools      
   tool/bvis1_vsol2D_tools.cxx            tool/bvis1_vsol2D_tools.h
   tool/bvis1_soview2D_tools.cxx          tool/bvis1_soview2D_tools.h
   tool/bvis1_correspond_tools.cxx        tool/bvis1_correspond_tools.h
)


AUX_SOURCE_DIRECTORY(Templates bvis1_sources)

ADD_LIBRARY(bvis1  ${bvis1_sources})

TARGET_LINK_LIBRARIES(bvis1 vidpro1 bgui mvl)


IF( BUILD_EXAMPLES )

  SUBDIRS( examples )

ENDIF( BUILD_EXAMPLES )


ENDIF (VXL_VGUI_FOUND)
