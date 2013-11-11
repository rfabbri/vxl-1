# Finds libraries and include files associated with Matlab Engine
# author: firat 

SET(MATLAB_ENGINE_FOUND "NO")

IF(WIN32)
    FIND_PATH(MATLAB_ENGINE_SHARED_LIBRARY_PATH liboctinterp.dll
        C:/ D:/
    )
    FIND_PATH(MATLAB_ENGINE_INCLUDE_PATH MATLAB_ENGINE.h
        C:/ D:/
    )    
ELSE(WIN32)
    IF(UNIX)
        FIND_PATH(MATLAB_ENGINE_SHARED_LIBRARY_PATH libeng.so
            /opt/matlab/bin/glnxa64/
        )
        FIND_PATH(MATLAB_ENGINE_INCLUDE_PATH engine.h
            /opt/matlab/extern/include/
        )           
    ENDIF(UNIX)
ENDIF(WIN32)  

IF(MATLAB_ENGINE_SHARED_LIBRARY_PATH)    
    IF(MATLAB_ENGINE_INCLUDE_PATH)
        IF(WIN32)
            IF(EXISTS "${MATLAB_ENGINE_SHARED_LIBRARY_PATH}/liboctinterp.dll")
                IF(EXISTS "${MATLAB_ENGINE_INCLUDE_PATH}/MATLAB_ENGINE.h")
                   SET(MATLAB_ENGINE_FOUND "YES") 
                ENDIF(EXISTS "${MATLAB_ENGINE_INCLUDE_PATH}/MATLAB_ENGINE.h")
            ENDIF(EXISTS "${MATLAB_ENGINE_SHARED_LIBRARY_PATH}/liboctinterp.dll")
        ELSE(WIN32)
            IF(UNIX)
                IF(EXISTS "${MATLAB_ENGINE_SHARED_LIBRARY_PATH}/libeng.so")
                    IF(EXISTS "${MATLAB_ENGINE_INCLUDE_PATH}/engine.h")
                       SET(MATLAB_ENGINE_FOUND "YES") 
                    ENDIF(EXISTS "${MATLAB_ENGINE_INCLUDE_PATH}/engine.h")
                ENDIF(EXISTS "${MATLAB_ENGINE_SHARED_LIBRARY_PATH}/libeng.so")
            ENDIF(UNIX)
        ENDIF(WIN32)        
    ENDIF(MATLAB_ENGINE_INCLUDE_PATH)
ENDIF(MATLAB_ENGINE_SHARED_LIBRARY_PATH)

