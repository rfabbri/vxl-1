# /basic/dbvidl/CMakeLists.txt
# Development Brown Video Library


include_directories( ${lems_SOURCE_DIR}/breye1 )
include_directories( ${lems_SOURCE_DIR}/breye1/basic )

set(dbvidl_sources 
    dbvidl_boulder_video_codec.cxx    dbvidl_boulder_video_codec.h      dbvidl_boulder_video_codec_sptr.h
dbvidl_parse_meta_file.h        dbvidl_parse_meta_file.cxx
)

aux_source_directory(Templates dbvidl_sources)

add_library(dbvidl  ${dbvidl_sources})

target_link_libraries(dbvidl vidl1)


add_subdirectory( pro )

if( BUILD_TESTING )
  add_subdirectory(tests)
endif( BUILD_TESTING )

