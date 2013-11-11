# basic/dbvidl/pro/CMakeLists.txt
# Development Brown Video Processing Extention Library

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )


SET( dbvidl_pro_sources
        vidpro1_load_boulder_video_process.cxx vidpro1_load_boulder_video_process.h
)
ADD_LIBRARY(dbvidl_pro  ${dbvidl_pro_sources})
TARGET_LINK_LIBRARIES(dbvidl_pro dbvidl dbpro bpro1)

