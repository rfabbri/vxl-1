include_directories( ${LEMS_SOURCE_DIR}/basic )
include_directories( ${LEMS_SOURCE_DIR}/contrib/dbpz_hindsite_2 )

add_executable(dbpz_hindsite_web_combined_process dbpz_hindsite_web_combined_process.cxx)
target_link_libraries(dbpz_hindsite_web_combined_process bxml dbpz_hindsite_2_lib)
