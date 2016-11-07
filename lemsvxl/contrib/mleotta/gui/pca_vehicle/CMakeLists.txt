project(pca_vehicle_wxgui)

find_package(wxWidgets COMPONENTS base core gl xrc)
if(wxWidgets_FOUND)

  include( ${wxWidgets_USE_FILE} )


  if(VXL_VGUI_WX_FOUND)

    # Example uses the wxWidgets XML resource fi
    set(pca_vehicle_wxgui_sources
      pca_vehicle_app.h      pca_vehicle_app.cxx
      pca_vehicle_frame.h    pca_vehicle_frame.cxx
      pca_vehicle_manager.h  pca_vehicle_manager.cxx
      
      # Resouce files
      default.parts
      default1.pca
      default2.pca
      default3.pca
      default_dodec.pca
      default_ferryman.pca
    )
    
    set_source_files_properties(
      default.parts
      default1.pca
      default2.pca
      default3.pca
      default_dodec.pca
      default_ferryman.pca
      PROPERTIES
      MACOSX_PACKAGE_LOCATION Resources
    )
    
    if(APPLE)
      # use CoreFoundation to locate the default files in the bundle
      include_directories(/System/Library/Frameworks/CoreFoundation.framework/Headers/)
    elseif(APPLE)
      # copy the default files to the binary directory
      configure_file( ${pca_vehicle_wxgui_SOURCE_DIR}/default1.pca
                      ${pca_vehicle_wxgui_BINARY_DIR}/default1.pca COPY_ONLY )
      configure_file( ${pca_vehicle_wxgui_SOURCE_DIR}/default2.pca
                      ${pca_vehicle_wxgui_BINARY_DIR}/default2.pca COPY_ONLY )
      configure_file( ${pca_vehicle_wxgui_SOURCE_DIR}/default3.pca
                      ${pca_vehicle_wxgui_BINARY_DIR}/default3.pca COPY_ONLY )
      configure_file( ${pca_vehicle_wxgui_SOURCE_DIR}/default_dodec.pca
                      ${pca_vehicle_wxgui_BINARY_DIR}/default_dodec.pca COPY_ONLY )
      configure_file( ${pca_vehicle_wxgui_SOURCE_DIR}/default_ferryman.pca
                      ${pca_vehicle_wxgui_BINARY_DIR}/default_ferryman.pca COPY_ONLY )
      configure_file( ${pca_vehicle_wxgui_SOURCE_DIR}/default.parts
                      ${pca_vehicle_wxgui_BINARY_DIR}/default.parts COPY_ONLY )
    endif(APPLE)

    
    WXWIDGETS_ADD_RESOURCES(pca_vehicle_wxgui_sources pca_vehicle.xrc)
    add_executable(pca_vehicle_wxgui WIN32 MACOSX_BUNDLE ${pca_vehicle_wxgui_sources})
    target_link_libraries(pca_vehicle_wxgui vgui_wx dbgui bgui3d bgui imesh modrec vidl_gui vil dbul)

  endif(VXL_VGUI_WX_FOUND)

endif(wxWidgets_FOUND)
