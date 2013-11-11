# This is /db3p/gl2ps

  INCLUDE(${CMAKE_ROOT}/Modules/FindOpenGL.cmake)
  # Build here only if OpenGL is available:
  IF (OPENGL_FOUND)
    IF (OPENGL_GLU_FOUND)
      SET(gl2ps_sources
          gl2ps.c    gl2ps.h
      )

      ADD_LIBRARY(gl2ps ${gl2ps_sources})

      TARGET_LINK_LIBRARIES(gl2ps ${OPENGL_LIBRARIES})

    ENDIF (OPENGL_GLU_FOUND)
  ENDIF (OPENGL_FOUND)

