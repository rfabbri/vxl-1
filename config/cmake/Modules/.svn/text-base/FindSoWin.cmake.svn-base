# Try to find SoWin
# Once done this will define
#
# SOWIN_FOUND        - system has SoWin from Open Inventor
# SOWIN_INCLUDE_DIR  - where the SoWin include directory can be found
# SOWIN_LIBRARY      - Link this to use SoWin
#
# Feb 09, 2005: by MingChing to enable contrib app to use SoWin.

IF (WIN32)
  IF (CYGWIN)

    FIND_LIBRARY(COIN_LIBRARY SoWin
      /usr/lib
      /usr/local/lib
    )

  ELSE (CYGWIN)

    FIND_PATH(SOWIN_INCLUDE_DIR Inventor/So.h
      "[HKEY_LOCAL_MACHINE\\SOFTWARE\\SIM\\Coin3D\\2;Installation Path]/include"
    )
    
    FIND_LIBRARY(SOWIN_LIBRARY_DEBUG sowin1d
      "[HKEY_LOCAL_MACHINE\\SOFTWARE\\SIM\\Coin3D\\2;Installation Path]/lib"
    )

    FIND_LIBRARY(SOWIN_LIBRARY_RELEASE sowin1
      "[HKEY_LOCAL_MACHINE\\SOFTWARE\\SIM\\Coin3D\\2;Installation Path]/lib"
    )

    IF (SOWIN_LIBRARY_DEBUG AND SOWIN_LIBRARY_RELEASE)
      SET(SOWIN_LIBRARY optimized ${SOWIN_LIBRARY_RELEASE}
                         debug ${SOWIN_LIBRARY_DEBUG})
    ELSE (SOWIN_LIBRARY_DEBUG AND SOWIN_LIBRARY_RELEASE)
      IF (SOWIN_LIBRARY_DEBUG)
        SET (SOWIN_LIBRARY ${SOWIN_LIBRARY_DEBUG})
      ENDIF (SOWIN_LIBRARY_DEBUG)
      IF (SOWIN_LIBRARY_RELEASE)
        SET (SOWIN_LIBRARY ${SOWIN_LIBRARY_RELEASE})
      ENDIF (SOWIN_LIBRARY_RELEASE)
    ENDIF (SOWIN_LIBRARY_DEBUG AND SOWIN_LIBRARY_RELEASE)

    IF (SOWIN_LIBRARY)
      ADD_DEFINITIONS ( -DSOWIN_NOT_DLL )
    #ELSE (SOWIN_LIBRARY)
    #  SET (SOWIN_LIBRARY coin2d CACHE STRING "SoWin Library (Debug) - Open Inventor GUI")
    ENDIF (SOWIN_LIBRARY)
    
    #IF (SOWIN_LIBRARY)
    #  ADD_DEFINITIONS ( -DSOWIN_NOT_DLL )
    #ELSE (SOWIN_LIBRARY)
    #  SET (SOWIN_LIBRARY sowin1d CACHE STRING "SoWin Library (Debug) - Open Inventor GUI")
    #ENDIF (SOWIN_LIBRARY)

  ENDIF (CYGWIN)

ELSE (WIN32)

  FIND_LIBRARY(SOWIN_LIBRARY SoWin
    /usr/lib
    /usr/local/lib
  )   

ENDIF (WIN32)

SET( SOWIN_FOUND "NO" )
IF(SOWIN_LIBRARY)  
  SET( SOWIN_FOUND "YES" )
ENDIF(SOWIN_LIBRARY)

