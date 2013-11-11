# - Try to find QGLViewer
# Once done this will define
#
#  QGLVIEWER_FOUND - system has QGLViewer
#  QGLVIEWER_INCLUDE_DIR - the QGLViewer include directory
#  QGLVIEWER_LIBRARY - Link these to use QGLViewer
#  QGLVIEWER_DEFINITIONS - Compiler switches required for using QGLViewer
#


IF (APPLE)

	FIND_PATH(QGLVIEWER_INCLUDE_DIR QGLViewer/qglviewer.h
	  /usr/include
      /usr/local/include
      /opt/local/include
	  ${QGLVIEWER_INCLUDE_DIR}
	  )
	
	FIND_LIBRARY(QGLVIEWER_LIBRARY QGLViewer
	  /usr/lib
	  /usr/local/lib
	  /opt/local/lib
	  )
	
		
	IF(QGLVIEWER_INCLUDE_DIR AND QGLVIEWER_LIBRARY)
		   SET(QGLVIEWER_FOUND TRUE)
	ENDIF(QGLVIEWER_INCLUDE_DIR AND QGLVIEWER_LIBRARY)
		
ENDIF(APPLE)


IF(QGLVIEWER_FOUND)
  IF(NOT QGLViewer_FIND_QUIETLY)
	MESSAGE(STATUS "Found QGLViewer: ${QGLVIEWER_LIBRARY}")
  ENDIF(NOT QGLViewer_FIND_QUIETLY)
ELSE(QGLVIEWER_FOUND)
  IF(QGLViewer_FIND_REQUIRED)
	MESSAGE(FATAL_ERROR "Could not find QGLViewer")
  ENDIF(QGLViewer_FIND_REQUIRED)
ENDIF(QGLVIEWER_FOUND)
	
# show the QGLVIEWER_INCLUDE_DIR and QGLVIEWER_LIBRARY variables only in the advanced view
MARK_AS_ADVANCED(QGLVIEWER_INCLUDE_DIR QGLVIEWER_LIBRARY )