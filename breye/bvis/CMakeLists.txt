# breye2s/bvis/CMakeLists.txt


include_directories( ${VXL_VGUI_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_OXL_INCLUDE_DIR} )


if (VXL_VGUI_FOUND)

set(bvis_sources 

#basic bvis classes
   bvis_macros.h
   bvis_manager.hxx               bvis_manager.h 
   bvis_view_tableau.cxx          bvis_view_tableau.h             bvis_view_tableau_sptr.h 
   bvis_proc_selector_tableau.cxx bvis_proc_selector_tableau.h    bvis_proc_selector_tableau_sptr.h
   bvis_video_view_tableau.cxx    bvis_video_view_tableau.h   
   bvis_mview_view_tableau.cxx    bvis_mview_view_tableau.h   
                                   bvis_displayer.h                bvis_displayer_sptr.h
   bvis_tool.cxx                  bvis_tool.h                     bvis_tool_sptr.h
   bvis_gl_codec.cxx              bvis_gl_codec.h                 bvis_gl_codec_sptr.h
   bvis_video_manager.cxx         bvis_video_manager.h            bvis_video_manager_sptr.h
   bvis_mview_manager.cxx         bvis_mview_manager.h            bvis_mview_manager_sptr.h
                                   bvis_manager_traits.h
# Displayers
   displayer/bvis_image_displayer.cxx    displayer/bvis_image_displayer.h
   displayer/bvis_vsol2D_displayer.cxx   displayer/bvis_vsol2D_displayer.h
   bvis_mapper.cxx                       bvis_mapper.h
                                          bvis_mapper_sptr.h
   displayer/bvis_vtol_displayer.cxx     displayer/bvis_vtol_displayer.h
# New Menus
   menu/bvis_file_menu.hxx               menu/bvis_file_menu.h
   menu/video/bvis_video_file_menu.cxx   menu/video/bvis_video_file_menu.h
   menu/mview/bvis_mview_io_dialog.cxx   menu/mview/bvis_mview_io_dialog.h
   menu/video/bvis_video_menu.cxx        menu/video/bvis_video_menu.h
   menu/bvis_repository_menu.hxx         menu/bvis_repository_menu.h
   menu/bvis_process_macro_menu.hxx      menu/bvis_process_macro_menu.h
   menu/bvis_tableau_menu.hxx            menu/bvis_tableau_menu.h
   menu/bvis_view_menu.hxx               menu/bvis_view_menu.h
   menu/video/bvis_video_view_menu.cxx   menu/video/bvis_video_view_menu.h
   menu/video/bvis_video_add_menu.cxx    menu/video/bvis_video_add_menu.h
   menu/bvis_command.hxx                 menu/bvis_command.h

#tools      
   bvis_tool_manager.hxx                 bvis_tool_manager.h
   tool/bvis_vsol2D_tools.cxx            tool/bvis_vsol2D_tools.h
   tool/bvis_soview2D_tools.cxx          tool/bvis_soview2D_tools.h
   tool/bvis_correspond_tools.cxx        tool/bvis_correspond_tools.h
)


aux_source_directory(Templates bvis_sources)

add_library(bvis ${bvis_sources})

target_link_libraries(bvis bpro vidpro bgui mvl vbl vbl_io)


if( BUILD_EXAMPLES )

  add_subdirectory( examples )

endif( BUILD_EXAMPLES )


endif (VXL_VGUI_FOUND)
