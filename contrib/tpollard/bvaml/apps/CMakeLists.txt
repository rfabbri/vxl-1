include_directories( ${VXLCORE_INCLUDE_DIR} )
include_directories( ${lems_SOURCE_DIR}/basic )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )

add_executable ( bvaml_run bvaml_run.cxx )
target_link_libraries( bvaml_run bvaml baml vcl vil vgl vnl)
