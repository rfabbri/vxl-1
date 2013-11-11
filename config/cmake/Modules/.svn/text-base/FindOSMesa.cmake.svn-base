#
# OSMesa # Original date  : Nov 22, 2005
# author: T. Orechia
#

IF (NOT WIN32)
  FIND_PATH(OSMESA_INCLUDE_PATH  GL/osmesa.h
    /usr/X11R6/include
    /usr/include
    /usr/local/include
  )

  FIND_LIBRARY(OSMESA_LIBRARY_PATH OSMesa
    /usr/X11R6/lib
    /usr/lib
    /usr/local/lib
  )


  IF (OSMESA_INCLUDE_PATH)
   IF (OSMESA_LIBRARY_PATH)
    IF (NOT HAS_OSMESA)
      SET(HAS_OSMESA "YES")
      SET( OSMESA_LIBRARIES ${OSMESA_LIBRARY_PATH} )
      INCLUDE_DIRECTORIES( ${OSMESA_INCLUDE_PATH} )
     ENDIF (NOT HAS_OSMESA)
    ENDIF (OSMESA_LIBRARY_PATH)
   ENDIF (OSMESA_INCLUDE_PATH)

ENDIF (NOT WIN32)

