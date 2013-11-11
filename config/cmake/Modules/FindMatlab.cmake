# Try to find Matlab
# Once done this will define
#
# MATLAB_FOUND        - system has MATLAB
# MATLAB_INCLUDE_DIR  - where the Matlab include directory can be found
# MATLAB_LIBRARY      - Link this to use Matlab
#




IF (WIN32)
  IF (CYGWIN)

    # FIX Me

  ELSE (CYGWIN)
    FIND_PATH(MATLAB_INCLUDE_DIR engine.h
      PATHS c:/ d:/
    )

    FIND_PATH(MATLAB_LIBRARY_PATH  mx eng
	PATHS      C:/ d:/
    )

    

  ENDIF (CYGWIN)

ELSE (WIN32)
    FIND_PATH(MATLAB_INCLUDE_DIR engine.h
    /usr/include
    /usr/local/MATLAB/extern/include
   /usr/local
  )

  FIND_PATH(MATLAB_LIBRARY_PATH libeng.so
    PATHS 
     /usr/lib
    /usr/local 
    /usr/local/MATLAB/bin/glnx86
  )   
  #FIX Me

ENDIF (WIN32)


SET( MATLAB_FOUND "NO" )
IF(MATLAB_LIBRARY)
  SET( MATLAB_FOUND "YES" )
ENDIF(MATLAB_LIBRARY)

