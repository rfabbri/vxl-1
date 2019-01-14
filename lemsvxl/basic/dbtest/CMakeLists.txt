# This is basic/dbtest/CMakeLists.txt


# Set up dbtest_where_root_dir.h to have a macro set to $DBTEST_SOURCE_ROOT_DIR
configure_file(
  ${LEMS_SOURCE_DIR}/basic/dbtest/dbtest_where_root_dir.h.in
  ${LEMS_BINARY_DIR}/basic/dbtest/dbtest_where_root_dir.h ESCAPE_QUOTES
)


find_path(DBTEST_WHERE_ROOT_DIR_H_DIRECTORY
          dbtest_where_root_dir.h
          ${lems_BINARY_DIR}/basic/dbtest 
          DOC "Path to directory containing the file dbtest_where_root_dir.h")

if (DBTEST_WHERE_ROOT_DIR_H_DIRECTORY)
  add_definitions(-DDBTEST_WHERE_ROOT_DIR_H_EXISTS)
  include_directories(${DBTEST_WHERE_ROOT_DIR_H_DIRECTORY})
endif(DBTEST_WHERE_ROOT_DIR_H_DIRECTORY)

set( dbtest_sources

  # Used to locate test files in source tree
  dbtest_root_dir.h   dbtest_root_dir.cxx
)

add_library( dbtest ${dbtest_sources} )
target_link_libraries( dbtest vcl )



if( BUILD_TESTING )
#  add_subdirectory( tests )
endif( BUILD_TESTING )
