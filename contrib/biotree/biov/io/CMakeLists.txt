include_directories( ${BIOTREE_SOURCE_DIR} ${VXL_DIR} ${SIMVOLEON_INCLUDE_DIR} ${COIN_INCLUDE_DIR} ${LEMS_SOURCE_DIR} )

set(vol_file_src
  vol_file.cxx                vol_file.h
)

add_library(vol_file ${vol_file_src})
target_link_libraries(vol_file vbl vil vil_algo vil_io)

add_subdirectory( examples )
