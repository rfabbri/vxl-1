include_directories( ${LEMS_SOURCE_DIR}/contrib/biotree/proc/ )

add_executable(bioproc_io_filter_xml_parser_example   proc_io_filter_xml_parser_example.cxx )
target_link_libraries(bioproc_io_filter_xml_parser_example bioproc_io expatpp vgui )

add_executable(bioproc_io_run_xml_parser_example   proc_io_run_xml_parser_example.cxx )
target_link_libraries(bioproc_io_run_xml_parser_example bioproc_io expatpp vgui)



