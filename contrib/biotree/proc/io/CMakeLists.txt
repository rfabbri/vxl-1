# contrib/biotree/proc/io/CMakeLists.txt

include_directories( ${lems_SOURCE_DIR}/db3p )

set(io_sources
   proc_io_filter_xml_parser.cxx proc_io_filter_xml_parser.h
   proc_io_run_xml_parser.cxx    proc_io_run_xml_parser.h 
)

if( BUILD_EXAMPLES )
  subdirs( examples )
endif( BUILD_EXAMPLES )


add_library(bioproc_io ${io_sources})


if ( APPLE )
  target_link_libraries(bioproc_io vgl vsl expatpp expat xmvg splr)
else( APPLE )
  target_link_libraries(bioproc_io vgl vsl expatpp xmvg splr)
endif ( APPLE )

