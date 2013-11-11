# Finds libraries and include files associated with Octave
# author: firat 

SET(OCTAVE_FOUND "NO")

IF(WIN32)
    FIND_PATH(OCTAVE_SHARED_LIBRARY_PATH liboctinterp.dll
        C:/ D:/
    )
    FIND_PATH(OCTAVE_INCLUDE_PATH octave.h
        C:/ D:/
    )    
ELSE(WIN32)
    IF(UNIX)
        FIND_PATH(OCTAVE_SHARED_LIBRARY_PATH liboctinterp.so
            /usr/lib/*
        )
        FIND_PATH(OCTAVE_INCLUDE_PATH octave.h
            /usr/include/*
        )           
    ENDIF(UNIX)
ENDIF(WIN32)  

IF(OCTAVE_SHARED_LIBRARY_PATH)    
    IF(OCTAVE_INCLUDE_PATH)
        IF(WIN32)
            IF(EXISTS "${OCTAVE_SHARED_LIBRARY_PATH}/liboctinterp.dll")
                IF(EXISTS "${OCTAVE_INCLUDE_PATH}/octave.h")
                   SET(OCTAVE_FOUND "YES") 
                ENDIF(EXISTS "${OCTAVE_INCLUDE_PATH}/octave.h")
            ENDIF(EXISTS "${OCTAVE_SHARED_LIBRARY_PATH}/liboctinterp.dll")
        ELSE(WIN32)
            IF(UNIX)
                IF(EXISTS "${OCTAVE_SHARED_LIBRARY_PATH}/liboctinterp.so")
                    IF(EXISTS "${OCTAVE_INCLUDE_PATH}/octave.h")
                       SET(OCTAVE_FOUND "YES") 
                    ENDIF(EXISTS "${OCTAVE_INCLUDE_PATH}/octave.h")
                ENDIF(EXISTS "${OCTAVE_SHARED_LIBRARY_PATH}/liboctinterp.so")
            ENDIF(UNIX)
        ENDIF(WIN32)        
    ENDIF(OCTAVE_INCLUDE_PATH)
ENDIF(OCTAVE_SHARED_LIBRARY_PATH)

