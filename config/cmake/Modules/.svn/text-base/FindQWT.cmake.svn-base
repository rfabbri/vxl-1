# - Try to find QGLViewer
# Once done this will define
#
#  QWT_FOUND - system has QWT
#  QWT_INCLUDE_DIR - the QWT include directory
#  QWT_LIBRARY - Link these to use QWT
#  QWT_DEFINITIONS - Compiler switches required for using QWT
#


IF (APPLE)

	FIND_PATH(QWT_INCLUDE_DIR qwt.h
	  /usr/include
      /usr/local/include
      /opt/local/include
	  ${QWT_INCLUDE_DIR}
	  )
	
	FIND_LIBRARY(QWT_LIBRARY qwt
	  /usr/lib
	  /usr/local/lib
	  /opt/local/lib
	  )
	
		
	IF(QWT_INCLUDE_DIR AND QWT_LIBRARY)
		   SET(QWT_FOUND TRUE)
	ENDIF(QWT_INCLUDE_DIR AND QWT_LIBRARY)
		
ENDIF(APPLE)


IF(QWT_FOUND)
  IF(NOT QWT_FIND_QUIETLY)
	MESSAGE(STATUS "Found QWT: ${QWT_LIBRARY}")
  ENDIF(NOT QWT_FIND_QUIETLY)
ELSE(QWT_FOUND)
  IF(QWT_FIND_REQUIRED)
	MESSAGE(FATAL_ERROR "Could not find QWT")
  ENDIF(QWT_FIND_REQUIRED)
ENDIF(QWT_FOUND)
	
# show the QWT_INCLUDE_DIR and QWT_LIBRARY variables only in the advanced view
MARK_AS_ADVANCED(QWT_INCLUDE_DIR QWT_LIBRARY )