# /basic/dbvidl/CMakeLists.txt
# Development Brown Video Library


INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1/basic )

SET(dbvidl_sources 
    dbvidl_boulder_video_codec.cxx    dbvidl_boulder_video_codec.h      dbvidl_boulder_video_codec_sptr.h
dbvidl_parse_meta_file.h        dbvidl_parse_meta_file.cxx
)

AUX_SOURCE_DIRECTORY(Templates dbvidl_sources)

ADD_LIBRARY(dbvidl  ${dbvidl_sources})

TARGET_LINK_LIBRARIES(dbvidl vidl1)


SUBDIRS( pro )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

