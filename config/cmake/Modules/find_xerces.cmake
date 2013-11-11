#
# xerces 1.4 # Original date  : Dec 07, 2002
# author: J. L. Mundy
#
# Xerces 2.5 updated by T Orechia Date: May 16, 2004
# Xerces 2.6 updated by Matt Leotta Date: March 17, 2004

IF (WIN32)
  IF (CYGWIN)

    FIND_PATH(XERCES_INCLUDE_PATH xercesc/dom/DOM.hpp
      /usr/include
      /usr/local/include
    )

    FIND_LIBRARY(XERCES_LIBRARY_PATH xerces-c
      /usr/lib
      /usr/local/lib
    )

  ELSE (CYGWIN)
  
    FIND_PATH(XERCES_INCLUDE_PATH xercesc/dom/DOM.hpp
      ${vxl_SOURCE_DIR}/../xerces-c_2_6_0/include
      ${vxl_SOURCE_DIR}/../../xerces-c_2_6_0/include
      ${vxl_SOURCE_DIR}/../../../xerces-c_2_6_0/include)

    FIND_LIBRARY(XERCES_LIBRARY_PATH xerces-c_2
      ${vxl_SOURCE_DIR}/../xerces-_c2_6_0/lib
      ${vxl_SOURCE_DIR}/../../xerces-_c2_6_0/lib
      ${vxl_SOURCE_DIR}/../../../xerces-c_2_6_0/lib )

  ENDIF (CYGWIN)

ELSE (WIN32)

  FIND_PATH(XERCES_INCLUDE_PATH xercesc/dom/DOM.hpp
    /usr/include
    /usr/local/include
  )

  FIND_LIBRARY(XERCES_LIBRARY_PATH xerces-c
    /usr/lib
    /usr/local/lib
  )

ENDIF (WIN32)



IF(XERCES_INCLUDE_PATH)
  IF(XERCES_LIBRARY_PATH)
    IF(NOT HAS_XERCES)
      SET(HAS_XERCES "YES")
      #SET(VER , "2_5_0")
      #SET(SO_TARGET_VERSION, "25")
      #SET(SO_TARGET_VERSION_MAJOR,"0")
      INCLUDE_DIRECTORIES( ${XERCES_INCLUDE_PATH} )
      SET( XERCES_LIBRARIES ${XERCES_LIBRARY_PATH} )
     ENDIF(NOT HAS_XERCES)
   ENDIF(XERCES_LIBRARY_PATH)
ENDIF(XERCES_INCLUDE_PATH)

