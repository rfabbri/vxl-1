INCLUDE_DIRECTORIES( ${BIOTREE_SOURCE_DIR} ${VXL_DIR} ${SIMVOLEON_INCLUDE_DIR} ${COIN_INCLUDE_DIR} ${LEMS_SOURCE_DIR} )

SET(vol_file_src
  vol_file.cxx                vol_file.h
)

ADD_LIBRARY(vol_file ${vol_file_src})
TARGET_LINK_LIBRARIES(vol_file vbl vil vil_algo vil_io)

SUBDIRS ( examples )
