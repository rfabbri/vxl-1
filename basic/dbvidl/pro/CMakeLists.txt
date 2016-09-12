# basic/dbvidl/pro/CMakeLists.txt
# Development Brown Video Processing Extention Library

include_directories( ${lems_SOURCE_DIR}/breye1/basic )
include_directories( ${lems_SOURCE_DIR}/breye1 )


set( dbvidl_pro_sources
        vidpro1_load_boulder_video_process.cxx vidpro1_load_boulder_video_process.h
)
add_library(dbvidl_pro  ${dbvidl_pro_sources})
target_link_libraries(dbvidl_pro dbvidl dbpro bpro1)

