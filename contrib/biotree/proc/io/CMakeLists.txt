# contrib/biotree/proc/io/CMakeLists.txt

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p )

SET(io_sources
   proc_io_filter_xml_parser.cxx proc_io_filter_xml_parser.h
   proc_io_run_xml_parser.cxx    proc_io_run_xml_parser.h 
)

IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )


ADD_LIBRARY(bioproc_io ${io_sources})


IF ( APPLE )
  TARGET_LINK_LIBRARIES(bioproc_io vgl vsl expatpp expat xmvg splr)
ELSE( APPLE )
  TARGET_LINK_LIBRARIES(bioproc_io vgl vsl expatpp xmvg splr)
ENDIF ( APPLE )

