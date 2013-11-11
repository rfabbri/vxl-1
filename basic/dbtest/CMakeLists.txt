# This is basic/dbtest/CMakeLists.txt


# Set up dbtest_where_root_dir.h to have a macro set to $DBTEST_SOURCE_ROOT_DIR
CONFIGURE_FILE(
  ${lems_SOURCE_DIR}/basic/dbtest/dbtest_where_root_dir.h.in
  ${lems_BINARY_DIR}/basic/dbtest/dbtest_where_root_dir.h ESCAPE_QUOTES
)


FIND_PATH(DBTEST_WHERE_ROOT_DIR_H_DIRECTORY
          dbtest_where_root_dir.h
          ${lems_BINARY_DIR}/basic/dbtest 
          DOC "Path to directory containing the file dbtest_where_root_dir.h")

IF (DBTEST_WHERE_ROOT_DIR_H_DIRECTORY)
  ADD_DEFINITIONS(-DDBTEST_WHERE_ROOT_DIR_H_EXISTS)
  INCLUDE_DIRECTORIES(${DBTEST_WHERE_ROOT_DIR_H_DIRECTORY})
ENDIF(DBTEST_WHERE_ROOT_DIR_H_DIRECTORY)

SET( dbtest_sources

  # Used to locate test files in source tree
  dbtest_root_dir.h   dbtest_root_dir.cxx
)

ADD_LIBRARY( dbtest ${dbtest_sources} )
TARGET_LINK_LIBRARIES( dbtest vcl )



IF( BUILD_TESTING )
#  SUBDIRS( tests )
ENDIF( BUILD_TESTING )
