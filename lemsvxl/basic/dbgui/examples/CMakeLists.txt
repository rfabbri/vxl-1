

  if( VXL_VGUI_FOUND )
    include_directories ({$LEMS_SOURCE_DIR}/db3p )
    # expat/lib is combined with expatpp
    include_directories( ${VXL_BRL_INCLUDE_DIR} )
    include_directories( ${VXL_BRL_INCLUDE_DIR}/b3p )
    include_directories (${VXL_EXPAT_INCLUDE_DIR} )
    
    include(${CMAKE_ROOT}/Modules/FindOpenGL.cmake)
    include( ${LEMS_CMAKE_DIR}/Modules/FindOSMesa.cmake )

    if (HAS_OSMESA)
      set (srcs dump_screen.cxx track.cxx track.h segmented_contours.cxx segmented_contours.h)
      add_executable( dump_screen ${srcs})
      target_link_libraries(dump_screen vgui vil vul expatpp ${OPENGL_LIBRARIES} ${OSMESA_LIBRARIES})
    endif (HAS_OSMESA)
  endif (VXL_VGUI_FOUND)
