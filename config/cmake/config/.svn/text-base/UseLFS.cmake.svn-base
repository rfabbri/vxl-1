# this is a configure file to use large file support
IF(WIN32)

ELSE(WIN32)
  EXEC_PROGRAM(getconf ARGS "LFS_CFLAGS" OUTPUT_VARIABLE FileOffsetBits)
  ADD_DEFINITIONS(${FileOffsetBits} -D_LARGEFILE_SOURCE)
ENDIF(WIN32)
