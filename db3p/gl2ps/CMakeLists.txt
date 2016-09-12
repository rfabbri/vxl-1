# This is /db3p/gl2ps

  include(${CMAKE_ROOT}/Modules/FindOpenGL.cmake)
  # Build here only if OpenGL is available:
  if (OPENGL_FOUND)
    if (OPENGL_GLU_FOUND)
      set(gl2ps_sources
          gl2ps.c    gl2ps.h
      )

      add_library(gl2ps ${gl2ps_sources})

      target_link_libraries(gl2ps ${OPENGL_LIBRARIES})

    endif (OPENGL_GLU_FOUND)
  endif (OPENGL_FOUND)

