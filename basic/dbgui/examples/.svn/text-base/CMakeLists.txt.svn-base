

  IF( VXL_VGUI_FOUND )
    INCLUDE_DIRECTORIES ({$lems_SOURCE_DIR}/db3p )
    # expat/lib is combined with expatpp
    INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
    INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/b3p )
    INCLUDE_DIRECTORIES (${VXL_EXPAT_INCLUDE_DIR} )
    
    INCLUDE(${CMAKE_ROOT}/Modules/FindOpenGL.cmake)
    INCLUDE( ${LEMS_CMAKE_DIR}/Modules/FindOSMesa.cmake )

    IF (HAS_OSMESA)
      SET (srcs dump_screen.cxx track.cxx track.h segmented_contours.cxx segmented_contours.h)
      ADD_EXECUTABLE( dump_screen ${srcs})
      TARGET_LINK_LIBRARIES(dump_screen vgui vidl1 vil vul expatpp ${OPENGL_LIBRARIES} ${OSMESA_LIBRARIES})
    ENDIF (HAS_OSMESA)
  ENDIF (VXL_VGUI_FOUND)
