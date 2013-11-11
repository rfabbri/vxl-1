# /basic/dbul/CMakeLists.txt
# Development Brown Utility Library
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )

SET(dbul_sources 
    dbul_solar_position.h   dbul_solar_position.cxx
    dbul_dir_file.h         dbul_dir_file.cxx
    dbul_parse_simple_file.h       dbul_parse_simple_file.cxx
    dbul_arg.h dbul_arg.cxx
    dbul_handle_files_embedded_in_vxl_source.h dbul_handle_files_embedded_in_vxl_source.cxx
    dbul_random.h    dbul_random.cxx
   )

IF(OCTAVE_FOUND)
    SET(dbul_sources
        ${dbul_sources}
        dbul_octave.h dbul_octave.cxx
    )
ENDIF(OCTAVE_FOUND)

#AUX_SOURCE_DIRECTORY(Templates dbul_sources)

ADD_LIBRARY(dbul  ${dbul_sources})

TARGET_LINK_LIBRARIES(dbul vul vpl)

IF(OCTAVE_FOUND)
    TARGET_LINK_LIBRARIES(dbul octinterp bxml dbxml)
ENDIF(OCTAVE_FOUND)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

SUBDIRS( cmdline )
